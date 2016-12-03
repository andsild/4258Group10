#include <efm32gg.h>

/** Configure the NVIC for interrupts
 */
void
setupNVIC ()
{
  *ISER0 = (unsigned long) 0x4100000802;	// page 12, (RTC, CMU, odd, even)
  *ISER0 |= (1 << 26);		// LETIMER0 on IRQ line

}
