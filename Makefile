# Sources
SRC = main.c grid.c button.c utils.c main_menu.c about_menu.c

# Output names
LINUX_OUT = build/main
WIN_OUT   = build/main.exe

# Raylib paths
RAYLIB_PATH = external/raylib/src
RAYLIB_LIB  = $(RAYLIB_PATH)/libraylib.a

# original libraylib.a used as temporary output inside raylib tree
RAYLIB_A = $(RAYLIB_PATH)/libraylib.a

# preserved separate copies so they don't stomp each other
RAYLIB_LIB_LINUX = $(RAYLIB_PATH)/libraylib_linux.a
RAYLIB_LIB_WIN   = $(RAYLIB_PATH)/libraylib_win.a

# Linux settings
CC_LINUX = gcc
CFLAGS_LINUX = -I$(RAYLIB_PATH)
LDFLAGS_LINUX = -lGL -lm -lpthread -ldl -lrt -lX11

# Windows settings
CC_WIN = x86_64-w64-mingw32-gcc
CFLAGS_WIN = -I$(RAYLIB_PATH)
LDFLAGS_WIN = -lopengl32 -lgdi32 -lwinmm

# Default target (Linux)
all: linux

linux: $(LINUX_OUT)

$(LINUX_OUT): $(SRC) $(RAYLIB_LIB_LINUX) | build
	$(CC_LINUX) $(SRC) -o $(LINUX_OUT) $(CFLAGS_LINUX) $(RAYLIB_LIB_LINUX)  $(LDFLAGS_LINUX)

# Windows target
windows: clean-raylib-win $(RAYLIB_LIB_WIN) $(WIN_OUT)

$(WIN_OUT): $(SRC) $(RAYLIB_LIB_WIN) | build
	$(CC_WIN) $(SRC) -o $(WIN_OUT) $(CFLAGS_WIN) $(RAYLIB_LIB_WIN) $(LDFLAGS_WIN)

# Build raylib for Linux and copy result to a unique file
$(RAYLIB_LIB_LINUX):
	# ensure a fresh linux build (avoids leftover mingw objects)
	$(MAKE) -C $(RAYLIB_PATH) clean
	$(MAKE) -C $(RAYLIB_PATH) PLATFORM=PLATFORM_DESKTOP CC=$(CC_LINUX) -j$(shell nproc)
	cp $(RAYLIB_A) $@

# Build raylib for Windows and copy result to a unique file
$(RAYLIB_LIB_WIN):
	# ensure a fresh win build
	$(MAKE) -C $(RAYLIB_PATH) clean
	$(MAKE) -C $(RAYLIB_PATH) \
		PLATFORM=PLATFORM_DESKTOP \
		OS=Windows_NT \
		CC=$(CC_WIN) \
		AR=x86_64-w64-mingw32-ar \
		RANLIB=x86_64-w64-mingw32-ranlib \
		-j$(shell nproc)
	cp $(RAYLIB_A) $@

# keep a clean-raylib-win target (optional helper)
clean-raylib-win:
	$(MAKE) -C $(RAYLIB_PATH) clean

# build folder
build:
	mkdir -p build

fullclean: clean
	$(MAKE) -C $(RAYLIB_PATH) clean

# Clean
clean:
	rm -rf build
