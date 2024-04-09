#ifndef PART1
#define PART1

#include "TinyTimber.h"
#include <stdio.h>
#include <stdlib.h>

#define DAC_DATA *((unsigned char*)0x4000741C)

// definition for class ToneGenerator

typedef struct
{
    Object super;
    int period;   // Period measuered in us
    int volume;   // Value between 0-20
    int mute;     // If mute=0 then quiet and 0xFFFFFFFF else
    int deadline; // If set to 1, deadlines will be used
    int silence;  // Check if we are in end of beat. Currently not the same as mute! Should be the same as mute maybe?
} ToneGenerator;

#define initToneGenerator(period)                                                                                                          \
    {                                                                                                                                      \
        initObject(), period >> 1, 5, 0xFFFFFFFF, 1, 0                                                                                     \
    }

int toggleDeadlinesToneGenerator(ToneGenerator*, int);
void toggleMute(ToneGenerator*, int);
int volume(ToneGenerator*, int);
void setDAC(ToneGenerator*, int);
void dummySetDac(ToneGenerator*, int);

// Defintion for class BackgroundTask

typedef struct
{
    Object super;
    int backgroundLoopRange;
    int deadline; // If = 1, deadlines will be used
} BackgroundTask;

#define initBackgroundTask()                                                                                                               \
    {                                                                                                                                      \
        initObject(), 1000, 1                                                                                                              \
    }

int toggleDeadlinesBackgroundTask(BackgroundTask*, int);
int load(BackgroundTask*, int);
void runLoad(BackgroundTask*, int);
void dummyRunLoad(BackgroundTask*, int);

#endif
