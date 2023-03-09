#include "part2.h"

void nextBeat(MusicPlayer* self, int index)
{
	
	// Sets a period to the toneGenerator and turns it on.
	ASYNC(self->TG, setSilence, 0);						
	self->TG->period = self->notePeriods[index];
	ASYNC(self->TG, setDAC, 0xFFFFFFFF);
	
	// Sleep until it should silence the toneGenerator
	const int toneDuration = MSEC(getBeatLenght(self->beatLength[index], self->tempo, self->silenceDuration));
	SEND(toneDuration, toneDuration + USEC(100), self, nextSilence, (index+1) & ~32); 	// (same as (index + 1) % 32)
}

void nextSilence(MusicPlayer* self, int index)
{
	// Turns of the tone generator
	ASYNC(self->TG, setSilence, 1);	

	// Sleep until the next note
	if(self->playing == -1)
	{
		const int silenceDuration = MSEC(self->silenceDuration);
		SEND(silenceDuration, silenceDuration + USEC(100), self, nextBeat, index);
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
		case 'b': return ((ms<<1) - silenceDuration);	// Why has "-" higher priority than "<<" ?!
		case 'c': return ((ms>>1) - silenceDuration);	// Why has "-" higher priority than "<<" ?!
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




// 0 = change to falling trigger
// 1 = change to rising trigger
int setButtonAction(UserButton* self, int time)
{
	if(self->fallingTrigger)
	{
		//Set to rising trigger
		self->fallingTrigger = 0;
		const Time timeSinceToggle = MSEC_OF(time - self->lastPress);
		
		if(timeSinceToggle < 100)
		{
			//jitter -> keep falling trigger
			self->fallingTrigger = 1;
			return 2; 
		}
		else if(timeSinceToggle < 1000)
		{
			//Valid press -> store in history
			self->pressTime[self->pressIndex] = self->lastPress;
			self->pressIndex = (self->pressIndex + 1) % 3;
			self->holdMode = -1;
			return 1; //valid press -> switch to rising trigger
		}
		else
		{
			//return the amount of time the button got held
			return MSEC_OF(time - self->lastPress);
		}
	}
	else
	{
		//Enter falling trigger mode and wait for button release
		self->fallingTrigger = 1;
		self->lastPress = time;
		//Call for hold mode after one second
		SEND(MSEC(1005), MSEC(1050), self, setHoldMode, 0);
		return 0;
	}
}


void setHoldMode(UserButton* self, int unused)
{
	//Sets to 1 if the button hasn't been released yet
	self->holdMode = self->holdMode == -1 ? 0 : 1;
}

int getHoldMode(UserButton* self, int unused)
{
	return self->holdMode;
}
	
