# Default target
all: build/main

# Build target
build/main: main.c grid.c | build
	gcc main.c grid.c -o build/main -lraylib

# Create build directory if missing
build:
	mkdir -p build

# Clean
clean:
	rm -rf build/main


