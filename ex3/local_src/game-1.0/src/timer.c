#include "timer.h"

#include <sys/time.h>
#include <string.h>
#include <signal.h>

struct itimerval timer;

/** Start an interrupt timer that triggers SIGALRM and repeats itself
 *
 * @param sec number of seconds to count down
 * @param usec number of microseconds to count down
 */
void startTimer(unsigned int sec, unsigned int usec)
{
    timer.it_value.tv_sec = sec;    // first-off timer
    timer.it_interval.tv_sec = sec; // repeat timer
    timer.it_value.tv_usec = usec;
    timer.it_interval.tv_usec = usec;

    setitimer(ITIMER_REAL, &timer, NULL);
}

/** Stop the global interrupt timer that triggers SIGALRM and repeats itself
 */
void stopTimer()
{
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);
}
