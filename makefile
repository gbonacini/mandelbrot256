OPTS = -O3 -I. -std=c++20 -pedantic -Wall -Wextra 
OPENMT := $(shell if [ "x$$(echo | cpp -fopenmp -dM -E - 2>/dev/null | grep -i openmp)" != "x" ] ; then  echo "-fopenmp"; fi)

all: mandelbrot

parseCmdLine.o: parseCmdLine.cpp parseCmdLine.hpp
	g++ -c $(OPTS)  $(OPENMT) -o $@ $<

mandelbrot256.o: mandelbrot256.cpp  mandelbrot256.hpp
	g++ -c $(OPTS)  $(OPENMT) -o $@  $<

mandelbrot.o: mandelbrot.cpp
	g++ -c $(OPTS)  $(OPENMT) -o $@  mandelbrot.cpp 

mandelbrot: mandelbrot.o mandelbrot256.o parseCmdLine.o
	g++ $(OPTS)   $(OPENMT) -o $@  $?

clean:
	rm -f  mandelbrot.o mandelbrot256.o parseCmdLine.o mandelbrot

