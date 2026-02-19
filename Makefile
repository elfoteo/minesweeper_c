# Sources
SRC = main.c grid.c button.c utils.c

# Output names
LINUX_OUT = build/main
WIN_OUT   = build/main.exe

# Raylib paths
RAYLIB_PATH = external/raylib/src
RAYLIB_LIB  = $(RAYLIB_PATH)/libraylib.a

# Linux settings
CC_LINUX = gcc
CFLAGS_LINUX = -I$(RAYLIB_PATH)
LDFLAGS_LINUX = -L$(RAYLIB_PATH) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Windows settings
CC_WIN = x86_64-w64-mingw32-gcc
CFLAGS_WIN = -I$(RAYLIB_PATH)
LDFLAGS_WIN = -L$(RAYLIB_PATH) -lraylib -lopengl32 -lgdi32 -lwinmm

# Default target (Linux)
all: linux

linux: $(LINUX_OUT)

$(LINUX_OUT): $(SRC) $(RAYLIB_LIB) | build
	$(CC_LINUX) $(SRC) -o $(LINUX_OUT) $(CFLAGS_LINUX) $(LDFLAGS_LINUX)

# Windows target
windows: clean-raylib-win $(RAYLIB_LIB) $(WIN_OUT)

$(WIN_OUT): $(SRC) | build
	$(CC_WIN) $(SRC) -o $(WIN_OUT) $(CFLAGS_WIN) $(LDFLAGS_WIN)

# Build raylib (Linux)
$(RAYLIB_LIB):
	$(MAKE) -C $(RAYLIB_PATH) PLATFORM=PLATFORM_DESKTOP -j$(shell nproc)

# Build raylib (Windows)
clean-raylib-win:
	$(MAKE) -C $(RAYLIB_PATH) clean
	$(MAKE) -C $(RAYLIB_PATH) \
		PLATFORM=PLATFORM_DESKTOP \
		OS=Windows_NT \
		CC=$(CC_WIN) \
		AR=x86_64-w64-mingw32-ar \
		RANLIB=x86_64-w64-mingw32-ranlib

# Build folder
build:
	mkdir -p build

# Clean
clean:
	rm -rf build

fullclean: clean
	$(MAKE) -C $(RAYLIB_PATH) clean

