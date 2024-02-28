# Makefile for ping_pong

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS =

# Source file
SRC = pingpong.cpp

# Executable name
EXEC = ping_pong

# Libraries
LIBS = -lX11

# Make all rule
all: $(EXEC)

# Compile rule
$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

# Clean rule
clean:
	rm -f $(EXEC)

# Run rule
run: $(EXEC)
	./$(EXEC)

