all: hr tests

hr:
	@echo ""
	@echo "======================================================================"

tests: tests.o parser.o
	${CXX} tests.o parser.o -o tests
	rm *.o

tests.o:
	${CXX} -std=c++11 -Wall -pedantic -g -gstabs -Wextra -c tests.cpp

parser.o:
	${CXX} -std=c++11 -Wall -pedantic -g -gstabs -Wextra -c parser.cpp
