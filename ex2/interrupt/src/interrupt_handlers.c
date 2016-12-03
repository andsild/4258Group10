#include "interrupt_handlers.h"
#include "gpio.h"
#include "timer.h"
#include "soundPlayer.h"
#include "dac.h"

#include <stdint.h>
#include <efm32gg.h>


uint8_t input = 0;
volatile int gpioLock = 0;

/** Timer interrupt handle
*/
void __attribute__ ((interrupt)) LETIMER0_IRQHandler ()
{
  /* Ideally the timer wouldn't be running while no song is selected, but
   * button mashing may enable this case. Therefore we need the following if statement */
  if (ByteBeatIndex > 0 || CurrentSoundFile > 0)
    playCurrentSound ();
  else
    {
      shutdownDAC ();
      *SCR = 4;
    }

  // Clear interrupts
  *LETIMER0_IFC = 0x1F;
  *LETIMER0_CMD = 0x4;
}

int
onlyOneButtonIsHeld ()
{
  input = *GPIO_PC_DIN;
  if (input == SW1 || input == SW2 || input == SW3 || input == SW4
      || input == SW5 || input == SW6 || input == SW7 || input == SW8)
    {
      return 1;
    }
  return 0;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler ()
{
    if (gpioLock == 0)
    {
        gpioLock = 1;
        if (onlyOneButtonIsHeld ())
        {
            resetSoundPlayer ();
            stopTimer ();
            setupDAC ();
            setupTimer ();
            buttonPressHandler ();

            *SCR = 2;
            startTimer ();
        }
        gpioLock = 0;
    }

    // Clear interrupt for button
    *GPIO_IFC = *GPIO_IF;
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler ()
{
    if (gpioLock == 0)
    {
        gpioLock = 1;
        if (onlyOneButtonIsHeld ())
        {
            resetSoundPlayer ();
            stopTimer ();
            setupDAC ();
            setupTimer ();
            buttonPressHandler ();

            *SCR = 2;
            startTimer ();
        }
        gpioLock = 0;
    }

    // Clear interrupt for button
    *GPIO_IFC = *GPIO_IF;
}
