
CFLAGS = -O2 -I ../include -std=c11
CXXFLAGS = -O2 -I ../include -std=c++11
LFLAGS = -lm -lstdc++

ifdef DEBUG
CFLAGS += -g3
CXXFLAGS += -g3
endif

DEPS = 

OBJS = main.o \
	../src/cbor.o \
	../src/cJSON.o \
	../src/karl.o


%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cc
	$(CXX) -c -o $@ $< $(CXXFLAGS)

example: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

all:
	make example;

clean:
	rm -f *.o
	rm -f ./example

