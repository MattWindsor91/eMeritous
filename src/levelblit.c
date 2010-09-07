/**************************************************************************
 *  levelblit.c                                                           *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <assert.h>

#include "graphics.h"
#include "dungeon.h"
#include "message.h"
#include "main.h"
#include "files.h"
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






int expired_ms = 0;
int frame_len = 33;
int WriteBitmaps = 0;
int WB_StartRange = 0;
int WB_EndRange = 1000000;
int training = 0;
int game_paused = 0;
int show_ending = 0;
int voluntary_exit = 0;
int tele_select = 0;
int enter_room_x = 0, enter_room_y = 0;


int agate_knife_loc = -1;




void DrawLevel(int off_x, int off_y, int hide_not_visited, int fog_of_war);
void DrawPlayer(int x, int y, int pl_dir, int pl_frm);
void LoadLevel();
void ActivateRoom(int room);

void DrawCircuit();
void ReleaseCircuit();
void DrawCircle(int x, int y, int r, unsigned char c);

void DrawArtifacts();

void HandleEvents();

void DrawShield();

int key_held[10] = {0};


int player_dying;
int magic_circuit;
int circuit_range;
int release_range;


int shield_hp;
int shield_recover;
int checkpoints_found;
int circuit_size;
int first_game;

int player_room;
int player_dir;
int player_wlk;
int player_walk_speed;
int wlk_wait;
int circuit_release;
int scroll_home;
int enter_pressed;



int checkpoint_x;
int checkpoint_y;

int explored = 0;

int artifacts[12];
SDL_Surface *artifact_spr = NULL;

int scroll_x, scroll_y;

int map_enabled;

int prv_player_room;

int specialmessage;
int specialmessagetimer;

int timer_ps = 0;
int timer_v[10];

float RandomDir()
{
  return (float)(rand()%256)*M_PI*2.0/256.0;
}

int UpgradePrice(int t);


void ScrollTo(int x, int y);



void SetGreyscalePalette();
void SetTonedPalette(float pct);
int TouchTile(int ix, int iy);
void SpecialTile(int x, int y);
void DrawRect(int x, int y, int w, int h, unsigned char c);

void DrawCircleEx(int x, int y, int r, int r2, unsigned char c);

void ThinLine(SDL_Surface *scr, int x1, int y1, int x2, int y2, Uint8 col);

void VideoUpdate()
{
  static int bmp = 0;
  char bmp_name[256];
	
  SDL_UpdateRect(screen, 0, 0, 0, 0);
  if (WriteBitmaps) {
    if ((bmp >= WB_StartRange)&&(bmp < WB_EndRange)) {
      sprintf(bmp_name, "v/bmp%d.bmp", bmp);
      SDL_SaveBMP(screen, bmp_name);
    }
    bmp++;
  }
}

void EndCycle(int n)
{
  static int last_ticks;
  int tick_delta;
  tick_delta = SDL_GetTicks() - last_ticks;
	
  if (n == 0) n = frame_len;

  if (tick_delta < n) {
    SDL_Delay(n-tick_delta);
  }
	
  if (!game_paused) expired_ms += n;
		
  last_ticks = SDL_GetTicks();
}

/*int min(int x, int y)
{
  if (x < y) return x;
  return y;
  }*/

void DummyEventPoll()
{
  SDL_Event e;
  SDL_PollEvent(&e);
}


Uint8 Uint8_Bound(int c)
{
  if (c<0) return 0;
  if (c>255) return 255;
  return c;
}

int dist(int x1, int y1, int x2, int y2)
{
  int dx, dy;
  dx = x2 - x1;
  dy = y2 - y1;

  return sqrt((dx*dx)+(dy*dy));
}

void ClearInput()
{
  key_held[K_SP] = 0;
  key_held[K_UP] = 0;
  key_held[K_DN] = 0;
  key_held[K_LT] = 0;
  key_held[K_RT] = 0;
}


void DrawMeter(int x, int y, int n)
{
  static SDL_Surface *meter = NULL;
  SDL_Rect drawfrom, drawto;
  if (meter == NULL) {
    meter = IMG_Load(IMG_METER);
    SDL_SetColorKey(meter, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
  }
	
  drawfrom.x = 0;
  drawfrom.y = 6;
  drawfrom.w = 150;
  drawfrom.h = 6;
	
  drawto.x = x;
  drawto.y = y;
	
  SDL_BlitSurface(meter, &drawfrom, screen, &drawto);
	
  drawfrom.w = n*6;
  drawfrom.y = 0;
	
  SDL_BlitSurface(meter, &drawfrom, screen, &drawto);
}



void Arc(SDL_Surface *s, int x, int y, int r, float dir)
{
  int bright;
  int i, c;
  float pdir, cdir, ndir;
	
  int l_x = x, l_y = y;
  int cx, cy, c1x, c1y, c2x, c2y;
	
  bright = rand()%128+63;
  i = 0;
  while (i < r) {
    i += rand()%5+25;
    pdir = dir + (float)(rand()%16)/16.0*2.0*(M_PI / 15.0);
    ndir = dir - (float)(rand()%16)/16.0*2.0*(M_PI / 15.0);
    cdir = dir + (float)(rand()%16)/16.0*2.0*(M_PI / 20.0) - (float)(rand()%16)/16.0*2.0*(M_PI / 20.0);

    bright += rand()%30;
    bright -= rand()%30;
		
    if (bright < 0) bright = 0;
    if (bright > 255) bright = 255;
		
    c1x = x + cos(pdir) * i;
    c1y = y + sin(pdir) * i;
    ThinLine(s, l_x, l_y, c1x, c1y, bright);
    c2x = x + cos(ndir) * i;
    c2y = y + sin(ndir) * i;
    ThinLine(s, l_x, l_y, c2x, c2y, bright);
		
    for (c = 0; c < 5; c++) {
      DrawRect(x + cos(dir - (M_PI / 10.0) + (float)(rand()%16)/16.0*2.0*(M_PI / 10.0)) * i, y + sin(dir - (M_PI / 10.0) +
                                                                                                     (float)(rand()%16)/16.0*2.0*(M_PI / 10.0)) * i, 1, 1, rand()%128+63);
    }
		
    i += rand()%5+25;
    cx = x + cos(cdir) * i;
    cy = y + sin(cdir) * i;
    ThinLine(s, c1x, c1y, cx, cy, bright);
    ThinLine(s, c2x, c2y, cx, cy, bright);
    l_x = cx;
    l_y = cy;
  }
	
}



void UpRoom()
{
  int i, nd;
	
  nd = rooms[player_room].s_dist + 1;
	
  for (i = 0; i < 3000; i++) {
    if (rooms[i].s_dist == nd) {
      player.x = rooms[i].x * 32 + 64;
      player.y = rooms[i].y * 32 + 64;
    }
  }
}

void
CancelVoluntaryExit(void)
{
  if (voluntary_exit) 
    {
      voluntary_exit = 0;
      game_paused = 0;
    }
}

void
HandleEvents(void)
{
  unsigned short db;
  static SDL_Event event;
  int pressed_tab = 0;
	
  if (PLAYBACK)
    {
      db = fgetc (record_file);
      db |= fgetc (record_file) << 8;
		
      key_held[K_UP] = (db & 0x0001) > 0;
      key_held[K_DN] = (db & 0x0002) > 0;
      key_held[K_LT] = (db & 0x0004) > 0;
      key_held[K_RT] = (db & 0x0008) > 0;
      key_held[K_SP] = (db & 0x0010) > 0;
      enter_pressed  = (db & 0x0020) > 0;
      map_enabled    = (db & 0x0040) > 0;
      game_running   = (db & 0x0080) > 0;
      game_paused    = (db & 0x0100) > 0;
      voluntary_exit = (db & 0x0200) > 0;
      pressed_tab    = (db & 0x0400) > 0;
      tele_select    = (db & 0x0800) > 0;

      return;
    }
	
  if (pressed_tab)
    {
      c_scroll_x = player.x;
      c_scroll_y = player.y;
    }
	
  enter_pressed = 0;

  while (SDL_PollEvent (&event)) 
    {
      if (event.type == SDL_KEYDOWN)
        {
          switch (event.key.keysym.sym)
            {
            case SDLK_w:
            case SDLK_UP:
              key_held[K_UP] = 1;
              CancelVoluntaryExit ();
              break;
            case SDLK_s:
            case SDLK_DOWN:
              key_held[K_DN] = 1;
              CancelVoluntaryExit ();
              break;
            case SDLK_a:
            case SDLK_LEFT:
              key_held[K_LT] = 1;
              CancelVoluntaryExit ();
              break;
            case SDLK_d:
            case SDLK_RIGHT:
              key_held[K_RT] = 1;
              CancelVoluntaryExit ();
              break;
            case SDLK_SPACE:
              key_held[K_SP] = 1;
              CancelVoluntaryExit ();
              break;
            case SDLK_RETURN:
              enter_pressed = 1;
              break;
            case SDLK_ESCAPE:
              if (map_enabled)
                {
                  map_enabled = 0;
                  game_paused = 0;
                  tele_select = 0;
                }
              else
                {
                  voluntary_exit ^= 1;
                  game_paused = voluntary_exit;
                }
              break;
            case SDLK_TAB:
              if (tele_select)
                {
                  map_enabled = 0;
                  game_paused = 0;
                  tele_select = 0;
                }
              else
                {
                  pressed_tab = 1;
                  map_enabled ^= 1;
                  game_paused = map_enabled;
                  c_scroll_x = player.x;
                  c_scroll_y = player.y;
                }

              CancelVoluntaryExit ();
              break;
            case SDLK_h:
              CancelVoluntaryExit ();
              ShowHelp ();
              break;
            case SDLK_p:
              game_paused ^= 1;
              CancelVoluntaryExit ();
              break;

              /*
                case SDLK_j:
                {
                player.reflect_shield = 20;
                player.circuit_refill = 20;
                player.circuit_charge = 20;
                }
                break;
                case SDLK_k:
                {
                int i, n, j;
                for (j = 0; j < 1; j++) {
                for (i = 0; i < 50000; i++) {
                n = rand()%3000;
                if (rooms[n].visited == 0) {
                player.x = rooms[n].x * 32 + rooms[n].w * 16;
                player.y = rooms[n].y * 32 + rooms[n].h * 16;
                rooms[n].visited = 1;
                explored++;
                break;
                }
                }
                }
                }
                break;

                case SDLK_m:
                {
                int i;
                for (i = 0; i < 8; i++) {
                artifacts[i] = 1;
                }
                for (i = 8; i < 11; i++) {
                artifacts[i] = 0;
                }
                artifacts[11] = 0;
                }
                break;
						
                case SDLK_n:
                {
                current_boss = 3;
                expired_ms = 1000000;
                }
                break;
              */
            default:
              break;
            }
        }

      if (event.type == SDL_KEYUP)
        {
          switch (event.key.keysym.sym)
            {
            case SDLK_w:
            case SDLK_UP:
              key_held[K_UP] = 0;
              break;
            case SDLK_s:
            case SDLK_DOWN:
              key_held[K_DN] = 0;
              break;
            case SDLK_a:
            case SDLK_LEFT:
              key_held[K_LT] = 0;
              break;
            case SDLK_d:
            case SDLK_RIGHT:
              key_held[K_RT] = 0;
              break;
            case SDLK_SPACE:
              key_held[K_SP] = 0;
              break;
            default:
              break;
            }
        }

      if (event.type == SDL_QUIT)
        voluntary_exit = 1;
    
    }
		
  if (RECORDING)
    {
      db = 0;
		
      db |= 0x0001 * key_held[K_UP];
      db |= 0x0002 * key_held[K_DN];
      db |= 0x0004 * key_held[K_LT];
      db |= 0x0008 * key_held[K_RT];
      db |= 0x0010 * key_held[K_SP];
      db |= 0x0020 * enter_pressed;
      db |= 0x0040 * map_enabled;
      db |= 0x0080 * game_running;
      db |= 0x0100 * game_paused;
      db |= 0x0200 * voluntary_exit;
      db |= 0x0400 * pressed_tab;
      db |= 0x0800 * tele_select;
		
      fputc(db & 0x00FF, record_file);
      fputc((db & 0xFF00)>>8, record_file);
      return;
    }	
}

void DrawTile(int x, int y, int off_x, int off_y, int hide_not_visited, int fog_of_war)
{
  SDL_Rect tilerec, screenrec;
  int i;
  int resolve_x, resolve_y;

  static SDL_Surface *tiles = NULL;
  static SDL_Surface *fog = NULL;

  if (tiles == NULL) {
    Uint8 *pp;

    tiles = IMG_Load(IMG_TILESET);
    fog = IMG_Load(IMG_TILESET);
				
    pp = fog->pixels;

    for (i = 0; i < fog->w*fog->h; i++) {
      *pp = *pp / 2 + 128;
      pp++;
    }
  }

  resolve_x = x + (off_x/32);
  resolve_y = y + (off_y/32);

  /* Don't bother with a tile blit if there isn't a tile here. */		
  if ((GetVisited(resolve_x, resolve_y) > 0) ||
      (player_room == GetRoom(resolve_x, resolve_y)) || 
      (hide_not_visited == 0)) {
    tilerec.x = Get(resolve_x, resolve_y) * 32;
    tilerec.y = 0;
    tilerec.w = 32;
    tilerec.h = 32;
			
    screenrec.x = x*32 - ( (off_x) %32);
    screenrec.y = y*32 - ( (off_y) %32);

    if ((player_room != GetRoom(resolve_x, resolve_y))&&(fog_of_war)) {
      SDL_BlitSurface(fog, &tilerec, screen, &screenrec);
    } else {
      SDL_BlitSurface(tiles, &tilerec, screen, &screenrec);
    }
  }
}

void DrawLevel(int off_x, int off_y, int hide_not_visited, int fog_of_war)
{
  int x, y;
	
  DrawRect(0, 0, SCREEN_W, SCREEN_H, BG_COLOUR);
	
  for (y = 0; y < 16; y++) {
    for (x = 0; x < 21; x++) {
      DrawTile(x, y, off_x, off_y, hide_not_visited, fog_of_war);
    }
  }
}

void DrawPlayer(int x, int y, int pl_dir, int pl_frm)
{
  static SDL_Surface *playersprite = NULL;
  SDL_Rect playerrec, screenrec;
	
  if (playersprite == NULL) {
    playersprite = IMG_Load(IMG_PLAYER);
    SDL_SetColorKey(playersprite, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
  }
	
  playerrec.x = pl_frm * PLAYER_W;
  playerrec.y = pl_dir * PLAYER_H;
  playerrec.w = PLAYER_W;
  playerrec.h = PLAYER_H;
	
  screenrec.x = x;
  screenrec.y = y;
	
  SDL_BlitSurface(playersprite, &playerrec, screen, &screenrec);
}

void SetGreyscalePalette(void)
{
  SDL_Color grey[256];
  SDL_Color pal[256];
  int i;
	
  float ip;
	
  for (i = 0; i < 256; i++) {
    grey[i].r = grey[i].g = grey[i].b = i;
  }
	
  for (i = 0; i < 256; i++) {
    ip = (float)i / 255.0;
    pal[i].r = (cos(ip * M_PI / 2.0 + M_PI) + 1.0) * 255;
    pal[i].g = (sin(ip * M_PI / 2.0) * 255 + i) / 2;
    pal[i].b = sin(ip * M_PI / 2.0) * 255;
  }
	
  SDL_SetPalette(screen, SDL_LOGPAL, grey, 0, 256);
  SDL_SetPalette(screen, SDL_PHYSPAL, pal, 0, 256);
}

void SetTonedPalette(float dct)
{
  SDL_Color pal[256];
  float pct = 1.0 - dct;
  float rp_dct, rp_pct;
  float ip;
  int ec;
  int i;
  static int tk = 0;
	
  ec = rooms[player_room].enemies;
	
  if (ec < 50) {
    rp_dct = (float)ec / 50.0;
  } else {
    rp_dct = 1.0;
  }
  rp_pct = 1.0 - rp_dct;

  /* Final boss effects. */
  if ((player_room == 0) &&
      (current_boss == BOSS_FINAL) &&
      (boss_fight_mode >= 3) ) {
    if (boss_fight_mode == 23) {
      for (i = 0; i < 256; i++) {
        pal[i].r = i;
        pal[i].g = i;
        pal[i].b = i;
      }
    } else {
      tk++;
      pct = sin((float)tk / 20.0 * M_PI) * (0.5 - (float)(boss_fight_mode-3)*0.025) + (0.5 - (float)(boss_fight_mode-3)*0.025);
			
      if (magic_circuit < 0.1) pct = 1.0;

      for (i = 0; i < 256; i++) {
        ip = (float)i / 255.0;
        pal[i].r = 255 - (255 - (cos(ip * M_PI / 2.0 + M_PI) + 1.0) * 255)*pct;
        pal[i].g = 255 - (255 - i)*pct;
        pal[i].b = 255 - (255 - sin(ip * M_PI / 2.0) * 255) * pct;
      }
			
			
      pal[1].r = 0;
      pal[1].g = 0;
      pal[1].b = 0;
    }
  } else {
    /* Cursed seal palette effect. */
    if (artifacts[ART_CURSED_SEAL]) {
      if (player_room == 0) {
        tk++;
        pct = sin((float)tk / 33.0 * M_PI) * 0.5 + 0.5;
        for (i = 0; i < 256; i++) {
          pal[i].r = i;
          pal[i].g = (i / 3)*pct;
          pal[i].b = (i * 2 / 3)*pct;
        }
      } else {
        for (i = 0; i < 256; i++) {
          ip = (float)i / 255.0;
          pal[i].r = i;
          pal[i].g = i * dct;
          pal[i].b = (cos(ip * M_PI / 2.0 + M_PI) + 1.0) * 255 * dct;
        }
      }
      /* Final boss Agate Knife special effect. */		
      if ((current_boss == BOSS_FINAL) &&
          (player.has_agate_knife) &&
          (player_room == 0)) {
        if (boss_lives <= 1) {
          tk++;
          for (i = 0; i < 256; i++) {
            pct = sin((float) (tk + i) / 24.0 * M_PI) * 0.5 + 0.5;
						
            pal[i].r = (i * 0.5 + 128)*pct;
            pal[i].g = i * 0.5 + 128;
            pal[i].b = (i * 0.5 + 128)*pct;
          }
        }
      }
    } else {
      for (i = 0; i < 256; i++) {
        ip = (float)i / 255.0;
        pal[i].r = (((cos(ip * M_PI / 2.0 + M_PI) + 1.0) * 255)*pct + i*dct)*rp_pct + (sin(ip * M_PI / 2.0) * 207 + 48)*rp_dct;
        pal[i].g = (i)*rp_pct + ((cos(ip * M_PI / 2.0 + M_PI) + 1.0) * 255)*rp_dct;
        pal[i].b = ((sin(ip * M_PI / 2.0) * 255 * pct)+((cos(ip * M_PI / 2.0 + M_PI) + 1.0) * 255 * dct))*rp_pct + ((cos(ip * M_PI / 2.0 + M_PI) + 1.0) * 255)*rp_dct;
      }
    }
  }
	
  SDL_SetPalette(screen, SDL_PHYSPAL, pal, 0, 256);
}


int IsSolid(unsigned char tile)
{
  return TileData[tile].Is_Solid;
}

void ActivateBossDoor(int x, int y)
{
  static int bd_timer = 0;
  int bx = x, by = y;
	
  /* find boss room */
  if (rooms[GetRoom(x+1, y)].room_type == ROOM_BOSS) {
    bx += 1;
  } else if (rooms[GetRoom(x-1, y)].room_type == ROOM_BOSS) {
    bx -= 1;
  } else if (rooms[GetRoom(x, y+1)].room_type == ROOM_BOSS) {
    by += 1;
  } else if (rooms[GetRoom(x, y-1)].room_type == ROOM_BOSS) {
    by -= 1;
  } else {
    return;
  }
	
  if (artifacts[ART_HOLY_SWORD + rooms[GetRoom(bx, by)].room_param]) {
    opening_door_x = x;
    opening_door_y = y;
    opening_door_i = 1;
    opening_door_n = rooms[GetRoom(bx, by)].room_param;
    if ((SDL_GetTicks() - bd_timer) > 100) {
      SND_Pos("dat/a/crystal2.wav", 100, 0);
      bd_timer = SDL_GetTicks();
    }
  }
}

int TouchTile(int ix, int iy)
{
  int i;
  int off_x, off_y;
  int ret = 1;
  unsigned char tile;
		
  for (i = 0; i < 4; i++) {
    off_x = 15*(i%2);
    off_y = 23*(i/2);

    tile = Get((ix+off_x)/32, (iy+off_y)/32);
    switch (tile) {
    case 38:
    case 39:
    case 40:
    case 41:
      ActivateBossDoor((ix+off_x)/32, (iy+off_y)/32);
      ret = 0;
      break;
    case 13:
      player.x = (ix + off_x) / 32 * 32 + 8;
      player.y = (iy/32 + 2)*32 + 32;
      return 1;
      break;
    case 14:
      player.x = (ix + off_x) / 32 * 32 + 8;
      player.y = (iy/32 - 2)*32 + 8;
      return 1;
      break;
    case 15:
      player.x = (ix/32 + 2)*32 + 32;
      player.y = (iy + off_y) / 32 * 32 + 4;
      return 1;
      break;
    case 16:
      player.x = (ix/32 - 2)*32 + 16;
      player.y = (iy + off_y) / 32 * 32 + 4;
      return 1;
      break;
    default:
      if (TileData[tile].Is_Solid) ret = 0;
      /*ret = 0; */
      break;
    }
  }
  if (ret == 1) {
    player.x = ix;
    player.y = iy;
  }
  return ret;
}

void ActivateRoom(int room)
{
  /*printf("Activating room %d (type %d)\n", room, rooms[room].room_type);*/
  if (rooms[room].checkpoint) {
    checkpoints_found++;
  }
  if (rooms[room].room_type == 3) {
    /* lock the doors!*/
    lock_doors(room);
  }
  ActivateEnemies(room);
}

void DrawRect(int x, int y, int w, int h, unsigned char c)
{
  SDL_Rect r;

  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;

  SDL_FillRect(screen, &r, c);
}

void DrawCircuit()
{
  int vd = 520;
  char buf[20];

  if (magic_circuit != 0) {
    DrawRect(110, 469, 8+abs(magic_circuit) * vd / circuit_size, 9, (magic_circuit > 0) ? 159 : 72);
    DrawRect(111, 470, 6+abs(magic_circuit) * vd / circuit_size, 7, (magic_circuit > 0) ? 183 : 80);
    DrawRect(112, 471, 4+abs(magic_circuit) * vd / circuit_size, 5, (magic_circuit > 0) ? 207 : 96);
    DrawRect(113, 472, 2+abs(magic_circuit) * vd / circuit_size, 3, (magic_circuit > 0) ? 231 : 112);
    DrawRect(114, 473, abs(magic_circuit) * vd / circuit_size, 1, (magic_circuit > 0) ? 255 : 128);
  }
  sprintf(buf, "%.1f", fabs((float)magic_circuit / 100.0));
  draw_text(115, 470, buf, 0);
  draw_text(3, 469, "Psi Circuit", 200);
}

void ReleaseCircuit()
{
  circuit_release = 1;
  release_range = circuit_range;
  release_x = player.x;
  release_y = player.y;
  release_str = magic_circuit;
  if (player.circuit_charge==30) {
    release_str *= 1.25;
  }
	
  SND_CircuitRelease(release_str);
  magic_circuit *= -1;
}

void DrawCircle(int x, int y, int r, unsigned char c)
{
  int circ_y;

  int len_x, outer_len_x, inner_len_x;

  int inner_r = r - 10;
  if (inner_r < 1) inner_r = 1;

  if (r < 1) return;
  /* a^2 + b^2 = c^2 */
  for (circ_y = 0; circ_y < r; circ_y++) {
    if (circ_y < (r-10)) {
      outer_len_x = sqrt(r*r - circ_y*circ_y);
      inner_len_x = sqrt((r-10)*(r-10) - circ_y*circ_y);
      DrawRect(x - outer_len_x, y - circ_y, (outer_len_x - inner_len_x), 1, c);
      DrawRect(x + inner_len_x, y - circ_y, (outer_len_x - inner_len_x), 1, c);
      DrawRect(x - outer_len_x, y + circ_y, (outer_len_x - inner_len_x), 1, c);
      DrawRect(x + inner_len_x, y + circ_y, (outer_len_x - inner_len_x), 1, c);
    } else {
      len_x = sqrt(r*r - circ_y*circ_y);
		
      DrawRect(x - len_x, y - circ_y, len_x*2, 1, c);
      DrawRect(x - len_x, y + circ_y, len_x*2, 1, c);
    }
  }
}

void DrawCircleEx(int x, int y, int r, int r2, unsigned char c)
{
  int circ_y;

  int len_x, outer_len_x, inner_len_x;

  int inner_r = r2;
  int diffi = r-r2;
  if (inner_r < 1) inner_r = 1;



  if (r < 1) return;
  /* a^2 + b^2 = c^2 */
  for (circ_y = 0; circ_y < r; circ_y++) {
    if (circ_y < (r-diffi)) {
      outer_len_x = sqrt(r*r - circ_y*circ_y);
      inner_len_x = sqrt((r-diffi)*(r-diffi) - circ_y*circ_y);
      DrawRect(x - outer_len_x, y - circ_y, (outer_len_x - inner_len_x), 1, c);
      DrawRect(x + inner_len_x, y - circ_y, (outer_len_x - inner_len_x), 1, c);
      DrawRect(x - outer_len_x, y + circ_y, (outer_len_x - inner_len_x), 1, c);
      DrawRect(x + inner_len_x, y + circ_y, (outer_len_x - inner_len_x), 1, c);
    } else {
      len_x = sqrt(r*r - circ_y*circ_y);
		
      DrawRect(x - len_x, y - circ_y, len_x*2, 1, c);
      DrawRect(x - len_x, y + circ_y, len_x*2, 1, c);
    }
  }
}

void DrawShield()
{
  static int t=0;
  int s_size;
  int belts = 0;
  int i, bpos;
  t++;
	
  /* Don't draw a shield if it doesn't presently exist. */
  if (player.reflect_shield == 0 || shield_hp == 0) { 
    return;
  }
	
  s_size = shield_hp;
  if (s_size > 15) {
    belts = s_size - 15;
    s_size = 15;
  }
  DrawCircleEx(SCREEN_W / 2, SCREEN_H / 2, 
               28 + s_size, 28 - s_size,
               128 + (shield_hp * 127 / player.reflect_shield) - 
               (50 * (shield_hp < player.reflect_shield) + shield_recover) - 
               45 + ((t % 4) * 15));
	
  for (i = 0; i < belts; i++) {
    bpos = 13 + (30 * (i+1) / (belts+1));
    DrawCircleEx(SCREEN_W / 2, SCREEN_H / 2, bpos + 1, bpos - 1, ((i + t) % 6 * 12));
  }
}

void ST_Teleport()
{
}



void RoomTreasure(int room, int typ)
{
  int treasure;
  int given_treasure = 0;
	
  if (typ == 0) {
    /* Treasure */
    treasure = rooms[room].room_param;
    artifacts[treasure] = 1;
    specialmessage = treasure + 1;
    specialmessagetimer = 30;
    SND_Pos("dat/a/crystal2.wav", 128, 0);
  }
  if (typ == 1) {
    /* Reward */
    while (!given_treasure) {
      treasure = rand() % 4;
			
      switch (treasure) {
      case 0:
        specialmessage = 20;
        player.crystals += rand()%((1 << (rooms[room].s_dist / 7)) * 1500);
        given_treasure = 1;
        SND_Pos("dat/a/tone.wav", 128, 0);
        break;
      case 1:
        if (player.reflect_shield <= MAX_UPGRADE) {
          specialmessage = 10;
          player.reflect_shield += 1;
          given_treasure = 1;
          SND_Pos("dat/a/tone.wav", 128, 0);
        }
        break;
      case 2:
        if (player.circuit_charge <= MAX_UPGRADE) {
          specialmessage = 11;
          player.circuit_charge += 1;
          given_treasure = 1;
          SND_Pos("dat/a/tone.wav", 128, 0);
        }
        break;
      case 3:
        if (player.circuit_refill <= MAX_UPGRADE) {
          specialmessage = 12;
          player.circuit_refill += 1;
          given_treasure = 1;
          SND_Pos("dat/a/tone.wav", 128, 0);
        }
        break;
      default:
        break;
      }
    }
    specialmessagetimer = 30;
  }
}

int GetNearestCheckpoint(int nx, int ny)
{
  int i;
  int nearest_checkpoint = -1;
  int nearest_dist = 10000000;
  int cp_x, cp_y, cp_dist;
  int room_chk[3000] = {0};
  int x, y, rx, ry;

  i = GetRoom(nx/32, ny/32);
  if (i != -1) {
    room_chk[i] = 1;
    if ((rooms[i].checkpoint != 0)&&(rooms[i].visited!=0)) {
      nearest_checkpoint = i;
    }
  }
  if (nearest_checkpoint == -1) {
				
    for (y = 0; y < 54;) {
      for (x = 0; x < 54;) {
        rx = nx/32 - 27 + x;
        ry = ny/32 - 27 + y;
				
        i = GetRoom(rx, ry);
        if (i != -1) {
          if (room_chk[i] == 0) {
            room_chk[i] = 1;
            if ((rooms[i].checkpoint != 0)&&(rooms[i].visited!=0)) {
              cp_x = rooms[i].x * 32 + rooms[i].w * 16;
              cp_y = rooms[i].y * 32 + rooms[i].h * 16;
              cp_dist = dist(cp_x, cp_y, nx, ny);
              if (cp_dist < nearest_dist) {
                nearest_dist = cp_dist;
                nearest_checkpoint = i;
              }
            }
          }
        }
        x += 2;
      }
      y += 2;
    }
		
  }
	
  return nearest_checkpoint;
}

void TeleportPlayerToRoom(int c_room)
{
  if (c_room == 0) {
    player.x = 8232;
    player.y = 8108;
  } else {
    player.x = rooms[c_room].x * 32 + (rooms[c_room].w / 2 * 32) + 8;
    player.y = rooms[c_room].y * 32 + (rooms[c_room].h / 2 * 32) + 4;
  }
  c_scroll_x = player.x;
  c_scroll_y = player.y;
  scroll_home = 1;
}


void TeleportPlayerToNextRoom()
{
  int c_room;
  c_room = (player_room + 1) % NUM_ROOMS;
  while (! ((rooms[c_room].checkpoint!=0)&&(rooms[c_room].visited!=0))) {
    c_room = (c_room + 1) % NUM_ROOMS;
  }
	
  if (c_room == 0) {
    player.x = 8232;
    player.y = 8108;
  } else {
    player.x = rooms[c_room].x * 32 + (rooms[c_room].w / 2 * 32) + 8;
    player.y = rooms[c_room].y * 32 + (rooms[c_room].h / 2 * 32) + 4;
  }
  c_scroll_x = player.x;
  c_scroll_y = player.y;
  scroll_home = 1;
}

void ActivateTile(unsigned char tile, int x, int y)
{
  int c_room;
	
  enter_pressed = 0;
  switch (tile) {
  case 25:
    /* Cursed seal stops checkpoints working. */
    if (artifacts[ART_CURSED_SEAL]) break;

    c_room = GetNearestCheckpoint(c_scroll_x, c_scroll_y);
    if (tele_select) {
      if (c_room != -1) {
        if (c_room == player_room) {
          TeleportPlayerToNextRoom();
        } else {
          TeleportPlayerToRoom(c_room);
        }
      }
    } else {
      map_enabled = 1;
      game_paused = 1;
      tele_select = 1;
				
      c_scroll_x = player.x;
      c_scroll_y = player.y;
    }
			
    break;
  case 26:
    RoomTreasure(GetRoom(x, y), (x+y)%2);
    Put(x, y, 27, GetRoom(x, y));
    break;
  case 28:
    if (player.reflect_shield >= 24) return;
    if (player.crystals >= UpgradePrice(UP_REFLECT_SHIELD)) {
      player.crystals -= UpgradePrice(UP_REFLECT_SHIELD);
      player.reflect_shield += 1;
      SND_Pos("dat/a/crystal.wav", 128, 0);
    }
    break;
  case 29:
    if (player.circuit_charge >= 24) return;
    if (player.crystals >= UpgradePrice(UP_CIRCUIT_CHARGE)) {
      player.crystals -= UpgradePrice(UP_CIRCUIT_CHARGE);
      player.circuit_charge += 1;
      SND_Pos("dat/a/crystal.wav", 128, 0);
    }
    break;
  case 30:
    if (player.circuit_refill >= 24) return;
    if (player.crystals >= UpgradePrice(UP_CIRCUIT_REFILL)) {
      player.crystals -= UpgradePrice(UP_CIRCUIT_REFILL);
      player.circuit_refill += 1;
      SND_Pos("dat/a/crystal.wav", 128, 0);
    }
    break;
  case 31:
    DoSaveGame();
    break;
  case 32:
    CrystalSummon();
    SND_Pos("dat/a/crystal.wav", 80, 0);
    break;
  default:
    break;
  }
}

void CompassPoint()
{
  int nearest = 1000000;
  int n_room = -1;
  int i;
  int loc_x, loc_y;
  int cdist;
  int rplx, rply;
  int boss_room, artifact_room;
  int bosses_defeated = current_boss;
  float pdir_1 = 0;
  float pdir_2 = 0;
  int pdir_1t = 0, pdir_2t = 0;
	
  rplx = player.x + PLAYER_W / 2;
  rply = player.y + PLAYER_H / 2;
  /* Find the nearest SIGNIFICANT LOCATION for the player */
	
  /* Look at the three artifacts
   * Unless the player is going for the place of power */
	
  if (current_boss < NUM_REGULAR_BOSSES) {
    for (i = 0; i < NUM_REGULAR_BOSSES; i++) {
      /* Work out the relevant boss and artifact room IDs. */
      boss_room = (((i + 1) * (NUM_ROOMS / NUM_REGULAR_BOSSES)) - 1);
      artifact_room = (((i + 1) * (NUM_ROOMS / NUM_REGULAR_BOSSES)) - 
                       (NUM_ROOMS / NUM_REGULAR_BOSSES / 2) - 1);
      /* Has the player got this artifact already? */
      if (artifacts[ART_HOLY_SWORD + i] == 0) {
        /* No - has the player already destroyed the boss? */
        if (rooms[boss_room].room_type == 2) {
          /* No - can the player get the artifact? */
          if (CanGetArtifact()) {
            /* Point player to this artifact room, if it is the nearest */
            loc_x = rooms[artifact_room].x * 32 + rooms[artifact_room].w * 16;
            loc_y = rooms[artifact_room].y * 32 + rooms[artifact_room].h * 16;
            cdist = dist(rplx, rply, loc_x, loc_y);
            if (cdist < nearest) {
              nearest = cdist;
              n_room = artifact_room;
            }
          }
        }
      } else {
        /* Yes, has artifact - Has the player already destroyed the boss? */
        if (rooms[boss_room].room_type == 2) {
          /* No - Point player to the boss room, if it is the nearest */
          loc_x = rooms[boss_room].x * 32 + rooms[i * 1000 + 999].w * 16;
          loc_y = rooms[boss_room].y * 32 + rooms[i * 1000 + 999].h * 16;
          cdist = dist(rplx, rply, loc_x, loc_y);
          if (cdist < nearest) {
            nearest = cdist;
            n_room = boss_room;
          }
        } else { /* yes */
          bosses_defeated++;
        }
      }
    }
  }
  /* If, on the other hand, the player has destroyed all three bosses, 
   * point them towards the PLACE OF POWER. 
   */
  if (bosses_defeated == NUM_REGULAR_BOSSES) {
    /* If the player already has the seal, point them to home */
    if (artifacts[ART_CURSED_SEAL] == 1) {
      loc_x = rooms[0].x * 32 + rooms[0].w * 16;
      loc_y = rooms[0].y * 32 + rooms[0].h * 16;
      cdist = dist(rplx, rply, loc_x, loc_y);
      if (cdist < nearest) {
        nearest = cdist;
        n_room = 0;
      }
    } else {
      /* Can the player touch the seal? */
      if (CanGetArtifact()) {
        loc_x = rooms[place_of_power].x * 32 + rooms[place_of_power].w * 16;
        loc_y = rooms[place_of_power].y * 32 + rooms[place_of_power].h * 16;
        cdist = dist(rplx, rply, loc_x, loc_y);
        if (cdist < nearest) {
          nearest = cdist;
          n_room = place_of_power;
        }
      }
    }
  }
	
  /* Did we find a room? If so, point to it */
	
  if (n_room != -1) {
    loc_x = rooms[n_room].x * 32 + rooms[n_room].w * 16;
    loc_y = rooms[n_room].y * 32 + rooms[n_room].h * 16;
	
    pdir_1 = PlayerDir(loc_x, loc_y) + M_PI;
    pdir_1t = 1;
		
    n_room = -1;
  }
	
  nearest = 1000000;
  /* Find the nearest uncleared artifact room */
  for (i = 0; i < NUM_ROOMS; i++) {
    if (rooms[i].room_type == 3) {
      loc_x = rooms[i].x * 32 + rooms[i].w * 16;
      loc_y = rooms[i].y * 32 + rooms[i].h * 16;
      cdist = dist(rplx, rply, loc_x, loc_y);
      if (cdist < nearest) {
        nearest = cdist;
        n_room = i;
      }
    }
  }
	
  if (n_room != -1) {
    loc_x = rooms[n_room].x * 32 + rooms[n_room].w * 16;
    loc_y = rooms[n_room].y * 32 + rooms[n_room].h * 16;
	
    pdir_2 = PlayerDir(loc_x, loc_y) + M_PI;
    pdir_2t = 1;
		
    n_room = -1;
  }
	
  /* Did we find at least one thing to point to? If not, abort */
  if (!(pdir_1t || pdir_2t))
    return;
	
  DrawCircleEx(rplx - scroll_x, rply - scroll_y, 200, 190, 255);
  if (pdir_1t)
    DrawCircleEx(rplx - scroll_x + cos(pdir_1) * 170, rply - scroll_y + sin(pdir_1) * 170, 30, 20, 255);
  if (pdir_2t)
    DrawCircleEx(rplx - scroll_x + cos(pdir_2) * 170, rply - scroll_y + sin(pdir_2) * 170, 30, 20, 195);
		
  for (i = 0; i < 50; i++) {
    if (pdir_1t)
      DrawCircle(rplx - scroll_x + cos(pdir_1) * (25 + i * 4), rply - scroll_y + sin(pdir_1) * (25 + i * 4), 5, 255);
    if (pdir_2t)
      DrawCircle(rplx - scroll_x + cos(pdir_2) * (25 + i * 4), rply - scroll_y + sin(pdir_2) * (25 + i * 4), 5, 195);
  }
  DrawCircleEx(rplx - scroll_x, rply - scroll_y, 30, 20, 255);
	
  DrawCircleEx(rplx - scroll_x, rply - scroll_y, 197, 193, 128);
  if (pdir_1t)
    DrawCircleEx(rplx - scroll_x + cos(pdir_1) * 170, rply - scroll_y + sin(pdir_1) * 170, 27, 23, 128);
  if (pdir_2t)
    DrawCircleEx(rplx - scroll_x + cos(pdir_2) * 170, rply - scroll_y + sin(pdir_2) * 170, 27, 23, 78);
	
  for (i = 0; i < 50; i++) {
    if (pdir_1t)
      DrawCircle(rplx - scroll_x + cos(pdir_1) * (25 + i * 4), rply - scroll_y + sin(pdir_1) * (25 + i * 4), 3, 128);
    if (pdir_2t)
      DrawCircle(rplx - scroll_x + cos(pdir_2) * (25 + i * 4), rply - scroll_y + sin(pdir_2) * (25 + i * 4), 3, 78);
  }
  DrawCircleEx(rplx - scroll_x, rply - scroll_y, 27, 23, 128);
}

void SpecialTile(int x, int y)
{
  static int otext = 0;
  static int t = 0;
  unsigned char tile;
  char message[100] = "";

  tile = Get(x, y);
  switch (tile) {
  case 25:
    /* The cursed seal stops checkpoints from working. */
    if (artifacts[ART_CURSED_SEAL]) {
      sprintf(message, "This is a checkpoint, but it doesn't seem to be working.");
      break;
    }
    if (checkpoints_found <= 1) {
      sprintf(message, "This is a checkpoint. You will return here when you die.");
    } else {
      sprintf(message, "Press ENTER to teleport between checkpoints.");
    }
    break;
  case 26:
    sprintf(message, "Press ENTER to open the storage chest");
    break;
  case 28:
    if (player.reflect_shield >= 25) {
      sprintf(message, "Your shield is already at full efficiency");
    } else {
      sprintf(message, "Press ENTER to upgrade shields (%d crystals)",
              UpgradePrice(UP_REFLECT_SHIELD));
    }
    break;
  case 29:
    if (player.circuit_charge >= 25) {
      sprintf(message, "Your circuit charge rate is already at its highest");
    } else {
      sprintf(message, "Press ENTER to upgrade circuit charge (%d crystals)",
              UpgradePrice(UP_CIRCUIT_CHARGE));
    }
    break;
  case 30:
    if (player.circuit_refill >= 25) {
      sprintf(message, "Your circuit refill rate is already at its highest");
    } else {
      sprintf(message, "Press ENTER to upgrade circuit refill (%d crystals)",
              UpgradePrice(UP_CIRCUIT_REFILL));
    }
    break;
  case 31:
    sprintf(message, "Press ENTER to record your progress");
    break;
  case 32:
    if (total_gems == 0) {
      sprintf(message, "This is a crystal device. It isn't working at the moment.");
    } else {
      sprintf(message, "Press ENTER to activate the crystal device");
    }
    break;
  case 42:
    if (rooms[player_room].room_type == 5) {
      if (CanGetArtifact(rooms[player_room].room_param)) {
					
      } else {
        sprintf(message, "The artifact is tainted with shadow. You must slay more of the shadow first.");
      }
    }
    break;
  case 53:
    CompassPoint();
    break;
  default:
    if (first_game) {
      if (otext < 60) {
        sprintf(message, "Press H to read the help file");
        otext++;
      }
    }
    break;
  }
	
  if (message[0] == 0) {
    if (specialmessage != 0) {
      /* Make sure the special message is in boundaries */
      if (specialmessage <= SM_AGATE_KNIFE_ACQ) {
        strncpy(message, special_messages[specialmessage - 1], 100);
      } else {
        sprintf(message, "ERROR: NO MESSAGE VALUE GIVEN");
      }
      specialmessagetimer--;
      if (specialmessagetimer <= 0) {
        specialmessage = 0;
      }
    }
  }
	
  if (message[0] == 0) return;
  
  DrawRect((SCREEN_W / 2) - strlen(message) * 8 / 2 - 20, 100,
           strlen(message) * 8 + 40, 48, 200);
  DrawRect((SCREEN_W / 2) - strlen(message) * 8 / 2 - 15, 105,
           strlen(message) * 8 + 30, 38, 32);
  DrawRect((SCREEN_W / 2) - strlen(message) * 8 / 2 - 10, 110, 
           strlen(message) * 8 + 20, 28, 64);

  draw_text((SCREEN_W / 2) - strlen(message) * 8 / 2, 120,
            message, t % 16 < 8 ? 255 : 192);

  t++;

  if (enter_pressed) {
    ActivateTile(tile, x, y);
  }
}

void ScrollTo(int x, int y)
{
  static int scrollspeed_x = 1, scrollspeed_y = 1;
  if (scroll_home == 0) {
    scroll_x = x;
    scroll_y = y;
    return;
  }

  if (scroll_home == 1) {
    scrollspeed_x = (x - scroll_x)/20;
    scrollspeed_y = (y - scroll_y)/20;
    scroll_home = 2;
  }

  if (scroll_home == 2) {
    scroll_x += (x - scroll_x)/2;
    scroll_y += (y - scroll_y)/2;

    if ((abs(scroll_x-x)<2)&&(abs(scroll_y-y)<2)) {
      scroll_x = x;
      scroll_y = y;
      scroll_home = 0;
    }
  }
}

void DrawArtifacts()
{
  int i;
  SDL_Rect from, to;
	
  if (artifact_spr == NULL) {
    artifact_spr = IMG_Load("dat/i/artifacts.png");
    SDL_SetColorKey(artifact_spr, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
  }
	
  for (i = 0; i < 12; i++) {
    if (artifacts[i]) {
      from.x = i * 32;
      from.y = 0;
      from.w = 32;
      from.h = 32;
			
      to.x = 608;
      to.y = 47 + i * 35;
      SDL_BlitSurface(artifact_spr, &from, screen, &to);
    }
  }
}

void Swap(int *a, int *b)
{
  *a ^= *b ^= *a ^= *b;
}

void ThinLine(SDL_Surface *scr, int x1, int y1, int x2, int y2, Uint8 col)
{
  int dx, dy, dm;
  int i, j;
	
  dx = (x2 - x1);
  dy = (y2 - y1);
	
  dm = abs(dx) > abs(dy) ? dx : dy;
	
  if (dm == 0) return;
	
  if (dm < 0) {
    Swap(&x1, &x2);
    Swap(&y1, &y2);
    dx = (x2 - x1);
    dy = (y2 - y1);
		
    dm = dm * -1;
  }

  if (dm == dx) {
    if (dy == 0) {
      DrawRect(x1, y1, x2-x1+1, 1, col);
      return;
    }
    for (i = 0; i < dm; i++) {
      j = (dy * i / dm);
      DrawRect(i+x1, j+y1, 1, 1, col);
    }
  }
  if (dm == dy) {
    if (dx == 0) {
      DrawRect(x1, y1, 1, y2-y1+1, col);
      return;
    }
    for (i = 0; i < dm; i++) {
      j = (dx * i / dm);
      DrawRect(j+x1, i+y1, 1, 1, col);
    }
  }
}
