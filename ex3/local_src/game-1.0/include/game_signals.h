#ifndef __SIGNAL_H
#define __SIGNAL_H

#include <stdint.h>

int startSignalHandlerMainloop();
int startSignalHandlerLoadingScreen();
void gameLoopSignalHandler(int signum);
void loadingScreenSignalHandler(int signal);

extern volatile char timerHasNotExpired;

#endif
