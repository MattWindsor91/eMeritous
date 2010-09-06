/**************************************************************************
 *  graphics.h                                                            *
 *                                                                        *
 *  Copyright 2007, 2008 Lancer-X/ASCEAI                                  *
 *  Copyright 2010       CaptainHayashi etc.                              *
 *                                                                        *
 *  This file is part of Meritous.                                        *
 *                                                                        *
 *  Meritous is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  Meritous is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with Meritous.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                        *
 **************************************************************************/

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <SDL.h>

extern SDL_Surface *screen; /* TODO: move to graphics */

/** Initialise the text font. */
void text_init(void);

/** Draw a character on screen.
 *
 */
void draw_char(int cur_x, int cur_y, int c, Uint8 tcol);
void draw_text(int x, int y, const char *str, Uint8 tcol);
void draw_text_ex(int x, int y, const char *str, Uint8 tcol, SDL_Surface *srf);

#endif /* __GRAPHICS_H__ */
