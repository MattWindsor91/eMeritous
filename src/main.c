/**************************************************************************
 *  main.c                                                                *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <assert.h>

#include "player.h"
#include "levelblit.h"
#include "mapgen.h"
#include "demon.h"
#include "gamemap.h"
#include "tiles.h"
#include "save.h"
#include "help.h"
#include "audio.h"
#include "boss.h"
#include "ending.h"
#include "screens.h"

#define MERITOUS_VERSION "v 1.2a"

int RECORDING = 0;
int PLAYBACK = 0;
FILE *record_file;
char record_filename[256];
int ticker_tick = 0;
int game_running = 1;

int main(int argc, const char **argv)
{
  int on_title = 1;
  int executable_running = 1;
  SDL_Surface *title, *title_pr;
  SDL_Surface *wm_icon;
  Uint8 *src_p, *col_p;
  Uint8 wm_mask[128];
  int i;
  int light = 0;
  int x, y;
  int pulse[SCREEN_W * SCREEN_H];
  int precalc_sine[400];
  int tick = 10000000;
  int option = 0;
  int can_continue = 0;
  int maxoptions;
	
  int last_key = 0;
	
  int fullscreen = 0;

  unsigned int stime = 0;
	
  FILE *wm_mask_file;

  if (argc > 1) {
    for (i = 1; i < argc; i++) {
      if (!strcasecmp(argv[i], "fullscreen")) {
        fullscreen = 1;
      }
      /*			if (!strcasecmp(argv[i], "record")) {
				RECORDING = 1;
				strcpy(record_filename, argv[i+1]);
                                }
                                if (!strcasecmp(argv[i], "play")) {
				PLAYBACK = 1;
				strcpy(record_filename, argv[i+1]);
                                }
                                if (!strcasecmp(argv[i], "framedelay")) {
				frame_len = atoi(argv[i+1]);
                                }
                                if (!strcasecmp(argv[i], "bmpwrite")) {
				WriteBitmaps = 1;
                                }
                                if (!strcasecmp(argv[i], "bmpstart")) {
				WB_StartRange = atoi(argv[i+1]);
                                }
                                if (!strcasecmp(argv[i], "bmpend")) {
				WB_EndRange = atoi(argv[i+1]);
                                } */
    }
  }

  if ((RECORDING) && (PLAYBACK)) {
    exit(1);
  }
  srand(time(NULL));
  if (RECORDING) {
    record_file = fopen(record_filename, "wb");
    stime = time(NULL);
		
    fputc(stime & 0x000000FF, record_file);
    fputc((stime & 0x0000FF00) >> 8, record_file);
    fputc((stime & 0x00FF0000) >> 16, record_file);
    fputc((stime & 0xFF000000) >> 24, record_file);
		
    srand(stime);
  }
  if (PLAYBACK) {
    record_file = fopen(record_filename, "rb");
    stime = fgetc(record_file);
    stime |= fgetc(record_file) << 8;
    stime |= fgetc(record_file) << 16;
    stime |= fgetc(record_file) << 24;
		
    srand(stime);
  }
	
  wm_icon = IMG_Load("dat/i/icon.png");
	
  screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 8, SDL_SWSURFACE | (SDL_FULLSCREEN * fullscreen));
	
  wm_mask_file = fopen("dat/d/icon_bitmask.dat", "rb");
  fread(wm_mask, 1, 128, wm_mask_file);
  fclose(wm_mask_file);
  SDL_WM_SetCaption("~ m e r i t o u s ~", "MT");
  SDL_WM_SetIcon(wm_icon, wm_mask);
  InitAudio();
	
  text_init();
  for (i = 0; i < 400; i++) {
    precalc_sine[i] = sin((float)i / 400 * M_PI * 2)*24+24;
  }
	
		
  for (i = 0; i < screen->w * screen->h; i++) {
    x = i % SCREEN_W;
    y = i / SCREEN_W;

    pulse[i] = dist(x, y, SCREEN_W / 2, SCREEN_H / 2);
  }
  SetGreyscalePalette();

  LogoScreen();
	
  while (executable_running) {
    ticker_tick = 0;
    TitleScreenMusic();
		
    if (IsSaveFile()) {
      can_continue = 1;
    } else {
      can_continue = 0;
    }
		
    maxoptions = 2 + can_continue;
	
    title = IMG_Load("dat/i/title.png");
    title_pr = IMG_Load("dat/i/title.png");
		
    while (on_title) {
      SetTitlePalette2(ticker_tick);
      col_p = (Uint8 *)title_pr->pixels;
      src_p = (Uint8 *)title->pixels;
      if ((tick % 10) == 0) {
        for (i = 0; i < 640*480; i++) {
          *(col_p++) = Uint8_Bound(*(src_p++)+precalc_sine[(pulse[i]+tick)%400]);
        }
      }
      SDL_BlitSurface(title_pr, NULL, screen, NULL);
			
      draw_text(17, 156, MERITOUS_VERSION, 225 + sin((float)ticker_tick / 15)*30);
      if (can_continue) draw_text((SCREEN_W - 14*8)/2, 310, "Continue", 255);
      draw_text((SCREEN_W - 14*8)/2, 310 + can_continue*10, "New Game", 255);
      draw_text((SCREEN_W - 14*8)/2, 320 + can_continue*10, "New Game (Wuss mode)", 255);
			
      if (ticker_tick >= 30) {
        draw_text((SCREEN_W - 14*8)/2 - 17, 310 + option * 10, "-", 205 + sin((float)ticker_tick / 5.0)*24);
        draw_text((SCREEN_W - 14*8)/2 - 20, 310 + option * 10, " >", 205 + sin((float)ticker_tick / 5.0)*24);
        draw_text((SCREEN_W - 14*8)/2 - 19, 310 + option * 10, " >", 190 + sin((float)ticker_tick / 5.0)*24);
        draw_text((SCREEN_W - 14*8)/2 - 21, 310 + option * 10, " >", 190 + sin((float)ticker_tick / 5.0)*24);
        draw_text((SCREEN_W - 14*8)/2 - 18, 310 + option * 10, " >", 165 + sin((float)ticker_tick / 5.0)*24);
        draw_text((SCREEN_W - 14*8)/2 - 22, 310 + option * 10, " >", 165 + sin((float)ticker_tick / 5.0)*24);
      }
	
      VideoUpdate();
       
      ticker_tick++; 

      if (ticker_tick > 30) {
        HandleEvents();
		
        if (key_held[K_UP]) {
          if (last_key != 1)
            if (option > 0) option--;
          last_key = 1;
        } else {
          if (key_held[K_DN]) {
            if (last_key != 2)
              if (option < maxoptions-1) option++;
            last_key = 2;
          } else {
            last_key = 0;
            if (key_held[K_SP] || enter_pressed) {
              on_title = 0;
            }
          }
        }
				
        if (voluntary_exit) {
          executable_running = 0;
          on_title = 0;
          SDL_Quit();
          exit(0);
        }
      }
			
      EndCycle(10);
	
      light = 0;
      tick -= 2;
    }
		
    ClearInput();
		
    if (executable_running == 1) {
      SDL_FreeSurface(title);
      SDL_FreeSurface(title_pr);
      if ((option == 0) && can_continue) {
        DungeonPlay("SaveFile.sav");
      } else {
        if (option == (0 + can_continue)) {
          training = 0;
          DungeonPlay("");
        } else {
          training = 1;
          DungeonPlay("");
        }
      }
      /* clean up */
      ClearInput();
      DestroyDungeon();
      DestroyThings();
      on_title = 1;
      game_load = 0;
			
      game_running = 1;
    }
  }

  /*	if (argc >= 2) DungeonPlay(argv[1]);
  	else DungeonPlay(""); */

  SDL_Quit();
  return 0;
}
