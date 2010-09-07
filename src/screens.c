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

#include "screens.h"
#include "graphics.h"
#include "levelblit.h"
#include "save.h"

#include <SDL.h>
#include <SDL_image.h>


void LogoScreen(void)
{
#ifndef NO_LOGO
  SDL_Surface *asceai;

  int i;

  asceai = IMG_Load("dat/i/asceai.png");

  SDL_BlitSurface(asceai, NULL, screen, NULL);
	
  for (i = 0; i < 75; i++) {
    SetTitlePalette(i * 5 - 375, i * 5 - 120);
    VideoUpdate();
    DummyEventPoll();
    EndCycle(20);
  }
  SDL_Delay(500);
  for (i = 0; i < 50; i++) {
    SetTitlePalette(i * 5, 255 - (i * 5));
    VideoUpdate();
    DummyEventPoll();
    EndCycle(20);
  }
  SDL_Delay(500);
  for (i = 0; i < 50; i++) {
    SetTitlePalette(255, (i * 5)+5);
    VideoUpdate();
    DummyEventPoll();
    EndCycle(20);
  }
#endif /* NO_LOGO */
}

void ProgressBarScreen(int part, float progress, char *message, float t_parts)
{
  memset(screen->pixels, 0, SCREEN_W * SCREEN_H);
	
  DrawRect(200, 217, 240, 50, 80);
  DrawRect(202, 219, 236, 46, 20);
  draw_text(232, 228, message, 255);
  DrawRect(232, 244, 176, 12, 128);
  DrawRect(234, 246, 172, 8, 0);
	
  if ((int)(172.0 * progress / t_parts + (172.0 / t_parts * part)) > 0) {
    DrawRect(234, 246, (int)(172.0 * progress / t_parts + (172.0 / t_parts * part)), 8, 200);
  }
  VideoUpdate();
  DummyEventPoll();
}

void LoadingScreen(int part, float progress)
{
  float t_parts;
	
  if (game_load) t_parts = 5.0;
  else t_parts = 3.0;
	
  ProgressBarScreen(part, progress, "Loading... please wait", t_parts);
  ClearInput();
}

void SavingScreen(int part, float progress)
{
  ProgressBarScreen(part, progress, "Saving... please wait", 4.0);
  ClearInput();
}
