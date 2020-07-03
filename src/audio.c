//
//   audio.c
//
//   Copyright 2007, 2008 Lancer-X/ASCEAI
//
//   This file is part of Meritous.
//
//   Meritous is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   Meritous is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with Meritous.  If not, see <http://www.gnu.org/licenses/>.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string.h>

#include "levelblit.h"
#include "mapgen.h"
#include "boss.h"

#ifdef PS2
#include "audio.h"
Mix_Music * musics[13];
char *tracks[13] = {"cdrom0:\\DATA\\M\\ICEFRONT.S3M",
					"cdrom0:\\DATA\\M\\CAVERN.XM",
					"cdrom0:\\DATA\\M\\CAVE.XM",
					"cdrom0:\\DATA\\M\\CAVE06.S3M",
					"cdrom0:\\DATA\\M\\WOOD.S3M",
					"cdrom0:\\DATA\\M\\ILLER.XM",
					"cdrom0:\\DATA\\M\\FEAR2.MOD",
					"cdrom0:\\DATA\\M\\CV_BOSS.MOD",
					"cdrom0:\\DATA\\M\\FR_BOSS.MOD",
					"cdrom0:\\DATA\\M\\CT_BOSS.MOD",
					"cdrom0:\\DATA\\M\\RPG_BAT1.XM",
					"cdrom0:\\DATA\\M\\AMBLIGHT.XM",
					"cdrom0:\\DATA\\M\\FINALBAT.S3M"};

char * sfxFileNames[NUMSFX] =
	{
		"cdrom0:\\DATA\\A\\CHARGE.WAV", 
		"cdrom0:\\DATA\\A\\CRYSTAL.WAV", 
		"cdrom0:\\DATA\\A\\CRYSTAL2.WAV", 
		"cdrom0:\\DATA\\A\\ENEMYH.WAV", 
		"cdrom0:\\DATA\\A\\HURT.WAV", 
		"cdrom0:\\DATA\\A\\MONS0S.WAV", 
		"cdrom0:\\DATA\\A\\MONS1S.WAV", 
		"cdrom0:\\DATA\\A\\MONS2S.WAV", 
		"cdrom0:\\DATA\\A\\MONS3S.WAV", 
		"cdrom0:\\DATA\\A\\MONS4S.WAV", 
		"cdrom0:\\DATA\\A\\MONS5S.WAV", 
		"cdrom0:\\DATA\\A\\MONS6S.WAV", 
		"cdrom0:\\DATA\\A\\MONS7S.WAV", 
		"cdrom0:\\DATA\\A\\MONS8S.WAV", 
		"cdrom0:\\DATA\\A\\MONS9S.WAV", 
		"cdrom0:\\DATA\\A\\MONS10S.WAV", 
		"cdrom0:\\DATA\\A\\RECOVER.WAV", 
		"cdrom0:\\DATA\\A\\RELEASE.WAV", 
		"cdrom0:\\DATA\\A\\SHIELDH.WAV", 
		"cdrom0:\\DATA\\A\\TELEPORT.WAV", 
		"cdrom0:\\DATA\\A\\TONE.WAV" 
	};
#else
char *tracks[13] = {"dat/m/ICEFRONT.S3M",
					"dat/m/cavern.xm",
					"dat/m/cave.xm",
					"dat/m/cave06.s3m",
					"dat/m/Wood.s3m",
					"dat/m/iller_knarkloader_final.xm",
					"dat/m/fear2.mod",
					"dat/m/Cv_boss.mod",
					"dat/m/Fr_boss.mod",
					"dat/m/CT_BOSS.MOD",
					"dat/m/rpg_bat1.xm",
					"dat/m/amblight.xm",
					"dat/m/FINALBAT.s3m"};
#endif

Mix_Music *bgm_music = NULL;
int bgm_track = -1;

#ifdef PS2
Mix_Chunk *c_sample[NUMSFX] = {NULL};
#else
Mix_Chunk *c_sample[16] = {NULL};
#endif

void InitAudio()
{
	int i;
	Mix_OpenAudio(44100, AUDIO_S16, 2, 4096);
	Mix_VolumeMusic(112);
	Mix_AllocateChannels(16);
	for(i=0; i<13;i++)
		musics[i] = Mix_LoadMUS(tracks[i]);
	for(i=0; i<NUMSFX;i++)
		c_sample[i] = Mix_LoadWAV(sfxFileNames[i]);
}

void BackgroundMusic();
void CircuitHum();

void MusicUpdate()
{
	BackgroundMusic();
	CircuitHum();
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
#ifndef PS2
				Mix_FreeChunk(c_sample[0]);
#endif
			}
#ifdef PS2
			Mix_PlayChannel(0, c_sample[CHARGE], -1);
#else
			c_sample[0] = Mix_LoadWAV("dat/a/circuitcharge.wav");
			Mix_PlayChannel(0, c_sample[0], -1);
#endif
			hum_play = 1;
		}
	}
	if (magic_circuit < 0) {
		hum_vol = (circuit_size - (magic_circuit + circuit_size)) * 80 / 3000;
		if (hum_play != 2) {
			if (hum_play == 1) {
				Mix_HaltChannel(0);
#ifndef PS2
				Mix_FreeChunk(c_sample[0]);
#endif
			}
#ifdef PS2
			Mix_PlayChannel(0, c_sample[RECOVER], -1);
#else
			c_sample[0] = Mix_LoadWAV("dat/a/circuitrecover.wav");
			Mix_PlayChannel(0, c_sample[0], -1);
#endif
			hum_play = 2;
		}
	}
	
	Mix_Volume(0, hum_vol);
}

void TitleScreenMusic()
{
	int new_track = 5;
	if (bgm_track != -1) {
		Mix_HaltMusic();
#ifndef PS2		
		Mix_FreeMusic(bgm_music);
		bgm_music = NULL;
#endif
	}
	
#ifdef PS2
	bgm_music = musics[new_track];
#else
	bgm_music = Mix_LoadMUS(tracks[new_track]);
#endif
	Mix_PlayMusic(bgm_music, -1);	
	bgm_track = new_track;
}

void BackgroundMusic()
{
	int new_track = -1;
	Mix_VolumeMusic(96);
	
	if (rooms[player_room].s_dist <= 15) {
			new_track = 0;
	}
	
	if (bgm_track == 1) {
		if ((rooms[player_room].s_dist <= 30) && (rooms[player_room].s_dist >= 10)) {
			new_track = 1;
		}
	}
	if (bgm_track == 2) {
		if ((rooms[player_room].s_dist <= 45) && (rooms[player_room].s_dist >= 25)) {
			new_track = 2;
		}
	}
	
	if (new_track == -1) {
		if ((rooms[player_room].s_dist <= 30) && (rooms[player_room].s_dist >= 16)) {
			new_track = 1;
		}
		if ((rooms[player_room].s_dist <= 39) && (rooms[player_room].s_dist >= 31)) {
			new_track = 2;
		}
		if (rooms[player_room].s_dist >= 40) {
			new_track = 3;
		}
	}
	
	if (rooms[player_room].room_type == 3) {
		new_track = 4;
	}
	
	if (artifacts[11]) {
		new_track = 6;
	}
	
	if (rooms[player_room].room_type == 2) {
		if (boss_fight_mode == 2) {
			if ( (current_boss == 3) && (player_shield == 30) ) {
				if (boss_lives <= 1) {
					new_track = 7;
				} else {
					new_track = 12;
				}
			} else {
				new_track = 7 + current_boss;
			}
			Mix_VolumeMusic(128);
		} else {
			new_track = -1;
		}
	}
	
	if ( (player_room == 0) && (current_boss == 3) && (boss_fight_mode >= 3) ) {
		new_track = 11;
	}
	
	if (new_track == bgm_track) return;

	if (bgm_track != -1) {
		Mix_HaltMusic();
#ifndef PS2
		Mix_FreeMusic(bgm_music);
		bgm_music = NULL;
#endif	
	}
	if (new_track != -1) {
#ifdef PS2
		bgm_music = musics[new_track];
#else
		bgm_music = Mix_LoadMUS(tracks[new_track]);
#endif
		Mix_PlayMusic(bgm_music, -1);
	}
	bgm_track = new_track;
}

int SND_GetChannel()
{
	int i;
	
	for (i = 8; i < 16; i++) {
		if (Mix_Playing(i) == 0) {
#ifndef PS2
			if (c_sample[i] != NULL) {
				Mix_FreeChunk(c_sample[i]);
				c_sample[i] = NULL;
			}
#endif
			return i;
		}
	}
	return -1;
}

#ifdef PS2
void SND_Play(int sfx, int vol)
#else
void SND_Play(char *filename, int vol)
#endif
{
	int ch;
		
	ch = SND_GetChannel();
	if (ch != -1) {
#ifdef PS2
		Mix_PlayChannel(ch, c_sample[sfx], 0);
#else
		c_sample[ch] = Mix_LoadWAV(filename);
		Mix_Volume(ch, vol);
		Mix_PlayChannel(ch, c_sample[ch], 0);
#endif
	}
}

#ifdef PS2
void SND_Pos(int sfx, int vol, int dist)
#else
void SND_Pos(char *filename, int vol, int dist)
#endif
{
	int real_vol;
	if (dist > 1600) return;
	
	real_vol = vol * (40 - (sqrt(dist))) / 40;
#ifdef PS2
	SND_Play(sfx, real_vol);
#else
	SND_Play(filename, real_vol);
#endif
}

void SND_CircuitRelease(int str)
{
#ifdef PS2
	SND_Play(RELEASE, sqrt(str * 2 + (str * 5300 / circuit_size) + (str > 100 ? 5300 : str*53)));
#else
	SND_Play("dat/a/circuitrelease.wav", sqrt(str * 2 + (str * 5300 / circuit_size) + (str > 100 ? 5300 : str*53)));
#endif
}

#ifdef PS2
void freeAudio(void)
{
	int i;

	for (i = 0; i < NUMSFX; i++) {
		if (c_sample[i] != NULL) {
			Mix_FreeChunk(c_sample[i]);
			c_sample[i] = NULL;
		}
	}

	Mix_HaltMusic();

	for(i=0; i<13;i++)
        Mix_FreeMusic(musics[i]);

    Mix_CloseAudio();
}
#endif
