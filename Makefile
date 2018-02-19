OS:=$(shell uname)

CXX = g++

ifeq ($(OS), Darwin)
    CXXFLAGS = -Iext/gl3w -std=c++11 -Wall -pedantic -DENABLE_SOUND
    LIB = -ldl -framework OpenGL -framework Cocoa -framework CoreFoundation -lglfw -lSDL -lSDL_mixer -lpthread -lSDLmain
else ifeq ($(OS), Linux)
    CXXFLAGS = -Iext/gl3w -std=c++11 -Wall -pedantic -DENABLE_SOUND -D LINUX
    LIB = -lglfw -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lSDL2 -lSDL2_mixer
else
    $(error Your OS $(OS) is not supported.) 
    exit 1
endif

BIN=a1

OBJ=$(BIN).o common.o fish.o player.o turtle.o world.o bullet.o background.o OsuParser.o BeatList.o

SUBDIRS = src/utils

default: build
	
build: $(BIN)
	
.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

test: build
	./$(BIN)

%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

$(BIN): $(SUBDIRS) $(OBJ)
	$(CXX) -o $@ $(OBJ) $(wildcard $(addsuffix /*.o,$(SUBDIRS))) $(LIB)

clean:
	- rm -f $(BIN) $(OBJ)
