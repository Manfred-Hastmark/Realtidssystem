#ifndef PART2
#define PART2
#include "TinyTimber.h"
#include "part0.h"
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
    Object obj;
    ToneGenerator* m_tone_generator_p; // ToneGenerator
    Melody* m_melody_p;
    int tempo;           // 60000 / bpm represended in ms
    int silenceDuration; // 60000 / bpm * 10 which is 10% of the note
    char* beatLength;    // How long every beat should be
    int notePeriods[32]; // A char array with 32 notePeriods
    int playing;
} MusicPlayer;

#define initMusicPlayer(BPM, beatLength, tone_generator_p, melody_p)                                                                       \
    {                                                                                                                                      \
        initObject(), tone_generator_p, melody_p, 60000 / (int)BPM, 6000 / (int)BPM, (char*)beatLength,                                    \
            {1136, 1012, 902, 1136, 1136, 1012, 902, 1136, 902, 851,  758,  902,  851,  758,  758,  676,                                   \
             758,  851,  902, 1136, 758,  676,  758, 851,  902, 1136, 1136, 1517, 1136, 1136, 1517, 1136},                                 \
            0                                                                                                                              \
    }

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

/*
 * @brief Plays the note given the index
 */
void play_note(MusicPlayer* self, int index);

/**
 * @brief Change the key of the melody
 */
void change_key(MusicPlayer* self, int key);

/**
 * @brief Change the bpm of the melody
 */
void change_bpm(MusicPlayer* self, int bpm);

/**
 * @brief Takes some parameters and calculates the correct beat length.
 *
 * @param char beatLength. How long the note should be.
 * @param int tempo. The time it takes to play a normal note (a) in MS.
 * @param int silenceDuration. How much of the note should be silent in MS.
 * @return int. Returns the length of a given note in ms.
 */
static inline int getBeatLenght(char c, int ms, int silenceDuration);

int togglePlaying(MusicPlayer* self, int unused);

#endif
