ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: bombcraft.exe

bombcraft.exe: bombcraft.o
	gcc -o bombcraft.exe bombcraft.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)
	
bombcraft.o: bombcraft.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c bombcraft.c


clean:
	del bombcraft.o
	del bombcraft.exe