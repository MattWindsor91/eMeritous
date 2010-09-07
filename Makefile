#
#   Makefile
#
#   Copyright 2007, 2008 Lancer-X/ASCEAI
#   Copyright 2010       eMeritous contributors.
#
#   This file is part of eMeritous.
#
#   Meritous is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   eMeritous is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with eMeritous.  If not, see <http://www.gnu.org/licenses/>.
#
LDFLAGS = `sdl-config --libs` -lSDL_image -lSDL_mixer -lz
CCFLAGS = -O2 -Wall `sdl-config --cflags` -g -pedantic -DDEBUG_STATS -DNO_LOGO -ansi
CC      = clang
#
OBJS = 		src/main.o \
		src/dungeon.o \
		src/graphics.o \
		src/files.o \
		src/message.o \
		src/levelblit.o \
		src/status.o \
		src/player.o \
		src/screens.o \
		src/mapgen.o \
		src/demon.o \
		src/gamemap.o \
		src/tiles.o \
		src/save.o \
		src/help.o \
		src/audio.o \
		src/boss.o \
		src/ending.o
#
default:	emeritous

%.o:		%.c
		${CC} -c $< -o $@ ${CCFLAGS}

emeritous:	${OBJS}
		${CC} -o $@ $+ ${LDFLAGS}

clean:		
		rm ${OBJS}

