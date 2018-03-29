OS:=$(shell uname)
CPUS ?= $(shell getconf _NPROCESSORS_ONLN)
MAKEFLAGS += --jobs=$(CPUS)
CXX = g++

ifeq ($(OS), Darwin)
	LIB = -ldl -framework OpenGL -framework Cocoa -framework CoreFoundation -lglfw -lSDL -lSDL_mixer -lpthread -lSDLmain -lfmod -lfmodL -lfreetype
	CXXFLAGS = -Iext/gl3w -Iext/tinydir -Iext/fmod/inc -Iext/freetype/include -std=c++11 -Wall -pedantic -DENABLE_SOUND -g
	LIBPATH = -Lext/fmod/lib
	LDFLAGS = "-Wl,-rpath,./ext/fmod/lib"
else ifeq ($(OS), Linux)
	CXXFLAGS = -Iext/gl3w -Iext/tinydir -Iext/fmod-lin/inc -Iext/freetype/include -std=c++11 -Wall -pedantic -DENABLE_SOUND -D LINUX -g
	LIB = -lglfw -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lSDL2 -lSDL2_mixer -lfmod -lfmodL -lfreetype
	LIBPATH = -Lext/fmod-lin/lib/x86_64
	LDFLAGS = "-Wl,-rpath,./ext/fmod-lin/lib/x86_64"
else
    $(error Your OS $(OS) is not supported.) 
    exit 1
endif

BIN=beatcoin
OBJ=$(BIN).o common.o Sprite.o Player.o World.o Bullet.o Background.o
OBJ2=BeatCircle.o BeatList.o OsuParser.o CenterBeatCircle.o Level.o
OBJ3=HealthBar.o LevelList.o Boss.o Enemy.o SpriteSheet.o BossHealthBar.o
OBJ4=Spawner.o TextureManager.o Hitbox.o Particle.o ParticleEmitter.o
OBJ5=Structure.o AudioEngine.o TextRenderer.o MainMenu.o Button.o SongSelect.o
OBJ6=CustomMenu.o CustomLocalMenu.o
ALL_OBJ=$(OBJ) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6)
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

$(BIN): $(SUBDIRS) $(ALL_OBJ) 
	$(CXX)  $(LDFLAGS) -o $@ $(ALL_OBJ) $(wildcard $(addsuffix /*.o,$(SUBDIRS))) $(LIBPATH) $(LIB)

clean:
	- rm -f $(BIN) $(ALL_OBJ)
