/**************************************************************************
 *  dungeon.c                                                             *
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

/* Global variables (get rid!) */
int opening_door_x, opening_door_y, opening_door_i = 0, opening_door_n;

int release_x;
int release_y;
int release_str;

void lock_doors(int r)
{
  /* printf("Locking room %d...", r); */
  int x, y;
  int rx, ry;
  int rt;
  int rcount = 0;
	
  for (y = 0; y < rooms[r].h; y++) {
    for (x = 0; x < rooms[r].w; x++) {
      rx = x + rooms[r].x;
      ry = y + rooms[r].y;
      rt = Get(rx, ry);
			
      if ((rt >= 13) && (rt <= 16)) {
        rcount++;
        Put(rx, ry, rt - 13 + 21, r);
      }
    }
  }
  /*printf("locked %d doors\n", rcount); */
}

void
kill_player(void)
{
  int lost_gems;
  int rg_x, rg_y, rg_v;

  player.hp--;
				
  if (player.hp <= 0)
    {
      if (!training)
        player.lives--;

      lost_gems = player.crystals / 3;
      player.crystals -= lost_gems;

      lost_gems = lost_gems * 95 / 100;

      while (lost_gems > 0)
        {
          rg_x = (rooms[player_room].x * 32 + 32
                  + rand () % (rooms[player_room].w * 32 - 64));
          rg_y = (rooms[player_room].y * 32 + 32
                  + rand () % (rooms[player_room].h * 32 - 64));
          rg_v = rand () % (lost_gems / 4 + 2);
                  
          CreateGem (rg_x, rg_y, player_room, rg_v);
          lost_gems -= rg_v;
        }
		
      player_dying = 0;
      shield_hp = 0;
			
      if (current_boss == BOSS_FINAL
          && boss_fight_mode != BSTA_NONE)
        {
          player.x = enter_room_x;
          player.y = enter_room_y;
          prv_player_room = 1;
        }
      else
        {
          player.x = checkpoint_x;
          player.y = checkpoint_y;
        }

      scroll_home = 1;
      CircuitBullets (player.x, player.y, 100);
      player.hp = 3 + (player.has_agate_knife) * 3;
    }
  else
    player_dying = 0;
}

int
play_dungeon(char *fname)
{
  extern SaveFile save; /* <- Temporary workaround. */

  int ix,  iy;
  int off_x, off_y;
  int t = 0;
  int i, j;

  int max_dist;
  int last_killed = 0;
  int n_arcs = 0;
  int can_move;
	
  float arcdir;
	
  char buf[50];
	
  expired_ms = 0;
  LoadingScreen (0, 0.0);

  if (fname[0] != '\0')
    LoadGame (fname);

  RandomGenerateMap ();
  InitEnemies ();
  InitBossVars ();

  PlayerDefaultStats ();

  if (game_load)
    {
      first_game = 0;
      read_player_data (&save, &player);
      /* Paint (rooms[0].x+1, rooms[0].y+1, rooms[0].w-2, rooms[0].h-2, "dat/d/fbossroom.loc"); */
    }
  else
    {
      /* Centre the player. */
      player.x = map.w * 32 / 2 - PLAYER_W/2;
      player.y = map.h * 32 / 2 - PLAYER_H/2;
    }

  InitAutomap ();

  if (game_load)
    CloseFile ();

  max_dist = 0;
  for (i = 0; i < NUM_ROOMS; i++)
    {
      if (rooms[i].s_dist > max_dist)
        max_dist = rooms[i].s_dist;
    }
	
  game_running = 1;
  while (game_running)
    {
      if (!game_paused)
        {
          /* Handle HP changes. */
          if (player_dying > 30)
            kill_player ();
          
        }
		
      circuit_size = 250 + 50 * (player.circuit_charge + player.circuit_refill);

      if (magic_circuit > 0)
        {
          circuit_range = ((sqrt (magic_circuit + 1) * 6 
                            + MIN(magic_circuit / 2, 50)) * 1.66);

          if (artifacts[ART_CIRCUIT_BOOSTER])
            circuit_range += circuit_range / 2.4;

        }
      else
        circuit_range = -1;

      player_room = GetRoom (player.x/32, player.y/32);

      /* Player moved. */
      if (player_room != prv_player_room) 
        {
          SetTonedPalette ((float) rooms[player_room].s_dist
                           / (float) max_dist);

          prv_player_room = player_room;
          RecordRoom (player_room);
			
          enter_room_x = player.x;
          enter_room_y = player.y;
						
          if (rooms[player_room].room_type == ROOM_BOSS) {
            lock_doors (player_room);
            BossRoom (player_room);
          }
          if ((rooms[player_room].checkpoint
               || player_room == 0)
              && !artifacts[ART_CURSED_SEAL])
            {
              checkpoint_x = (rooms[player_room].x * 32
                              + (rooms[player_room].w / 2 * 32) + 8);
              checkpoint_y = (rooms[player_room].y * 32
                              + (rooms[player_room].h / 2 * 32) + 4);
            }

          if (rooms[player_room].visited == 0)
            {
              rooms[player_room].visited = 1;
              explored++;
            
              /** Place the Agate Knife in the last room if not found yet. */
              if (explored == NUM_ROOMS)
                agate_knife_loc = player_room;
				
              ActivateRoom (player_room);
            }
        }
    
      if (last_killed != killed_enemies)
        {
          SetTonedPalette ((float)rooms[player_room].s_dist / (float)max_dist);
          last_killed = killed_enemies;
        }
      else if ((player_room == 0) && (artifacts[ART_CURSED_SEAL] == 1))
        SetTonedPalette(0);
    

      if (!map_enabled)
        {
          ScrollTo (player.x + PLAYER_W/2 - (SCREEN_W / 2), 
                    player.y + PLAYER_H/2 - (SCREEN_H / 2));
      
          DrawLevel (scroll_x, scroll_y, 1, 1);
          /*DrawLevel (player.x + 8 - 320, player.y + 12 - 240); */
	
          if (player_dying == 0)
            {
              DrawShield ();
				
              if (magic_circuit > 0 && circuit_release == 0)
                {
                  arcdir = RandomDir ();
                  n_arcs = (1 + (circuit_size / 200 + 2)
                            * magic_circuit / circuit_size);

                  for (i = 0; i < n_arcs; i++)
                    {
                      Arc (screen,
                           player.x - scroll_x + PLAYER_W/2,
                           player.y - scroll_y + PLAYER_H/2,
                           circuit_range, arcdir);
                  
                      arcdir += (float)((rand () % 16)
                                        / 16.0 * (M_PI * 2 / (float) n_arcs));
                    }
                }
			
              DrawPlayer ((SCREEN_W / 2) - (PLAYER_W / 2), 
                          (SCREEN_H / 2) - (PLAYER_H / 2),
                          player_dir, player_wlk / wlk_wait);
            }
          else
            {
              if (t % 2 == 0)
                DrawPlayer ((SCREEN_W / 2) - (PLAYER_W / 2), 
                            (SCREEN_H / 2) - (PLAYER_H / 2),
                            player_dir, player_wlk / wlk_wait);
	
              if (!game_paused)
                player_dying++;
            }

          t++;

          if (boss_fight_mode != 0
              && boss_fight_mode < 23
              && !game_paused)
            BossControl ();

          DrawEntities ();

          if (!game_paused)
            MoveEntities ();
			
          if (boss_fight_mode == BSTA_FIGHTING)
            DrawBossHP (100);
			
          if ((rooms[player_room].room_type == 5)
              || (rooms[player_room].room_type == 6
                  && current_boss == BOSS_FINAL)
              || (rooms[player_room].room_type == 4
                  && (player_room % (NUM_ROOMS / NUM_REGULAR_BOSSES)
                      == (NUM_ROOMS / NUM_REGULAR_BOSSES) - 1)))
            DrawPowerObject ();

          /* Player finds the Agate Knife */
          if (player_room == agate_knife_loc)
            {
              SDL_Rect draw_to;
              int xpos, ypos;
              int room_w, room_h;
              int room_x, room_y;
              static float agate_t = 0.0;
              static SDL_Surface *agate_knife = NULL;
					
              room_x = rooms[player_room].x * 32 + 32;
              room_y = rooms[player_room].y * 32 + 32;
              room_w = rooms[player_room].w * 32 - 64;
              room_h = rooms[player_room].h * 32 - 64;
					

              if (agate_knife == NULL)
                {
                  agate_knife = IMG_Load (IMG_AGATE);
                  SDL_SetColorKey (agate_knife, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
                }

              xpos = (int)((sin (agate_t * 1.33)*0.5+0.5) 
                           * (float)room_w) + room_x;
              ypos = (int)((cos (agate_t * 0.7)*0.5+0.5)
                           * (float)room_h) + room_y;
					
              if (dist(player.x, player.y, xpos, ypos) < 20)
                {
                  agate_knife_loc = -1;
                  specialmessage = 50;
                  specialmessagetimer = 150;
                  SND_Pos ("dat/a/crystal2.wav", 128, 0);
				
                  player.has_agate_knife = 1;
                  player.reflect_shield  = 30;
                  player.circuit_charge  = 30;
                  player.circuit_refill  = 30;
                  player.hp = 6;
                }

              draw_to.x = xpos - 16 - scroll_x;
              draw_to.y = ypos - 16 - scroll_y;
					
              SDL_BlitSurface (agate_knife, NULL, screen, &draw_to);
          
              agate_t += 0.05;
            }

          if (opening_door_i > 0)
            {
              DrawArtifactOverhead (opening_door_n);

              for (i = 0; i < 5; i++)
                {
                  j = i * 50 - 250 + (opening_door_i * 5);
              
                  if (j > 0)
                    DrawCircle (player.x - scroll_x, player.y - scroll_y, j, 255);
                }
				
              if (!game_paused)
                {
                  opening_door_i++;

                  if (opening_door_i >= 100)
                    {
                      opening_door_i = 0;
                      Put (opening_door_x, opening_door_y,
                           Get (opening_door_x, opening_door_y) - 38 + 13,
                           GetRoom (opening_door_x, opening_door_y));
                    }
                }
            }
	
          if (circuit_release > 0)
            {
              DrawCircle (release_x - player.x + 320,
                          release_y - player.y + 240,
                          circuit_release * release_range / 20,
                          sin ((float) circuit_release / 20.0) * 127 + 127);
          
              if (!game_paused)
                {
                  CircuitBullets (release_x, release_y,
                                  circuit_release * release_range / 20);
            
                  /*HurtEnemies(release_x, release_y, circuit_release * release_range / 20, release_str);*/
            
                  circuit_release += 2;
		
                  if (circuit_release > 24)
                    {
                      circuit_release = 0;
                      HurtEnemies (release_x, release_y,
                                   release_range, release_str);

                      if (boss_fight_mode == BSTA_FIGHTING)
                        TryHurtBoss (release_x, release_y,
                                     release_range, release_str);
                    }
                }
            }
	
          if (!game_paused)
            {
              if (shield_hp < player.reflect_shield)
                {
                  shield_recover += (player.reflect_shield * 3 
                                     / (3 - training
                                        - player.has_agate_knife));

                  if (artifacts[ART_SHIELD_BOOSTER])
                    {
                      shield_recover += (player.reflect_shield * 3
                                         / (3 - training
                                            - (player.has_agate_knife)));
                    }

                  if (shield_recover >= 50)
                    {
                      shield_hp++;
                      shield_recover -= 50 - (player.has_agate_knife) * 25;
                    }
                }
            }
        }
	
      DrawRect (0, 0, 640, 29, 0);
      DrawRect (1, 1, 638, 27, 32);
      DrawRect (2, 2, 636, 25, 64);

      DrawStatusBar (t);

      if (!training)
        {
          buf[0] = 30;

          /* Draw player lives. */
          if (player.lives <= 99)
            {
              if (player.lives < 10)
                sprintf (buf + 1, " %d", player.lives);
              else
                sprintf (buf + 1, "%d", player.lives);
            }
          else
            sprintf (buf + 1, "**");
	
          draw_text (615, 4, buf, 200);

          DrawRect (615, 13, 24, 4, 240);
          DrawRect (616, 14, 22, 2, 0);

          i = (player.lives_part * 22 / 88);

          if (i > 0)
            DrawRect (616, 14, i, 2, 160 + (t % 40));

        }

      if (player.reflect_shield != 30)
        {
          for (i = 0; i < player.hp; i++)
            buf[i] = 3;

          buf[player.hp] = 0;
        }
      else
        {
          for (i = 0; i < (player.hp / 2); i++)
            buf[i] = 3;
      
          if ((player.hp % 2) == 1)
            buf[(player.hp + 1) / 2 - 1] = 2;
        
          buf[(player.hp + 1) / 2] = 0;
        }
	
      draw_text (615, 18 - (5 * training), buf, 200);

      DrawRect (0, 466, 640, 14, 0);
      DrawRect (1, 467, 638, 12, 32);
      DrawRect (2, 468, 636, 10, 64);
		
      DrawCircuit ();
      DrawArtifacts ();
		
      SpecialTile ((player.x + PLAYER_W / 2) / 32,
                   (player.y + PLAYER_H / 2) / 32);

      if (map_enabled)
        DisplayAutomap ();
		
      /* Is this correct? */
      if (boss_fight_mode != 0 
          && boss_fight_mode == 23
          && !game_paused)
        BossControl ();

      if (boss_dlg != 0
          && !game_paused)
        BossDialog ();
		
      if (game_paused
          && !map_enabled
          && !voluntary_exit)
        {
        
          for (i = 0; i < 10; i++)
            DrawRect ((640 - 6 * 8) / 2 - i,
                      (480 - 8) / 2 - i,
                      6 * 8 + 2 * i,
                      8 + 2 * i, 64 - i * 5);
      
          draw_text ((640 - 6 * 8) / 2, (480 - 8) / 2, "Paused", 255);
			
          {
            int t_days;
            int t_hours;
            int t_minutes;
            int t_seconds;
				
            t_seconds = (expired_ms / 1000) % 60;
            t_minutes = ((expired_ms / 1000) / 60) % 60;
            t_hours = (((expired_ms / 1000) / 60) / 60) % 24;
            t_days = (((expired_ms / 1000) / 60) / 60) / 24;
          
            if (t_days > 0)
              sprintf (buf, "%dd %dh %dm %ds",
                       t_days, t_hours, t_minutes, t_seconds);
            else if (t_hours > 0)
              sprintf (buf, "%dh %dm %ds",
                       t_hours, t_minutes, t_seconds);
            else
              sprintf (buf, "%dm %ds",
                       t_minutes, t_seconds);

            draw_text (636 - strlen (buf) * 8, 470, buf, 255);
          }
        }
		
      if (voluntary_exit)
        {
          DrawRect (152, 200, 336, 80, 128);
          DrawRect (160, 208, 320, 64, 64);
          draw_text ((SCREEN_W - 30 * 8) / 2,
                     (SCREEN_H - 8) / 2 - 4,
                     "Are you sure you want to quit?", 255);
          draw_text ((SCREEN_W - 23 * 8) / 2,
                     (SCREEN_H - 8) / 2 + 4,
                     "Press enter to confirm.", 255);
        }
		
      VideoUpdate ();
		
      MusicUpdate ();
		
      EndCycle (0);
		
      can_move = 1;
		
      if (player_dying != 0
          && player.hp <= 1)
        can_move = 0;

      if (rooms[player_room].room_type == 5
          && CanGetArtifact ()
          && Get ((player.x + PLAYER_W / 2) / 32,
                  (player.y + PLAYER_H / 2) / 32) == 42
          && rooms[player_room].enemies == 0)
        can_move = 0;
						
      if (rooms[player_room].room_type == 6
          && CanGetArtifact () 
          && PlayerDist (rooms[player_room].w * 16
                         + rooms[player_room].x * 32,
                         rooms[player_room].h * 16
                         + rooms[player_room].y * 32) < 32
          && rooms[player_room].enemies == 0
          && current_boss == BOSS_FINAL)
        can_move = 0;
					
      if (scroll_home != 0)
        can_move = 0;

      if (boss_fight_mode == BSTA_ENTERING)
        can_move = 0;

      if (boss_fight_mode >= BSTA_DYING)
        can_move = 0;

      if (opening_door_i != 0)
        can_move = 0;
    
      if (game_paused)
        can_move = 0;
		
      HandleEvents();

      if (map_enabled)
        game_paused = 1;
		
      if (can_move)
        {
          float speedmul;

          ix = player.x;
          iy = player.y;
          off_x = 0;
          off_y = 0;

          /* Work out speed. */
          if (artifacts[ART_METABOLISM_ENHANCER])
            speedmul = 1.4;
          else
            speedmul = 1;

          if (key_held[K_UP] && !key_held[K_DN])
            {
              iy -= player_walk_speed * speedmul;
              player_dir = 0;
            }
          
          if (key_held[K_DN] && !key_held[K_UP])
            {
              iy += player_walk_speed * speedmul;
              player_dir = 1;
              off_y = PLAYER_H;
            }
          
          if (key_held[K_LT] && !key_held[K_RT])
            {
              ix -= player_walk_speed * speedmul;
              
              if (!(key_held[K_UP] || key_held[K_DN]))
                player_dir = 3;
            }

          if (key_held[K_RT] && !key_held[K_LT])
            {
              off_x = PLAYER_W;
              ix += player_walk_speed * speedmul;

              if (!(key_held[K_UP] || key_held[K_DN]))
                player_dir = 2;
            }

          /* PSI circuit. */
          if ((key_held[K_SP]) && (magic_circuit >= 0))
            {
              magic_circuit += (player.circuit_charge
                                * (3 + training
                                   + (player.has_agate_knife)) / 3);

            }
          else if (magic_circuit < 0)
            {
              magic_circuit += (player.circuit_refill
                                * (3 + training 
                                   + (player.has_agate_knife)) / 3);
              if (magic_circuit > 0) magic_circuit = 0;
            }
          else if (magic_circuit > 0)
            ReleaseCircuit();

          if (magic_circuit > circuit_size)
            magic_circuit = circuit_size;
			
          if (ix != player.x || iy != player.y)
            {
              /* Are we changing to a new square? */
              if (((player.x / TILE_W) != ((ix + off_x) / TILE_W))
                  || ((player.y / 32) != ((iy + off_y) / TILE_H)))
                {
                  /* printf("%d\n", tile) */;
                  if (TouchTile (ix, iy))
                    player_wlk = ((player_wlk + 1
                                   + (artifacts[ART_METABOLISM_ENHANCER]
                                      * 3))
                                  % (4 * wlk_wait));          
                  else
                    {
                      if (TouchTile (player.x, iy))
                        player_wlk = ((player_wlk + 1
                                       + (artifacts[ART_METABOLISM_ENHANCER]
                                          * 3))
                                      % (4 * wlk_wait));
                      else if (TouchTile (ix, player.y))
                        {
                          player_wlk = ((player_wlk + 1
                                         + (artifacts[ART_METABOLISM_ENHANCER]
                                            * 3))
                                        % (4 * wlk_wait));

                          if (off_x > 0)
                            player_dir = 2;
                          else
                            player_dir = 3;
                        }					
                    }
                }
              else
                {
                  player.x = ix;
                  player.y = iy;
					
                  player_wlk = ((player_wlk + 1 
                                 + artifacts[ART_METABOLISM_ENHANCER ] * 3)
                                % (4 * wlk_wait));
                }
            }
        }

      if ((t % (33 * 10)) == (33 * 10 - 1))
        ActivateRand ();
		
      if (voluntary_exit && enter_pressed)
        {
          voluntary_exit = 0;
          game_running = 0;
          game_paused = 0;
        }
		
      if (player.lives == 0 && !training) 
        break;
      
      if (show_ending)
        break;
      
    }
  
  if (show_ending)
    {
      show_ending = 0;
      ShowEnding ();
    }
	
  if (player.lives == 0 && !training)
    {
      SDL_FillRect (screen, NULL, 0);
      draw_text (252, 236, "G A M E   O V E R", 255);
      VideoUpdate ();
      SDL_Delay (2000);
    }
	
  return 0;
}
