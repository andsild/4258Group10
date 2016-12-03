#ifndef _GPIO_H
#define _GPIO_H

#define INPUT_EMPTY 0xFF
#define SW1 0xFE
#define SW2 0xFD
#define SW3 0xFB
#define SW4 0xF7
#define SW5 0xEF
#define SW6 0xDF
#define SW7 0xBF
#define SW8 0x7F

void setupGPIO();
void buttonPressHandler();

#endif
