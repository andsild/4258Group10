#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "game.h"
#include "graphics.h"
#include "timer.h"
#include "devio.h"
#include "game_signals.h"

int main(int argc, char *argv[])
{
    if (initFrameBuffer() != 0) {
        fprintf(stderr, "Initializing framebuffer failed!\n");
        return EXIT_FAILURE;
    }

    if (initializeBackgroundScreen() < 0) {
        closeFrameBuffer();
        return EXIT_FAILURE;
    }

    if (loadGamepadDriver() != 0) {
        fprintf(stderr, "failed to initialize driver\n");
        clearMemory();
        return EXIT_FAILURE;
    }

    if (startSignalHandlerLoadingScreen() != 0) {
        fprintf(stderr, "could not initialize signal handler\n");
        close(fdGamepadDriver);
        closeFrameBuffer();
        return EXIT_FAILURE;
    }

    printf
        ("Welcome to StEpMaNiA!\n\n\nSelect difficulty SW1-7, or SW8 to quit\n");
    while (Difficulty == 0) {
        __asm("wfi");
    }

    printf
        ("Difficulty %d selected, use SW5 or SW7 to de- or increase difficulty\n",
         Difficulty);
    printf("Use left,up,right,down on your SW1-4 to use arrows\n");
    printf("\nHave fun :)\n");

    for (size_t i = 0; i < MAX_NUMBEROF_INCOMING * COLCOUNT; i++)
        markerColumns[i].isNotActive = 1;

    uint8_t tickCounter = 0;
    int numberOfTicksSinceLastMarkerAdded = 0;
    unsigned int columnToAssignMarkerTo = 0;

    startSignalHandlerMainloop();
    startTimer(TICK_FREQUENCY_SEC, TICK_FREQUENCY_MICROSEC);

    /** Each iteration in this loop is referred to as a "tick" */
    while (1) {
        while (timerHasNotExpired) {
            __asm("wfi");
        }
        timerHasNotExpired = 1;

        /** We only add markers when there is free space on the bottom.
         * We ensure this by counting ticks since last time we added a marker
         */
        if (numberOfTicksSinceLastMarkerAdded-- < 0) {
            columnToAssignMarkerTo = getNextMarkerColumn();
            if (columnToAssignMarkerTo) {
                numberOfTicksSinceLastMarkerAdded = 10;
                addMarkerToBoard(columnToAssignMarkerTo - 1);
            }
        }

        /** Remove previously drawn objects */
        cleanArrowCanvas();

        /** Move every marker that is in play */
        moveAllMarkers();

        renderHealthbar();
        renderScore();

        /** If we should draw a flash text, draw it */
        if (DoDrawFlashText) {
            setFlashText(DrawFlash);
            DoDrawFlashText--;
        }

        /** The above methods add objects to framebuffer memory, this is where we actually draw it */
        renderFramebufferCanvas();
        tickCounter++;
    }

    clearMemory();
    return EXIT_FAILURE;        /* Should not exit from here */
}
