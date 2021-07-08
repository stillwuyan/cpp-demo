all:
	@g++ --std=c++11 main.cpp -O2 -c -S -o - -masm=intel | c++filt | grep -vE '\s+\.'
