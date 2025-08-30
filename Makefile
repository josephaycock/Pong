# ---------- Variables ----------
COMPILER   = clang

# Headers live in ./include (NOT lib/)
SOURCE_LIBS = -Iinclude/

# Static library lives in ./lib; link required macOS frameworks
# NOTE: -L is for directories; the .a file itself should NOT be prefixed by -L
OSX_OPT   = -Llib/ lib/libraylib.a -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL

# Output binary
OSX_OUT   = -o "bin/build_osx"

# All C sources
CFILES    = src/*.c

# ---------- Build Commands ----------
build_osx:
	$(COMPILER) $(CFILES) $(SOURCE_LIBS) $(OSX_OUT) $(OSX_OPT)