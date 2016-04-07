CXX = g++

CFILES = $(shell find -name "*.cpp")
OBJS = $(CFILES:.cpp=.obj)
INC_DIR = ./inc

CXXFLAGS = -O2 -Wall
CXXFLAGS += -std=gnu++11
CXXFLAGS += -I$(INC_DIR)

cache.bin: $(OBJS)
	$(CXX) $(OBJS) -o cache.bin

%.obj: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

.PHONY: clean

clean:
	rm $(OBJS) 2> /dev/null
	rm cache.bin 2> /dev/null
