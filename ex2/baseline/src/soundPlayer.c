#include "soundPlayer.h"

#include "sleep.h"
#include "soundTables.h"

#include <stdint.h>

volatile uint8_t ByteBeatIndex = 0;
SoundFile* CurrentSoundFile;


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
uint16_t getCurrentSoundFile()
{
    if(ByteBeatIndex == VIZNUT_BYTEBEAT)
    {
        soundToPlay = viznutByteBeat();
    }
    else if(ByteBeatIndex == RYGBY_BYTEBEAT) 
    {
        soundToPlay = rygByteBeat();
    }
    else
    {
        delay(18);
        soundToPlay = getFromSoundTable();
    }

    return soundToPlay; // 12 bits
}

/** Reset all counters and state information for playing sounds
 */
void resetSoundPlayer()
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
uint16_t viznutByteBeat()
{
    tPos++;
    delay(50);
    return (tPos*5&tPos>>7)|(tPos*3&tPos>>10) % 4095 ;
}

/** Return the next number in the "ryg bytebeat"
 *
 * @return the next sound to be played
 */
uint16_t rygByteBeat()
{
    //dananananan
    tPos++;
    delay(70);
    return ((tPos<<1)^((tPos<<1)+(tPos>>7)&tPos>>12))|tPos>>(4-(1^7&(tPos>>19)))|tPos>>7 % 4095;
}

/** Return the next frequency from the current soundfile
 *
 * @return the next sound to be played
 */
uint16_t getFromSoundTable()
{
    /** Have we reached the end of the sound? */
    if (sampleIndex > CurrentSoundFile->SoundFileLength - 1) {
        /** Finsihed the song, shut down DAC and timer */
        resetSoundPlayer();
        return 0;
    }

    trackA++;
     /** If we have reached the end of a tone, reset the counter and increment trackB */
    if (trackA ==
            CurrentSoundFile->SoundFileData[sampleIndex][0]->SoundLength) {
        trackA = 0;
        trackB++;
    }

    /** if we have reached the end of a tone */
    if ((trackB*(CurrentSoundFile->SoundFileData[sampleIndex][0]->SoundLength)/33) > 
         (uint32_t)(CurrentSoundFile->SoundFileData[sampleIndex][1])) {
        trackB = 0;
        sampleIndex++;
    }


    return CurrentSoundFile->SoundFileData[sampleIndex][0]->SoundData[trackA];
}
