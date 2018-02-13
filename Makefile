OS:=$(shell uname)

CXX = g++

ifeq ($(OS), Darwin)
    CXXFLAGS = -Iext/gl3w -std=c++11 -Wall -pedantic -DENABLE_SOUND
    LIB = -ldl -framework OpenGL -framework Cocoa -framework CoreFoundation -lglfw -lSDL -lSDL_mixer -lpthread -lSDLmain
else ifeq ($(OS), Linux)
    CXXFLAGS = -Iext/gl3w -Iext/sdl -std=c++11 -Wall -pedantic -DENABLE_SOUND
    LIB = -lglfw3 -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lSDL2 -lSDL2_mixer
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
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(BIN): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LIB)

clean:
	- rm -f $(BIN) $(OBJ)
