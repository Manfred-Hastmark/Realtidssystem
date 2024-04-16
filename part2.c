#include "part2.h"
#include "TinyTimber.h"
#include "application.h"
#include "stdbool.h"

void check_notes_to(MusicPlayer* self, int index);

void nextBeat(MusicPlayer* self, int unused)
{
    if (SYNC(self->m_board_handler_p, get_conductor_index, 0) == RANK)
    {
        self->index++;
        int player_index = SYNC(self->m_board_handler_p, get_next_musician_index, 0);
        if (player_index == RANK)
        {
            self->TG.silence = 0;
            self->TG.period = self->notePeriods[self->index];
            ASYNC(&self->TG, setDAC, 0xFFFFFFFF);
        }
        else
        {
            static Notes notes_msg;
            notes_msg.id = NOTESID;
            notes_msg.note_index = self->index;
            notes_msg.key = self->m_melody_p->key;
            notes_msg.player = player_index;
            notes_msg.tempo = self->tempo;
            ASYNC(self, send_tone_msg, (int)&notes_msg);
            ASYNC(self, check_notes_to, self->index);
        }
    }

    // Sleep until it should silence the toneGenerator
    const int toneDuration = MSEC(getBeatLenght(self->beatLength[self->index], self->tempo, self->silenceDuration));
    self->index += 1;
    self->index %= 32;
    SEND(toneDuration, toneDuration + USEC(100), self, nextSilence, self->index); // (same as (index + 1) % 32)
}

void nextSilence(MusicPlayer* self, int unused)
{
    // Turns of the tone generator
    self->TG.silence = 1;

    // Sleep until the next note
    if (self->playing == 1 && SYNC(self->m_board_handler_p, get_next_musician_index, 0) == RANK)
    {
        const int silenceDuration = MSEC(self->silenceDuration);
        SEND(silenceDuration, silenceDuration + USEC(100), self, nextBeat, self->index);
    }
}

int getBeatLenght(char c, int ms, int silenceDuration)
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

void setTempo(MusicPlayer* self, int bpm)
{
    self->tempo = 60000 / bpm;
    self->silenceDuration = self->tempo / 10;
}

void setPeriods(MusicPlayer* self, int arrIn)
{
    int* arr = (int*)arrIn;
    for (int i = 0; i < 32; i++)
    {
        self->notePeriods[i] = arr[i];
    }
}

int togglePlaying(MusicPlayer* self, int unused)
{
    if (self->playing == 1)
    {
        self->playing = 0;
    }
    else
    {
        ASYNC(self, nextBeat, 0);
        self->playing = 1;
    }
    return self->playing;
}

void set_note_index(MusicPlayer* self, int index)
{
    self->index = index;
}

int is_playing(MusicPlayer* self)
{
    return self->playing;
}

Time notes_timeouts[LENGTH];
void check_notes_to(MusicPlayer* self, int index)
{
    const notes_to = MSEC(1);
    if (CURRENT_OFFSET() - notes_timeouts[index] > notes_to && self->index == index)
    {
        self->TG.silence = 0;
        self->TG.period = self->notePeriods[self->index];
        ASYNC(&self->TG, setDAC, 0xFFFFFFFF);
    }
}

void notes_ack(MusicPlayer* self, int index)
{
    notes_timeouts[index] = CURRENT_OFFSET();
}
