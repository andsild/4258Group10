#include "dac.h"
#include "gpio.h"
#include "sleep.h"
#include "soundPlayer.h"
#include "soundTables.h"

#include <stdint.h>
#define EXIT_FAILURE 1

#include <efm32gg.h>

int main(void)
{
    setupGPIO();
    setupDAC();

    // System Control Register
    *SCR = 0; // DAC not enabled in SCR 4 and 6

    /** Variable to prevent a single button push from triggering multiple
     * events
     */
    char aButtonIsEnabled = 0;

    *GPIO_PA_DOUT = 0xFF00;

    while (1)
    {
        /** Has a button been pushed? */
        if(*GPIO_PC_DIN != INPUT_EMPTY)  
        {
            /** Is this the first time we're registering this button push? */
            if(aButtonIsEnabled == 0)
            {
                resetSoundPlayer();
                setupDAC();

                buttonPressHandler();
                aButtonIsEnabled = 1;
            }
        }
        else
        {
            aButtonIsEnabled = 0;
        }

        /** Is a sound selected?
         */
        if(ByteBeatIndex > 0 || CurrentSoundFile > 0)
            playCurrentSound();
        else
            shutdownDAC();
    }

    return EXIT_FAILURE; // Should not be reached
}
