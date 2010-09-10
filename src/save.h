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

#include <zlib.h>

typedef struct SaveFile SaveFile;

struct SaveFile
{
  gzFile *file;   /**< Actual file pointer. */
  int position;   /**< Position in file. */
  char last_char; /**< Last character written to file. */
};

extern int game_load;

extern SaveFile save;

void
DoSaveGame (void);

/** Read 8 bits from a save file.
 *
 *  @param f  The SaveFile to read from.
 *
 *  @return   The byte read from the file.
 */

unsigned char
save_file_read_char (SaveFile *f);

/** Write 8 bits to a save file.
 *
 *  @param f  The SaveFile to write to.
 *  @param i  The byte, in unsigned char format, to write.
 */

void
save_file_write_char (SaveFile *f, unsigned char i);

/** Read 32 bits from a save file.
 *
 *  @param f  The SaveFile to read from.
 *
 *  @return  The integer read from the file.
 */

int
save_file_read_int (SaveFile *f);

/** Write an integer to the open GZipped file.
 *
 *  @param f  The SaveFile to write to.
 *  @param i  The integer value to write.
 */
void save_file_write_int (SaveFile *f, int i);


/** Read a floating point number from the open GZipped file.
 *
 *  @param f  The SaveFile to read from.
 *
 *  @return  The float read from the file.
 */
float save_file_read_float (SaveFile *f);


/** Write a floating point number to the open GZipped file.
 *
 *  @param f  The SaveFile to write to.
 *  @param i  The float to write to the file.
 */
void save_file_write_float (SaveFile *f, float i);

/* Deprecation block */

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
