#include "TinyTimber.h"
#include "sciTinyTimber.h"
#include "canTinyTimber.h"
#include <stdlib.h>
#include <stdio.h>

/* HOW TO USE
 * LAB0
 * 'F' clears the threeHist
 * Keys 0-9 and '-' are used to input number
 * Ending a number input with 'e', adds it to threeHist and calculates sum and median
 * Ending a number input with 'k', interprets it as a key and prints the periods for Brother john melody, note a key must be within [-5, 5]
 * 
 * LAB1
 * 'v' is used to increase volume
 * 'c' is used to decrease volume
 * 'm' toggles mute on/off
 * 'd' toggles deadlines on/off
 * 'x' increase backgroundload by 500
 * 'z' decreases backgroundload by 500
 */

#define UNUSED 0

//#define LAB0
//#define LAB1
#define LAB2

#ifdef LAB0
#include "part0.h"
#endif

#ifdef LAB1
#include "part0.h"
#include "part1.h"
#endif

#ifdef LAB2
#include "part0.h"
#include "part1.h"
#include "part2.h"
#endif

//Brother John melody
const int length = 32;
const int brotherJohn[32] = {0, 2, 4, 0, 0, 2, 4, 0, 4, 5, 7, 4, 5, 7, 7, 9, 7, 5, 4, 0, 7, 9, 7, 5, 4, 0, 0, -5, 0, 0, -5, 0};
const char brotherJohnBeatLength[32] = "aaaaaaaaaabaabccccaaccccaaaabaab";
const char brotherJohnBeatLength1[32] =  "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";


void print(char*, int);

typedef struct {
    Object super;
    int count;
    char c;
} App;

App app = { initObject(), 0, 'X' };
void reader(App*, int);
//void receiver(App*, int);
void keyHandler(App*, int);

#ifdef LAB0
void receiveKey(App*, int);
void receiveInt(App*, int);
ThreeHist threeHist = initThreeHist();
Melody melody = initMelody(brotherJohn, length);
#endif

#ifdef LAB1
void runTest();
ToneGenerator toneGenerator = initToneGenerator(1000);  //1000Hz = 1000us, 769Hz = 1300us, 537Hz = 1862
BackgroundTask backgroundTask = initBackgroundTask();
#endif

#ifdef LAB2
void recieveBPM();
void receiveKey();
Melody melody = initMelody(brotherJohn, length);
MusicPlayer musicPlayer = initMusicPlayer(500, brotherJohnBeatLength);
#endif




Serial sci0 = initSerial(SCI_PORT0, &app, reader);

ReadBuffer readBuffer = initReadBuffer();

//Can can0 = initCan(CAN_PORT0, &app, receiver);

/*
void receiver(App *self, int unused) 
{
    CANMsg msg;
    CAN_RECEIVE(&can0, &msg);
    SCI_WRITE(&sci0, "Can msg received: ");
    SCI_WRITE(&sci0, msg.buff);
}*/

void reader(App *self, int c) 
{
    SCI_WRITE(&sci0, "Rcv: \'");
    SCI_WRITECHAR(&sci0, c);
    SCI_WRITE(&sci0, "\'\n");
	
	//Call the keyhandler
	ASYNC(&app, keyHandler, c);
}

void keyHandler(App* self, int c)
{
	switch (c)
	{
		#ifdef LAB0
		case 'F': //Erase the three history
			ASYNC(&threeHist, emptyThreeHist, UNUSED);
			SCI_WRITE(&sci0, "The 3-history has been erased\n");
			break;
		case '0'...'9': //Add character to readbuffer
		case '-':
			ASYNC(&readBuffer, readBufferAdd, c);
			break;
		case 'e':	//An int was inputed
			ASYNC(&app, receiveInt, UNUSED);
			break;
		case 'k':	//A key was received
			ASYNC(&app, receiveKey, UNUSED);
			break;
		#endif
		#ifdef LAB1
		case 'v': //Raise volume
			print("Volume changed to: %d\n", SYNC(&toneGenerator, volume, 1));
			break;
		case 'c': //Lower volume
			print("Volume changed to: %d\n", SYNC(&toneGenerator, volume, -1));
			break;
		case 'x': //Raise backgroundTask
			print("BackgroundTask load is: %d\n", SYNC(&backgroundTask, load, 500));
			break;
		case 'z': //Lower backgroundTask
			print("BackgroundTask load is: %d\n", SYNC(&backgroundTask, load, -500));
			break;
		case 'm': //Toggle muting
			ASYNC(&toneGenerator, toggleMute, UNUSED);
			break;
		case 'd': //Toggle deadlines
		{
			const int deadlineToneGen = SYNC(&toneGenerator, toggleDeadlinesToneGenerator, UNUSED);
			const int deadlineBackTask = SYNC(&backgroundTask, toggleDeadlinesBackgroundTask, UNUSED);
			if(deadlineBackTask & deadlineToneGen)
				SCI_WRITE(&sci0, "Deadlines on\n");
			else
				SCI_WRITE(&sci0, "Deadlines off\n"); 
 			break;
		}
		#endif
		#ifdef LAB2
		case '0'...'9': //Add character to readbuffer
		case '-':
			ASYNC(&readBuffer, readBufferAdd, c);
			break;
		case 'c': //Lower volume
			print("Volume changed to: %d\n", SYNC(&musicPlayer.TG, volume, -1));
			break;
		case 'v': //Raise volume
			print("Volume changed to: %d\n", SYNC(&musicPlayer.TG, volume, 1));
			break;
		case 'k':	//A key was received
			receiveKey();
			break;
		case 'm': //Toggle muting
			ASYNC(&musicPlayer.TG, toggleMute, UNUSED);
			break;
		case 't':
			recieveBPM();
			break;
//SYNC(&melody, setMelodyPeriods, (int)(musicPlayer.notePeriods));
		#endif
	}
}

void print(char* string, int val)
{
	char output[50];
	sprintf(output, string, val);
	SCI_WRITE(&sci0, output);
}

#ifdef LAB0
void receiveInt(App* self, int unused)
{
	const int number = SYNC(&readBuffer, endBuffer, UNUSED);
	SYNC(&threeHist, addThreeHist, number);
	const int sum = SYNC(&threeHist, sumBuffer, UNUSED);
	const int median = SYNC(&threeHist, medianBuffer, UNUSED);
	char output[80];
	sprintf(output, "Entered integer %d: sum = %d, median = %d\n", number, sum, median);
	SCI_WRITE(&sci0, output);
}

void receiveKey(App* self, int unused) 
{
	//Get key, print it and set it
	const int key = SYNC(&readBuffer, endBuffer, UNUSED);
	char output[50];
	sprintf(output, "Key: %d\n", key);
	SCI_WRITE(&sci0, output);
	SYNC(&melody, setKey, key);
	
	int melodyPeriods[length];
	
	SYNC(&melody, setMelodyPeriods, (int) melodyPeriods);
	
	//Print the periods
	for(int i = 0; i < length; i++)
	{
		sprintf(output, "%d ", melodyPeriods[i]);
		SCI_WRITE(&sci0, output);
	}
	SCI_WRITECHAR(&sci0, '\n');
}
#endif

#ifdef LAB1
void runTest()
{
	int toneGeneratorTest[2];
	int backgroundTaskTest[2];
	char output[50];
	SCI_WRITE(&sci0, "Performing test\n");
	
	SYNC(&toneGenerator, dummySetDac, (int) toneGeneratorTest);
	sprintf(output, "ToneGeneratorTest: max is %d, avg is: %d\n", toneGeneratorTest[0], toneGeneratorTest[1]);
	SCI_WRITE(&sci0, output);
	
	backgroundTaskTest[0] = 21000;
	SYNC(&backgroundTask, dummyRunLoad, (int) backgroundTaskTest);
	sprintf(output, "BackgroundTest21000: max is %d, avg is: %d\n", backgroundTaskTest[0], backgroundTaskTest[1]);
	SCI_WRITE(&sci0, output);
	
	backgroundTaskTest[0] = 1000;
	SYNC(&backgroundTask, dummyRunLoad, (int) backgroundTaskTest);
	sprintf(output, "BackgroundTest1000: max is %d, avg is: %d\n", backgroundTaskTest[0], backgroundTaskTest[1]);
	SCI_WRITE(&sci0, output);
}
#endif

#ifdef LAB2
void receiveKey() 
{
	//Get key, print it and set it
	const int key = SYNC(&readBuffer, endBuffer, UNUSED);
	char output[50];
	sprintf(output, "Key: %d\n", key);
	SCI_WRITE(&sci0, output);
	SYNC(&melody, setKey, key);
	
	int melodyPeriods[length];
	
	SYNC(&melody, setMelodyPeriods, (int) melodyPeriods);
	ASYNC(&musicPlayer, setPeriods, (int) melodyPeriods);
}

void recieveBPM() 
{
	//Get key, print it and set it
	const int bpm = SYNC(&readBuffer, endBuffer, UNUSED);
	char output[50];
	sprintf(output, "BPM: %d\n", bpm);
	SCI_WRITE(&sci0, output);
	ASYNC(&musicPlayer, setTempo, bpm);
}


#endif

void startApp(App *self, int arg) 
{
    //CANMsg msg;

    //CAN_INIT(&can0);
    SCI_INIT(&sci0);
    SCI_WRITE(&sci0, "Hello, hello...\n");

	/*
    msg.msgId = 1;
    msg.nodeId = 1;
    msg.length = 6;
    msg.buff[0] = 'H';
    msg.buff[1] = 'e';
    msg.buff[2] = 'l';
    msg.buff[3] = 'l';
    msg.buff[4] = 'o';
    msg.buff[5] = 0;*/
    //CAN_SEND(&can0, &msg);
	
	#ifdef LAB1
	//runTest();
	ASYNC(&toneGenerator, setDAC, 0); 
	ASYNC(&backgroundTask, runLoad, 1000);
	#endif
	
	#ifdef LAB2
	// Verri Naice
	ASYNC(&musicPlayer, nextBeat, 0);
	#endif
}

int main() 
{
    INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
	//INSTALL(&can0, can_interrupt, CAN_IRQ0);
    TINYTIMBER(&app, startApp, 0);
    return 0;
}
