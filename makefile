# Declaration of variables
CC = g++
CC_FLAGS = -w -IHeaders/ -Ilibraries/trimesh/include/ -I/usr/include/GraphicsMagick
LIBS := -lglfw -lIlmImf -lXrender -lpthread -ldrm -lGLEW -lrt -lXrandr -lXi -lGL -lm -lXdamage -lX11-xcb -lxcb-glx -ldl -lX11 -lXxf86vm -fopenmp -lGraphicsMagick++
USER_OBJS := libraries/trimesh/lib.Linux64/libtrimesh.a


# File Names
EXEC = run
SOURCES = $(wildcard Source/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main Target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(USER_OBJS) $(LIBS)

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
