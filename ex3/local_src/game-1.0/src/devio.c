#include "devio.h"

#include "timer.h"
#include "game.h"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int fdGamepadDriver;

#define DRIVER_LOCATION "/dev/driver-gamepad"

/** Load and initialize the file-descriptor for the gamepad-driver
 *
 * @return nonzero on error
 */
int loadGamepadDriver()
{
    fdGamepadDriver = open(DRIVER_LOCATION, O_RDONLY);
    if (!fdGamepadDriver) {
        perror("No such driver: " DRIVER_LOCATION);
        return -1;
    }

    if (fcntl(fdGamepadDriver, F_SETOWN, getpid()) == -1) {
        perror("Could not set current process as owner of " DRIVER_LOCATION);
        return -1;
    }

    if (fcntl
        (fdGamepadDriver, F_SETFL,
         fcntl(fdGamepadDriver, F_GETFL) | FASYNC) == -1) {
        perror("Could not setup asynchronous reading on " DRIVER_LOCATION);
        return -1;
    }

    return 0;
}
