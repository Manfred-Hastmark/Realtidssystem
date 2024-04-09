#ifndef PART0
#define PART0

#include "TinyTimber.h"
#include <stdio.h>
#include <stdlib.h>

// Definition for "Class" ReadBuffer, used to read user input

typedef struct
{
    Object super;
    char buffer[50]; // Maximum amount of characters to input at once will be 50
    int index;
} ReadBuffer;

#define initReadBuffer()                                                                                                                   \
    {                                                                                                                                      \
        initObject(), {}, 0                                                                                                                \
    }

void readBufferAdd(ReadBuffer*, int);
int endBuffer(ReadBuffer*, int);

// Defintion for "Class" ThreeHist

typedef struct
{
    Object super;
    int buffer[3];
    int index;
    int count;
} ThreeHist;

#define initThreeHist()                                                                                                                    \
    {                                                                                                                                      \
        initObject(), {}, 0, 0                                                                                                             \
    }

void addThreeHist(ThreeHist*, int);
void emptyThreeHist(ThreeHist*, int);
int sumBuffer(ThreeHist*, int);
int medianBuffer(ThreeHist*, int);

// Definition for class Melody

typedef struct
{
    Object super;
    int* melody;
    int length;
    int key;
} Melody;

#define initMelody(melody, length)                                                                                                         \
    {                                                                                                                                      \
        initObject(), (int*)melody, (int)length, 0                                                                                         \
    }

void setMelodyPeriods(Melody*, int);
void setKey(Melody*, int);

// Definition for class MelodyGeneral, this will work for any key and any melody. Not required for lab

typedef struct
{
    Object super;
    int periods[60];
    int* melody;
    int length;
    int key;
    int min;
    int max;
} MelodyGeneral;

#define initMelodyGeneral(melody, length)                                                                                                  \
    {                                                                                                                                      \
        initObject(), {}, (int*)melody, (int)length, 0, 0, 0                                                                               \
    }

void generatePeriodGeneral(MelodyGeneral*, int);
void calculateEdgesGeneral(MelodyGeneral*, int);
void setMelodyPeriodsGeneral(MelodyGeneral*, int);

#endif
