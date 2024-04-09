#include <part1.h>

// Functions for class ToneGenerator

int toggleDeadlinesToneGenerator(ToneGenerator* self, int unused)
{
    self->deadline ^= 1;
    return self->deadline;
}

void toggleMute(ToneGenerator* self, int unused)
{
    self->mute ^= 0xFFFFFFFF;
}

int volume(ToneGenerator* self, int change)
{
    // Do a check so that we are in range
    if (self->volume + change > 20 || self->volume + change < 0)
        return self->volume;

    self->volume += change;
    return self->volume;
}

void setDAC(ToneGenerator* self, int val)
{

    if (self->silence)
    {
        DAC_DATA = 0;
        return;
    }

    DAC_DATA = val & self->volume & self->mute;
    const int nextBaseline = USEC(self->period);
    if (self->deadline)
    {
        SEND(nextBaseline, nextBaseline + USEC(100), self, setDAC, val ^ 0xFFFFFFFF);
    }
    else
    {
        AFTER(nextBaseline, self, setDAC, val ^ 0xFFFFFFFF);
    }
}

void dummySetDac(ToneGenerator* self, int arr)
{
    int* res = (int*)arr;

    volatile int dac_data;

    int max = 0;
    int avg = 0;
    int avgBef = CURRENT_OFFSET();
    for (int i = 0; i < 500; i++)
    {
        int before = CURRENT_OFFSET();

        // This is the function
        dac_data = 1 & self->volume & self->mute;
        volatile const int nextBaseline = USEC(self->period);
        if (self->deadline)
        {
            volatile const int t1 = nextBaseline + USEC(100);
            volatile const int t2 = 1 ^ 0xFFFFFFFF;
            // SEND(nextBaseline, nextBaseline + USEC(100), self, setDAC, val ^ 0xFFFFFFFF);
        }
        else
        {
            volatile const int t3 = 1 ^ 1;
            // AFTER(nextBaseline, self, setDAC, val ^ 1);
        }

        int time = CURRENT_OFFSET() - before;
        max = (max < time) ? time : max;
    }
    avg = CURRENT_OFFSET() - avgBef;
    res[0] = USEC_OF(max);
    res[1] = USEC_OF(avg) / 500;
}

// Functions for class BackgroundTask

int toggleDeadlinesBackgroundTask(BackgroundTask* self, int unused)
{
    self->deadline ^= 1;
    return self->deadline;
}

int load(BackgroundTask* self, int change)
{
    // Do a check so we are in range
    if (self->backgroundLoopRange + change > 24000 || self->backgroundLoopRange + change < 1000)
        return self->backgroundLoopRange;

    self->backgroundLoopRange += change;
    return self->backgroundLoopRange;
}

void runLoad(BackgroundTask* self, int backgroundLoopRange)
{
    while (backgroundLoopRange--)
        ;

    if (self->deadline)
    {
        SEND(USEC(1300), USEC(1300), self, runLoad, self->backgroundLoopRange);
    }
    else
    {
        AFTER(USEC(1300), self, runLoad, self->backgroundLoopRange);
    }
}

void dummyRunLoad(BackgroundTask* self, int arr)
{
    int* res = (int*)arr;

    int max = 0;
    int avg = 0;
    int avgBef = CURRENT_OFFSET();
    for (int i = 0; i < 500; i++)
    {
        int backgroundLoopRange = res[0];
        int before = CURRENT_OFFSET();

        while (backgroundLoopRange--)
            ;
        volatile const int deadline = USEC(1300);
        if (self->deadline)
        {
            // BEFORE(deadline, self, runLoad, self->backgroundLoopRange);
        }
        else
        {
            // ASYNC(self, runLoad, self->backgroundLoopRange);
        }

        int time = CURRENT_OFFSET() - before;
        max = (max < time) ? time : max;
    }
    avg = CURRENT_OFFSET() - avgBef;
    res[0] = USEC_OF(max);
    res[1] = USEC_OF(avg) / 500;
}
