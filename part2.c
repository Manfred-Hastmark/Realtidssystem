#include "part2.h"

void printWithSerial(Serial* sci, char* string, int val);

void nextBeat(MusicPlayer* self, int evenOdd)
{
	if(self->lightExist == 0)
	{
		self->lightExist = 1;
		self->lightPlaying = 1;
		ASYNC(self, blinkLed, evenOdd);
	}
	
	// Sets a period to the toneGenerator and turns it on.
	ASYNC(self->TG, setSilence, 0);
	ASYNC(self->TG, setPeriod, self->notePeriods[self->index]);
	ASYNC(self->TG, setDAC, 0xFFFFFFFF);
	
	evenOdd += (self->beatLength[self->index] == 'c') ? 1 : 0;
	
	// Sleep until it should silence the toneGenerator
	const int toneDuration = MSEC(getBeatLenght(self->beatLength[self->index], self->tempo, self->silenceDuration));
	self->index = (self->index + 1) & ~ 32;
	SEND(toneDuration, toneDuration + USEC(100 * self->tempo >> 5), self, nextSilence, evenOdd & 1); 	// (same as (index + 1) % 32)
}

void blinkLed(MusicPlayer* self, int onOff)
{
	if(self->lightPlaying)
	{
		SIO_WRITE(self->sysIO, onOff);
		AFTER(MSEC((self->tempo) >> 1), self, blinkLed, onOff ^ 1);
	} 
	else
	{
		self->lightExist = 0;
	}
}

void nextSilence(MusicPlayer* self, int evenOdd)
{
	// Turns of the tone generator
	ASYNC(self->TG, setSilence, 1);

	// Sleep until the next note
	if(self->playing)
	{
		const int silenceDuration = MSEC(self->silenceDuration);
		SEND(silenceDuration, silenceDuration + USEC(900 * self->tempo >> 5), self, nextBeat, evenOdd);
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
	self->lightPlaying = 0;
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
		self->lightPlaying = 0;
		self->playing = 0;
	}
	else
	{
		//Note to ourselves, add semaphore if we have time!!!
		self->lightPlaying = 0;
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


int toggleRisingTrigger(UserButton* self, int unused)
{
	self->pressed ^= 1;
	return self->pressed;
}

void resetInterTimer(UserButton* self, int unused)
{
	if(self->pressed)
	{
		T_RESET(&self->timer);
	}
}

void pressAndHold(UserButton* self, int unused)
{
	//SCI_WRITE(self->sci, "We are trying to go to press and hold\n");
	if(T_SAMPLE(&self->pressTimer) >= SEC(1) && self->pressed)
	{
		self->mode = 1;
		SCI_WRITE(self->sci, "PRESS-AND-HOLD-MODE\n");
	}
}

// 0 = change to falling trigger
// 1 = change to rising trigger
int setButtonAction(UserButton* self, int unused)
{
	if(self->pressed == 0)
	{
		Time interArrival = T_SAMPLE(&self->timer); 
		
		const int milliseconds = MSEC_OF(interArrival) + 1000 * SEC_OF(interArrival);
		
		//Avoid jitter
		if(milliseconds > 100)
		{
			self->pressIndex++;
			T_RESET(&self->pressTimer);
			AFTER(SEC(1), self, pressAndHold, 0);
			T_RESET(&self->timer);
			if(self->pressIndex != 1)
			{

				printWithSerial(self->sci, "The inter-arrival time was: %d\n", milliseconds);
				if(!SYNC(self->interBuffer, addInterBuffer, milliseconds))
				{
					ASYNC(self->interBuffer, emptyInterBuffer, 0);
					self->pressIndex = 1;
					SCI_WRITE(self->sci, "Inter arrival times differed by more than 100 ms, please try again\n");
				}
				else if(self->pressIndex == 4)
				{
					const int bpm = SYNC(self->interBuffer, getBpmInterBuffer, 0);
					ASYNC(self->interBuffer, emptyInterBuffer, 0);
					self->pressIndex = 1;
					return bpm;
				}
				return 0;
			}
		}
	}
	else if(self->mode && self->pressed)
	{
		self->mode = 0;
		Time pressTime = T_SAMPLE(&self->timer);
		printWithSerial(self->sci, "The press-and-hold time was: %d\n", MSEC_OF(pressTime) + 1000 * SEC_OF(pressTime));
		ASYNC(self->interBuffer, emptyInterBuffer, 0);
		self->pressIndex = 0;
		return MSEC_OF(pressTime) + 1000 * SEC_OF(pressTime);
	}
	return 0;
}

void printWithSerial(Serial* sci, char* string, int val)
{
	char output[50];
	sprintf(output, string, val);
	SCI_WRITE(sci, output);
}

//InterBuffer buffer functions

int addInterBuffer(InterBuffer* self, int number)
{
	for(int i = 0; i < self->index; i++)
	{
		if(abs(self->buffer[i] - number) > 100)
			return 0;
	}
	self->buffer[self->index] = number;
	self->index = (self->index + 1) % 3;
	self->count++;
	return 1;
}

void emptyInterBuffer(InterBuffer* self, int unused)
{
	self->buffer[0] = 0;
	self->buffer[1] = 0;
	self->buffer[2] = 0;
	self->index = 0;
	self->count = 0;
}

int getBpmInterBuffer(InterBuffer* self, int unused)
{
	const int averageInterTime = (self->buffer[0] + self->buffer[1] + self->buffer[2]) / 3;
	return 60000 / averageInterTime;
}