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

enum {
  NUM_CHARS = 128, /**< Number of characters to store. */
  CHAR_W    = 8,   /**< Width of one text character. */
  CHAR_H    = 8,   /**< Height of one text character. */
  CHAR_P    = 2    /**< Padding between lines of text. */
};

/** Initialise the text font. */
void text_init(void);


/** Draw a character onto the screen.
 *
 *  @param cur_x  X co-ordinate to start the draw from.
 *  @param cur_y  Y co-ordinate to start the draw from.
 *  @param c      The character to draw.
 *  @param tcol   The colour of the character (8-bit greyscale).
 */
void draw_char(int cur_x, int cur_y, int c, Uint8 tcol);


/** Draw a text string onto the screen.
 *
 *  The text string can include newlines, which will be properly 
 *  formatted.
 *
 *  @param x     X co-ordinate to start the draw from.
 *  @param y     Y co-ordinate to start the draw from.
 *  @param str   The string to draw onto the screen.
 *  @param tcol  The colour of the character (8-bit greyscale).
 */
void draw_text(int x, int y, const char *str, Uint8 tcol);


/** Draw text onto a surface. (?)
 *
 *  @param x     X co-ordinate to start the draw from.
 *  @param y     Y co-ordinate to start the draw from.
 *  @param str   The string to draw onto the screen.
 *  @param tcol  The colour of the character (8-bit greyscale).
 *  @param srf   The surface to write the string to.
 */

void draw_text_ex(int x, int y, const char *str, Uint8 tcol, SDL_Surface *srf);


#endif /* __GRAPHICS_H__ */
