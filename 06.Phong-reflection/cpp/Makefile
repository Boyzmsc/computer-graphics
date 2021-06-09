IMGUI_DIR = imGui/

HEADERS = 
SOURCES = main.cpp Camera.cpp Mesh.cpp Model.cpp 
SOURCES += $(IMGUI_DIR)imgui_draw.cpp $(IMGUI_DIR)imgui_impl_glfw.cpp $(IMGUI_DIR)imgui_impl_opengl3.cpp $(IMGUI_DIR)imgui_widgets.cpp $(IMGUI_DIR)imgui.cpp
SOURCES += $(IMGUI_DIR)imGuIZMO/imGuIZMOquat.cpp
CC = g++
CFLAGS = -std=c++11
LDFLAGS = -lGL -lGLEW -lglfw -lassimp
EXECUTABLE = phong
RM = rm -rf

all: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES) $(LDFLAGS)

clean:
	$(RM) *.o $(EXECUTABLE)

