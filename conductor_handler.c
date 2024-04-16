
#include "conductor_handler.h"
#include "TinyTimber.h"
#include "application.h"
#include "canMsgs.h"
#include "part2.h"

void start_conductor(ConductorHandler* self, int unused)
{
    if (SYNC(self->m_board_handler_p, get_conductor_index, 0) == RANK)
    {
        self->m_musicplayer_p->index++;
        int player_index = SYNC(self->m_board_handler_p, get_next_musician_index, 0);
        if (player_index == RANK)
        {
            ASYNC(self->m_musicplayer_p, nextBeat, 0);
        }
        else
        {
            Notes notes_msg;
            notes_msg.id = NOTESID;
            notes_msg.note_index = self->m_musicplayer_p->index;
            notes_msg.key = self->m_musicplayer_p->m_melody_p->key;
            notes_msg.player = player_index;
            notes_msg.tempo = self->m_musicplayer_p->tempo;
            notes_msg.volume = self->m_musicplayer_p->TG.volume;
        }
    }
}
