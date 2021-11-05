all: main.cpp
	g++ -std=c++11 -I./include main.cpp -o test -L./lib -lcurl

run: test
	@LD_LIBRARY_PATH=./lib ./test
