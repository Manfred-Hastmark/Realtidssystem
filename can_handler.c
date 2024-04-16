
#include "can_handler.h"
#include "TinyTimber.h"
#include "application.h"
#include "canMsgs.h"
#include "canTinyTimber.h"
#include "part2.h"

void can_receiver(CanHandler* self, int raw_msg_p)
{
    CANMsg* msg_p = (CANMsg*)raw_msg_p;

    switch (msg_p->msgId)
    {
    case HEARTBEATID ... HEARTBEATID + MAX_BOARDS - 1: {
        static HeartBeat heart_beat_msg;
        data_to_heart_beat(msg_p, &heart_beat_msg);
        heart_beat_msg.id -= HEARTBEATID;
        ASYNC(self->m_heart_beat_handler_p, heart_beat_received, (int)&heart_beat_msg);
        break;
    }
    case NOTESID: {
        Notes notes_msg;
        data_to_notes(msg_p, &notes_msg);
        self->m_music_player_p->index = notes_msg.note_index;
        SYNC(self->m_music_player_p, change_key, notes_msg.key);
        SYNC(self->m_music_player_p, change_bpm, notes_msg.tempo);
        if (notes_msg.player == RANK)
        {
            ASYNC(self->m_music_player_p, play_note, notes_msg.note_index);
        }
        break;
    }
    default:
        break;
    }
}
