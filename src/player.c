/**************************************************************************
 *  player.c                                                              *
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

#include "player.h"
#include "mapgen.h"
#include "levelblit.h"
#include "save.h"

/* This is the player instance the rest of the game uses. */
struct PlayerData player;


/* Read player data from the opened data file.*/
void read_player_data(struct PlayerData *p)
{
  int i;

  expired_ms = FRInt();
  p->x = FRInt();
  p->y = FRInt();
  scroll_x = FRInt();
  scroll_y = FRInt();
  magic_circuit = FRInt();
  checkpoint_x = FRInt();
  checkpoint_y = FRInt();
  player_walk_speed = FRInt();
  wlk_wait = FRInt();
  p->circuit_charge = FRInt();
  p->circuit_refill = FRInt();
  explored = FRInt();
  p->reflect_shield = FRInt();
  shield_recover = FRInt();
  shield_hp = FRInt();
  p->crystals = FRInt();
  checkpoints_found = FRInt();
  p->hp = FRInt();
  p->lives = FRInt();
  p->lives_part = FRInt();
  current_boss = FRInt();
  training = FRInt();

  agate_knife_loc = FRInt();

  for (i = 0; i < 12; i++) {
    artifacts[i] = FRChar();
  }

  /* In previous versions, whether or not the Agate Knife was held by 
     the player was designated by the player reflect shield being 30. 
     In order to maintain save compatibility, we check this here 
     and set the new Agate Knife flag. */

  if (p->reflect_shield == 30) {
    p->has_agate_knife = 1;
  } else {
    p->has_agate_knife = 0;
  }
}


/** Write player data to the opened data file. */
void write_player_data(struct PlayerData *p)
{
  int i;

  FWInt(expired_ms);
  FWInt(checkpoint_x);
  FWInt(checkpoint_y);
  FWInt(scroll_x);
  FWInt(scroll_y);
  FWInt(magic_circuit);
  FWInt(checkpoint_x);
  FWInt(checkpoint_y);
  FWInt(player_walk_speed);
  FWInt(wlk_wait);
  FWInt(p->circuit_charge);
  FWInt(p->circuit_refill);
  FWInt(explored);
  FWInt(p->reflect_shield);
  FWInt(shield_recover);
  FWInt(shield_hp);
  FWInt(p->crystals);
  FWInt(checkpoints_found);
  FWInt(p->hp);
  FWInt(p->lives);
  FWInt(p->lives_part);
  FWInt(current_boss);
  FWInt(training);
  FWInt(agate_knife_loc);

  for (i = 0; i < 12; i++) {
    FWChar(artifacts[i]);
  }
}



void PlayerDefaultStats(void)
{
  int i;

  player.x = map.w * 32 / 2 - PLAYER_W/2;
  player.y = map.h * 32 / 2 - PLAYER_H/2;
	
  player_dying = 0;
  magic_circuit = 0;
  circuit_range = 100;
  release_range = 100;
  shield_hp = 0;
  shield_recover = 0;
  player.crystals = 0;
  checkpoints_found = 0;
  circuit_size = 1000;
  first_game = 1;
  player.hp = INITIAL_HP;
	
  explored = 0;
	
  voluntary_exit = 0;
  player_room = 0;
  player_dir = 0;
  player_wlk = 0;
  player_walk_speed = 5;
  player.lives = INITIAL_LIVES;
  player.lives_part = 0;
  wlk_wait = 8;
  circuit_release = 0;
  scroll_home = 0;
  enter_pressed = 0;
  show_ending = 0;
	
  game_paused = 0;

  /* Initial upgrades. */
  player.reflect_shield = 0;
  player.circuit_charge = 2;
  player.circuit_refill = 3;
	
  prv_player_room = -1;

  specialmessage = 0;
  specialmessagetimer = 0;
	
  opening_door_i = 0;

  map_enabled = 0;
	
  for (i = 0; i < 12; i++) {
    artifacts[i] = 0;
  }
	
#ifdef DEBUG_STATS
	
  player.reflect_shield = player.circuit_charge = player.circuit_refill = 24;
	
  for (i = 0; i < 12; i++) {
    artifacts[i] = 1;
  }
	
#endif
}

int UpgradePrice(int type)
{
  int price;

  switch (type) {
  case UP_REFLECT_SHIELD:
    price = ((100 - (training * 50)) * player.reflect_shield + 
             (5 << player.reflect_shield) * (5 - training * 2));
    break;
  case UP_CIRCUIT_CHARGE:
    price = ((80 - (training * 40)) * player.circuit_charge + 
             (5 << player.circuit_charge) * (4 - training * 2));
    break;
  case UP_CIRCUIT_REFILL:
    price = ((80 - (training * 40)) * player.circuit_refill + 
             (5 << player.circuit_refill) * (4 - training * 2));
    break;
  default:
    price = 123;
    break;
  }

  return price;
}
