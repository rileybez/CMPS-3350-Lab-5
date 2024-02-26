CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11

# Include SDL2 compiler flags and linker flags
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS := $(shell sdl2-config --libs)

# Target: test
test: test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(SDL_CFLAGS) $(SDL_LIBS)

.PHONY: clean

clean:
	rm -f test

