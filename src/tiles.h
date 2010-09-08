/**************************************************************************
 *  tiles.h                                                               *
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

#ifndef __TILES_H__
#define __TILES_H__

enum
  {
    /* Tile types. */
    TILE_CHECKPOINT = 25, /**< A checkpoint tile. */
    TILE_CHEST      = 26, /**< A treasure chest tile. */
    TILE_CHEST_OPEN = 27, /**< An opened treasure chest tile. */
    TILE_SHIELD_UP  = 28, /**< A shield upgrade tile. */
    TILE_CHARGE_UP  = 29, /**< A circuit charge upgrade tile. */
    TILE_REFILL_UP  = 30, /**< A circuit refill upgrade tile. */
    TILE_SAVE       = 31, /**< A save point tile. */
    TILE_CRYSTAL    = 32  /**< A crystal machine tile. */
  };

struct TileInfo
{
  int Is_Solid;
  int Is_Special;
  int Is_Passage;
  int Is_Wall;
  int Type;
};

extern struct TileInfo TileData[57];

extern unsigned char automap_cols[10];

#endif /* __TILES_H__ */

