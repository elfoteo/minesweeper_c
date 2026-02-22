# Makefile - object-file caching + multi-target (linux / windows / web)
# Keeps build artefacts under build/ to avoid dirtying the repo tree.

# Sources
SRC := $(shell find src -type f -name '*.c') $(ASSET_IMPLEMENTATIONS_C)

# Output names
LINUX_OUT = build/main
WIN_OUT   = build/main.exe

# Debug outputs
LINUX_OUT_DEBUG = build/main_debug
WIN_OUT_DEBUG   = build/main_debug.exe

# Asset embedding
RESOURCES_SRC := $(shell find assets -type f)
RESOURCES_H   = src/resources.h
ASSET_DECLARATIONS_H = src/asset_declarations.h
ASSET_IMPLEMENTATIONS_C = src/asset_implementations.c

# Raylib location (source)
RAYLIB_PATH = external/raylib/src
RAYLIB_A = $(RAYLIB_PATH)/libraylib.a

# Where we keep copies of compiled raylib libs (inside build)
RAYLIB_OUT_DIR = build/raylib
RAYLIB_LIB_LINUX = $(RAYLIB_OUT_DIR)/libraylib_linux.a
RAYLIB_LIB_WIN   = $(RAYLIB_OUT_DIR)/libraylib_win.a
RAYLIB_LIB_WEB   = $(RAYLIB_OUT_DIR)/libraylib_web.a

# Compiler settings
# Linux
CC_LINUX = gcc
CFLAGS_LINUX = -I$(RAYLIB_PATH) -Ibuild -Isrc -O2 -Wall -Wextra
LDFLAGS_LINUX = -lm

# Linux debug (separate object dir)
DEBUG_CFLAGS_LINUX = -g3 -O0 -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined
DEBUG_LDFLAGS_LINUX = -fsanitize=address -fsanitize=undefined

# Windows (mingw)
CC_WIN = x86_64-w64-mingw32-gcc
CFLAGS_WIN = -I$(RAYLIB_PATH) -Ibuild -Isrc -O2 -Wall -Wextra
LDFLAGS_WIN = -lopengl32 -lgdi32 -lwinmm

DEBUG_CFLAGS_WIN = -g3 -O0 -fno-omit-frame-pointer
DEBUG_LDFLAGS_WIN =

# Web (emscripten)
CC_WEB = emcc
WEB_DIR = build/web
WEB_HTML = $(WEB_DIR)/index.html

# Object directories (separate per target to avoid cross-target pollution)
OBJ_DIR_LINUX = build/obj/linux
OBJ_DIR_LINUX_DEBUG = build/obj/linux_debug
OBJ_DIR_WIN = build/obj/win
OBJ_DIR_WIN_DEBUG = build/obj/win_debug
OBJ_DIR_WEB = build/obj/web

# Map source files to per-target objects
OBJ_LINUX := $(patsubst src/%.c,$(OBJ_DIR_LINUX)/%.o,$(SRC))
OBJ_LINUX_DEBUG := $(patsubst src/%.c,$(OBJ_DIR_LINUX_DEBUG)/%.o,$(SRC))
OBJ_WIN := $(patsubst src/%.c,$(OBJ_DIR_WIN)/%.o,$(SRC))
OBJ_WIN_DEBUG := $(patsubst src/%.c,$(OBJ_DIR_WIN_DEBUG)/%.o,$(SRC))
OBJ_WEB := $(patsubst src/%.c,$(OBJ_DIR_WEB)/%.o,$(SRC))

# Dependency files
DEPS_LINUX := $(OBJ_LINUX:.o=.d)
DEPS_LINUX_DEBUG := $(OBJ_LINUX_DEBUG:.o=.d)
DEPS_WIN := $(OBJ_WIN:.o=.d)
DEPS_WIN_DEBUG := $(OBJ_WIN_DEBUG:.o=.d)
DEPS_WEB := $(OBJ_WEB:.o=.d)

# Default target
.PHONY: all
all: linux

# ----------------------------
# Linux (release + debug)
# ----------------------------
.PHONY: linux
linux: $(LINUX_OUT)

$(LINUX_OUT): $(OBJ_LINUX) $(RAYLIB_LIB_LINUX) $(RESOURCES_H) | build
	$(CC_LINUX) $(OBJ_LINUX) -o $(LINUX_OUT) $(RAYLIB_LIB_LINUX) $(LDFLAGS_LINUX)

.PHONY: linux-debug
linux-debug: $(LINUX_OUT_DEBUG)

$(LINUX_OUT_DEBUG): $(OBJ_LINUX_DEBUG) $(RAYLIB_LIB_LINUX) $(RESOURCES_H) | build
	$(CC_LINUX) $(OBJ_LINUX_DEBUG) -o $(LINUX_OUT_DEBUG) \
		$(RAYLIB_LIB_LINUX) $(LDFLAGS_LINUX) $(DEBUG_LDFLAGS_LINUX)

# ----------------------------
# Windows (release + debug)
# ----------------------------
.PHONY: windows
windows: clean-raylib-win $(RAYLIB_LIB_WIN) $(WIN_OUT)

$(WIN_OUT): $(OBJ_WIN) $(RAYLIB_LIB_WIN) $(RESOURCES_H) | build
	$(CC_WIN) $(OBJ_WIN) -o $(WIN_OUT) $(RAYLIB_LIB_WIN) $(LDFLAGS_WIN)

.PHONY: windows-debug
windows-debug: $(WIN_OUT_DEBUG)

$(WIN_OUT_DEBUG): $(OBJ_WIN_DEBUG) $(RAYLIB_LIB_WIN) $(RESOURCES_H) | build
	$(CC_WIN) $(OBJ_WIN_DEBUG) -o $(WIN_OUT_DEBUG) \
		$(RAYLIB_LIB_WIN) $(LDFLAGS_WIN) $(DEBUG_LDFLAGS_WIN)

# ----------------------------
# Web
# ----------------------------
check-emcc:
	@command -v emcc >/dev/null 2>&1 || \
		{ echo "emcc not found. Install Emscripten and make sure emcc is on PATH."; exit 1; }

.PHONY: web
web: check-emcc $(WEB_HTML)

$(WEB_DIR):
	mkdir -p $(WEB_DIR)

$(WEB_HTML): $(OBJ_WEB) $(RAYLIB_LIB_WEB) $(RESOURCES_H) | $(WEB_DIR)
	$(CC_WEB) $(OBJ_WEB) -o $(WEB_HTML) \
		-I$(RAYLIB_PATH) \
		-Ibuild -Isrc \
		$(RAYLIB_LIB_WEB) \
		-DPLATFORM_WEB \
		-s USE_GLFW=3 \
		-s ASYNCIFY \
		-s NO_EXIT_RUNTIME=1 \
		--shell-file web/index.html \
		-O2

# ----------------------------
# Raylib builds (produce separate .a into build/raylib)
# ----------------------------
$(RAYLIB_LIB_LINUX): | $(RAYLIB_OUT_DIR)
	# Ensure a fresh linux build (avoids mingw/emscripten objects staying around)
	$(MAKE) -C $(RAYLIB_PATH) clean
	$(MAKE) -C $(RAYLIB_PATH) PLATFORM=PLATFORM_DESKTOP CC=$(CC_LINUX) -j$(shell nproc)
	cp $(RAYLIB_A) $@

$(RAYLIB_LIB_WIN): | $(RAYLIB_OUT_DIR)
	# Build raylib for windows (mingw) and copy to build folder
	$(MAKE) -C $(RAYLIB_PATH) clean
	$(MAKE) -C $(RAYLIB_PATH) \
		PLATFORM=PLATFORM_DESKTOP \
		OS=Windows_NT \
		CC=$(CC_WIN) \
		AR=x86_64-w64-mingw32-ar \
		RANLIB=x86_64-w64-mingw32-ranlib \
		-j$(shell nproc)
	cp $(RAYLIB_A) $@

$(RAYLIB_LIB_WEB): | $(RAYLIB_OUT_DIR)
	$(MAKE) -C $(RAYLIB_PATH) clean
	$(MAKE) -C $(RAYLIB_PATH) PLATFORM=PLATFORM_WEB -j$(shell nproc)
	# raylib's emscripten make creates libraylib.web.a in its tree; copy it to our build dir.
	cp $(RAYLIB_PATH)/libraylib.web.a $@

# ----------------------------
# Resources header generation
# ----------------------------
$(RESOURCES_H): $(RESOURCES_SRC) | build
	@rm -f $@
	@echo "/* Generated resources.h - do not edit */" > $@
	@for f in $(RESOURCES_SRC); do \
		echo "" >> $@; \
		xxd -i $$f >> $@; \
	done

$(ASSET_DECLARATIONS_H): $(RESOURCES_H) | build
	@echo "Generating $(ASSET_DECLARATIONS_H) from $(RESOURCES_H)..."
	@echo "#ifndef ASSET_DECLARATIONS_H" > $@
	@echo "#define ASSET_DECLARATIONS_H" >> $@
	@echo "" >> $@
	@# Generate extern declarations for unsigned char arrays
	@grep -E '^unsigned char assets_.*\[\] =' $< | \
		sed -E 's/^unsigned char (assets_.*\[\]) = .*/extern unsigned char \1;/' >> $@
	@echo "" >> $@
	@# Generate extern declarations for unsigned int lengths (fix extern const order)
	@grep -E '^unsigned int assets_.*_len =' $< | \
		sed -E 's/^unsigned int (assets_.*_len) = .*/extern const unsigned int \1;/' >> $@
	@echo "" >> $@
	@echo "#endif // ASSET_DECLARATIONS_H" >> $@

# ----------------------------
# Object compilation rules (per-target, separate directories)
# Use -MMD -MP to generate header deps next to objects.
# ----------------------------
$(OBJ_DIR_LINUX) $(OBJ_DIR_LINUX_DEBUG) $(OBJ_DIR_WIN) $(OBJ_DIR_WIN_DEBUG) $(OBJ_DIR_WEB):
	mkdir -p $@

# Linux (release)
$(OBJ_DIR_LINUX)/%.o: src/%.c $(ASSET_DECLARATIONS_H)
	@mkdir -p $(dir $@)
	$(CC_LINUX) $(CFLAGS_LINUX) -MMD -MP -c $< -o $@

# Linux debug (separate compile flags)
$(OBJ_DIR_LINUX_DEBUG)/%.o: src/%.c $(ASSET_DECLARATIONS_H)
	@mkdir -p $(dir $@)
	$(CC_LINUX) $(CFLAGS_LINUX) $(DEBUG_CFLAGS_LINUX) -MMD -MP -c $< -o $@

# Windows (release)
$(OBJ_DIR_WIN)/%.o: src/%.c $(ASSET_DECLARATIONS_H)
	@mkdir -p $(dir $@)
	$(CC_WIN) $(CFLAGS_WIN) -MMD -MP -c $< -o $@

# Windows debug
$(OBJ_DIR_WIN_DEBUG)/%.o: src/%.c $(ASSET_DECLARATIONS_H)
	@mkdir -p $(dir $@)
	$(CC_WIN) $(CFLAGS_WIN) $(DEBUG_CFLAGS_WIN) -MMD -MP -c $< -o $@

# Web objects (emscripten)
$(OBJ_DIR_WEB)/%.o: src/%.c $(ASSET_DECLARATIONS_H)
	@mkdir -p $(dir $@)
	$(CC_WEB) -I$(RAYLIB_PATH) -Ibuild -MMD -MP -c $< -o $@

# ----------------------------
# Include auto-generated dependency files (if present)
# ----------------------------
-include $(wildcard $(DEPS_LINUX))
-include $(wildcard $(DEPS_LINUX_DEBUG))
-include $(wildcard $(DEPS_WIN))
-include $(wildcard $(DEPS_WIN_DEBUG))
-include $(wildcard $(DEPS_WEB))

# ----------------------------
# Helpers / clean
# ----------------------------
.PHONY: build fullclean clean clean-raylib-win
build:
	mkdir -p build
	mkdir -p $(RAYLIB_OUT_DIR)
	mkdir -p $(OBJ_DIR_LINUX) $(OBJ_DIR_LINUX_DEBUG) $(OBJ_DIR_WIN) $(OBJ_DIR_WIN_DEBUG) $(OBJ_DIR_WEB)

clean:
	rm -rf build

fullclean: clean
	$(MAKE) -C $(RAYLIB_PATH) clean

# keep a clean-raylib-win target (optional helper)
clean-raylib-win:
	$(MAKE) -C $(RAYLIB_PATH) clean

.PHONY: distclean
distclean: fullclean
	@echo "distclean complete"

# End of Makefile
