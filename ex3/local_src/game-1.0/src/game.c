#include "game.h"
#include "devio.h"
#include "graphics.h"
#include "game_signals.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

int PlayerOneScore = 0;
int PlayerOneHealth = PLAYERONE_STARTING_HEALTH;
volatile char userIsSelectingSong = 1;
char DrawFlash = 0;
volatile int DoDrawFlashText = 0;
volatile int Difficulty = 0;

Song *currentSong;

/** Gracefully ALL malloced-regions and file descriptors
 */
void clearMemory()
{
    free(currentSong);
    free(bitmapInfoHeader);
    free(bitmapFileHeader);

    if (close(fdGamepadDriver) == -1)
        perror("Error when closing gamepad-driver");
    closeFrameBuffer();
}

/** Move all the active markers on-screen
 */
void moveAllMarkers()
{
    uint16_t yPos;

    unsigned char *bitmapData;
    bitmapData = getArrow();

    for (size_t c = 0; c < COLCOUNT; c++) {
        uint8_t rotation;
        rotation = c;
        /* Some columns are not the same on-screen as they are in the loadBMP  method - so we need to if-check them */
        if (c == 0)
            rotation = 1;
        else if (c == 1)
            rotation = 0;

        /** For every marker in this column */
        for (size_t i = c * MAX_NUMBEROF_INCOMING;
             i < (c * MAX_NUMBEROF_INCOMING) + MAX_NUMBEROF_INCOMING; i++) {
            if (markerColumns[i].isNotActive)
                continue;

            // TODO: could be a constant instead of in-struct  as it is right now...
            markerColumns[i].yPos -= markerColumns[i].movementSpeed;

            yPos = markerColumns[i].yPos;
            /** If maker is still on screen (valid range) */
            if (yPos >= ARROW_CEILING && yPos <= ARROW_STARTING_YPOS) {
                loadBMPToFrameBuffer(bitmapData, rotation, 1,
                                     COLUMN_POSITION + (ARROW_PADDING * c),
                                     markerColumns[i].yPos);
            } else {
                markerColumns[i].isNotActive = 1;
                /** If a player failed to hit this, penalize the player */
                if (!markerColumns[i].isHitByPlayerOne) {
                    PlayerOneHealth -= HEALTH_BONUS;
                    /** Keep health at sane levels (not below 0) */
                    if (PlayerOneHealth < 0)
                        PlayerOneHealth = 0;
                    DrawFlash = MISS;
                    DoDrawFlashText = FLASHTEXT_TICK_DURATION;
                }
            }
        }
    }

    free(bitmapData);
    bitmapData = NULL;
}

/** Add a marker to a column
 * 
 * @column the column to add a marker to
 */
void addMarkerToBoard(uint8_t column)
{
    uint16_t offsetStart = MAX_NUMBEROF_INCOMING * column;

    /** For each slot in this column  */
    for (size_t offset = offsetStart;
         offset < offsetStart + MAX_NUMBEROF_INCOMING; offset++)
        if (markerColumns[offset].isNotActive) {
            markerColumns[offset] = (Marker) {
            ARROW_STARTING_YPOS, ARROW_MOVEMENT_PER_TICK, 0, 0};
            break;
        }
}

//TODO: rewrite signal-safe! Subject to race-conditions on multiple button pushes or timer-interrupt
/** Main loop handler for putton push events */
void buttonHandler(const uint8_t column)
{
    int closestYposDistance = INT32_MAX;
    int closestMarkerIndex = -1;

    /** For all markers */
    for (size_t i = column * MAX_NUMBEROF_INCOMING;
         i < (column * MAX_NUMBEROF_INCOMING) + MAX_NUMBEROF_INCOMING; i++) {
        unsigned int distance =
            abs(markerColumns[i].yPos - STATIONARY_ARROW_YPOS);
        /** If this is the best column seen so far that the player has not received points for */
        if (!markerColumns[i].isNotActive && !markerColumns[i].isHitByPlayerOne
            && distance < closestYposDistance) {
            closestYposDistance = distance;
            /** Remember which marker this is */
            closestMarkerIndex = i;
        }
    }

    /** Is the marker in a feasible range? */
    // We don't care if a player pushes a button and theres no marker in range
    if (closestMarkerIndex == -1 || closestYposDistance > SCORE_RADIUS)
        return;

    unsigned int score;

    score = MAX_SCORE - (closestYposDistance);

    markerColumns[closestMarkerIndex].isHitByPlayerOne = 1;
    PlayerOneScore += score;

    // Self-explanatory?
    if (score > SCORE_PERFECT_THRESHOLD) {
        DrawFlash = PERFECT;
        DoDrawFlashText = FLASHTEXT_TICK_DURATION;
        PlayerOneHealth += HEALTH_BONUS * 2;
    } else if (score > SCORE_GREAT_THRESHOLD) {
        DrawFlash = GREAT;
        DoDrawFlashText = FLASHTEXT_TICK_DURATION;
        PlayerOneHealth += HEALTH_BONUS;
    } else if (score > SCORE_BOO_THRESHOLD) {
        DrawFlash = BOO;
        DoDrawFlashText = FLASHTEXT_TICK_DURATION;
    } else {
        setFlashText(MISS);
        if (PlayerOneHealth - HEALTH_BONUS > 0)
            PlayerOneHealth -= HEALTH_BONUS;
    }

    /** Keep health at sane levels: no more than max length */
    if (PlayerOneHealth > LENGTH_OF_HEALTHBAR)
        PlayerOneHealth = LENGTH_OF_HEALTHBAR;
}

/** Get the next marker column, randomly, with a chance given by global variable Difficulty
 *
 * @return column to put next marker in, 1-indexed
 */
unsigned int getNextMarkerColumn()
{
    int RandomChance = rand() % 10;

    if (RandomChance < (Difficulty + 1)) {
        return (rand() % COLCOUNT) + 1;
    }

    return 0;
}
