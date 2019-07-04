UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
PKG_CONFIG_PATH=$$(pkg-config allegro-5 allegro_audio-5 allegro_acodec-5 allegro_font-5 allegro_ttf-5 allegro_image-5 --libs --cflags)

all:
	gcc ./src/bombcraft.c -o ./bin/bombcraft $(PKG_CONFIG_PATH) -lallegro -lallegro_main

run:
	./bin/bombcraft
endif

ifeq ($(UNAME), Windows)
ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: bombcraft.exe

bombcraft.exe: bombcraft.o
	gcc -o \bin\bombcraft.exe bombcraft.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)
	
bombcraft.o: bombcraft.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c \src\bombcraft.c

clean:
	del bombcraft.o
	del bombcraft.exe
endif