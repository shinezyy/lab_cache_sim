CXX = g++

INC_DIR = ./inc
SRC_DIR = ./src 
OBJ_DIR = ./obj

CPPFILES = $(shell find -name "*.cpp")
OBJS := $(patsubst %.cpp, $(OBJ_DIR)/%.obj, $(CPPFILES))

CXXFLAGS = -O2 -Wall -Werror
CXXFLAGS += -std=gnu++11
CXXFLAGS += -I$(INC_DIR)


cache.bin: $(OBJS)
	$(CXX) $(OBJS) -o cache.bin

$(OBJ_DIR)/%.obj: %.cpp
	@mkdir -p $(@D)
	$(CXX) $< $(CXXFLAGS) -c -o $@

run: cache.bin
	./cache.bin

.PHONY: clean

clean:
	rm $(OBJS) 2> /dev/null
	rm cache.bin 2> /dev/null
