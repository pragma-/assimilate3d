VERSION=0.1.3

CC     = gcc
OSTYPE = $(shell uname -s)

CFLAGS = -W -Wall -g -I./include -I/usr/include/SDL
CLIBS  = -L/usr/lib -lSDL -lSDL_image -lm

ifeq ($(findstring CYGWIN, $(OSTYPE)), CYGWIN)
  CFLAGS += -DHAS_MULTITEXTURE -I/usr/include/GL
  CLIBS  += -lglut32 -lglu32 -lopengl32
else 
ifeq ($(OSTYPE),Linux)
  CFLAGS += -DNEEDS_STRICMP -DPOSIX -DHAS_MULTITEXTURE
  CLIBS  += -L/usr/X11R6/lib -lGL -lGLU 
else
ifeq ($(findstring MINGW, $(OSTYPE)), MINGW)
  CFLAGS += -D_GNU_SOURCE=1 -Dmain=SDL_main
  CLIBS += -lmingw32 -L/usr/lib -lSDLmain -lSDL -lglu32 -lopengl32 -lwsock32
else
  $(error Unknown OSTYPE==$(OSTYPE))
endif
endif
endif

SERVER_OBJ_FILES =	server/server.o

SHARED_OBJ_FILES =	shared/main.o shared/a3dmath.o shared/time.o shared/config.o \
			shared/cvar.o shared/bsp.o shared/frustum.o shared/entity.o \
			shared/network.o shared/sdl.o

CLIENT_OBJ_FILES =	client/gl.o client/render.o client/input.o \
			client/camera.o client/font.o \
			client/tga.o client/model.o client/lights.o \
			client/console.o \
			client/interp.o client/commands.o client/ms3d.o client/texture.o \
			client/kbnames.o \
			client/skybox.o client/gui_elements.o client/gui.o client/client.o

all: echoOS server shared client link

server: echoOS $(SERVER_OBJ_FILES)
shared: echoOS $(SHARED_OBJ_FILES)
client: echoOS $(CLIENT_OBJ_FILES)
clean:
	rm client/*.o shared/*.o server/*.o
link: echoOS
	@echo ===============================================
	@echo Linking all components
	@echo ===============================================
	$(CC) -o a3d $(SERVER_OBJ_FILES) $(SHARED_OBJ_FILES) \
		$(CLIENT_OBJ_FILES) $(CLIBS)

echoOS:
	@echo OSTYPE: $(OSTYPE)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
