#ifndef __SOUNDEFFECT_H
#define __SOUNDEFFECT_H

#include "soundTables.h"

#include <stdint.h>

#define VIZNUT_BYTEBEAT 1
#define RYGBY_BYTEBEAT 2

extern volatile uint8_t ByteBeatIndex;
extern SoundFile* CurrentSoundFile;

void resetByteBeat();
void resetSoundPlayer();

uint16_t viznutByteBeat();
uint16_t rygByteBeat();
uint16_t getFromSoundTable();
uint16_t getCurrentSoundFile();

#endif
