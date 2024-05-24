/*
 * The MIT License (MIT)
 *
 * Copyright © 2014 faith
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 *
 * refer: weston/simple-shm
 *   http://cgit.freedesktop.org/wayland/weston/
 */

/*======================================
    Header include
======================================*/

#include "wayland.h"

/*======================================
    Prototypes
======================================*/

static bool direct_yuv = false;
static void signal_int(int signum);

static struct display *create_display(void);
static void destroy_display(struct display *display);

static struct window *create_window(struct display *display, unsigned int surfaceId, int width, int height, bool use_egl);
static void destroy_window(struct window *window, bool use_egl);

static int create_shm_buffer(struct display *display, struct wl_data *buffer, int width, int height, uint32_t format);
static int create_anonymous_file(off_t size);
static int set_cloexec_or_close(int fd);
static void buffer_release(void *data, struct wl_buffer *buffer);

static void redraw(void *data, struct wl_callback *callback, uint32_t time);
static struct wl_data *window_next_buffer(struct window *window);

static void registry_handle_global(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
static void registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name);

static void shm_format(void *data, struct wl_shm *wl_shm, uint32_t format);

static void redraw_gl(void *data, struct wl_callback *callback, uint32_t time);
static void init_gl(struct window *window);
static GLuint CreateSimpleTexture2D(unsigned int width, unsigned int height, unsigned char* data, GLenum format);
static void UpdateSimpleTexture2D(struct window *window, unsigned int width, unsigned int height, unsigned char* data, GLenum format);
static GLuint create_shader(struct window *window, const char *source, GLenum shader_type);
static void init_egl(struct display *display);
static void fini_egl(struct display *display);

static void
handle_ivi_surface_configure(void *data, struct ivi_surface *ivi_surface,
                 int32_t width, int32_t height)
{
	/* struct window *window = data; */

    /* Simple-shm is resizable */
    fprintf(stdout, "handle_ivi_surface_configure %d %d\n", width, height);
}

static const struct ivi_surface_listener ivi_surface_listener = {
    handle_ivi_surface_configure,
};

/*======================================
    Variables
======================================*/

static void
buffer_release(void *data, struct wl_buffer *buffer)
{
    struct wl_data *mybuf = data;

    mybuf->busy = 0;
}

static const struct wl_buffer_listener buffer_listener = {
    buffer_release
};

static const struct wl_callback_listener frame_listener = {
    redraw
};

static const struct wl_registry_listener registry_listener = {
    registry_handle_global,
    registry_handle_global_remove
};

struct wl_shm_listener shm_listener = {
    shm_format
};

static int running = 1;


/*======================================
    Public functions
======================================*/

struct wayland_ctx *
wayland_init(unsigned int surfaceId, unsigned int width, unsigned int height, bool use_egl, bool raw)
{
    direct_yuv = raw;
    struct sigaction sigint;
    struct display *display;
    struct window *window;
    struct wayland_ctx *ctx;

    ctx = (struct wayland_ctx *)malloc(sizeof(struct wayland_ctx));
    if (!ctx)
        return NULL;

    ctx->buffer = (unsigned char *)malloc(width * height * 4);
    if (!ctx->buffer) {
        free(ctx);
        return NULL;
    }

    unsigned char* p = ctx->buffer;
    for (unsigned int h = 0; h < height; h++) {
        for (unsigned int w = 0; w < width; w++) {
            *(unsigned int*)(p + h * width * 4 + w * 4) = 0xFF000000;
        }
    }

    display = create_display();
    if (use_egl) {
        init_egl(display);
    }
    window = create_window(display, surfaceId, width, height, use_egl);
    if (use_egl) {
        init_gl(window);
    }

    sigint.sa_handler = signal_int;
    sigemptyset(&sigint.sa_mask);
    sigint.sa_flags = SA_RESETHAND;
    sigaction(SIGINT, &sigint, NULL);

    /* Initialise damage to full surface, so the padding gets painted */
    /* wl_surface_damage(window->surface, 0, 0, window->width, window->height); */
    if (!use_egl) {
        redraw(window, NULL, 0);
    }

    ctx->display = display;
    ctx->window = window;
    if (use_egl) {
        ctx->buffer_empty = false;
    } else {
        ctx->buffer_empty = true;
    }
    display->ctx = ctx;

    if (use_egl) {
        window->gl.textureId = CreateSimpleTexture2D(window->width, window->height, ctx->buffer, GL_RGBA);
        redraw_gl(window, NULL, 0);
    }

    return ctx;
}

void
wayland_terminate(struct wayland_ctx *ctx, bool use_egl)
{
    if (!ctx)
        return;

    destroy_window(ctx->window, use_egl);
    if (use_egl) {
        fini_egl(ctx->display);
    }
    destroy_display(ctx->display);

    free(ctx->buffer);
    free(ctx);
}

unsigned int
wayland_get_width(struct wayland_ctx *ctx)
{
    if (!ctx)
        return 0;

    return ctx->window->width;
}

unsigned int
wayland_get_height(struct wayland_ctx *ctx)
{
    if (!ctx)
        return 0;

    return ctx->window->height;
}

bool
wayland_is_running()
{
    return running;
}

int
wayland_dispatch_event(struct wayland_ctx *ctx)
{
    if (!ctx)
        return -1;

    return wl_display_dispatch(ctx->display->display);
}

bool
wayland_queue_buffer(struct wayland_ctx *ctx, void *buff, unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool use_egl)
{
    if (!ctx)
        return false;

    if (!ctx->buffer_empty)
        return false;

    unsigned char* p = ctx->buffer + y * ctx->window->width * 4 + x * 4;
    for (unsigned int i = 0; i < h; i++) {
        for (unsigned int j = 0; j < w; j++) {
            *(unsigned int *)(p + i * ctx->window->width * 4 + j * 4) = *(unsigned int *)((unsigned char *)buff + i * w * 4 + j * 4);
        }

    }
    ctx->buffer_empty = false;

    if (use_egl) {
        UpdateSimpleTexture2D(ctx->window, ctx->window->width, ctx->window->height, ctx->buffer, GL_RGBA);
        redraw_gl(ctx->window, NULL, 0);
    }

    return true;
}


/*======================================
    Inner functions
======================================*/

static void
signal_int(int signum)
{
    running = 0;
}

static struct display *create_display(void)
{
    struct display *display;
    fprintf(stdout, "create_display start\n");
    display = malloc(sizeof *display);
    if (display == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    display->display = wl_display_connect(NULL);
    assert(display->display);

    display->formats = 0;
    display->registry = wl_display_get_registry(display->display);
    wl_registry_add_listener(display->registry,
        &registry_listener, display);
    wl_display_roundtrip(display->display);
    if (display->shm == NULL) {
        fprintf(stderr, "No wl_shm global\n");
        exit(1);
    }

    wl_display_roundtrip(display->display);

    if (!(display->formats & (1 << WL_SHM_FORMAT_ARGB8888))) {
        fprintf(stderr, "WL_SHM_FORMAT_XRGB32 not available\n");
        exit(1);
    }

    //wl_display_get_fd(display->display);
    fprintf(stdout, "create_display end\n");
    return display;
}

static void destroy_display(struct display *display)
{
    if (display->shm)
        wl_shm_destroy(display->shm);

    if (display->ivi_application) {
        fprintf(stdout, "release ivi_application\n");
        ivi_application_destroy(display->ivi_application);
        display->ivi_application = NULL;
    }

    if (display->compositor)
        wl_compositor_destroy(display->compositor);

    wl_registry_destroy(display->registry);
    wl_display_flush(display->display);
    wl_display_disconnect(display->display);
    free(display);
}

static struct window *
create_window(struct display *display, unsigned int surfaceId, int width, int height, bool use_egl)
{
    struct window *window;
    fprintf(stdout, "create_window start %d %d\n", width, height);
    window = calloc(1, sizeof *window);
    if (!window)
        return NULL;

    window->callback = NULL;
    window->display = display;
    window->width = width;
    window->height = height;
    window->surface = wl_compositor_create_surface(display->compositor);

    if (use_egl) {
        window->native =
            wl_egl_window_create(window->surface,
                        window->width,
                        window->height);
        window->egl_surface =
            weston_platform_create_egl_surface(display->egl.dpy,
                            display->egl.conf,
                            window->native, NULL);
    }

    if (display->ivi_application ) {

        //uint32_t id_ivisurf = IVI_SURFACE_ID;// + (uint32_t)getpid();
        fprintf(stdout, "set surface id %d\n", surfaceId);
        window->ivi_surface =
            ivi_application_surface_create(display->ivi_application,
                            surfaceId, window->surface);
        if (window->ivi_surface == NULL) {
            fprintf(stderr, "Failed to create ivi_client_surface\n");
            assert(0);
        }

        ivi_surface_add_listener(window->ivi_surface,
                    &ivi_surface_listener, window);

        if (use_egl) {
            wl_surface_commit(window->surface);

            EGLBoolean ret;
            ret = eglMakeCurrent(window->display->egl.dpy, window->egl_surface,
                        window->egl_surface, window->display->egl.ctx);
            assert(ret == EGL_TRUE);

            window->frame_sync = true;
            if (!window->frame_sync)
                eglSwapInterval(display->egl.dpy, 0);
        }
    } else {
        fprintf(stderr, "Failed to create ivi_client_surface2\n");
        assert(0);
    }
    fprintf(stdout, "create_window end\n");

    if (use_egl) {
        window->frames = 0;
        window->delay = 0;
    }
    return window;
}

static void
destroy_window(struct window *window, bool use_egl)
{
    if (use_egl) {
        /* Required, otherwise segfault in egl_dri2.c: dri2_make_current()
        * on eglReleaseThread(). */
        eglMakeCurrent(window->display->egl.dpy, EGL_NO_SURFACE, EGL_NO_SURFACE,
                EGL_NO_CONTEXT);

        weston_platform_destroy_egl_surface(window->display->egl.dpy,
                            window->egl_surface);
        wl_egl_window_destroy(window->native);
    }

    if (window->callback)
        wl_callback_destroy(window->callback);

    if (window->buffers[0].buffer)
        wl_buffer_destroy(window->buffers[0].buffer);
    if (window->buffers[1].buffer)
        wl_buffer_destroy(window->buffers[1].buffer);

    if (window->ivi_surface) 
    {
        ivi_surface_destroy(window->ivi_surface);
        window->ivi_surface = NULL;
    }
    wl_surface_destroy(window->surface);
    free(window);
}

static int
create_shm_buffer(struct display *display, struct wl_data *wl_data,
    int width, int height, uint32_t format)
{
    struct wl_shm_pool *pool;
    int fd, size, stride;
    void *data;

    stride = width * 4;
    size = stride * height;

    fd = create_anonymous_file(size);
    if (fd < 0) {
        fprintf(stderr, "creating a buffer file for %d B failed: %m\n", size);
        return -1;
    }

    data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        fprintf(stderr, "mmap failed: %m\n");
        close(fd);
        return -1;
    }

    pool = wl_shm_create_pool(display->shm, fd, size);
    wl_data->buffer = wl_shm_pool_create_buffer(pool, 0,
                        width, height,
                        stride, format);
    wl_buffer_add_listener(wl_data->buffer, &buffer_listener, wl_data);
    wl_shm_pool_destroy(pool);
    close(fd);

    wl_data->shm_data = data;

    return 0;
}

static int
create_anonymous_file(off_t size)
{
    static const char template[] = "/weston-shared-XXXXXX";
    const char *path;
    char *name;
    int fd;
    int ret;

    path = getenv("XDG_RUNTIME_DIR");
    if (!path) {
        errno = ENOENT;
        return -1;
    }

    name = malloc(strlen(path) + sizeof(template));
    if (!name)
        return -1;

    strcpy(name, path);
    strcat(name, template);

    fd = mkstemp(name);
    if (fd >= 0) {
        fd = set_cloexec_or_close(fd);
        unlink(name);
    }

    free(name);

    if (fd < 0)
        return -1;

    ret = ftruncate(fd, size);
    if (ret < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

static int
set_cloexec_or_close(int fd)
{
    long flags;

    if (fd == -1)
        return -1;

    flags = fcntl(fd, F_GETFD);
    if (flags == -1)
        goto err;

    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
        goto err;

    return fd;

err:
    close(fd);
    return -1;
}

static void
redraw(void *data, struct wl_callback *callback, uint32_t time)
{
    struct window *window = data;
    struct wl_data *wl_data;
    struct wayland_ctx *ctx = window->display->ctx;

    if (callback && ctx && ctx->buffer_empty)
    {
        fprintf(stderr, "redraw ctx->buffer_empty:%d\n", ctx->buffer_empty);
        return;
    }

    wl_data = window_next_buffer(window);
    if (!wl_data) {
        fprintf(stderr,
            !callback ? "Failed to create the first buffer.\n" :
            "Both buffers busy at redraw(). Server bug?\n");
        abort();
    }

    if (ctx && !ctx->buffer_empty) {
        memcpy(wl_data->shm_data, ctx->buffer, window->width * window->height * 4);
        ctx->buffer_empty = true;
    }

    wl_surface_attach(window->surface, wl_data->buffer, 0, 0);
    wl_surface_damage(window->surface, 0, 0, window->width, window->height);

    if (callback)
        wl_callback_destroy(callback);

    window->callback = wl_surface_frame(window->surface);
    wl_callback_add_listener(window->callback, &frame_listener, window);
    wl_surface_commit(window->surface);
    wl_data->busy = 1;
}

static struct wl_data *
window_next_buffer(struct window *window)
{
    struct wl_data *wl_data;
    int ret = 0;

    if (!window->buffers[0].busy)
        wl_data = &window->buffers[0];
    else if (!window->buffers[1].busy)
        wl_data = &window->buffers[1];
    else
        return NULL;

    if (!wl_data->buffer) {
        ret = create_shm_buffer(window->display, wl_data,
                window->width, window->height,
                WL_SHM_FORMAT_ARGB8888);

        if (ret < 0)
            return NULL;

        unsigned char* p = wl_data->shm_data;
        for (unsigned int h = 0; h < window->height; h++) {
            for (unsigned int w = 0; w < window->width; w++) {
                *(unsigned int*)(p + h * window->width * 4 + w * 4) = 0x00000000;
            }
        }
        /* memset(wl_data->shm_data, 0xFF, window->width * window->height * 4); */
    }

    return wl_data;
}

static void
registry_handle_global(void *data, struct wl_registry *registry,
    uint32_t id, const char *interface, uint32_t version)
{
    struct display *d = data;

    if (strcmp(interface, "wl_compositor") == 0) {
        d->compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    } else if (strcmp(interface, "ivi_application") == 0) {
        d->ivi_application = wl_registry_bind(registry, id, &ivi_application_interface, 1);
    } else if (strcmp(interface, "wl_shm") == 0) {
        d->shm = wl_registry_bind(registry, id, &wl_shm_interface, 1);
        wl_shm_add_listener(d->shm, &shm_listener, d);
    }
}

static void
registry_handle_global_remove(void *data, struct wl_registry *registry,
    uint32_t name)
{
}

static void
shm_format(void *data, struct wl_shm *wl_shm, uint32_t format)
{
    struct display *d = data;

    d->formats |= (1 << format);
}

bool wayland_surface_check(struct wl_display *wlDisplay, unsigned int surfaceId)
{
    int count = 0;
    unsigned int* array = NULL;
    bool result = false;

    ilm_initWithNativedisplay((t_ilm_nativedisplay)wlDisplay);
    ilm_commitChanges();

    ilmErrorTypes callResult = ilm_getSurfaceIDs(&count, &array);
    if (ILM_SUCCESS != callResult)
    {
        fprintf(stderr, "wayland_surface_check Failed to get surface IDs: %s\n", ILM_ERROR_STRING(callResult));
        return result;
    }

    for (int i = 0; i < count; ++i)
    {
        if (array[i] == surfaceId)
            result = true;
    }

    free(array);
    return result;
}

void wayland_window_visible(unsigned int layerId, unsigned int surfaceId, unsigned int screenId, bool visibility, unsigned int width, unsigned int height)
{
    ilmErrorTypes callResult;

    ilm_layerRemove(layerId);
    ilm_commitChanges();

    callResult = ilm_layerCreateWithDimension(&layerId, width, height);
    if (ILM_SUCCESS != callResult)
    {
        fprintf(stderr, "window_visible returned: %s\n", ILM_ERROR_STRING(callResult));
        fprintf(stderr, "Failed to create layer with ID: %d\n", layerId);
        return;
    }

    ilm_commitChanges();

    callResult = ilm_layerAddSurface(layerId, surfaceId);
    if (ILM_SUCCESS != callResult)
    {
        fprintf(stderr, "window_visible returned: %s\n", ILM_ERROR_STRING(callResult));
        fprintf(stderr, "Failed to add surfacer %d to layer %d\n", surfaceId, layerId);
        return;
    }

    ilm_commitChanges();

    callResult = ilm_layerSetDestinationRectangle(layerId, 0, 0, width, height);
    if (ILM_SUCCESS != callResult)
    {
        fprintf(stderr, "window_visible returned: %s\n", ILM_ERROR_STRING(callResult));
        fprintf(stderr, "Failed to set destination rectangle %d %d %d %d for layer with ID %d\n", 0, 0 , width, height, layerId);
        return;
    }

    callResult = ilm_layerSetSourceRectangle(layerId, 0, 0, width, height);
    if (ILM_SUCCESS != callResult)
    {
        fprintf(stderr, "window_visible returned: %s\n", ILM_ERROR_STRING(callResult));
        fprintf(stderr, "Failed to set source rectangle %d %d %d %d for layer with ID %d\n", 0, 0 , width, height, layerId);
        return;
    }

    callResult = ilm_surfaceSetDestinationRectangle(surfaceId, 0, 0, width, height);
    if (ILM_SUCCESS != callResult)
    {
        fprintf(stderr, "window_visible returned: %s\n", ILM_ERROR_STRING(callResult));
        fprintf(stderr, "Failed to set destination rectangle %d %d %d %d for surface with ID %d\n", 0, 0 , width, height, surfaceId);
        return;
    }

    callResult = ilm_surfaceSetSourceRectangle(surfaceId, 0, 0, width, height);
    if (ILM_SUCCESS != callResult)
    {
        fprintf(stderr, "window_visible returned: %s\n", ILM_ERROR_STRING(callResult));
        fprintf(stderr, "Failed to set source rectangle %d %d %d %d for surface with ID %d\n", 0, 0 , width, height, surfaceId);
        return;
    }

    ilm_commitChanges();

    callResult = ilm_layerSetVisibility(layerId, true);
    if (ILM_SUCCESS != callResult)
    {
        fprintf(stderr, "window_visible returned: %s\n", ILM_ERROR_STRING(callResult));
        fprintf(stderr, "Failed to set visibility %d for layer with ID %d\n", visibility, layerId);
        return;
    }

    callResult = ilm_surfaceSetVisibility(surfaceId, true);
    if (ILM_SUCCESS != callResult)
    {
        fprintf(stderr, "window_visible returned: %s\n", ILM_ERROR_STRING(callResult));
        fprintf(stderr, "Failed to set visibility %d for surface with ID %d\n", visibility, surfaceId);
        return;
    }

    ilm_commitChanges();

    unsigned int count = 1;
    unsigned int array[1] = {0};
    array[0] = layerId;

    callResult = ilm_displaySetRenderOrder(screenId, array, count);
    if (ILM_SUCCESS != callResult)
    {
        fprintf(stderr, "window_visible returned: %s\n", ILM_ERROR_STRING(callResult));
        fprintf(stderr, "Failed to set render order for screen with ID %d\n", screenId);
        return;
    }

    ilm_commitChanges();
}

static const char *vert_shader_text =
	"attribute vec4 pos;                     \n"
	"attribute vec2 coord;                   \n"
	"varying vec2 v_texCoord;                \n"
	"void main()                             \n"
	"{                                       \n"
	"   gl_Position = pos;                   \n"
	"   v_texCoord = coord;                  \n"
	"}                                       \n";

static const char *frag_shader_text_rgb =
	"precision mediump float;                            \n"
	"varying vec2 v_texCoord;                            \n"
	"uniform sampler2D s_texture;                        \n"
	"void main()                                         \n"
	"{                                                   \n"
	"  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
	"}                                                   \n";

static const char *frag_shader_text_yuv =
    "precision mediump float;                        \n"
    "varying vec2 v_texCoord;                        \n"
    "uniform sampler2D s_texture;                    \n"
    "void main() {                                   \n"
    "    vec2 texCoord = v_texCoord;                 \n"
    "    vec4 yuyv = texture2D(s_texture, texCoord); \n"
    "    float y1 = yuyv.r;                          \n"
    "    float u = yuyv.g - 0.5;                     \n"
    "    float y2 = yuyv.b;                          \n"
    "    float v = yuyv.a - 0.5;                     \n"
    "    vec3 rgb1 = vec3(                           \n"
    "        y1 + 1.402 * v,                         \n"
    "        y1 - 0.344136 * u - 0.714136 * v,       \n"
    "        y1 + 1.772 * u                          \n"
    "    );                                          \n"
    "    vec3 rgb2 = vec3(                           \n"
    "        y2 + 1.402 * v,                         \n"
    "        y2 - 0.344136 * u - 0.714136 * v,       \n"
    "        y2 + 1.772 * u                          \n"
    "    );                                          \n"
    "    if (mod(gl_FragCoord.x, 2.0) < 1.0) {       \n"
    "        gl_FragColor = vec4(rgb1, 1.0);         \n"
    "    } else {                                    \n"
    "        gl_FragColor = vec4(rgb2, 1.0);         \n"
    "    }                                           \n"
    "}                                               \n";

static void
init_egl(struct display *display)
{
	static const struct {
		char *extension, *entrypoint;
	} swap_damage_ext_to_entrypoint[] = {
		{
			.extension = "EGL_EXT_swap_buffers_with_damage",
			.entrypoint = "eglSwapBuffersWithDamageEXT",
		},
		{
			.extension = "EGL_KHR_swap_buffers_with_damage",
			.entrypoint = "eglSwapBuffersWithDamageKHR",
		},
	};

	static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	const char *extensions;

	EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 1,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	EGLint major, minor, n, count, i;
	EGLConfig *configs;
	EGLBoolean ret;

	display->egl.dpy =
		weston_platform_get_egl_display(EGL_PLATFORM_WAYLAND_KHR,
						display->display, NULL);
	assert(display->egl.dpy);

	ret = eglInitialize(display->egl.dpy, &major, &minor);
	assert(ret == EGL_TRUE);
	ret = eglBindAPI(EGL_OPENGL_ES_API);
	assert(ret == EGL_TRUE);
    printf("egl version major: %d, minor: %d\n", major, minor);

	if (!eglGetConfigs(display->egl.dpy, NULL, 0, &count) || count < 1)
		assert(0);

	configs = calloc(count, sizeof *configs);
	assert(configs);

	ret = eglChooseConfig(display->egl.dpy, config_attribs,
			      configs, count, &n);
	assert(ret && n >= 1);

	for (i = 0; i < n; i++) {
		EGLint buffer_size, red_size;
		eglGetConfigAttrib(display->egl.dpy,
				   configs[i], EGL_BUFFER_SIZE, &buffer_size);
		eglGetConfigAttrib(display->egl.dpy,
				   configs[i], EGL_RED_SIZE, &red_size);
		if (red_size < 10) {
			display->egl.conf = configs[i];
			break;
		}
	}
	free(configs);
	if (display->egl.conf == NULL) {
		fprintf(stderr, "did not find config\n");
		exit(EXIT_FAILURE);
	}

	display->egl.ctx = eglCreateContext(display->egl.dpy,
					    display->egl.conf,
					    EGL_NO_CONTEXT, context_attribs);
	assert(display->egl.ctx);

	display->swap_buffers_with_damage = NULL;
	extensions = eglQueryString(display->egl.dpy, EGL_EXTENSIONS);
	if (extensions &&
	    weston_check_egl_extension(extensions, "EGL_EXT_buffer_age")) {
		for (i = 0; i < (int) ARRAY_LENGTH(swap_damage_ext_to_entrypoint); i++) {
			if (weston_check_egl_extension(extensions,
						       swap_damage_ext_to_entrypoint[i].extension)) {
				/* The EXTPROC is identical to the KHR one */
				display->swap_buffers_with_damage =
					(PFNEGLSWAPBUFFERSWITHDAMAGEEXTPROC)
					eglGetProcAddress(swap_damage_ext_to_entrypoint[i].entrypoint);
				break;
			}
		}
	}

	if (display->swap_buffers_with_damage)
		printf("has EGL_EXT_buffer_age and %s\n", swap_damage_ext_to_entrypoint[i].extension);

}

static void
fini_egl(struct display *display)
{
	eglTerminate(display->egl.dpy);
	eglReleaseThread();
}

static GLuint
create_shader(struct window *window, const char *source, GLenum shader_type)
{
	GLuint shader;
	GLint status;

	shader = glCreateShader(shader_type);
	assert(shader != 0);

	glShaderSource(shader, 1, (const char **) &source, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char log[1000];
		GLsizei len;
		glGetShaderInfoLog(shader, 1000, &len, log);
		fprintf(stderr, "Error: compiling %s: %.*s\n",
			shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment",
			len, log);
		exit(1);
	}

	return shader;
}

static GLuint CreateSimpleTexture2D(unsigned int width, unsigned int height, unsigned char* data, GLenum format)
{
    // Texture object handle
    GLuint textureId;

    // Use tightly packed data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Generate a texture object
    glGenTextures(1, &textureId);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Load the texture
    unsigned int w = (direct_yuv ? width/2 : width);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, height, 0, format, GL_UNSIGNED_BYTE, data);

    // Set the filtering mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureId;
}

static void UpdateSimpleTexture2D(struct window *window, unsigned int width, unsigned int height, unsigned char* data, GLenum format)
{
    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, window->gl.textureId);

    // Load the texture
    unsigned int w = (direct_yuv ? width/2: width);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, height, format, GL_UNSIGNED_BYTE, data);
}

static void
init_gl(struct window *window)
{
	GLuint frag, vert;
	GLuint program;
	GLint status;

    const char* frag_shader_text = (direct_yuv ? frag_shader_text_yuv : frag_shader_text_rgb);
	frag = create_shader(window, frag_shader_text, GL_FRAGMENT_SHADER);
	vert = create_shader(window, vert_shader_text, GL_VERTEX_SHADER);

	program = glCreateProgram();
	glAttachShader(program, frag);
	glAttachShader(program, vert);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (!status) {
		char log[1000];
		GLsizei len;
		glGetProgramInfoLog(program, 1000, &len, log);
		fprintf(stderr, "Error: linking:\n%.*s\n", len, log);
		exit(1);
	}

	glUseProgram(program);

	window->gl.pos = 0;
	window->gl.coo = 1;

	glBindAttribLocation(program, window->gl.pos, "pos");
	glBindAttribLocation(program, window->gl.coo, "coord");
	glLinkProgram(program);

	window->gl.texture = glGetUniformLocation(program, "s_texture");
}

static void
redraw_gl(void *data, struct wl_callback *callback, uint32_t time)
{
    struct window *window = data;
    struct display *display = window->display;
    struct wayland_ctx *ctx = window->display->ctx;

    if (ctx->buffer_empty) {
        printf("not redraw\n");
        return;
    }

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    static const GLfloat posVertices[4][2] = {
        {-1.0f, -1.0f}, // Position 0
        {1.0f, -1.0f},  // Position 1
        {-1.0f, 1.0f},  // Position 2
        {1.0f, 1.0f},   // Position 3
    };

	static const GLfloat texVertices[4][2] = {
        {0.0f,  1.0f},  // TexCoord 2
        {1.0f,  1.0f},  // TexCoord 3
        {0.0f,  0.0f},  // TexCoord 0
        {1.0f,  0.0f},  // TexCoord 1
    };
    GLushort indices[] = { 0, 1, 3, 0, 2, 3 };

	static const uint32_t benchmark_interval = 5;
	EGLint rect[4];
	EGLint buffer_age = 0;
	struct timeval tv;

	assert(window->callback == callback);
	window->callback = NULL;

	if (callback)
		wl_callback_destroy(callback);

	gettimeofday(&tv, NULL);
	time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	if (window->frames == 0)
		window->benchmark_time = time;
	if (time - window->benchmark_time > (benchmark_interval * 1000)) {
		printf("%d frames in %d seconds: %f fps\n",
		       window->frames,
		       benchmark_interval,
		       (float) window->frames / benchmark_interval);
		window->benchmark_time = time;
		window->frames = 0;
	}

	if (display->swap_buffers_with_damage)
		eglQuerySurface(display->egl.dpy, window->egl_surface,
				EGL_BUFFER_AGE_EXT, &buffer_age);

	glViewport(0, 0, window->width, window->height);

	/* glUniformMatrix4fv(window->gl.rotation, 1, GL_FALSE, (GLfloat *) rotation); */

    glClearColor(0.0, 0.0, 0.0, 0.5);
	glClear(GL_COLOR_BUFFER_BIT);

	// Load the vertex position
	glVertexAttribPointer(window->gl.pos, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
	// Load the texture coordinate
	glVertexAttribPointer(window->gl.coo, 2, GL_FLOAT, GL_FALSE, 0, texVertices);
	glEnableVertexAttribArray(window->gl.pos);
	glEnableVertexAttribArray(window->gl.coo);

	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, window->gl.textureId);

	// Set the sampler texture unit to 0
    glUniform1i(window->gl.texture, 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
	glDisableVertexAttribArray(window->gl.pos);
	glDisableVertexAttribArray(window->gl.coo);
	usleep(window->delay);

    wl_surface_set_opaque_region(window->surface, NULL);

	if (display->swap_buffers_with_damage && buffer_age > 0) {
		rect[0] = window->width / 4 - 1;
		rect[1] = window->height / 4 - 1;
		rect[2] = window->width / 2 + 2;
		rect[3] = window->height / 2 + 2;
		display->swap_buffers_with_damage(display->egl.dpy,
						  window->egl_surface,
						  rect, 1);
	} else {
		eglSwapBuffers(display->egl.dpy, window->egl_surface);
	}
	window->frames++;
    ctx->buffer_empty = true;
}
