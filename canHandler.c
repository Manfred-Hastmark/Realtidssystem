

#include "canHandler.h"
#include "TinyTimber.h"
#include "application.h"
#include "canMsgs.h"
#include "canTinyTimber.h"
#include "software_defines.h"

Can can0 = initCan(CAN_PORT0, &app, receive_msg);

Time last_heart_beat_1 = 0;
Time last_heart_beat_2 = 0;

void install_can_handler()
{
    INSTALL(&can0, can_interrupt, CAN_IRQ0);
}

void init_can_handler()
{
    CAN_INIT(&can0);
}

void send_msg(int index, const uchar* data, int length)
{
    CANMsg msg;
    msg.msgId = index;
    msg.nodeId = RANK_SELF;
    msg.length = length;
    *(unsigned long long*)msg.buff = *(unsigned long long*)data;
}

void receive_msg(App* self, uint8_t* data)
{
    CANMsg msg;
    CAN_RECEIVE(&can0, &msg);
    print("Can ID: %c, ", msg.msgId);
    uchar data_buff[8];
    *(unsigned long long*)data_buff = *(unsigned long long*)data;

    switch (msg.msgId)
    {
    case TIMESYNCID:
        return;
    case HEARTBEATID + RANK_OTHER_1:
        last_heart_beat_1 = CURRENT_OFFSET();
        SEND(HEARTBEATTO, HEARTBEATDL, &app, heartbeat_tmo_check_1, (int)&last_heart_beat_1);
        return;
    case HEARTBEATID + RANK_OTHER_2:
        last_heart_beat_2 = CURRENT_OFFSET();
        SEND(HEARTBEATTO, HEARTBEATDL, &app, heartbeat_tmo_check_2, (int)&last_heart_beat_2);
        return;
    case CLAIMCONDUCTORID:
        return;
    case NOTESID: {
        Notes notes_msg;
        data_to_notes(data_buff, &notes_msg);
        notes_handler(&notes_msg);
        return;
    }
    case NOTEACKSID:
        return;
    case HANDOUTCONDUCTORID:
        return;
    default:
        return;
    }
}
