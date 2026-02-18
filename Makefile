# Default target
all: build/main

# Build target
build/main: main.c | build
	gcc main.c -o build/main -lraylib

# Create build directory if missing
build:
	mkdir -p build

# Clean
clean:
	rm -rf build/main

