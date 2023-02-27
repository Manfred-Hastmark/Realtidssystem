#include "part2.h"

void nextBeat(MusicPlayer* self, int unused)
{
	// Sets a period to the toneGenerator and turns it on.
	ASYNC(self->TG, setSilence, 0);
	ASYNC(self->TG, setPeriod, self->notePeriods[self->index]);
	ASYNC(self->TG, setDAC, 0xFFFFFFFF);
	
	// Sleep until it should silence the toneGenerator
	const int toneDuration = MSEC(getBeatLenght(self->beatLength[self->index], self->tempo, self->silenceDuration));
	self->index = (self->index + 1) & ~ 32;
	SEND(toneDuration, toneDuration + USEC(100 * self->tempo >> 5), self, nextSilence, 0); 	// (same as (index + 1) % 32)
}

void nextSilence(MusicPlayer* self, int unused)
{
	// Turns of the tone generator
	ASYNC(self->TG, setSilence, 1);

	// Sleep until the next note
	if(self->playing)
	{
		const int silenceDuration = MSEC(self->silenceDuration);
		SEND(silenceDuration, silenceDuration + USEC(900 * self->tempo >> 5), self, nextBeat, 0);
	}
	else
	{
		self->exist = 0;
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
	if(self->playing)
	{
		self->playing = 0;
	}
	else
	{
		//Note to ourselves, add semaphore if we have time!!!
		
		self->playing = 1;
		if(!self->exist)
		{
			self->index = 0;
			self->exist = 1;
			ASYNC(self, nextBeat, 0);
		}
	}
	return self->playing;
}




// 0 = change to falling trigger
// 1 = change to rising trigger
int setButtonAction(UserButton* self, int time)
{
	if(self->pressed)
	{
		const Time offsTime = MSEC_OF(time - self->time);
		const Time jitterTime = MSEC_OF(time - self->jitterTime);
		self->jitterTime = time;
		
		if(jitterTime < 100)
		{
			return 0; // keep falling trigger
		}
		else if(offsTime < 1000)
		{
			//Register a released button
			self->pressed = 0;
			return 1;
		}
		else
		{
			// press-momentary
		}
	}
	else
	{
		self->jitterTime = time;
		self->time = time;
		self->pressed = 1;
		return 0;
	}
}