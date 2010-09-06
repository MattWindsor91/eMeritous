/**************************************************************************
 *  levelblit.h                                                           *
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

#ifndef __LEVELBLIT_H__
#define __LEVELBLIT_H__

#include <SDL.h>
#include <SDL_image.h>

enum {
  SCREEN_W = 640, /**< Screen width. */
  SCREEN_H = 480, /**< Screen height. */

  TILE_W = 32,    /**< Width of one tile. */
  TILE_H = 32,    /**< Height of one tile. */

  PLAYER_W = 16,  /**< Width of player sprite. */
  PLAYER_H = 24,  /**< Height of player sprite. */

  BG_COLOUR = 2,  /**< Colour (8-bit greyscale) of the background. */

  K_UP = 0,       /**< Up key. */
  K_DN,           /**< Down key. */
  K_LT,           /**< Left key. */
  K_RT,           /**< Right key. */
  K_SP            /**< Spacebar key. */
};

/* Global variables for graphics. (TODO: re-organise?) */

extern SDL_Surface *screen;
extern SDL_Surface *artifact_spr;

extern int player_x, player_y;
extern int scroll_x, scroll_y;
extern int player_room;
extern int prv_player_room;
extern int magic_circuit;
extern int circuit_size;
extern int circuit_range;

extern int shield_hp;
extern int shield_recover;
extern int enter_room_x, enter_room_y;

extern int player_dying;

extern int checkpoint_x;
extern int checkpoint_y;

extern int specialmessage;
extern int specialmessagetimer;

extern int tele_select;

extern int artifacts[];

extern int enter_pressed;

extern int game_paused;

extern int key_held[];

extern int training;
extern int show_ending;

void DrawCircle(int x, int y, int r, unsigned char c);
void DrawCircleEx(int x, int y, int r, int r2, unsigned char c);
void DrawRect(int x, int y, int w, int h, unsigned char c);
int IsSolid(unsigned char tile);
void draw_char(int cur_x, int cur_y, int c, Uint8 tcol);
void draw_text(int x, int y, char *str, Uint8 tcol);
void draw_text_ex(int x, int y, char *str, Uint8 tcol, SDL_Surface *srf);

void WritePlayerData();
void ReadPlayerData();


void ThinLine(SDL_Surface *scr, int x1, int y1, int x2, int y2, Uint8 col);
float RandomDir();

void Arc(SDL_Surface *s, int x, int y, int r, float dir);


void VideoUpdate();
void EndCycle(int n);

void DrawLevel(int off_x, int off_y, int hide_not_visited, int fog_of_war);
void DrawPlayer(int x, int y, int pl_dir, int pl_frm);
int GetNearestCheckpoint(int x, int y);
int dist(int x1, int y1, int x2, int y2);


#endif /* __LEVELBLIT_H__ */
