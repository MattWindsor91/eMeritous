/**************************************************************************
 *  screens.h                                                             *
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

#ifndef __SCREENS_H__
#define __SCREENS_H__

/** Display the development team logo. */

void LogoScreen(void);

/** Display a progress bar screen.
 *
 *  @param part     ???
 *  @param progress ???
 *  @param message  The message to display on the progress bar.
 *  @param t_parts  ???
 */
void ProgressBarScreen(int part, float progress, char *message, float t_parts);

/** Display a loading screen.
 *
 *  @param part     ???
 *  @param progress ???
 */
void LoadingScreen(int part, float progress);

/** Display a saving screen.
 *
 *  @param part     ???
 *  @param progress ???
 */
void SavingScreen(int part, float progress);

#endif /* __SCREENS_H__ */
