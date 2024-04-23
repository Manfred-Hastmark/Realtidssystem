#include "application.h"
#include "TinyTimber.h"
#include "board_handler.h"
#include "canMsgs.h"
#include "canTinyTimber.h"
#include "can_handler.h"
#include "heart_beat_handler.h"
#include "part0.h"
#include "part1.h"
#include "part2.h"
#include "sciTinyTimber.h"
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_KEY 0
#define DEFAULT_BPM 120

/* HOW TO USE
 * LAB0
 * 'F' clears the threeHist
 * Keys 0-9 and '-' are used to input number
 * Ending a number input with 'e', adds it to threeHist and calculates sum and
 * median Ending a number input with 'k', interprets it as a key and prints the
 * periods for Brother john melody, note a key must be within [-5, 5]
 *
 * LAB1
 * 'v' is used to increase volume
 * 'c' is used to decrease volume
 * 'm' toggles mute on/off
 * 'd' toggles deadlines on/off
 * 'x' increase backgroundload by 500
 * 'z' decreases backgroundload by 500
 * 'b' sets bpm
 * 's' start / pause
 */

#define UNUSED 0

// Brother John melody
const int brotherJohn[LENGTH] = {0, 2, 4, 0, 0, 2, 4, 0, 4, 5, 7, 4, 5, 7, 7, 9, 7, 5, 4, 0, 7, 9, 7, 5, 4, 0, 0, -5, 0, 0, -5, 0};
const char brotherJohnBeatLength[LENGTH] = "aaaaaaaaaabaabccccaaccccaaaabaab";
const char brotherJohnBeatLength1[LENGTH] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

void receiveKey();
void recieveBPM();
void reader(App*, int);
void receive(App*, int);
void send(App*, int);
void keyHandler(App*, int);
void send_heart_beat(App*, int);
void reset_fail2(App* self, int unused);

Melody melody = initMelody(brotherJohn, LENGTH);
ToneGenerator tone_generator = initToneGenerator(1000);
App app = {initObject(), 0, 'X', 0, 1, 1};
BoardHandler board_handler = initBoardHandler(&app);
MusicPlayer musicPlayer = initMusicPlayer(120, brotherJohnBeatLength, &tone_generator, &melody, &app, &board_handler);
ReadBuffer readBuffer = initReadBuffer();
HeartBeatHandler heart_beat_handler = initHeartBeatHandler(&app, &board_handler);
CanHandler can_handler = initCanHandler(&app, &heart_beat_handler, &musicPlayer, &board_handler);
Can can0 = initCan(CAN_PORT0, &app, receive);
Serial sci0 = initSerial(SCI_PORT0, &app, reader);

void receive(App* self, int unused)
{
    static CANMsg msg;
    CAN_RECEIVE(&can0, &msg);
    ASYNC(&can_handler, can_receiver, (int)&msg);
}

void send(App* self, int msg_p)
{
    if (CAN_SEND(&can0, (CANMsg*)msg_p) != 0)
    {
        self->can_fail = 1;
        SYNC(&board_handler, reset_connection, UNUSED);

        if(board_handler.node_states[RANK] == CONDUCTOR)
        {
            SYNC(&board_handler, handle_conductor_disconnect, UNUSED);
        }
        
    }
    else if(self->can_fail)
    {
        self->can_fail = 0;
        AFTER(MSEC(500), &board_handler, join_choir, UNUSED); //Join after heartbeats should have arrived
    }
}

void send_heart_beat(App* self, int role)
{
    static HeartBeat heart_beat_msg;
    heart_beat_msg.role = role;
    heart_beat_msg.id = HEARTBEATID + RANK;

    static CANMsg msg;
    heart_beat_to_data(&msg, &heart_beat_msg);

    ASYNC(self, send, (int)&msg);
}

void send_notes_msg(App* self, int raw_notes_msg_p)
{
    static CANMsg msg;
    notes_to_data(&msg, (Notes*)raw_notes_msg_p);
    ASYNC(self, send, (int)&msg);
}

void send_note_ack(App* self, int note_index)
{
    static CANMsg msg;
    msg.msgId = NOTEACKSID + RANK;
    msg.buff[0] = note_index;
    msg.length = 1;
    ASYNC(self, send, (int)&msg);
}

void send_claim_conductorship(App* self, int unused)
{
    static CANMsg msg;
    msg.msgId = CLAIMCONDUCTORID + RANK;
    msg.length = 0;
    ASYNC(self, send, (int)&msg);
}

void send_handout_conductor(App* self, int index)
{
    print("Sending handout to %i\n", index);
    static CANMsg msg;
    msg.msgId = HANDOUTCONDUCTORID + RANK;
    msg.buff[0] = index;
    msg.length = 1;
    ASYNC(self, send, (int)&msg);
}

void start_playing(App* self, int unused)
{
    if (!musicPlayer.playing)
    {
        SYNC(&musicPlayer, togglePlaying, 0);
        return;
    }
    SYNC(&musicPlayer, togglePlaying, 0);
    SYNC(&musicPlayer, togglePlaying, 0);
}

void reader(App* self, int c)
{
    SCI_WRITE(&sci0, "Rcv: \'");
    SCI_WRITECHAR(&sci0, c);
    SCI_WRITE(&sci0, "\'\n");

    // Call the keyhandler
    ASYNC(&app, keyHandler, c);
}

int silent_failure = 0;

void keyHandler(App* self, int c)
{
    switch (c)
    {
    case 'x': {
        print("Forced musician\n", 0);
        board_handler.node_states[RANK] = MUSICIAN;
        break;
    }
    case 'z': {
        print("Forced conductor\n", 0);
        board_handler.node_states[RANK] = CONDUCTOR;
        break;
    }
    case 'R': {
        if (board_handler.node_states[RANK] == CONDUCTOR)
        {
            ASYNC(&musicPlayer, change_key, DEFAULT_KEY);
            ASYNC(&musicPlayer, change_bpm, DEFAULT_BPM);
        }
        break;
    }
    case '0' ... '9': // Add character to readbuffer
    case '-':
        ASYNC(&readBuffer, readBufferAdd, c);
        break;
    case 'b':
        if (board_handler.node_states[RANK] == CONDUCTOR)
        {
            recieveBPM();
        }
        break;
    case 'k':
        if (board_handler.node_states[RANK] == CONDUCTOR)
        {
            receiveKey();
        }
        break;
    case 'w': {
        if (silent_failure == 0)
        {
            print("Silent Failure\n", 0);
            print("Conductorship Void Due To Failure\n", 0);
            SYNC(&board_handler, reset_connection, 0);
            self->ack_notes = 0;
            self->to_heart_beat = 1;
            silent_failure = 1;
            musicPlayer.index = 0;
            board_handler.node_states[RANK] = DISCONNECTED;
        }
        else
        {
            print("Leave Silent Failure\n", 0);
            board_handler.node_states[RANK] = MUSICIAN;
            self->ack_notes = 1;
            self->to_heart_beat = 0;
            silent_failure = 0;
            AFTER(MSEC(500), &board_handler, join_choir, UNUSED);
            
        }
    }
    break;
    case 'q': {
        print("Silent Failure\n", 0);
        print("Conductorship Void Due To Failure\n", 0);
        self->ack_notes = 0;
        self->to_heart_beat = 1;
        silent_failure = 1;
        musicPlayer.index = 0;
        board_handler.node_states[RANK] = DISCONNECTED;
        AFTER(MSEC(5000), self, reset_fail2, 0);
    }
    break;

    case 'p': {
        ASYNC(&board_handler, request_conductorship, 0);
        break;
    }
    case 's': {
        if (board_handler.node_states[RANK] == CONDUCTOR)
        {
            ASYNC(&musicPlayer, togglePlaying, 0);
        }
        break;
    }
    case 'r': {
        if (board_handler.node_states[RANK] == CONDUCTOR)
        {
            musicPlayer.index = 0;
        }
        break;
    }
    case 'm':
        ASYNC(&board_handler, print_status, 0);
        break;
    case 'c': // Lower volume
        print("Volume changed to: %d\n", SYNC(musicPlayer.m_tone_generator_p, volume, -1));
        break;
    case 'v': // Raise volume
        print("Volume changed to: %d\n", SYNC(musicPlayer.m_tone_generator_p, volume, 1));
        break;
    case 'l': {
        SYNC(&tone_generator, toggleMute, 0);
        break;
    }
    }
}

void reset_fail2(App* self, int unused)
{
    print("Leave Silent Failure\n", 0);
    self->ack_notes = 1;
    self->to_heart_beat = 0;
    silent_failure = 0;
    board_handler.node_states[RANK] = MUSICIAN;
    ASYNC(&board_handler, join_choir, 0);
}

void print(char* string, int val)
{
    char output[50];
    sprintf(output, string, val);
    SCI_WRITE(&sci0, output);
}

void receiveKey()
{
    // Get key, print it and set it
    const int key = SYNC(&readBuffer, endBuffer, UNUSED);
    char output[50];
    sprintf(output, "Key: %d\n", key);
    SCI_WRITE(&sci0, output);
    ASYNC(&musicPlayer, change_key, key);
}

void recieveBPM()
{
    // Get key, print it and set it
    const int bpm = SYNC(&readBuffer, endBuffer, UNUSED);
    char output[50];
    sprintf(output, "BPM: %d\n", bpm);
    SCI_WRITE(&sci0, output);
    ASYNC(&musicPlayer, change_bpm, bpm);
}

void startApp(App* self, int arg)
{
    CAN_INIT(&can0);
    SCI_INIT(&sci0);
    SCI_WRITE(&sci0, "Hello, hello...\n");
    static HeartBeat hb_msg;
    hb_msg.id = RANK;
    hb_msg.role = MUSICIAN;
    AFTER(SEC(1), &board_handler, handle_node_alive, (int)&hb_msg);
    ASYNC(&musicPlayer, nextBeat, 0);
    ASYNC(&heart_beat_handler, init, 0);
}

void reset_index(App* self, int unused)
{
    musicPlayer.index = 0;
}

int main()
{
    INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
    INSTALL(&can0, can_interrupt, CAN_IRQ0);
    TINYTIMBER(&app, startApp, 0);
    return 0;
}
