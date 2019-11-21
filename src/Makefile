
CFLAGS = -O2 -I ../include -std=c11
CXXFLAGS = -O2 -I ../include -std=c++11
LFLAGS = -lm -lstdc++

ifdef DEBUG
CFLAGS += -g3
CXXFLAGS += -g3
endif

DEPS = 
OBJS = cbor.o cJSON.o karl.o

TARGET_LIB = libkarl.a

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cc
	$(CXX) -c -o $@ $< $(CXXFLAGS)

all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(AR) rcs $@ $^

clean:
	rm -f *.o
	rm -f $(TARGET_LIB)
