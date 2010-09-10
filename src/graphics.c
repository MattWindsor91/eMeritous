/**************************************************************************
 *  graphics.c                                                            *
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

#include <stdlib.h>

#include "graphics.h"

/* Graphics globals. */

SDL_Surface *screen; /**< The main video screen. */

/* Graphics statics. */

static unsigned char font_data[NUM_CHARS][CHAR_W][CHAR_H]; /**< The font bitmap. */

/* Initialise the text font. */
void
text_init (void)
{
  FILE *font_data_file;
  int chr, x, y;
  font_data_file = fopen ("dat/d/font.dat", "rb");
	
  for (chr = 0; chr < NUM_CHARS; chr++)
    {
      for (y = 0; y < CHAR_H; y++)
        {
          for (x = 0; x < CHAR_W; x++)
            font_data[chr][x][y] = fgetc (font_data_file);
        }
    }

  fclose (font_data_file);
}

/* Draw a character onto the screen. */
void
draw_char (int x, int y, int c, Uint8 tcol)
{
  int px, py;
  Uint8 *pix;
	
  for (py = 0; py < CHAR_H; py++)
    {
      pix = (Uint8 *) screen->pixels;
      pix += (py + y) * screen->w;
      pix += x;

      if (x >= 0
          && py + y >= 0
          && x < screen->w - CHAR_W
          && py + y < screen->h)
        {
          for (px = 0; px < CHAR_W; px++)
            {
              if (font_data[c][px][py] == 255)
                *pix = tcol; 
              else if (font_data[c][px][py] < 255
                       && font_data[c][px][py] > 0)
                *pix = (((int) tcol * font_data[c][px][py] / 256)
                        + ((int) * pix * (256-font_data[c][px][py]) / 256));

              pix++;
            }
        }
    }
}

/* Draw a text string onto the screen. */
void
draw_text (int x, int y, const char *str, Uint8 tcol)
{
  int c, cur_x, cur_y;
	
  cur_x = x;
  cur_y = y;

  for (c = *str; c != '\0'; str++, c = *str)
    {
      /* Newline handling. */
      if (c == '\n')
        {
          cur_x = x;
          cur_y += CHAR_H + CHAR_P;
        }
      else
        {
          draw_char (cur_x, cur_y, c, tcol);
          cur_x += CHAR_W;
        }
    }
}


/* Variant text drawing function used for map dumps. */
void
draw_map_text (int x, int y, const char *str, Uint8 tcol,
               SDL_Surface *surface)
{
  Uint8 *pix;
  int c, cur_x, cur_y, px, py;
	
  cur_x = x;
  cur_y = y;

  for (c = *str; c != '\0'; str++, c = *str)
    {
      /* Newline handling. */
      if (c == '\n')
        {
          cur_x = x;
          cur_y += CHAR_H; /* Shouldn't this be + CHAR_P as well? */
        }
      else
        {
          for (py = 0; py < CHAR_H; py++)
            {
              pix = (Uint8 *) surface->pixels;
              pix += (py + cur_y) * surface->w;
              pix += cur_x;

              for (px = 0; px < CHAR_W; px++)
                {
                  if (font_data[c][px][py])
                    {
                      *pix = tcol;
                    }
                  pix++;
                }
            }
          cur_x += CHAR_H;
        }
    }
}
