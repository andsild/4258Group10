#include "soundPlayer.h"

#include "dac.h"
#include "soundTables.h"
#include "timer.h"

#include <efm32gg.h>

#include <stdint.h>

volatile uint8_t ByteBeatIndex = 0;
SoundFile *CurrentSoundFile;

uint32_t sampleIndex = 0;
/** trackA keeps track of where in the current tone (i.e. struct Sound) we are currently playing */
uint32_t trackA = 0;
/** trackB keeps track of which tone we are playing in a melody (i.e. Struct SoundFile) */
uint32_t trackB = 0;
/** Current position in a bytebeat
 */
uint16_t tPos = 0;
uint16_t soundToPlay;

/** Get the current sound
 *
 * @return the next sound to be played
 */
inline uint16_t
getCurrentSoundFile ()
{
  if (ByteBeatIndex == VIZNUT_BYTEBEAT)
    {
      soundToPlay = viznutByteBeat ();
    }
  else if (ByteBeatIndex == RYGBY_BYTEBEAT)
    {
      soundToPlay = rygByteBeat ();
    }
  else
    {
      soundToPlay = getFromSoundTable ();
    }

  return soundToPlay;
}

/** Reset all counters and state information for playing sounds
 */
void
resetSoundPlayer ()
{
  sampleIndex = 0;
  trackA = 0;
  trackB = 0;
  CurrentSoundFile = 0;
  tPos = 0;

  ByteBeatIndex = 0;
}

/** Return the next number in the "viznut bytebeat"
 *
 * @return the next sound to be played
 */
inline uint16_t
viznutByteBeat ()
{
  tPos++;
  if (tPos > 8095)
    tPos = 0;
  return (tPos * 5 & tPos >> 7) | (tPos * 3 & tPos >> 10) % 4095;
}

/** Return the next number in the "ryg bytebeat"
 *
 * @return the next sound to be played
 */
inline uint16_t
rygByteBeat ()
{
  //dananananan
  tPos++;
  return (((tPos << 1) ^ ((tPos << 1) + (tPos >> 7) & tPos >> 12)) | tPos >>
	  (4 - (1 ^ 7 & (tPos >> 19))) | tPos >> 7) & 0x1FF;
}

/** Return the next frequency from the current soundfile
 *
 * @return the next sound to be played
 */
inline uint16_t
getFromSoundTable ()
{
    /** Have we reached the end of the sound? */
  if (sampleIndex > CurrentSoundFile->SoundFileLength - 1)
    {
      resetSoundPlayer ();

      // We _could_ put this code in dac.c to avoid mixing logic with hardware coding
      // but I don't want complex return values
	/** Finsihed the song, shut down DAC and timer */
      shutdownDAC ();

      *SCR = 6;

      return 0;
    }

  trackA++;
     /** If we have reached the end of a tone, reset the counter and increment trackB */
  if (trackA == CurrentSoundFile->SoundFileData[sampleIndex][0]->SoundLength)
    {
      trackA = 0;
      trackB++;
    }

    /** if we have reached the end of a tone */
  if ((trackB *
       (CurrentSoundFile->SoundFileData[sampleIndex][0]->SoundLength) / 33) >
      (uint32_t) (CurrentSoundFile->SoundFileData[sampleIndex][1]))
    {
      trackB = 0;
      sampleIndex++;
    }

  return CurrentSoundFile->SoundFileData[sampleIndex][0]->SoundData[trackA];
}
