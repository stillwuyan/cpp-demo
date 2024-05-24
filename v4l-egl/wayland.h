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
#ifndef _MY_WAYLAND_H
#define _MY_WAYLAND_H

/*======================================
	Header include
======================================*/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#include "common.h"

#include <wayland-client.h>
#include <wayland-egl.h>

#include <ilm/ivi-application-client-protocol.h>
#include <ilm/ilm_control.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "helpers.h"
#include "platform.h"
#include "weston-egl-ext.h"

/*======================================
	Structures
======================================*/


/*======================================
    Structures
======================================*/

struct wayland_ctx;

struct display {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct ivi_application *ivi_application;
    struct wl_shm *shm;
    uint32_t formats;

    struct wayland_ctx *ctx;

	struct {
		EGLDisplay dpy;
		EGLContext ctx;
		EGLConfig conf;
	} egl;

	PFNEGLSWAPBUFFERSWITHDAMAGEEXTPROC swap_buffers_with_damage;
};

struct wl_data {
    struct wl_buffer *buffer;
    void *shm_data;
    int busy;
};

struct window {
    struct display *display;
    int width, height;
    struct wl_surface *surface;
    struct ivi_surface *ivi_surface;
    struct wl_data buffers[2];
    //struct buffer *prev_buffer;
    struct wl_callback *callback;
    bool frame_sync;

	struct {
		GLuint pos;
		GLuint coo;
		GLuint textureId;
		GLint texture;
	} gl;
	uint32_t benchmark_time, frames;
    int delay;
	struct wl_egl_window *native;
	EGLSurface egl_surface;
};

struct wayland_ctx {
    struct display *display;
    struct window *window;
    unsigned char *buffer;
    bool buffer_empty;
};

/*======================================
	Prototypes
======================================*/

/* #ifdef __cplusplus */
/* extern "C" { */
/* #endif /\* __cplusplus *\/ */

struct wayland_ctx *wayland_init(unsigned int surfaceId, unsigned int width, unsigned int height, bool use_egl, bool raw);
void wayland_terminate(struct wayland_ctx *ctx, bool use_egl);
unsigned int wayland_get_width(struct wayland_ctx *ctx);
unsigned int wayland_get_height(struct wayland_ctx *ctx);
bool wayland_is_running();
int wayland_dispatch_event(struct wayland_ctx *ctx);
bool wayland_queue_buffer(struct wayland_ctx *ctx, void *buff, unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool use_egl);

bool wayland_surface_check(struct wl_display *wlDisplay, unsigned int surfaceId);
void wayland_window_visible(unsigned int layerId, unsigned int surfaceId, unsigned int screenId, bool visibility, unsigned int width, unsigned int height);

/* #ifdef __cplusplus */
/* } */
/* #endif /\* __cplusplus *\/ */

#endif /* _WAYLAND_H */
