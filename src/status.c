/**************************************************************************
 *  status.c                                                              *
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

#include "mapgen.h"
#include "levelblit.h"
#include "status.h"
#include "player.h"

void DrawStatusBar(int t)
{
  char buf[50];

  if (!tele_select) {
    /* Draw status bar */
    sprintf(buf, "Psi Crystals: %d", player.crystals);
    draw_text(3, 3, buf, 200);

    sprintf(buf, "Explored: %.1f%% (%d/%d rooms)", 
            (float)explored / (float)NUM_ROOMS * 100.0,
            explored, NUM_ROOMS);
    draw_text(3, 11, buf, 200);

    sprintf(buf, "Cleared: %.1f%% (%d/%d monsters)", 
            (float)killed_enemies / (float)total_enemies * 100.0,
            killed_enemies, total_enemies);
    draw_text(3, 19, buf, 200);
			
    /* The next three strings have a time-based glowing colour effect 
       if they can be upgraded (hence the ternary expression for colour). */

    draw_text(316, 3, "Reflect shield", 
              (player.crystals >= UpgradePrice(UP_REFLECT_SHIELD)) && 
              (player.reflect_shield != 30) ? (231 + (t % 13) * 2) : 200);
    DrawMeter(434, 3, player.reflect_shield);
			
    draw_text(316, 11, "Circuit charge",
              (player.crystals >= UpgradePrice(UP_CIRCUIT_CHARGE)) && 
              (player.circuit_charge != 30) ? (231 + (t % 13) * 2) : 200);
    DrawMeter(434, 11, player.circuit_charge);
			
    draw_text(316, 19, "Circuit refill",
              (player.crystals >= UpgradePrice(UP_CIRCUIT_REFILL)) &&
              (player.circuit_refill != 30) ? (231 + (t % 13) * 2) : 200);
    DrawMeter(434, 19, player.circuit_refill);
			
  } else {
    draw_text(80, 11-6, "Use the movement keys to locate a checkpoint. Press ENTER to",
              240);
    draw_text(52, 11+6, "teleport to this checkpoint. Press ESCAPE or TAB once you are done.",
              240);
  }
		
}
