/**************************************************************************
 *  main.h                                                                *
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

#ifndef __MAIN_H__
#define __MAIN_H__

/* Global variables */

extern int RECORDING;
extern int PLAYBACK;
extern FILE *record_file;
extern int game_running;

extern const char MERITOUS_VERSION[]; /**< Version string. */

/** The main function.
 *
 *  @param argc  Argument count.
 *  @param argv  Argument vector.
 * 
 *  @return Exit status.
 */

int main (int argc, const char **argv);

#endif /* __MAIN_H__ */
