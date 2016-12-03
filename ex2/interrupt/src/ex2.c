#include "dac.h"
#include "gpio.h"
#include "nvic.h"
#include "timer.h"
#include "soundPlayer.h"
#include "soundTables.h"

#include <stdint.h>
#define EXIT_FAILURE 1

#include <efm32gg.h>

int
main (void)
{

  setupGPIO ();
  setupDAC ();

  setupTimer ();
  setupNVIC ();

  shutdownDAC ();
  stopTimer ();

  *GPIO_PA_DOUT = 0xFF00;


  // System Control Register
  *SCR = 6;

  while (1)
    {
      __asm ("wfi");
    }

  return EXIT_FAILURE;		// Should not be reached
}
