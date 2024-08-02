CXX := g++
CPPFLAGS := -g -I/ucrt64/include/gstreamer-1.0 -I/ucrt64/include -I/ucrt64/include/glib-2.0 -I/ucrt64/lib/glib-2.0/include -I.
LDFLAGS += -L/ucrt64/bin -lgstreamer-1.0-0 -lglib-2.0-0 -lgobject-2.0-0

objs = main.o RTPCapture.o FileUtils.o

.PHONY: all run clean

all: $(objs)
	$(CXX) $(LDFLAGS) $^ -o capture

clean:
	rm -f *.o capture

run: all
	./capture

main.o: main.cpp
	$(CXX) $(CPPFLAGS) -c $^

FileUtils.o: FileUtils.cpp
	$(CXX) $(CPPFLAGS) -c $^

RTPCapture.o: RTPCapture.cpp
	$(CXX) $(CPPFLAGS) -c $^
