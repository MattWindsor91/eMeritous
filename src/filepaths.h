/**************************************************************************
 *  filepaths.h                                                           *
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

#ifndef __FILEPATHS_H__
#define __FILEPATHS_H__

/* Some filepath-related enums. */
enum {
#ifdef MAX_PATH
  FN_BUFFER_LEN = MAX_PATH /**< Length of the file name composition buffer. */
#else /* !MAX_PATH */
  FN_BUFFER_LEN = 4096 /**< Length of the file name composition buffer. */
#endif /* MAX_PATH */
};

/* Global prefix. */
#ifndef PREFIX
#define PREFIX ""
#endif /* PREFIX */

/* Resource directory (subdirectory of prefix) */
#ifndef RES_DIR
#define RES_DIR "dat/"
#endif /* RES_DIR */

/* Audio directory (subdirectory of RES_DIR) */
#ifndef AUDIO_DIR
#define AUDIO_DIR "a/"
#endif /* AUDIO_DIR */

#endif /* __FILEPATHS_H__ */
