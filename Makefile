C_HEADERS = $(wildcard headers/*.hpp)
C_SOURCES = $(wildcard src/*.cpp)
HEADERS_PATH = headers
GCC_FLAGS = -Wall -Wextra -Wpedantic -lz
COMPILER_BINS = /usr/bin
OTHER_HEADERS_PATH = /usr/include

.PHONY: all

all:
	g++ -o gitathome $(C_SOURCES) -B$(COMPILER_BINS) $(GCC_FLAGS) -I$(HEADERS_PATH) -I$(OTHER_HEADERS_PATH)