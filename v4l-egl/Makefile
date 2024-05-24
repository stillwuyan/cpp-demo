#CC := gcc

CFLAGS := -g -Wall -O2 $(shell pkg-config --cflags wayland-client)

LDFLAGS := $(shell pkg-config --libs wayland-client ivi-application ilmControl ilmCommon libjpeg egl glesv2 wayland-egl) -lm

OUTPUT := wl-camera-shm

OBJS := main.o camera.o convert.o wayland.o util.o

.PHONY : all clean

all : $(OUTPUT)

$(OUTPUT) : $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

.c.o :
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm -f $(OUTPUT) $(OBJS)
