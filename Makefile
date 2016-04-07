CXX = g++
CXXFLAGS = -O2 -Wall

CFILES = $(shell find -name "*.cpp")
OBJS = $(CFILES:.cpp=.obj)


.PHONY: build clean

build: $(OBJS)

%.obj: %.cpp
	$(CXX) $< $(CXXFLAGS) -o $@

clean:
	rm $(BINARY) 2> /dev/null
