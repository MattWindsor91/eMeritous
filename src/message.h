/**************************************************************************
 *  message.h                                                             *
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

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

enum {
  /* Special message IDs */
  /* Message IDs 1 to 8 are retrieval messages for artifacts 0-7. */
  SM_COMMON_ARTIFACT    = 1,  /**< Start of common artifact special
                                 message range. */
  SM_SHIELD_UPGRADE     = 10, /**< Special message for reflect shield
                                 upgrade. */
  SM_CHARGE_UPGRADE     = 11, /**< Special message for circuit charge
                                 upgrade. */
  SM_REFILL_UPGRADE     = 12, /**< Special message for circuit refill
                                 upgrade. */
  SM_PSI_CRYSTALS       = 20, /**< Special message for PSI crystal
                                 reward. */
  SM_HOLY_SWORD_ACQ     = 30, /**< Special message for holy sword
                                 acquisition. */
  SM_MYSTIC_HALBERD_ACQ = 31, /**< Special message for mystic halberd
                                 acquisition. */
  SM_DIVINE_BOW_ACQ     = 32, /**< Special message for divine bow
                                 acquisition. */
  SM_CURSED_SEAL_ACQ    = 33, /**< Special message for cursed seal
                                 acquisition. */
  SM_HOLY_SWORD_RET     = 40, /**< Special message for holy sword
                                 return. */
  SM_MYSTIC_HALBERD_RET = 41, /**< Special message for mystic halberd
                                 return. */
  SM_DIVINE_BOW_RET     = 42, /**< Special message for divine bow
                                 return. */
  SM_AGATE_KNIFE_ACQ    = 50  /**< Special message for Agate Knife
                                 acquisition. */
};

extern const char *special_messages[]; /**< Special message matrix. */

#endif /* __MESSAGE_H__ */
