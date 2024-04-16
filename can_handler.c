
#include "can_handler.h"
#include "TinyTimber.h"
#include "application.h"
#include "canMsgs.h"
#include "canTinyTimber.h"

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
        break;
    }
    default:
        break;
    }
}
