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
#include "levelblit.h"

/* Graphics globals. */

SDL_Surface *screen; /**< The main video screen. */

/* Graphics statics. */

static unsigned char font_data[NUM_CHARS][CHAR_W][CHAR_H]; /**< The font bitmap. */
static unsigned char dirty_list[22][17]; /** List of tiles to update. */
static unsigned char prev_dirty_list[22][17]; /** List of tiles updated last frame. */

void
gfx_init (void)
{
  memset (dirty_list, 0, 22 * 17);
  memset (prev_dirty_list, 1, 22 * 17);
  scroll_x = 0;
  scroll_y = 0;
}

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

void
mark_dirty_rect (int rect_x, int rect_y, int rect_w, int rect_h)
{
  int start_x, start_y;
  int end_x, end_y;
  int x, y;

  /* De-translate x and y. */

  start_x = ((rect_x + (scroll_x % TILE_W))
             / TILE_W);
  start_y = ((rect_y + (scroll_y % TILE_H))
             / TILE_H);

  end_x = ((rect_x + rect_w + (scroll_x % TILE_W))
           / TILE_W);
  end_y = ((rect_y + rect_h + (scroll_y % TILE_H))
           / TILE_H);

  /* Bound the X and Y. */

  if (start_x < 0)
    start_x = 0;

  if (start_y < 0)
    start_y = 0;

  if (start_x > 21)
    start_x = 21;
 
  if (start_y > 16)
    start_y = 16;

  if (end_x < 0)
    end_x = 0;

  if (start_y < 0)
    end_y = 0;

  if (start_x > 21)
    end_x = 21;
 
  if (start_y > 16)
    end_y = 16;

  /* Check to see if tiles are in dirty list. */
 

  for (x = start_x; x <= end_x; x++)
    {
      for (y = start_y; y <= end_y; y++)
        {
          if (dirty_list[x][y] == 0)
            {
              dirty_list[x][y] = 1;
              DrawTile(x, y, scroll_x, scroll_y, 1, 1);
            }
        }
    }
}

void
dirty_blit (SDL_Surface *source, SDL_Rect *sourcerect, 
            SDL_Surface *destination, SDL_Rect *destinationrect)
{
  mark_dirty_rect (destinationrect->x, 
                   destinationrect->y,
                   destinationrect->w,
                   destinationrect->h);

  SDL_BlitSurface(source, sourcerect, destination, destinationrect);
}

int
was_dirty (int x, int y)
{
  return prev_dirty_list[x][y];
}

void
clear_dirty_list (void)
{
  memcpy (prev_dirty_list, dirty_list, sizeof (dirty_list));
  memset (dirty_list, 0, sizeof (dirty_list));
}
