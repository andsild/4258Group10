#ifndef __TIMER_H
#define __TIMER_H value

void timer_handler(int signum);
void startTimer(unsigned int, unsigned int);
void stopTimer();
void waitForTimer();
void time_handler();

#endif                          /* ifndef __TIMER_H */
