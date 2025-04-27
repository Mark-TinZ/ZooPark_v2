.DEFAULT_GOAL := all

CXX = g++
BUILD_DIR = build
EXE = prog
IMGUI_DIR = imgui
INCLUDE = include
SRC = src

# Используем полные пути для исходников
SOURCES = $(SRC)/main.cpp \
          $(IMGUI_DIR)/imgui.cpp \
          $(IMGUI_DIR)/imgui_demo.cpp \
          $(IMGUI_DIR)/imgui_draw.cpp \
          $(IMGUI_DIR)/imgui_tables.cpp \
          $(IMGUI_DIR)/imgui_widgets.cpp \
          $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
          $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp \
          $(SRC)/app.cpp \
          $(SRC)/window.cpp \
          $(SRC)/console.cpp \
		  $(SRC)/cheat_menu.cpp \
          $(SRC)/main_window.cpp \
          $(SRC)/demo_window.cpp \
          $(SRC)/modal_window.cpp \
          $(SRC)/main_menubar.cpp \
          $(SRC)/console_window.cpp \
          $(SRC)/statistics_window.cpp

# Объектники будем хранить в build, беря только имя файла (предполагается, что имена файлов уникальны)
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGLEW -lGL -lglfw -lSDL2 -lSDL2_image  # Add SDL2 and SDL2_image libraries

CXXFLAGS = -std=c++20 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(INCLUDE)
CXXFLAGS += -g -Wall -Wformat -fpermissive
CXXFLAGS += -DIMGUI_ENABLE_DOCKING -DIMGUI_IMPL_OPENGL_LOADER_GLAD  

# убрать варнинги и предупреждение
CXXFLAGS += -Wno-invalid-conversion

LIBS =

ifeq ($(UNAME_S), Linux)
    LIBS += $(LINUX_GL_LIBS) `pkg-config --libs glfw3`
    CXXFLAGS += -I/usr/include/GL `pkg-config --cflags glfw3`
endif

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Компиляция файлов из src
$(BUILD_DIR)/%.o: $(SRC)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Компиляция файлов из imgui (без backends)
$(BUILD_DIR)/%.o: $(IMGUI_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Компиляция файлов из imgui/backends
$(BUILD_DIR)/%.o: $(IMGUI_DIR)/backends/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

all: $(BUILD_DIR) $(EXE)
	@echo "Build complete for Linux"

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -rf $(EXE) $(BUILD_DIR)/*
