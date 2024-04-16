#include "application.h"
#include "TinyTimber.h"
#include "board_handler.h"
#include "canHandler.h"
#include "canMsgs.h"
#include "canTinyTimber.h"
#include "part0.h"
#include "part1.h"
#include "part2.h"
#include "sciTinyTimber.h"
#include "software_defines.h"
#include <stdbool.h>
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
#define MUSICIAN

// Brother John melody
const int brotherJohn[LENGTH] = {0, 2, 4, 0, 0, 2, 4, 0, 4, 5, 7, 4, 5, 7, 7, 9, 7, 5, 4, 0, 7, 9, 7, 5, 4, 0, 0, -5, 0, 0, -5, 0};
const char brotherJohnBeatLength[32] = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'b', 'a', 'a', 'b', 'c', 'c',
                                        'c', 'c', 'a', 'a', 'c', 'c', 'c', 'c', 'a', 'a', 'a', 'a', 'b', 'a', 'a', 'b'};
const char brotherJohnBeatLength1[32] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

void print(char*, int);

void reader(App*, int);
void keyHandler(App*, int);

void recieveBPM();
void receiveKey();
App app = {initObject(), 0, 'X'};
Melody melody = initMelody(brotherJohn, LENGTH);
BoardHandler board_handler = init_board_handler();

MusicPlayer musicPlayer = initMusicPlayer(120, &melody, &board_handler, brotherJohnBeatLength);
CanHandler can_handler = init_can_object(&app, &musicPlayer, &board_handler);
Can can0 = initCan(CAN_PORT0, &can_handler, receive_msg);

void notes_hanlder(Notes* msg);

Serial sci0 = initSerial(SCI_PORT0, &app, reader);
ReadBuffer readBuffer = initReadBuffer();

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
    switch (c)
    {
#ifdef MUSICIAN
    case '0' ... '9': // Add character to readbuffer
    case '-':
        ASYNC(&readBuffer, readBufferAdd, c);
        break;
    case 'c': // Lower volume
        print("Volume changed to: %d\n", SYNC(&musicPlayer.TG, volume, -1));
        break;
    case 'v': // Raise volume
        print("Volume changed to: %d\n", SYNC(&musicPlayer.TG, volume, 1));
        break;
    case 'k': // A key was received
        receiveKey();
        break;
    case 'm': // Toggle muting
        ASYNC(&musicPlayer.TG, toggleMute, UNUSED);
        break;
    case 'b':
        recieveBPM();
        break;
    case 's':
        if (musicPlayer.isConductor)
        {
            if (SYNC(&musicPlayer, togglePlaying, UNUSED))
                SCI_WRITE(&sci0, "Playing\n");
            else
                SCI_WRITE(&sci0, "Paused\n");
        }
        break;
    case 'z':
        ASYNC(&musicPlayer, assignMode, true);
        break;
    case 'x':
        ASYNC(&musicPlayer, assignMode, false);
        break;
    }
#elif defined(CONDUCTOR)
        switch (c)
        {
        case '0' ... '9': // Add character to readbuffer
        case '-':
            ASYNC(&readBuffer, readBufferAdd, c);
            break;
        case 'c': // Lower volume
            sendCANMsg('c', 0);
            break;
        case 'v': // Raise volume
            sendCANMsg('v', 0);
            break;
        case 'k':                                                  // A key was received
            sendCANMsg('k', SYNC(&readBuffer, endBuffer, UNUSED)); //
            break;
        case 'm': // Toggle muting
            sendCANMsg('m', 0);
            break;
        case 'b': // BPM
            sendCANMsg('b', SYNC(&readBuffer, endBuffer, UNUSED));
            break;
        case 's': // Start / stop
            sendCANMsg('s', 0);
            break;
        }
#endif
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
    SYNC(&melody, setKey, key);

    int melodyPeriods[LENGTH];

    SYNC(&melody, setMelodyPeriods, (int)melodyPeriods);
    ASYNC(&musicPlayer, setPeriods, (int)melodyPeriods);
}

void recieveBPM()
{
    // Get key, print it and set it
    const int bpm = SYNC(&readBuffer, endBuffer, UNUSED);
    char output[50];
    sprintf(output, "BPM: %d\n", bpm);
    SCI_WRITE(&sci0, output);
    ASYNC(&musicPlayer, setTempo, bpm);
}

void startApp(App* self, int arg)
{
    SCI_INIT(&sci0);
    SCI_WRITE(&sci0, "Hello, hello...\n");
    ASYNC(&musicPlayer, nextBeat, 0);
}

int main()
{
    INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
    init_canhandler(&can_handler, &can0);
    TINYTIMBER(&app, startApp, 0);
    return 0;
}

void heartbeat_tmo_check_1(App* self, int call_time_p)
{
    Time call_time = *((int*)call_time_p);
    if (CURRENT_OFFSET() - call_time > HEARTBEATTO)
    {
    }
}

void heartbeat_tmo_check_2(App* self, int call_time_p)
{
    Time call_time = *((int*)call_time_p);
    if (CURRENT_OFFSET() - call_time > HEARTBEATTO)
    {
    }
}
