/**************************************************************************
 *  save.c                                                                *
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

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <zlib.h>

#include "player.h"
#include "screens.h"
#include "files.h"
#include "levelblit.h"
#include "mapgen.h"
#include "demon.h"
#include "tiles.h"

gzFile Filefp;

SaveFile save;

int game_load = 0;

unsigned char lchar;
int fpos = 0;

/* Read a byte from the open GZipped file. */
unsigned char
save_file_read_char (SaveFile *f)
{
  unsigned char c;

  c = gzgetc (f->ptr);
  c ^= 0x55;
  c ^= f->pos & 0xFF;

  lchar = c;
  f->pos++;
  return c;
}


/* Read a byte from the open GZipped file. */
void
save_file_write_char (SaveFile *f, unsigned char i)
{
  unsigned char c;

  c = i;
  c ^= 0x55;
  c ^= f->pos & 0xFF;
  gzputc (f->ptr, c);

  lchar = c;
  f->pos++;
}


/* Read an integer from the open GZipped file. */
int
save_file_read_int (SaveFile *f)
{
  int val;
  int i, s;

  i =  save_file_read_char (f);
  i |= save_file_read_char (f) << 8;
  i |= save_file_read_char (f) << 16;
  i |= save_file_read_char (f) << 24;
  s  = save_file_read_char (f);

  val = i * (s ? -1 : 1);

  return val;
}


/* Write an integer from the open GZipped file. */
void
save_file_write_int (SaveFile *f, int val)
{
  int i, s;
  i = abs (val);
  s = (val >= 0) ? 0 : 1;

  save_file_write_char (f, (i & 0xFF) >> 0);
  save_file_write_char (f, (i & 0xFF00) >> 8);
  save_file_write_char (f, (i & 0xFF0000) >> 16);
  save_file_write_char (f, (i & 0xFF000000) >> 24);

  save_file_write_char (f, s);
}


/* Read a floating point number from the open GZipped file. */
float
save_file_read_float (SaveFile *f)
{
  float i;
  int num;
  int frac;

  double f_frac;

  num  = save_file_read_int (f);
  frac = save_file_read_int (f);

  f_frac = (double)frac / INT_MAX;

  i = (float) num + (float) f_frac;

  return i;
}


/* Write a floating point number to the open GZipped file. */
void
save_file_write_float (SaveFile *f, float i)
{
  int num;
  int frac;

  num = (int) (floorf (i));
  save_file_write_int (f, num);
  frac = (int) ((i - (float) num) * INT_MAX);

  save_file_write_int (f, frac);
}


/***************************
 * BEGIN DEPRECATION BLOCK *
 ***************************/

/* Read a byte from the open GZipped file. */
unsigned char FRChar(void)
{
  return save_file_read_char(&save);
}


/* Read a floating point number from the open GZipped file. */
void FWChar(unsigned char i)
{
  save_file_write_char(&save, i);
}


/* Read an integer from the open GZipped file. */
int FRInt(void)
{
  int val;
  int i, s;

  i = FRChar();
  i |= FRChar() << 8;
  i |= FRChar() << 16;
  i |= FRChar() << 24;
  s = FRChar();
  val = i * (s?-1:1);

  return val;
}


/* Write an integer from the open GZipped file. */
void FWInt(int val)
{
  int i, s;
  i = abs(val);
  s = (val >= 0) ? 0 : 1;

  FWChar((i & 0xFF) >> 0);
  FWChar((i & 0xFF00) >> 8);
  FWChar((i & 0xFF0000) >> 16);
  FWChar((i & 0xFF000000) >> 24);

  FWChar(s);
}


/* Read a floating point number from the open GZipped file. */
float FRFloat(void)
{
  float i;
  int num;
  int frac;

  double f_frac;

  num = FRInt();
  frac = FRInt();

  f_frac = (double)frac / 2147483647.0;

  i = (float)num + (float)f_frac;
  return i;
}


/* Write a floating point number to the open GZipped file. */
void FWFloat(float i)
{
  int num;
  int frac;

  num = (int)(floorf(i));
  FWInt(num);
  frac = (int)((i - (float)num)*2147483647.0);

  FWInt(frac);
}

/*************************
 * END DEPRECATION BLOCK *
 *************************/

void
SaveGame (const char *filename)
{
  lchar = 0x7c;
  fpos = 0;

  save.ptr = Filefp = gzopen (filename, "wb9");
  save.pos = 0;

  save_file_write_char (&save, 0x7C);
  WriteMapData ();
  WriteCreatureData ();
  write_player_data (&save, &player);

  gzclose (Filefp);
}


void
LoadGame (const char *filename)
{
  unsigned char parity;
  fpos = 0;
  lchar = 0x7c;

  save.ptr = Filefp = gzopen (filename, "rb");
  save.pos = 0;

  parity = save_file_read_char (&save);
  if (parity != 0x7C) {
    fprintf (stderr, "Parity byte in error (%x != 0x7C)\nAborting\n", parity);
    exit (2);
  }
  game_load = 1;
}

void CloseFile()
{
  gzclose(Filefp);
}

void DoSaveGame()
{
  SavingScreen(0, 0.0);
  SaveGame(SAVE_FILE);
}

int IsSaveFile()
{
  FILE *fp;
  if ((fp = fopen(SAVE_FILE, "rb")) != NULL) {
    fclose(fp);
    return 1;
  }
  return 0;
}
