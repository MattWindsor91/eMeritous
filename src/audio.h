/**************************************************************************
 *  audio.h                                                               *
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

#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "filepaths.h"

enum {
  /* Music tracks. */
  BGM_INNER          = 0,  /**< Normal Inner Ring BGM. */
  BGM_MIDWAY         = 1,  /**< Normal Midway Ring BGM. */
  BGM_OUTER          = 2,  /**< Normal Outer Ring BGM. */
  BGM_EDGE           = 3,  /**< Normal Edge Ring BGM. */
  BGM_ART_CHALLENGE  = 4,  /**< Artifact Challenge BGM. */ 
  BGM_TITLE          = 5,  /**< Title theme BGM. */
  BGM_CURSED_SEAL    = 6,  /**< Post-Cursed Seal BGM. */
  BGM_MERIDIAN       = 7,  /**< Meridian boss BGM. */
  BGM_ATARAXIA       = 8,  /**< Meridian boss BGM. */
  BGM_MERODACH       = 9,  /**< Meridian boss BGM. */
  BGM_FINAL          = 10, /**< Final boss BGM. */
  BGM_FINAL_AGATE    = 12, /**< True final boss BGM. */
  BGM_FINAL_CRITICAL = 7,  /**< True final boss critical BGM. */
  BGM_FINAL_DEAD     = 11  /**< Final boss victory music. */
};

/* Enemy sound information */
extern const int ENEMY_SND_VOLS[];   /**< Matrix of enemy sound volumes. */
extern const int ENEMY_SND_DELAYS[]; /**< Matrix of enemy sound delays. */

void InitAudio();
void MusicUpdate();
void TitleScreenMusic();
void SND_CircuitRelease(int str);

/** Get an available mixer channel.
 *
 *  @return  -1 for failure; otherwise, the ID of the channel retrieved, 
 *           which can then be used by play_sound_on_channel.
 */
int
get_mixer_channel(void);

/** Play a sound on the specified mixer channel.
 *
 *  @warning  This function currently assumes that the requested 
 *            mixer channel is empty.  This may change in the future.
 *
 *  @param filename  Name of the sound file to play.
 *  @param volume    Volume of the sound.
 *  @param channel   ID of the channel to play the file on (0-15).
 *  @param loops     How many times the sound should loop 
 *                   (0 for no loops; -1 for indefinite looping)
 */

void
play_sound_on_channel (const char *filename, int volume,
                       int channel, int loops);

/** Play a sound.
 *
 *  @param filename  Name of the sound file to play.
 *  @param volume    Volume of the sound.
 */

void
play_sound (const char *filename, int volume);

/** Play a sound, adjusting the volume to account for distance. 
 *
 *  @param filename  Name of the sound file to play.
 *  @param volume    Unadjusted volume of the sound.
 *  @param distance  Distance of the origin of the sound from the 
 *                   player.
 */

void
play_positioned_sound (const char *filename, int volume, int distance);

/** Play the circuit release sound. 
 *
 *  The sound volume is determined by the given circuit strength.
 *
 *  @param str  Strength of the circuit.
 */

void
play_circuit_release_sound (int strength);

/* Sound file names (SND) and printf formats (SNDF).*/

extern const char SND_CIRCUIT_CHARGE[]; /**< Circuit charge sound. */
extern const char SND_CIRCUIT_RECOVER[]; /**< Circuit recover
                                            sound. */
extern const char SND_CIRCUIT_RELEASE[]; /**< Circuit release
                                            sound. */

extern const char SND_GET_HP[]; /**< Player gets a heart sound. */
extern const char SND_GET_LIFE[]; /**< Player gets a life sound. */
extern const char SND_GET_LIFE_PART[]; /**< Player gets a life part
                                          sound. */
extern const char SND_GET_PSI_KEY[]; /**< Player gets a PSI key
                                        sound. */
extern const char SND_GET_AGATE_KNIFE[]; /**< Player gets the Agate
                                            Knife sound. */

extern const char SND_UPGRADE[]; /**< Player upgrades a skill
                                    sound. */

extern const char SND_CRYSTAL_SUMMON[]; /**< Player uses a
                                           crystal machine sound. */

extern const char SND_PLAYER_HURT[]; /**< Player hurt sound. */
extern const char SND_SHIELD_HIT[]; /**< Shield hit sound. */

extern const char SND_ENEMY_HIT[]; /**< Enemy hit sound. */

extern const char SNDF_ENEMY_SHOT[]; /**< Enemy shot sound (printf
                                        format). */

#endif /* __AUDIO_H__ */
