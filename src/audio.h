//
//   audio.h
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

// Exposes audio.c functionality and types

#ifndef AUDIO_H
#define AUDIO_H

void InitAudio();
void MusicUpdate();
void TitleScreenMusic();
void SND_CircuitRelease(int str);
#ifdef PS2
void SND_Pos(int sfx, int vol, int dist);
#else
void SND_Pos(char *filename, int vol, int dist);
#endif

#ifdef PS2
void freeAudio(void);

enum SFX 
	{
		CHARGE=0, 
		CRYSTAL, 
		CRYSTAL2, 
		ENEMYH, 
		HURT, 
		MONS0S, 
		MONS1S, 
		MONS2S, 
		MONS3S, 
		MONS4S, 
		MONS5S, 
		MONS6S, 
		MONS7S, 
		MONS8S, 
		MONS9S, 
		MONS10S, 
		RECOVER, 
		RELEASE, 
		SHIELDH, 
		TELEPORT, 
		TONE, 
		NUMSFX
	};
#endif
#endif
