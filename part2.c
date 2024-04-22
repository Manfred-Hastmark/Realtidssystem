#include "part2.h"
#include "TinyTimber.h"
#include "application.h"
#include "board_handler.h"
#include "canMsgs.h"

typedef struct
{
    unsigned short timeout;
    unsigned short id;
} TOContainer;

void nextBeat(MusicPlayer* self, int unused)
{
    if (self->m_board_handler_p->node_states[RANK] == CONDUCTOR)
    {
        self->index++;
        self->index %= LENGTH;
        int player_index = SYNC(self->m_board_handler_p, get_next_player, 0);
        if (player_index == RANK)
        {
            self->m_tone_generator_p->silence = 0;
            self->m_tone_generator_p->period = self->notePeriods[self->index];
            ASYNC(self->m_tone_generator_p, setDAC, 0xFFFFFFFF);
        }
        if (player_index != -1)
        {
            static Notes notes_msg;
            notes_msg.note_index = self->index;
            notes_msg.player = player_index;
            notes_msg.tempo = 60000 / self->tempo;
            notes_msg.key = self->m_melody_p->key;
            notes_msg.id = NOTESID;
            ASYNC(self->m_app_p, send_notes_msg, (int)&notes_msg);
        }
    }
    const int toneDuration = MSEC(getBeatLenght(self->beatLength[self->index], self->tempo, self->silenceDuration));
    SEND(toneDuration, toneDuration + USEC(100), self, nextSilence, 0);
}

void nextSilence(MusicPlayer* self, int unused)
{
    // Turns of the tone generator
    self->m_tone_generator_p->silence = 1;

    if (self->m_board_handler_p->conductor_change)
    {
        self->m_board_handler_p->conductor_change = 0;
        const int silenceDuration = MSEC(self->silenceDuration);
        AFTER(silenceDuration, self->m_app_p, send_handout_conductor, self->m_board_handler_p->new_conductor_index);
        return;
    }

    // Sleep until the next note
    if (self->playing == 1 && self->m_board_handler_p->node_states[RANK] == CONDUCTOR)
    {
        const int silenceDuration = MSEC(self->silenceDuration);
        SEND(silenceDuration, silenceDuration + USEC(100), self, nextBeat, 0);
        return;
    }
    self->playing = 0;
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
