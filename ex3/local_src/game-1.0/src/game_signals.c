#include "game_signals.h"

#include "game.h"
#include "devio.h"
#include "timer.h"

#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

volatile char timerHasNotExpired = 1;

/** Start the asynchronous signal-handlers for the loading screen
 *
 * @return < 0 on error
 */
int startSignalHandlerLoadingScreen()
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = loadingScreenSignalHandler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGIO, &sa, NULL) < 0) {
        perror("Cannot handle SIGIO");
        return -1;
    }

    sa.sa_flags = SA_RESETHAND;

    return 0;
}

/** Start the asynchronous signal-handlers for the main gameloop
 *
 * @return < 0 on error
 */
int startSignalHandlerMainloop()
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = gameLoopSignalHandler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("Cannot handle SIGINT");
        return -1;
    }
    if (sigaction(SIGIO, &sa, NULL) < 0) {
        perror("Cannot handle SIGIO");
        return -1;
    }
    if (sigaction(SIGALRM, &sa, NULL) < 0) {
        perror("Cannot handle SIGALRM");
        return -1;
    }
    return 0;
}

/** Signalhanlder for main game loop
 *
 * @param signal to handle
 */
void gameLoopSignalHandler(int signum)
{
    /** Button push */
    if (signum == SIGIO) {
        int numberOfBytesRead __attribute__ ((unused));
        uint16_t buttonValue;
        numberOfBytesRead = read(fdGamepadDriver, &buttonValue, 2);

        if (buttonValue == SW8) {
            clearMemory();
            exit(0);
        }
        // Assuming SW1-4 is left,up,right,down, 
        // we need to map them to the arrow index on the gamepad: left,down, up, right
        else if (buttonValue == SW1)
            buttonHandler(0);
        else if (buttonValue == SW2)
            buttonHandler(2);
        else if (buttonValue == SW3)
            buttonHandler(3);
        else if (buttonValue == SW4)
            buttonHandler(1);
        /** Decrease difficulty */
        else if (buttonValue == SW5)
            Difficulty--;       // Would be nice to notify the user with printf, but that is not async-safe...
        /** Increase difficulty */
        else if (buttonValue == SW7)
            Difficulty++;       // Would be nice to notify the user with printf, but that is not async-safe...

    }
    /** An interrupt - would be nice to clear out the memory properly */
    else if (signum == SIGINT) {
        clearMemory();
        exit(1);
    }
    /** timer event */
    // (requires someone calling startTimer(...))
    else if (signum == SIGALRM) {
        timerHasNotExpired = 0;
    } else {
        fprintf(stderr, "Caught unexpected signal %d\n", signum);
    }
}

/** Signal handler for loading screen
 */
void loadingScreenSignalHandler(int signal)
{
    int __attribute__ ((unused)) numberOfBytesRead;
    uint16_t buttonValue;
    numberOfBytesRead = read(fdGamepadDriver, &buttonValue, 2);

    // Yep, every button has an action. such game.
    if (buttonValue == SW1) {
        Difficulty = 1;
    } else if (buttonValue == SW2) {
        Difficulty = 2;
    } else if (buttonValue == SW3) {
        Difficulty = 3;
    } else if (buttonValue == SW4) {
        Difficulty = 4;
    } else if (buttonValue == SW5) {
        Difficulty = 5;
    } else if (buttonValue == SW6) {
        Difficulty = 6;
    } else if (buttonValue == SW7) {
        Difficulty = 7;
    } else if (buttonValue == SW8) {
        clearMemory();
        printf("closing program: SW8 hit\n");
        exit(0);
    }
}
