OS:=$(shell uname)

ifeq ($(OS), Darwin)
		CXXFLAGS = -Iext/gl3w -std=c++11 -Wall -pedantic -DENABLE_SOUND
		LIB = -ldl -framework OpenGL -framework Cocoa -framework CoreFoundation -lglfw -lSDL -lSDL_mixer -lpthread -lSDLmain
else ifeq ($(OS), Linux)
		CXXFLAGS = -Iext/gl3w -std=c++11 -Wall -pedantic -DENABLE_SOUND
		LIB = -lglfw -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lSDL -lSDL_mixer -lSDLmain
else
    $(error Your OS $(OS) is not supported.) 
    exit 1
endif

OBJ = StringUtils.o

SUBDIRS =

default: build
	
build: $(OBJ)

%.o: %.cpp
	g++ -c $(CXXFLAGS) -o $@ $<

.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)
$(SUBDIRS): 
	$(MAKE) -C $@

clean:
	- rm -f $(OBJ)
