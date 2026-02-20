# Sources
SRC = $(wildcard src/*.c)

# Output names
LINUX_OUT = build/main
WIN_OUT   = build/main.exe

# Asset embedding
RESOURCES_SRC = $(wildcard assets/*)
RESOURCES_H   = build/resources.h

# Raylib paths
RAYLIB_PATH = external/raylib/src
RAYLIB_LIB  = $(RAYLIB_PATH)/libraylib.a

# original libraylib.a used as temporary output inside raylib tree
RAYLIB_A = $(RAYLIB_PATH)/libraylib.a

# preserved separate copies so they don't stomp each other
RAYLIB_LIB_LINUX = $(RAYLIB_PATH)/libraylib_linux.a
RAYLIB_LIB_WIN   = $(RAYLIB_PATH)/libraylib_win.a

# Debug flags (optional)
DEBUG_CFLAGS_LINUX = -g3 -O0 -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined
DEBUG_LDFLAGS_LINUX = -fsanitize=address -fsanitize=undefined
DEBUG_CFLAGS_WIN = -g3 -O0 -fno-omit-frame-pointer
DEBUG_LDFLAGS_WIN =

LINUX_OUT_DEBUG = build/main_debug
WIN_OUT_DEBUG = build/main_debug.exe

# Linux settings
CC_LINUX = gcc
CFLAGS_LINUX = -I$(RAYLIB_PATH) -Ibuild
LDFLAGS_LINUX = -lm


# Windows settings
CC_WIN = x86_64-w64-mingw32-gcc
CFLAGS_WIN = -I$(RAYLIB_PATH) -Ibuild
LDFLAGS_WIN = -lopengl32 -lgdi32 -lwinmm

# Web settings
CC_WEB = emcc
WEB_DIR = build/web
WEB_HTML = $(WEB_DIR)/index.html
WEB_JS = $(WEB_DIR)/index.js
WEB_WASM = $(WEB_DIR)/index.wasm

RAYLIB_LIB_WEB = $(RAYLIB_PATH)/libraylib_web.a

# Default target (Linux)
all: linux

linux: $(LINUX_OUT)

$(LINUX_OUT): $(SRC) $(RAYLIB_LIB_LINUX) $(RESOURCES_H) | build
	$(CC_LINUX) $(SRC) -o $(LINUX_OUT) $(CFLAGS_LINUX) $(RAYLIB_LIB_LINUX)  $(LDFLAGS_LINUX)

# Linux debug build
linux-debug: $(LINUX_OUT_DEBUG)

$(LINUX_OUT_DEBUG): $(SRC) $(RAYLIB_LIB_LINUX) $(RESOURCES_H) | build
	$(CC_LINUX) $(SRC) -o $(LINUX_OUT_DEBUG) \
		$(CFLAGS_LINUX) $(DEBUG_CFLAGS_LINUX) $(RAYLIB_LIB_LINUX) $(LDFLAGS_LINUX) $(DEBUG_LDFLAGS_LINUX)

# Windows target
windows: clean-raylib-win $(RAYLIB_LIB_WIN) $(WIN_OUT)

$(WIN_OUT): $(SRC) $(RAYLIB_LIB_WIN) $(RESOURCES_H) | build
	$(CC_WIN) $(SRC) -o $(WIN_OUT) $(CFLAGS_WIN) $(RAYLIB_LIB_WIN) $(LDFLAGS_WIN)

windows-debug: $(WIN_OUT_DEBUG)


$(WIN_OUT_DEBUG): $(SRC) $(RAYLIB_LIB_WIN) $(RESOURCES_H) | build
	$(CC_WIN) $(SRC) -o $(WIN_OUT_DEBUG) \
		$(CFLAGS_WIN) $(DEBUG_CFLAGS_WIN) $(RAYLIB_LIB_WIN) $(LDFLAGS_WIN)

# Web target
check-emcc:
	@command -v emcc >/dev/null 2>&1 || \
		{ echo "emcc not found. How would even think about compiling for web without installing emscripten first!"; exit 1; }

web: check-emcc $(WEB_HTML) $(WEB_JS) $(WEB_WASM)

$(WEB_DIR):
	mkdir -p $(WEB_DIR)

$(WEB_HTML) $(WEB_JS) $(WEB_WASM): $(SRC) $(RAYLIB_LIB_WEB) $(RESOURCES_H) | $(WEB_DIR)
	$(CC_WEB) $(SRC) -o $(WEB_HTML) \
		-I$(RAYLIB_PATH) \
		-Ibuild \
		$(RAYLIB_LIB_WEB) \
		-DPLATFORM_WEB \
		-s USE_GLFW=3 \
		-s ASYNCIFY \
		-s NO_EXIT_RUNTIME=1 \
		--shell-file web/index.html \
		-O2


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

$(RAYLIB_LIB_WEB):
	$(MAKE) -C $(RAYLIB_PATH) clean
	$(MAKE) -C $(RAYLIB_PATH) PLATFORM=PLATFORM_WEB -j$(shell nproc)
	cp $(RAYLIB_PATH)/libraylib.web.a $@

$(RESOURCES_H): $(RESOURCES_SRC) | build
	rm -f $@
	for f in $(RESOURCES_SRC); do \
		xxd -i $$f >> $@; \
	done

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
