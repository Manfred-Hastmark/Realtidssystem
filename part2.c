#include "part2.h"

void nextBeat(MusicPlayer* self, int index)
{
	
	// Sets a period to the toneGenerator and turns it on.
	ASYNC(self->TG, setSilence, 0);
	ASYNC(self->TG, setPeriod, self->notePeriods[index]);
	ASYNC(self->TG, setDAC, 0xFFFFFFFF);
	
	// Sleep until it should silence the toneGenerator
	const int toneDuration = MSEC(getBeatLenght(self->beatLength[index], self->tempo, self->silenceDuration));
	SEND(toneDuration, toneDuration + USEC(100 * self->tempo >> 5), self, nextSilence, (index+1) & ~32); 	// (same as (index + 1) % 32)
}

void nextSilence(MusicPlayer* self, int index)
{
	// Turns of the tone generator
	ASYNC(self->TG, setSilence, 1);

	// Sleep until the next note
	if(self->playing == -1)
	{
		const int silenceDuration = MSEC(self->silenceDuration);
		SEND(silenceDuration, silenceDuration + USEC(900 * self->tempo >> 5), self, nextBeat, index);
	}
	else
	{
		self->playing = index;
	}
}

static inline int getBeatLenght(char c, int ms, int silenceDuration) 
{
	switch(c)
	{
		case 'a': return (ms - silenceDuration);
		case 'b': return ((ms<<1) - silenceDuration);	// Why is it that "-" has a higher priority than "<<" ?!
		case 'c': return ((ms>>1) - silenceDuration);	// Why is it that "-" has a higher priority than "<<" ?!
	}
	// Should crash the programm;
	return -1;
}

void setTempo(MusicPlayer* self, int bpm)
{	
	self->tempo = 60000 / bpm;	
	self->silenceDuration = self->tempo / 10;
}

void setPeriods(MusicPlayer* self, int arrIn)
{
	int* arr = (int*) arrIn;
	for(int i = 0; i < 32; i++)
	{
		self->notePeriods[i] = arr[i];
	}
}

int togglePlaying(MusicPlayer* self, int unused)
{
	if(self->playing == -1)
	{
		self->playing = 0;
	}
	else
	{
		ASYNC(self, nextBeat, self->playing);	// This cannot happen before -1 :)
		self->playing = -1;
	}
	return self->playing;
}
