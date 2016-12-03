#ifndef __TIMER_H
#define __TIMER_H

/** low frequency a clock enable register 0  */
#define CMU_LFACLKEN0 ((volatile uint32_t*)(CMU_BASE2 + 0x058 ))
#define CMU_LFCLKSEL ((volatile uint32_t*)(CMU_BASE2 + 0x028 ))
 // low frequency clock select register
#define CMU_LFAPRESC0 ((volatile uint32_t*)(CMU_BASE2 + 0x068 ))

#define CMU_OSCENCMD      ((volatile uint32_t*)(CMU_BASE2 + 0x020))

#define DEFAULT_SAMPLE_PERIOD 317

void setupTimer ();
void startTimer ();
void stopTimer ();
void configureTimerForByteBeat ();

#endif
