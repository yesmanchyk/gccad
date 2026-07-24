CXX = /src/gcc/yesmanchyk/gcc-mirror-install/bin/g++
CXXFLAGS = -std=c++26 -freflection -I.

all: test_derivatives

generator: main.cpp functions.h differentiator.h
	$(CXX) $(CXXFLAGS) main.cpp -o generator

derivatives.h: generator functions.h differentiator.h
	./generator

test_derivatives: test_derivatives.cpp derivatives.h
	$(CXX) $(CXXFLAGS) test_derivatives.cpp -o test_derivatives

run: test_derivatives
	./test_derivatives

clean:
	rm -f generator derivatives.h test_derivatives

.PHONY: all run clean
