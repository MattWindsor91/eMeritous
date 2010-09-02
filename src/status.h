/**************************************************************************
 *  status.h                                                              *
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

#ifndef __STATUS_H__
#define __STATUS_H__

/* Global variables (TODO: remove/rearrange? */

extern int player_gems;
extern int killed_enemies;
extern int total_enemies;

/** Draw the overhead status bar.
 *
 *  If the player is currently using a teleporter, the status bar 
 *  will be replaced instead with help text. 
 *
 *  @param t  Time integer used for glows.
 */
void DrawStatusBar(int t);

#endif /* __STATUS_H__ */
