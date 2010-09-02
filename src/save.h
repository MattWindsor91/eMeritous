/**************************************************************************
 *  save.h                                                                *
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

#ifndef __SAVE_H__
#define __SAVE_H__

extern int game_load;

void DoSaveGame(void);


/** Read a byte from the open GZipped file.
 *
 *  @return  The byte read from the file.
 */
unsigned char FRChar(void);


/** Write a byte to the open GZipped file.
 *
 *  @param i  The byte, in unsigned char format, to write.
 */
void FWChar(unsigned char i);


/** Read an integer from the open GZipped file.
 *
 *  @return  The integer read from the file.
 */
int FRInt(void);


/** Write an integer to the open GZipped file.
 *
 *  @param val  The integer value to write.
 */
void FWInt(int val);


/** Read a floating point number from the open GZipped file.
 *
 *  @return  The float read from the file.
 */
float FRFloat(void);


/** Write a floating point number to the open GZipped file.
 *
 *  @param i  The float to write to the file.
 */
void FWFloat(float i);


void SaveGame(const char *filename);
void LoadGame(const char *filename);
void CloseFile();

int IsSaveFile();

#endif /* __SAVE_H__ */
