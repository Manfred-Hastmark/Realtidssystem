#ifndef PART2
#define PART2
#include "canMsgs.h"
#include "part1.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * How to run this lib:
 * - Create an int array of size 32 with 32 notes.clTabCtrl
 * 		- TODO: Make it take any size of list.
 *
 * - Create a char array of size 32 with beat length that corresponds to each note:
 * 		- a -> full note, b -> 2*a, c -> a/2.
 *
 * - Use the initMusicPlayer macro to init a music player with the corresponding arguments:
 * 		- BPM which sets the BPM of the song.
 * 		- beatLength which is the the char array if beat length.
 *
 *  - To start the music player: use nextBeat(MusicPlayer*, int) and give it the starting index of the song. (Usually 0).
 *  - To change tempo use: setTempo(MusicPlayer*, int).
 * 	- To change key use: void setPeriods(MusicPlayer*, int) and give it a 32 int array with the new song / key.
 */

typedef struct
{
    ToneGenerator TG;    // ToneGenerator
    int tempo;           // 60000 / bpm represended in ms
    int silenceDuration; // 60000 / bpm * 10 which is 10% of the note
    char* beatLength;    // How long every beat should be
    int notePeriods[32]; // A char array with 32 notePeriods
    int playing;
    int index;
    bool isConductor;
    bool isInitialized;
} MusicPlayer;

#define initMusicPlayer(BPM, beatLength)                                                                                                   \
    {                                                                                                                                      \
        initToneGenerator(1000), 60000 / (int)BPM, 6000 / (int)BPM, (char*)beatLength,                                                     \
            {1136, 1012, 902, 1136, 1136, 1012, 902, 1136, 902, 851,  758,  902,  851,  758,  758,  676,                                   \
             758,  851,  902, 1136, 758,  676,  758, 851,  902, 1136, 1136, 1517, 1136, 1136, 1517, 1136},                                 \
            0, false                                                                                                                       \
    }

int getBeatLenght(char c, int ms, int silenceDuration);

/**
 * @brief Looks up the frequency for the next note and applies it to the tone generator. Turns on the tone generator and sleeps until the
 * note ends.
 *
 * @param MusicPlayer* musicPlayer.
 * @param int index. Current note index.
 * @return void.
 */

void nextBeat(MusicPlayer* self, int index);

/**
 * @brief Do not call this funtion. Internal only! Stops the tone generator and tells it to silence the DAC. Then sleeps until the next note
 * should start.
 *
 * @param MusicPlayer* musicPlayer.
 * @param int index. Current note index.
 * @return void.
 */
void nextSilence(MusicPlayer* self, int index);

/**
 * @param MusicPlayer* musicPlayer.
 * @param int BPM. New BPM.
 * @return void.
 */
void setTempo(MusicPlayer* self, int bpm);

/**
 * @brief Sets the current index
 */
void set_index(MusicPlayer* self, int index);

/**
 * @brief Changes all the periods of a given array.
 *
 * @param MusicPlayer* musicPlayer.
 * @param (int) int* array. Takes an array with new precalculated periods (currently hard coded to a size of 32).
 * @return void.
 */
void setPeriods(MusicPlayer* self, int arrIn);

int togglePlaying(MusicPlayer* self, int unused);

/**
 * @brief returns -1 if playing
 */
int is_playing(MusicPlayer* self);

void assignMode(MusicPlayer* self, int mode);

#endif
