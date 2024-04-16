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
#define MUSICIAN

#define UNUSED 0

#define CONDUCTOR
// #define MUSICIAN

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

Melody melody = initMelody(brotherJohn, LENGTH);
ToneGenerator tone_generator = initToneGenerator(1000);
MusicPlayer musicPlayer = initMusicPlayer(120, brotherJohnBeatLength, &tone_generator, &melody);
App app = {initObject(), 0, 'X', 0};
ReadBuffer readBuffer = initReadBuffer();
BoardHandler board_handler = initBoardHandler();
HeartBeatHandler heart_beat_handler = initHeartBeatHandler(&app, &board_handler);
CanHandler can_handler = initCanHandler(&app, &heart_beat_handler, &musicPlayer);
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
    CAN_SEND(&can0, (CANMsg*)msg_p);
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

void reader(App* self, int c)
{
    SCI_WRITE(&sci0, "Rcv: \'");
    SCI_WRITECHAR(&sci0, c);
    SCI_WRITE(&sci0, "\'\n");

    // Call the keyhandler
    ASYNC(&app, keyHandler, c);
}

void keyHandler(App* self, int c)
{
    if (c == 'k')
    {
        self->to_heart_beat ^= 1;
    }
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
    ASYNC(&musicPlayer, nextBeat, 0);
    ASYNC(&heart_beat_handler, init, 0);
}

int main()
{
    INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
    INSTALL(&can0, can_interrupt, CAN_IRQ0);
    TINYTIMBER(&app, startApp, 0);
    return 0;
}
