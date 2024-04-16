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

/**
 * - Gå inte vidare i melodin föräns acks är receivet, om timeout
 *      så spelar conductorn noten själv och går senare vidare
 *
 *
 *
 *
 *
 */

#define PROBLEM_1

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
CanHandler can_handler = init_can_object(&musicPlayer, &board_handler);
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
        if (SYNC(&board_handler, is_conductor, 0))
        {
            if (SYNC(&musicPlayer, togglePlaying, UNUSED))
            {
                SCI_WRITE(&sci0, "Playing\n");
            }
            else
            {
                SCI_WRITE(&sci0, "Paused\n");
            }
        }
        break;
#ifdef PROBLEM_1
    case 'z': {
        if (board_handler.node_states[RANK] == DISCONNECTED)
        {
            SetBoardState state = {CONDUCTOR, RANK};
            SYNC(&board_handler, set_index, (int)&state);
            break;
        }
    }
    case 'x': {
        if (board_handler.node_states[RANK] == DISCONNECTED)
        {
            SetBoardState state = {MUSICIAN, RANK};
            SYNC(&board_handler, set_index, (int)&state);
            break;
        }
    }
#endif
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
    SCI_WRITE(&sci0, "Initialize the board as conductor by typing 'z' or 'x' for musician\n");
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

void send_tone_msg(MusicPlayer* musig_player_p, int notes_msg_p)
{
    static CANMsg msg;
    notes_to_data(&msg, (Notes*)notes_msg_p);
    ASYNC(&can0, send_msg, (int)&msg);
}

void send_conductor_handout(BoardHandler* board_handler, int handout_msg_p)
{
    static CANMsg msg;
    handout_conductor_to_data(&msg, (HandoutConductor*)handout_msg_p);
    ASYNC(&can0, send_msg, (int)&msg);
}
