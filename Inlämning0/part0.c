#include "part0.h"

//us for each note needed in lab 0 for non-general solution
const int notePeriods[25] =
{
	2024, //B  (-10)
	1911, //C  (-9)
	1804, //C# (-8)
	1703, //D  (-7)
	1607, //D# (-6)
	1517, //E  (-5)
	1432, //F  (-4)
	1351, //F# (-3)
	1276, //G  (-2)
	1204, //G# (-1)
	1136, //A  (0)
	1073, //A# (1)
	1012, //B  (2)
	956,  //C  (3)
	902,  //C# (4)
	851,  //D  (6)
	804,  //D# (7)
	758,  //E  (8)
	716,  //F  (9)
	676,  //F# (10)
	638,  //G  (11)
	602,  //G# (12)
	568,  //A  (13)
	536,   //A#  (14)
	506
};

//Function for class Melody

void setMelodyPeriods(Melody* self, int arr)
{
	int* retArr = (int*) arr;
	
	for(int i = 0; i < self->length; i++) 
	{
		retArr[i] = notePeriods[self->melody[i] + self->key + 10]; //+10 because 5 for min index in song, 5 for min key offs.
	}
}

void setKey(Melody* self, int key)
{
	self->key = key;
}

//Functions for ReadBuffer

void readBufferAdd(ReadBuffer* self, int character)
{
	self->buffer[self->index++] = character;
}

int endBuffer(ReadBuffer* self, int unused)
{
	self->buffer[self->index] = '\0';
	self->index = 0;
	return atoi(self->buffer);
}

//Functions for class ThreeHist

void addThreeHist(ThreeHist* self, int number)
{
	self->buffer[self->index] = number;
	self->index = (self->index + 1) % 3;
	self->count++;
}

void emptyThreeHist(ThreeHist* self, int unused)
{
	self->buffer[0] = 0;
	self->buffer[1] = 0;
	self->buffer[2] = 0;
	self->index = 0;
	self->count = 0;
}

int sumBuffer(ThreeHist* self, int unused)
{
	return self->buffer[0] + self->buffer[1] + self->buffer[2];
}

int medianBuffer(ThreeHist* self, int unused)
{
	switch (self->count) 
	{
		case 0:
			return 0;
		case 1:
			return self->buffer[0];
		case 2:
			return (self->buffer[0] + self->buffer[1]) >> 1;
	}
	int x = self->buffer[0] - self->buffer[1];
	int y = self->buffer[1] - self->buffer[2];
	int z = self->buffer[0] - self->buffer[2];
	
	if(x * y > 0)
		return self->buffer[1];
	else if (x * z > 0)
		return self->buffer[2];
	else
		return self->buffer[0];
}



//Functions for class MelodyGeneral

//uS for each halfperiod of every note in A=440Hz, used for general solution
const int notePeriodsGeneral[13] = 
{
	1137, //A
	1073, //A#
	1013, //B
	956,  //C
	902,  //C#
	852,  //D
	804,  //D#
	759,  //E
	716,  //F
	676,  //F#
	638,  //G
	602,  //G#
	568	  //A
};

void calculateEdgesGeneral(MelodyGeneral* self, int unused)
{
	int min = 2147483647;
	int max = -2147483648;
	for(int i = 0; i < self->length; i++)
	{
		if(self->melody[i] < min) min = self->melody[i];
		if(self->melody[i] > max) max = self->melody[i];
	}
	
	self->min = min;
	self->max = max;
}

void generatePeriodGeneral(MelodyGeneral* self, int key)
{
	self->key = key;
	int index = 0;
	for(int i = self->min + key; i < self->max + key + 1; i++) 
	{
		int notePeriodIndex = (i < 0) ? i % 12 + 12 : i % 12;
		int shift = (i < 0) ? i / 12 - 1 : i / 12;
		int note = notePeriodsGeneral[notePeriodIndex];
		note = (shift < 0) ? note << abs(shift) : note >> shift;
		self->periods[index] = note;
		index++;
	}
}

void setMelodyPeriodsGeneral(MelodyGeneral* self, int arr)
{
	int* retArr = (int*) arr;
	
	for(int i = 0; i < self->length; i++) 
	{
		retArr[i] = self->periods[self->melody[i] - self->min];
	}
}