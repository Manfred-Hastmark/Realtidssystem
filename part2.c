#include "part2.h"
#include "TinyTimber.h"
#include "application.h"
#include "canMsgs.h"

void nextBeat(MusicPlayer* self, int unused)
{
    // Sets a period to the toneGenerator and turns it on.
    // self->m_tone_generator_p->silence = 0;
    // self->m_tone_generator_p->period = self->notePeriods[self->index];
    // ASYNC(self->m_tone_generator_p, setDAC, 0xFFFFFFFF);

    self->index++;
    self->index %= LENGTH;

    static Notes notes_msg;
    notes_msg.note_index = self->index;
    notes_msg.player = RANK;
    notes_msg.tempo = 60000 / self->tempo;
    notes_msg.key = self->m_melody_p->key;
    notes_msg.id = NOTESID;
    ASYNC(self->m_app_p, send_notes_msg, (int)&notes_msg);

    const int toneDuration = MSEC(getBeatLenght(self->beatLength[self->index], self->tempo, self->silenceDuration));
    SEND(toneDuration, toneDuration + USEC(100), self, nextSilence, 0);
}

void nextSilence(MusicPlayer* self, int unused)
{
    // Turns of the tone generator
    self->m_tone_generator_p->silence = 1;

    // Sleep until the next note
    if (self->playing == 1)
    {
        const int silenceDuration = MSEC(self->silenceDuration);
        SEND(silenceDuration, silenceDuration + USEC(100), self, nextBeat, 0);
    }
}

void silence(MusicPlayer* self, int unused)
{
    self->m_tone_generator_p->silence = 1;
}

void play_note(MusicPlayer* self, int index)
{
    self->m_tone_generator_p->silence = 0;
    self->m_tone_generator_p->period = self->notePeriods[index];
    ASYNC(self->m_tone_generator_p, setDAC, 0xFFFFFFFF);

    const int toneDuration = MSEC(getBeatLenght(self->beatLength[index], self->tempo, self->silenceDuration));
    SEND(toneDuration, toneDuration + USEC(100), self, silence, 0);
}

void change_key(MusicPlayer* self, int key)
{
    SYNC(self->m_melody_p, setKey, key);
    int melodyPeriods[LENGTH];
    SYNC(self->m_melody_p, setMelodyPeriods, (int)melodyPeriods);
    for (int i = 0; i < LENGTH; i++)
    {
        self->notePeriods[i] = melodyPeriods[i];
    }
}

void change_bpm(MusicPlayer* self, int bpm)
{
    self->tempo = 60000 / bpm;
    self->silenceDuration = self->tempo / 10;
}

static inline int getBeatLenght(char c, int ms, int silenceDuration)
{
    switch (c)
    {
    case 'a':
        return (ms - silenceDuration);
    case 'b':
        return ((ms << 1) - silenceDuration); // Why has "-" higher priority than "<<" ?!
    case 'c':
        return ((ms >> 1) - silenceDuration); // Why has "-" higher priority than "<<" ?!
    }
    // Should crash the programm;
    return -1;
}

int togglePlaying(MusicPlayer* self, int unused)
{
    if (!self->playing)
    {
        ASYNC(self, nextBeat, 0);
        self->playing = 1;
    }
    else
    {
        self->playing = 0;
    }
    return self->playing;
}
