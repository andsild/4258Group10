#ifndef __SOUNDEFFECT_H
#define __SOUNDEFFECT_H

#include "soundTables.h"

#include <stdint.h>

#define VIZNUT_BYTEBEAT 1
#define RYGBY_BYTEBEAT 2

extern volatile uint8_t ByteBeatIndex;
extern SoundFile *CurrentSoundFile;


extern uint32_t sampleIndex;
/** trackA keeps track of where in the current tone (i.e. struct Sound) we are currently playing */
extern uint32_t trackA;
/** trackB keeps track of which tone we are playing in a melody (i.e. Struct SoundFile) */
extern uint32_t trackB;

void resetByteBeat ();
void resetSoundPlayer ();

uint16_t viznutByteBeat ();
uint16_t rygByteBeat ();
uint16_t getFromSoundTable ();
uint16_t getCurrentSoundFile ();

#endif
