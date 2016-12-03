#include "dac.h"
#include "soundPlayer.h"

#include <efm32gg.h>

#include <stdint.h>

void configureDacForByteBeat()
{
    //*DAC0_CH0CTRL = 0x50010;
    *DAC0_CH0CTRL = 0x50010;
    *DAC0_CH0CTRL = 0x01;
    *DAC0_CH1CTRL = 0x01;
}


/** Disable the DAC
 */
void shutdownDAC()
{
    *DAC0_CH0CTRL = 0x0;
    *DAC0_CH0CTRL = 0;
    *DAC0_CH1CTRL = 0;
}

void playCurrentSound()
{
    uint16_t sound = getCurrentSoundFile();
    *DAC0_CH0DATA = sound;
    *DAC0_CH1DATA = sound;
}

void setupDAC()
{
    *CMU_HFPERCLKEN0 = *CMU_HFPERCLKEN0 | (0x01 << 17);
    *DAC0_CTRL = 0x50010;
    *DAC0_CH0CTRL = 0x01;
    *DAC0_CH1CTRL = 0x01;
}
