/**************************************************************************
 *  screens.c                                                             *
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

#include <math.h>
#include <SDL.h>
#include <SDL_image.h>

#include "main.h"
#include "screens.h"
#include "graphics.h"
#include "levelblit.h"
#include "save.h"


void
LogoScreen(void)
{
#ifndef NO_LOGO
  SDL_Surface *asceai;

  int i;

  asceai = IMG_Load("dat/i/asceai.png");

  SDL_BlitSurface(asceai, NULL, screen, NULL);
	
  for (i = 0; i < 75; i++) 
    {
      set_logo_palette (i * 5 - 375, i * 5 - 120);
      VideoUpdate ();
      DummyEventPoll ();
      EndCycle (20);
    }

  SDL_Delay(500);

  for (i = 0; i < 50; i++)
    {
      set_logo_palette (i * 5, 255 - (i * 5));
      VideoUpdate ();
      DummyEventPoll ();
      EndCycle (20);
    }

  SDL_Delay(500);

  for (i = 0; i < 50; i++)
    {
      set_logo_palette (255, (i * 5) + 5);
      VideoUpdate ();
      DummyEventPoll ();
      EndCycle (20);
    }
#endif /* NO_LOGO */
}

void
ProgressBarScreen(int part, float progress, char *message, float t_parts)
{
  memset (screen->pixels, 0, SCREEN_W * SCREEN_H);
	
  DrawRect (200, 217, 240, 50, 80);
  DrawRect (202, 219, 236, 46, 20);
  draw_text (232, 228, message, 255);
  DrawRect (232, 244, 176, 12, 128);
  DrawRect (234, 246, 172, 8, 0);
	
  if ((int) (172.0 * progress / t_parts + (172.0 / t_parts * part)) > 0)
    {
      DrawRect (234, 246,
                (int) (172.0 * progress / t_parts + (172.0 / t_parts * part)),
                8, 200);
    }

  VideoUpdate ();
  DummyEventPoll ();
}

void
LoadingScreen(int part, float progress)
{
  float t_parts;
	
  if (game_load)
    t_parts = 5.0;
  else
    t_parts = 3.0;
	
  ProgressBarScreen (part, progress, "Loading... please wait", t_parts);
  ClearInput ();
}

void
SavingScreen(int part, float progress)
{
  ProgressBarScreen (part, progress, "Saving... please wait", 4.0);
  ClearInput ();
}

int
main_menu(void)
{ 
  SDL_Surface *title, *title_pr;
  int precalc_sine[400];
  int pulse[SCREEN_W * SCREEN_H];
  int i, x, y = 0;
  int light = 0;
  int on_title = 1;
  int can_continue = 0;
  int last_key = 0;
  int option = 0;
  int ticker_tick = 0;
  int maxoptions = 0;
  int tick = 10000000;
  Uint8 *src_p, *col_p;

  for (i = 0; i < screen->w * screen->h; i++)
    {
      x = i % SCREEN_W;
      y = i / SCREEN_W;

      pulse[i] = dist(x, y, SCREEN_W / 2, SCREEN_H / 2);
    }

  for (i = 0; i < 400; i++)
    precalc_sine[i] = sin ((float) i / 400 * M_PI * 2) * 24 + 24;
  
  TitleScreenMusic();
		
  if (IsSaveFile())
    can_continue = 1;
  else
    can_continue = 0;
		
  maxoptions = 3 + can_continue;
	
  title = IMG_Load ("dat/i/title.png");
  title_pr = IMG_Load ("dat/i/title.png");
		
  while (on_title)
    {
      set_title_palette(ticker_tick);
      col_p = (Uint8 *)title_pr->pixels;
      src_p = (Uint8 *)title->pixels;

      if ((tick % 10) == 0)
        {
          for (i = 0; i < SCREEN_W * SCREEN_H; i++, col_p++, src_p++)
            {
              *col_p = Uint8_Bound (*src_p + precalc_sine[(pulse[i] + tick) % 400]);
            }
        }

      SDL_BlitSurface (title_pr, NULL, screen, NULL);
			
      draw_text (17, 156, MERITOUS_VERSION, 225 + sin((float) ticker_tick / 15) * 30);

      if (can_continue) 
        draw_text ((SCREEN_W - 14 * 8) / 2, 310, "Continue", 255);

      draw_text ((SCREEN_W - 14 * 8) / 2, 310 + can_continue * 10, 
                 "New Game", 255);
      draw_text ((SCREEN_W - 14 * 8) / 2, 320 + can_continue * 10,
                 "New Game (Wuss mode)", 255);
      draw_text ((SCREEN_W - 14 * 8) / 2, 330 + can_continue * 10,
                 "Help/Licence", 255);
		
      if (ticker_tick >= 30)
        {
          draw_text ((SCREEN_W - 14 * 8) / 2 - 17, 310 + option * 10, "-",
                     205 + sin ((float) ticker_tick / 5.0) * 24);
          draw_text ((SCREEN_W - 14 * 8) / 2 - 20, 310 + option * 10, " >",
                     205 + sin ((float) ticker_tick / 5.0) * 24);
          draw_text ((SCREEN_W - 14 * 8) / 2 - 19, 310 + option * 10, " >",
                     190 + sin ((float) ticker_tick / 5.0) * 24);
          draw_text ((SCREEN_W - 14 * 8) / 2 - 21, 310 + option * 10, " >",
                     190 + sin ((float) ticker_tick / 5.0) * 24);
          draw_text ((SCREEN_W - 14 * 8) / 2 - 18, 310 + option * 10, " >",
                     165 + sin ((float) ticker_tick / 5.0) * 24);
          draw_text ((SCREEN_W - 14 * 8) / 2 - 22, 310 + option * 10, " >",
                     165 + sin ((float) ticker_tick / 5.0) * 24);
      }
	
      VideoUpdate();
       
      ticker_tick++; 

      if (ticker_tick > 30)
        {
          HandleEvents ();

          if (key_held[K_UP])
            {
              if (last_key != 1)
                {
                  if (option > 0)
                    option--;

                  last_key = 1;
                }
            }
          else if (key_held[K_DN])
            {
              if (last_key != 2)
                {
                  if (option < maxoptions-1)
                    option++;
                }

              last_key = 2;
            } 
          else
            {
              last_key = 0;

              if (key_held[K_SP] || enter_pressed)
                {
                  on_title = 0;
                }
            }
        }
      
      if (voluntary_exit)
        {
          /*return -1;*/
          on_title = 0;
          SDL_FreeSurface (title);
          SDL_FreeSurface (title_pr);
          SDL_Quit ();
          exit (0);
          return -1;
        }
			
      EndCycle (10);
	
      light = 0;
      tick -= 2;
    }

  SDL_FreeSurface (title);
  SDL_FreeSurface (title_pr);
  return option;
}

void
set_logo_palette (int curve_start, int curve_end)
{
  SDL_Color pal[256];
  int ec;
  int i;
	
  for (i = 0; i < 256; i++) {
    ec = (i - curve_start) * 255 / (curve_end-curve_start);
    if (ec < 0) ec = 0;
    if (ec > 255) ec = 255;
		
    pal[i].r = ec;
    pal[i].g = ec;
    pal[i].b = ec;
  }
	
  SDL_SetPalette(screen, SDL_PHYSPAL, pal, 0, 256);
}

void
set_title_palette (int t)
{
  SDL_Color pal[256];
  int i;
	
  float ip;
  float bright;
  float b_coeff;
	
  bright = 1 - ((float)t / 30.0);
  if (bright < 0.0) bright = 0.0;
  b_coeff = 1 - bright;
	
  for (i = 0; i < 256; i++) {
    ip = (float)i / 255.0;
    pal[i].r = (cos(ip * M_PI / 2.0 + M_PI) + 1.0) * 255 * b_coeff + 255*bright;
    pal[i].g = (sin(ip * M_PI / 2.0) * 255 + i) / 2 * b_coeff + 255*bright;
    pal[i].b = sin(ip * M_PI / 2.0) * 255 * b_coeff + 255*bright;
  }
	
  SDL_SetPalette(screen, SDL_PHYSPAL, pal, 0, 256);
}
