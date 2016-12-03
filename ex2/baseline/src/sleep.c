#include "sleep.h"

#include <stdint.h>

/** Sleep a given amount of time, measured in loop iteration counts
 *
 * @param duration how many cycles of the while loop to iterate. Must be > 0.
 */
void delay(uint32_t duration)
{
    uint32_t i = 0;
    do {
        i += 1;
    } while (i < duration);
}
