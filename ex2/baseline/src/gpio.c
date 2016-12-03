#include "gpio.h"

#include "dac.h"
#include "soundPlayer.h"
#include "soundTables.h"

#include <efm32gg.h>

#include <stdint.h>

/** Configure GPIO pins for IO
 */
void setupGPIO()
{
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;	/* enable GPIO clock */
	*GPIO_PA_CTRL = 2;	/* set high drive strength */
	*GPIO_PA_MODEH = 0x55555555;	/* set pins A8-15 as output */
	*GPIO_PA_DOUT = 0x0700;	/* turn on LEDs D4-D8 (LEDs are active low) */
    *GPIO_PC_MODEL = 0x33333333; // enable pins
    *GPIO_PC_DOUT = 0xFF; // set pull-up
}

/** Map each button to a corresponding action
 */
void buttonPressHandler()
{
    uint8_t input = *GPIO_PC_DIN;

    if (input == SW1) { 
        CurrentSoundFile = &ImperialMarch;
    }
    else if (input == SW2) {
        CurrentSoundFile = &Zelda;
    }
    else if (input == SW3) { 
        CurrentSoundFile = &Laser;
    }
    else if (input == SW4) {
        CurrentSoundFile = &Death;
    }
    else if (input == SW5) { 
        CurrentSoundFile = &Nukileare;
    }
    else if (input == SW6) { 
        ByteBeatIndex = VIZNUT_BYTEBEAT;
    }
    else if (input == SW7) { 
        ByteBeatIndex = RYGBY_BYTEBEAT;
    }
    else if (input == SW8) {
        CurrentSoundFile = &GunShot;
    }

    // The byte beat doesn't need to run as fast, so we modify the timer
    if(ByteBeatIndex > 0)
    {
        configureDacForByteBeat();
    }


}
