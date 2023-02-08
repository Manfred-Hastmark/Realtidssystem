#ifndef PART2
#define PART2
#include "part0.h"
#include "part1.h"
#include "TinyTimber.h"
#include "sciTinyTimber.h"
#include "canTinyTimber.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct 
{
	ToneGenerator TG;
	int tempo;				// 60000 / bpm represended in ms
	int silenceDuration;	// Silence after note in ms?
	char* beatLength;		// How long every beat should be
	int notePeriods[32];
} MusicPlayer;

#define initMusicPlayer(tempo, silenceDuration, beatLength) {initToneGenerator(1000), (int)tempo, (int)silenceDuration, (char*)beatLength, {1136, 1012, 902, 1136, 1136, 1012, 902, 1136, 902, 851, 758, 902, 851, 758, 758, 676, 758, 851, 902, 1136, 758, 676, 758, 851, 902, 1136, 1136, 1517, 1136, 1136, 1517, 1136 }}

void nextBeat(MusicPlayer*, int);

void nextSilence(MusicPlayer*, int);

int getBeatLenght(char, int, int);

void setTempo(MusicPlayer*, int);

void setPeriods(MusicPlayer*, int);

#endif