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

#include "dungeon.h"
#include "graphics.h"
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

const char MERITOUS_VERSION[] = "v 1.2a";


int RECORDING = 0;
int PLAYBACK = 0;
FILE *record_file;
char record_filename[256];
int game_running = 1;

int main(int argc, const char **argv)
{
  int executable_running = 1;

  SDL_Surface *wm_icon;

  Uint8 wm_mask[128];
  int i;

  int x, y;



  int option = 0;
  int can_continue = 0;


	

	
  int fullscreen = 0;

  unsigned int stime = 0;
	
  FILE *wm_mask_file;

  if (argc > 1)
    {

    for (i = 1; i < argc; i++)
      {

      if (!strcasecmp (argv[i], "fullscreen"))
        fullscreen = 1;

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

  if ((RECORDING) && (PLAYBACK))
    exit (1);

  srand (time (NULL));

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
	
  screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 8, SDL_SWSURFACE
                            | (SDL_FULLSCREEN * fullscreen));
	
  wm_mask_file = fopen("dat/d/icon_bitmask.dat", "rb");
  fread(wm_mask, 1, 128, wm_mask_file);
  fclose(wm_mask_file);
  SDL_WM_SetCaption("~ m e r i t o u s ~", "MT");
  SDL_WM_SetIcon(wm_icon, wm_mask);
  InitAudio();
	
  text_init();
	       

  SetGreyscalePalette();

  LogoScreen();
	
  while (executable_running) {
    option = main_menu();

    if (IsSaveFile())
      {
        can_continue = 1;
      } 
    else
      {
        can_continue = 0;
      }
		
    ClearInput();
		
    if (executable_running == 1) {

      if ((option == 0) && can_continue) {
        play_dungeon("SaveFile.sav");
      } else {
        if (option == (0 + can_continue)) {
          training = 0;
          play_dungeon("");
        } else {
          training = 1;
          play_dungeon("");
        }
      }
      /* clean up */
      ClearInput();
      DestroyDungeon();
      DestroyThings();
      /*on_title = 1;*/
      game_load = 0;
			
      game_running = 1;
    }
  }

  /*	if (argc >= 2) DungeonPlay(argv[1]);
  	else DungeonPlay(""); */

  SDL_Quit();
  return 0;
}
