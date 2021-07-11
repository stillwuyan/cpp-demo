demo:main.cpp
	g++ -g -std=c++11 main.cpp -o demo 

test: test1 test2 test3 test4

test1: demo tmp
	@echo "------------------------------------"
	@echo -e "\033[0;34mtest case 1:\033[0m"
	@-rm -f ./tmp/123.txt ||:
	./demo ../123.txt
	@echo ""
	@-cat ./123.txt ||:
	@-cat ./tmp/123.txt ||:
	@echo "------------------------------------"
	@echo ""

test2: demo tmp
	@echo "------------------------------------"
	@echo -e "\033[0;34mtest case 2:\033[0m"
	@-rm -f ./tmp/123.txt ||:
	./demo \.\./123.txt
	@echo ""
	@-cat ./123.txt ||:
	@-cat ./tmp/123.txt ||:
	@echo "------------------------------------"
	@echo ""

test3: demo tmp
	@echo "------------------------------------"
	@echo -e "\033[0;34mtest case 3:\033[0m"
	@rm -f ./tmp/123.txt ||:
	./demo \\.\\./123.txt
	@echo ""
	@-cat ./123.txt ||:
	@-cat ./tmp/123.txt ||:
	@echo "------------------------------------"
	@echo ""

test4: demo tmp
	@echo "------------------------------------"
	@echo -e "\033[0;34mtest case 4:\033[0m"
	@rm -f ./tmp/123.txt ||:
	./demo \\\.\\\./123.txt
	@echo ""
	@-cat ./123.txt ||:
	@-cat ./tmp/123.txt ||:
	@echo "------------------------------------"
	@echo ""

tmp:
	mkdir tmp
