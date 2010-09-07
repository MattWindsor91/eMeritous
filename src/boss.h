/**************************************************************************
 *  boss.h                                                                *
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

#ifndef __BOSS_H__
#define __BOSS_H__

enum {
  NUM_REGULAR_BOSSES = 3, /**< Number of regular bosses.
                             This is used, for example, to calculate 
                             where to put the bosses.
                             There should be one per PSI key. */

  /* Boss battle states. */

  BSTA_NONE     = 0, /**< No boss fight. */
  BSTA_ENTERING = 1, /**< Entering a boss room. */
  BSTA_FIGHTING = 2, /**< Fighting a boss. */
  BSTA_DYING    = 3, /**< Boss is dying. */

  /* Boss IDs. */

  BOSS_MERIDIAN = 0, /**< Meridian's boss ID. */
  BOSS_ATARAXIA = 1, /**< Ataraxia's boss ID. */
  BOSS_MERODACH = 2, /**< Merodach's boss ID. */
  BOSS_FINAL    = 3  /**< Final boss ID. */
};

extern int boss_fight_mode, current_boss_room;
extern const char *boss_names[];
extern const char *artifact_names[];
extern int current_boss;
extern int boss_lives;

void DrawPowerObject();
void DrawArtifactOverhead(int p_obj);
int CanGetArtifact();

void BossRoom(int room);
void BossControl();

void TryHurtBoss(int x, int y, int range, int power);

void InitBossVars();

void DrawBossHP(int bar_length);

int PDist(int x1, int y1, int x2, int y2);

extern int proxy_seek;

extern int boss_dlg;

extern int resetboss;

void BossDialog();

#endif /* __BOSS_H__ */
