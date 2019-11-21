ifndef CC
CC = gcc
AR = ar
endif
ifndef CXX
CXX = g++
endif

all:
	cd src; make all;
	cd example; make all;
	mkdir output;
	mv src/libkarl.a ./output
	mv example/example ./output

clean:
	cd src; make clean;
	cd example; make clean;


rebuild: clean all
