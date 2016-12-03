#ifndef __GAME_H
#define __GAME_H

#include <stdint.h>
#include <stddef.h>

#define SPACING_BETWEEN_WAVES 30
#define ARROW_MOVEMENT_PER_TICK 8
#define COLCOUNT 4
#define HEALTH_BONUS 10
#define MAX_NUMBEROF_INCOMING 2
#define PLAYERONE_STARTING_HEALTH 50
#define SCORE_BOO_THRESHOLD (MAX_SCORE - 40)
#define SCORE_GREAT_THRESHOLD (MAX_SCORE - 30)
#define SCORE_PERFECT_THRESHOLD (MAX_SCORE - 10)
#define SCORE_RADIUS 45
#define SONG_MAXLENGTH 20
#define TICK_FREQUENCY_MICROSEC 50000
#define TICK_FREQUENCY_SEC 0
#define FLASHTEXT_TICK_DURATION 10
#define MAX_SCORE (SCORE_RADIUS+25)

extern volatile char userIsSelectingSong;

typedef struct Song {
    uint8_t columns[SONG_MAXLENGTH];
    int timings[SONG_MAXLENGTH];
    uint8_t index;
    uint8_t length;
} Song;

extern Song *currentSong;

typedef struct Marker {
    unsigned int yPos;
    uint16_t movementSpeed;
    /** Whether or not player has been given points for this
     */
    char isHitByPlayerOne;
    char isNotActive;
} Marker;
//1d better than 2d: http://stackoverflow.com/a/17260533/2428827
Marker markerColumns[MAX_NUMBEROF_INCOMING * COLCOUNT];

extern volatile int Difficulty;
extern int PlayerOneScore;
extern int PlayerOneHealth;
volatile extern int DoDrawFlashText;
extern char DrawFlash;

void addMarkerToBoard(uint8_t);
void buttonHandler(const uint8_t);
void clearMemory();
void moveAllMarkers();
unsigned int getNextMarkerColumn();

inline int gameIsFinished()
{
    for (size_t i = 0; i < MAX_NUMBEROF_INCOMING * COLCOUNT; i++)
        if (!markerColumns[i].isNotActive)
            return 0;
    return 1;
}

#endif
