/**************************************************************************
 *  audio.c                                                               *
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string.h>

#include "audio.h"
#include "dungeon.h"
#include "filepaths.h"
#include "player.h"
#include "levelblit.h"
#include "mapgen.h"
#include "boss.h"

const int ENEMY_SND_VOLS[11] = {48, 
                                112,
                                110,
                                110,
                                110,
                                80,
                                110,
                                110,
                                110,
                                110,
                                110};

const int ENEMY_SND_DELAYS[11] = {200, 
                                  500,
                                  1000,
                                  500,
                                  900,
                                  60,
                                  1000,
                                  600,
                                  700,
                                  242,
                                  250};

const char *tracks[13] = {"ICEFRONT.S3M",
                          "cavern.xm",
                          "cave.xm",
                          "cave06.s3m",
                          "Wood.s3m",
                          "iller_knarkloader_final.xm",
                          "fear2.mod",
                          "Cv_boss.mod",
                          "Fr_boss.mod",
                          "CT_BOSS.MOD",
                          "rpg_bat1.xm",
                          "amblight.xm",
                          "FINALBAT.s3m"};

const char SND_CIRCUIT_CHARGE[]  = "circuitcharge.wav";
const char SND_CIRCUIT_RECOVER[] = "circuitrecover.wav";
const char SND_CIRCUIT_RELEASE[] = "circuitrelease.wav";

const char SND_GET_HP[] = "crystal.wav";
const char SND_GET_LIFE[] = "crystal2.wav";
const char SND_GET_LIFE_PART[] = "tone.wav";

const char SND_PLAYER_HURT[] = "playerhurt.wav";
const char SND_SHIELD_HIT[] = "shieldhit.wav";

const char SND_ENEMY_HIT[] = "enemyhit.wav";
const char SNDF_ENEMY_SHOT[] = "mons%ushot.wav";

Mix_Music *bgm_music = NULL;
int bgm_track = -1;

Mix_Chunk *c_sample[16] = {NULL};

void InitAudio()
{
  Mix_OpenAudio(44100, AUDIO_S16, 2, 4096);
  Mix_VolumeMusic(112);
  Mix_AllocateChannels(16);
}

void BackgroundMusic();
void CircuitHum();

void MusicUpdate()
{
  BackgroundMusic();
  CircuitHum();
}

void SND_Play_Channel(const char *filename, int vol, int ch, int loops)
{
  char buffer[FN_BUFFER_LEN] = "\0";

  strcat(buffer, PREFIX);
  strcat(buffer, RES_DIR);
  strcat(buffer, AUDIO_DIR);
  strcat(buffer, filename);

  printf("%s\n", buffer);

  c_sample[ch] = Mix_LoadWAV(buffer);
  Mix_Volume(ch, vol);
  Mix_PlayChannel(ch, c_sample[ch], loops);
}

void CircuitHum()
{
  int hum_vol = 0;
  static int hum_play = 0;
	
  if (magic_circuit == 0) {
    hum_vol = 0;
    if (hum_play != 0) {
      Mix_HaltChannel(0);
      hum_play = 0;
    }
  }
  if (magic_circuit > 0) {
    hum_vol = (circuit_size * 24 / 3000)+12;
    hum_vol = (magic_circuit * 96 / circuit_size) * hum_vol / 36 + 32;
		
    if (hum_play != 1) {
      if (hum_play == 2) {
        Mix_HaltChannel(0);
        Mix_FreeChunk(c_sample[0]);
      }
      SND_Play_Channel(SND_CIRCUIT_CHARGE, hum_vol, 0, -1);
      hum_play = 1;
    }
  }
  if (magic_circuit < 0) {
    hum_vol = (circuit_size - (magic_circuit + circuit_size)) * 80 / 3000;
    if (hum_play != 2) {
      if (hum_play == 1) {
        Mix_HaltChannel(0);
        Mix_FreeChunk(c_sample[0]);
      }
      SND_Play_Channel(SND_CIRCUIT_RECOVER, hum_vol, 0, -1);
      hum_play = 2;
    }
  }
	
  Mix_Volume(0, hum_vol);
}

/* The following code is from the Debian patch set.
   Until I can find a better place to put this acknowledgment, 
   be it known that this code was mostly written by Dylan R. E. Moonfire. */
 
int IsBackgroundMusicFile(char *buffer, int track)
{
  FILE *fp;

  /* Figure out the filename */
  sprintf(buffer, "dat/m/%s",
          tracks[track]);

  printf("%s", buffer);

  /* See if the file exists */
  if ((fp = fopen(buffer, "rb")) != NULL) {
    fclose(fp);
    return 1;
  }
  return 0;
}

void PlayBackgroundMusic(int track)
{
  /* Setup variables */
  char filename[64];
  int found_track = 0;

  /* See if we have a file of the appropriate type. */
  found_track = IsBackgroundMusicFile(filename, track);

#if 0
  if (!found_track)
    found_track = IsBackgroundMusicFile(filename, track, "ogg");
  if (!found_track)
    found_track = IsBackgroundMusicFile(filename, track, "mp3");
  if (!found_track)
    found_track = IsBackgroundMusicFile(filename, track, "s3m");
  if (!found_track)
    found_track = IsBackgroundMusicFile(filename, track, "xm");
  if (!found_track)
    found_track = IsBackgroundMusicFile(filename, track, "mod");
#endif

  /* Play the selected music */
  if (found_track) {
    bgm_music = Mix_LoadMUS(filename);
    Mix_PlayMusic(bgm_music, -1);
  }

  /* Always save the track to reduce load */
  bgm_track = track;
}

void TitleScreenMusic()
{
  int new_track = 5;
  if (bgm_track != -1) {
    Mix_HaltMusic();
    Mix_FreeMusic(bgm_music);
    bgm_music = NULL;
  }
	
  bgm_music = Mix_LoadMUS(tracks[new_track]);
  Mix_PlayMusic(bgm_music, -1);	
  bgm_track = new_track;
  PlayBackgroundMusic(new_track);
}

void BackgroundMusic()
{
  int new_track = -1;
  Mix_VolumeMusic(96);
	
  if (rooms[player_room].s_dist <= 15) {
    new_track = BGM_INNER;
  }
	
  if (bgm_track == BGM_MIDWAY) {
    if ((rooms[player_room].s_dist <= 30) && (rooms[player_room].s_dist >= 10)) {
      new_track = BGM_MIDWAY;
    }
  }
  if (bgm_track == BGM_OUTER) {
    if ((rooms[player_room].s_dist <= 45) && (rooms[player_room].s_dist >= 25)) {
      new_track = BGM_OUTER;
    }
  }
	
  if (new_track == -1) {
    if ((rooms[player_room].s_dist <= 30) && (rooms[player_room].s_dist >= 16)) {
      new_track = BGM_MIDWAY;
    }
    if ((rooms[player_room].s_dist <= 39) && (rooms[player_room].s_dist >= 31)) {
      new_track = BGM_OUTER;
    }
    if (rooms[player_room].s_dist >= 40) {
      new_track = BGM_EDGE;
    }
  }
	
  if (rooms[player_room].room_type == ROOM_ART_CHALLENGE) {
    new_track = BGM_ART_CHALLENGE;
  }
	
  if (artifacts[ART_CURSED_SEAL]) {
    new_track = BGM_CURSED_SEAL;
  }
	
  if (rooms[player_room].room_type == ROOM_BOSS) {
    if (boss_fight_mode == BSTA_FIGHTING) {
      if ((current_boss == BOSS_FINAL) && 
          (player.has_agate_knife) ) {
        if (boss_lives <= 1) {
          new_track = BGM_FINAL_CRITICAL;
        } else {
          new_track = BGM_FINAL_AGATE;
        }
      } else {
        new_track = BGM_MERIDIAN + current_boss;
      }
      Mix_VolumeMusic(128);
    } else {
      new_track = -1;
    }
  }
	
  if ( (player_room == 0) &&
       (current_boss == BOSS_FINAL) &&
       (boss_fight_mode >= BSTA_DYING) ) {
    new_track = BGM_FINAL_DEAD;
  }
	
  if (new_track == bgm_track) return;

  if (bgm_track != -1) {
    Mix_HaltMusic();
    Mix_FreeMusic(bgm_music);
    bgm_music = NULL;
  }
  if (new_track != -1) {
    PlayBackgroundMusic(new_track);
  }
  bgm_track = new_track;
}

int SND_GetChannel(void)
{
  int i;
	
  for (i = 8; i < 16; i++) {
    if (Mix_Playing(i) == 0) {
      if (c_sample[i] != NULL) {
        Mix_FreeChunk(c_sample[i]);
        c_sample[i] = NULL;
      }
      return i;
    }
  }
  return -1;
}

void SND_Play(const char *filename, int vol)
{
  int ch;
		
  ch = SND_GetChannel();
  if (ch != -1) {
    SND_Play_Channel(filename, vol, ch, 0);
  }
}

void SND_Pos(const char *filename, int vol, int dist)
{
  int real_vol;
  if (dist > 1600) return;
	
  real_vol = vol * (40 - (sqrt(dist))) / 40;
  SND_Play(filename, real_vol);
}

void SND_CircuitRelease(int str)
{
  SND_Play(SND_CIRCUIT_RELEASE, sqrt(str * 2 + (str * 5300 / circuit_size) + (str > 100 ? 5300 : str*53)));
}
