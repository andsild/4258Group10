#include "timer.h"

#include <stdint.h>
#include <stdbool.h>

#include <efm32gg.h>

/** Configure the timer for use
 */
void
setupTimer ()
{
  *CMU_HFCORECLKEN0 |= 0x10;	// enable EBI (chapter 14)
  *CMU_LFCLKSEL |= 0x2;		// LowFrequencyCLocKSelect (choose crystal oscillator as source)
  *CMU_OSCENCMD |= 0x10;	// Toggle the crystal oscillator
  *CMU_LFACLKEN0 |= 0x4;	// toggle LETimer page 152
  *LETIMER0_CMD = 0x2;		// Clear LETimer
  *LETIMER0_CTRL |= (0x2 << 8);	// Restart the clock (RTC) after reaching COMP0

  // Parameters that affect timer frequency:
  *CMU_LFAPRESC0 |= (0x5 << 8);	// prescaler: divide by n (bits 11 to 8)
  *LETIMER0_COMP0 = 2;		// generate interrupt for every count

  //Clear interrupt
  //*LETIMER0_IFC = 0x1F; // Clear all interrupt flags that may be set
  *LETIMER0_IEN = 0x4;		// Enable REP1 interrupt
}

void
configureTimerForByteBeat ()
{
  *CMU_LFAPRESC0 |= (0x5 << 8);	// prescaler: divide by n (bits 11 to 8)
  *LETIMER0_COMP0 = 90;		// interval 1, generate interrupt for every count
}

/** Stop the timer
 */
void
stopTimer ()
{
  *LETIMER0_CMD = 0x2;
}

/** Start the timer
 */
void
startTimer ()
{
  *LETIMER0_CMD = 0x1;
}
