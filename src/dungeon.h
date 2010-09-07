/**************************************************************************
 *  dungeon.h                                                             *
 *                                                                        *
 *  Copyright 2007, 2008 Lancer-X/ASCEAI                                  *
 *  Copyright 2010       CaptainHayashi etc.                              *
 *                                                                        *
 *  This file is part of eMeritous.                                       *
 *                                                                        *
 *  eMeritous is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  eMeritous is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with eMeritous.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                        *
 **************************************************************************/

#ifndef __DUNGEON_H__
#define __DUNGEON_H__

enum {
  /* Room types */
  ROOM_BOSS = 2
};

#define MIN(x, y) (((x) < (y) ? (x) : (y)))

/* Global variables (get rid!) */

extern int opening_door_x, opening_door_y;
extern int opening_door_i, opening_door_n;
extern int release_x, release_y;
extern int release_str;

void lock_doors(int r);

int play_dungeon(char *fname);

#endif /* __DUNGEON_H__ */
