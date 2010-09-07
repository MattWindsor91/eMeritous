/**************************************************************************
 *  player.h                                                              *
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

/* Not to be confused with PLAYER_H! */
#ifndef __PLAYER_H__
#define __PLAYER_H__

enum {
  ART_MAP                 = 0,  /**< ID of Complete Map artifact. */
  ART_SHIELD_BOOSTER      = 1,  /**< ID of Shield Booster artifact. */
  ART_CRYSTAL_PURIFIER    = 2,  /**< ID of Crystal Purifier
                                   artifact. */
  ART_CIRCUIT_BOOSTER     = 3,  /**< ID of Circuit Booster artifact. */
  ART_METABOLISM_ENHANCER = 4,  /**< ID of Metabolism Enhancer
                                   artifact. */
  ART_DODGE_ENHANCER      = 5,  /**< ID of Dodge Enhancer artifact. */
  ART_ETHEREAL_MONOCLE    = 6,  /**< ID of Ethereal Monocle
                                   artifact. */
  ART_CRYSTAL_SUMMONER    = 7,  /**< ID of Crystal Summoner
                                   artifact. */
  ART_HOLY_SWORD          = 8,  /**< ID of Holy Sword PSI key
                                   artifact. */
  ART_MYSTIC_HALBERD      = 9,  /**< ID of Mystic Halberd PSI key
                                   artifact. */
  ART_DIVINE_BOW          = 10, /**< ID of Divine Bow PSI key
                                   artifact. */
  ART_CURSED_SEAL         = 11, /**< ID of Cursed Seal artifact. */

  UP_REFLECT_SHIELD = 0,    /**< ID of Reflect Shield upgrade. */
  UP_CIRCUIT_CHARGE = 1,    /**< ID of Circuit Charge upgrade. */
  UP_CIRCUIT_REFILL = 2,    /**< ID of Circuit Refill upgrade. */
  INITIAL_LIVES = 5,  /**< Amount of lives to start the game with. */
  INITIAL_HP    = 3,  /**< Amount of hitpoints (hearts) to start the
                         game with. */
  MAX_UPGRADE   = 24, /**< Maximum amount that upgradeables can be
                         upgraded to. */
  LIFE_PART_MAX = 88  /**< If more life parts are collected than this
                         figure, a new life will be gained. */
};

/** A structure to hold player data. */
struct PlayerData {
  int x;               /**< X co-ordinate. */
  int y;               /**< Y co-ordinate. */
  int lives;           /**< Lives remaining. */
  int lives_part;      /**< Part of life. */
  int hp;              /**< Number of hearts (hitpoints). */
  int crystals;        /**< Number of PSI crystals. */
  int reflect_shield;  /**< Reflect shield strength. */
  int circuit_charge;  /**< Rate of circuit charge. */
  int circuit_refill;  /**< Rate of circuit refill. */
  int has_agate_knife; /**< Whether or not the player has the agate
                          knife. */
};

extern struct PlayerData player;

/* Global variables (woo) */

extern int release_range;
extern int checkpoints_found;
extern int first_game;
extern int explored;
extern int voluntary_exit;
extern int player_dir;
extern int player_wlk;
extern int player_walk_speed;
extern int wlk_wait;
extern int circuit_release;
extern int scroll_home;
extern int opening_door_i;
extern int map_enabled;
extern int expired_ms;
extern int current_boss;
extern int agate_knife_loc;


/** Initialise the player information to defaults. */
void PlayerDefaultStats(void);


/** Read player data from the opened data file.
 *
 *  @param p  Pointer to the player data structure.
 */
void read_player_data(struct PlayerData *p);


/** Write player data to the opened data file.
 *
 *  @param p  Pointer to the player data structure.
 */
void write_player_data(struct PlayerData *p);


/** Calculate the price of an upgrade.
 *
 *  @param type  The type of upgrade.
 *
 *  @return the price of an upgrade, in crystals.
 */
int UpgradePrice(int type);

#endif /* __PLAYER_H__ */
