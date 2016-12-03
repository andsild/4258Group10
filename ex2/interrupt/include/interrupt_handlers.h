#ifndef _INTERRUPT_HANDLER
#define _INTERRUPT_HANDLER

#include <stdint.h>

extern uint8_t count;

void changeLEDs ();
void LETIMER0_IRQHandler ();
void GPIO_EVEN_IRQHandler ();
void GPIO_ODD_IRQHandler ();

#endif
