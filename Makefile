OS:=$(shell uname)
CPUS ?= $(shell getconf _NPROCESSORS_ONLN)
MAKEFLAGS += --jobs=$(CPUS)
CXX = g++

ifeq ($(OS), Darwin)
    CXXFLAGS = -Iext/gl3w -std=c++11 -Wall -pedantic -DENABLE_SOUND -g
    LIB = -ldl -framework OpenGL -framework Cocoa -framework CoreFoundation -lglfw -lSDL -lSDL_mixer -lpthread -lSDLmain
else ifeq ($(OS), Linux)
    CXXFLAGS = -Iext/gl3w -std=c++11 -Wall -pedantic -DENABLE_SOUND -D LINUX -g
    LIB = -lglfw -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lSDL2 -lSDL2_mixer
else
    $(error Your OS $(OS) is not supported.) 
    exit 1
endif

BIN=beatcoin
OBJ=$(BIN).o common.o Sprite.o Player.o Turtle.o World.o Bullet.o Background.o BeatCircle.o BeatList.o OsuParser.o CenterBeatCircle.o Level.o HealthBar.o LevelList.o Boss.o Enemy.o SpriteSheet.o BossHealthBar.o Structure.o


SUBDIRS = src/utils

default:; @$(MAKE) _all $(MAKEFLAGS)

_all: build

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
