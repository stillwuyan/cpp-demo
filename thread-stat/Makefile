CXX?=g++
all: main.cpp
	$(CXX) --std=c++14 -O1 main.cpp -o cap_stat -lpthread
	realpath cap_stat
debug: main.cpp
	$(CXX) --std=c++14 -g main.cpp -o cap_stat -lpthread
	scp cap_stat car:/var/
test: main.cpp
	g++ --std=c++14 -g main.cpp -o test -lpthread
	./test


