OS:=$(shell uname)

ifeq ($(OS), Darwin)
    CXXFLAGS = -Iext/gl3w -std=c++11 -Wall -pedantic -DENABLE_SOUND
    LIB = -ldl -framework OpenGL -framework Cocoa -framework CoreFoundation -lglfw -lSDL -lSDL_mixer -lpthread -lSDLmain
else ifeq ($(OS), Linux)
    CXXFLAGS = -Iext/gl3w -Iext/glfw/include -Iext/sdl/include -Iext/stb_image -std=c++11 -Wall -pedantic -DENABLE_SOUND
    # LIB = -lglfw -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lSDL -lSDL_mixer -lSDLmain
    LIB = -lglfw -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -Lext/sdl/lib -lSDL -lSDL_mixer -lSDLmain
else
    $(error Your OS $(OS) is not supported.) 
    exit 1
endif

BIN=a1
OBJ=$(BIN).o common.o fish.o player.o turtle.o world.o

default: build
	
build: $(BIN)

test: build
	./$(BIN)

%.o: src/%.cpp
	g++ -c $(CXXFLAGS) -o $@ $<

$(BIN): $(OBJ)
	g++ -o $@ $(OBJ) $(LIB)

clean:
	- rm -f $(BIN) $(OBJ)
