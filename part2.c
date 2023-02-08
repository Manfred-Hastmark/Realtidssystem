#include "part2.h"

void nextBeat(MusicPlayer* self, int index)
{
	self->TG.silence = 0;
	self->TG.period = self->notePeriods[index];
	ASYNC(&self->TG, setDAC, 0xFFFFFFFF);
	const int toneDuration = MSEC(getBeatLenght(self->beatLength[index], self->tempo, self->silenceDuration));
	SEND(toneDuration, toneDuration + USEC(100), self, nextSilence, (index + 1) % 32); // (samma som index + 1 % 32)
}

void nextSilence(MusicPlayer* self, int index)
{
	self->TG.silence = 1;
	const int silenceDuration = MSEC(self->silenceDuration);
	SEND(silenceDuration, silenceDuration + USEC(100), self, nextBeat, index);
}

int getBeatLenght(char c, int ms, int silenceDuration) 
{
	switch(c)
	{
		case 'a': return (ms - silenceDuration);
		case 'b': return ((ms<<1) - silenceDuration);
		case 'c': return ((ms>>1) - silenceDuration);
	}
	// Should crash the programm;
	return -1;
}

void setPeriods(MusicPlayer* self, int arrIn)
{
	int* arr = (int*) arrIn;
	for(int i = 0; i < 32; i++)
	{
		self->notePeriods[i] = arr[i];
	}
}

void setTempo(MusicPlayer* self, int bpm)
{	
	self->tempo = 60000 / bpm;	
	self->silenceDuration = self->tempo / 10;
}