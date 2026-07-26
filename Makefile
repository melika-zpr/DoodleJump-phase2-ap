# =====================
# OS Detection & Settings
# =====================
ifeq ($(OS),Windows_NT)
    # --- تنظیمات ویندوز ---
    # اجبار به استفاده از CMD ویندوز برای جلوگیری از تداخل با Git Bash
    SHELL = cmd.exe
    CXX = g++
    SFML_DIR = SFML
    INCFLAGS = -Iinclude -I$(SFML_DIR)/include
    LDFLAGS = -L$(SFML_DIR)/lib
    LIBFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
    EXE_EXT = .exe
    
    # دستورات ساخت پوشه (بدون ارور در صورت وجود داشتن)
    MKDIR_BUILD = -mkdir "build" 2>nul || type nul
    MKDIR_RELEASE = -mkdir "bin\release" 2>nul || type nul
    MKDIR_DEBUG = -mkdir "bin\debug" 2>nul || type nul

    # تغییر مهم: فقط فایل‌های اجرایی پاک می‌شوند و DLLها دست‌نخورده می‌مانند
    RM_CMD = -rmdir /s /q "build" 2>nul & del /q "bin\release\*.exe" 2>nul & del /q "bin\debug\*.exe" 2>nul || type nul
else
    # --- تنظیمات مک / لینوکس ---
    CXX = c++
    SFML_PKG_CONFIG_PATH =
    ifneq ($(wildcard /opt/homebrew/opt/sfml@2/lib/pkgconfig),)
        SFML_PKG_CONFIG_PATH = /opt/homebrew/opt/sfml@2/lib/pkgconfig
    else ifneq ($(wildcard /usr/local/opt/sfml@2/lib/pkgconfig),)
        SFML_PKG_CONFIG_PATH = /usr/local/opt/sfml@2/lib/pkgconfig
    endif
    SFML_CFLAGS = $(shell PKG_CONFIG_PATH="$(SFML_PKG_CONFIG_PATH)" pkg-config --cflags sfml-graphics sfml-window sfml-system sfml-audio 2>/dev/null)
    SFML_LIBS   = $(shell PKG_CONFIG_PATH="$(SFML_PKG_CONFIG_PATH)" pkg-config --libs sfml-graphics sfml-window sfml-system sfml-audio 2>/dev/null)
    
    INCFLAGS = -Iinclude $(SFML_CFLAGS)
    LDFLAGS = 
    LIBFLAGS = $(SFML_LIBS)
    EXE_EXT = 
    
    MKDIR_BUILD = mkdir -p build
    MKDIR_RELEASE = mkdir -p bin/release
    MKDIR_DEBUG = mkdir -p bin/debug

    # تغییر مهم برای سیستم مک هم‌گروهی‌تان
    RM_CMD = rm -rf build bin/release/game bin/debug/game_debug
endif

# =====================
# Files & General Flags
# =====================
CXXFLAGS = -std=c++17 -Wall -Wno-deprecated-declarations -MMD -MP
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC))
DEP = $(patsubst src/%.cpp,build/%.d,$(SRC))

RELEASE = bin/release/game$(EXE_EXT)
DEBUG = bin/debug/game_debug$(EXE_EXT)

# =====================
# Make Rules
# =====================
all: release

release: $(RELEASE)

debug: $(DEBUG)

build/%.o: src/%.cpp
	@$(MKDIR_BUILD)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c $< -o $@

-include $(DEP)

$(RELEASE): $(OBJ)
	@$(MKDIR_RELEASE)
	$(CXX) $(CXXFLAGS) -O2 -DNDEBUG $^ -o $@ $(LDFLAGS) $(LIBFLAGS)

$(DEBUG): $(OBJ)
	@$(MKDIR_DEBUG)
	$(CXX) $(CXXFLAGS) -g $^ -o $@ $(LDFLAGS) $(LIBFLAGS)

run: release
	./$(RELEASE)

clean:
	@$(RM_CMD)

.PHONY: all release debug run clean