#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#define BOARD_HEIGHT 240
#define BOARD_WIDTH 320

#include "game.h"

extern int columnArrowPositions[COLCOUNT];

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define IMAGE_FOLDER "/usr/local/share/game/images/"
#define COLUMN_POSITION 76
#define ARROW_PADDING 42
#define ARROW_STARTING_YPOS 180
#define ARROW_CEILING 35
#define STATIONARY_ARROW_YPOS 43

#define LENGTH_OF_HEALTHBAR 129

#define BOO 0x1
#define GREAT 0x2
#define MISS 0x3
#define PERFECT 0x4

#define GREEN 0x07E0
#define YELLOW 0xFFE0
#define RED 0xF800

#define DRAWTILE_XWIDTH 2
#define DRAWTILE_YHEIGHT 240

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef long LONG;

/** To align the struct correctly for fseek when reading image metadata
            */// http://stackoverflow.com/a/14279511/2428827
#pragma pack(push, 1)

typedef struct BMPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BitmapFileHeader;
#pragma pack(pop)

#pragma pack(push, 1)

typedef struct BMPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BitmapInfoHeader;

#pragma pack(pop)

extern BitmapInfoHeader *bitmapInfoHeader;
extern BitmapFileHeader *bitmapFileHeader;

int *loadPartialBMPToFrameBuffer(unsigned char *bitmapData, int rotation,
                                 int type, int xsize, int ysize, int xoffset,
                                 int yoffset);
int drawPrimitive(int color, int xsize, int ysize, int xoffset, int yoffset);
int initFrameBuffer();
int initializeBackgroundScreen();
unsigned char *loadBMPToMemory(char *FileName);
void closeFrameBuffer();
void loadBMPToFrameBuffer(unsigned char *bitmapData, int rotation, int type,
                          int xoffset, int yoffset);
void renderScore();
void setAllLines(int yoffset);
void setFlashText(int type);
void renderHealthbar();
void updateFrameBuffer();
void updateFrameBufferPosition(int xoffset, int yoffset, int xsize, int ysize);
int getDigit(int number, int position);

inline uint32_t PixelColor(uint8_t Red, uint8_t Green)
{
    return (Green << 9) | Red;
}

inline void renderFramebufferCanvas()
{
    /** Arrow region */
    updateFrameBufferPosition(79, 30, (37 + 4) * 4, 175);
    /** Score region */
    updateFrameBufferPosition(5, 215, 95, 16);
    /** Health region */
    updateFrameBufferPosition(3, 9, 131, 14);
}

inline void cleanArrowCanvas()
{
    unsigned char *bitmapData;

    bitmapData = loadBMPToMemory(IMAGE_FOLDER "GameOverlay.bmp");
    loadPartialBMPToFrameBuffer(bitmapData, 0, 1, 79, 35, (37 + 4) * 4, 175);
    free(bitmapData);
    bitmapData = NULL;
}

inline unsigned char *getArrow()
{
    unsigned char *bitmapData;
    bitmapData = loadBMPToMemory(IMAGE_FOLDER "RedArrow.bmp");
    if (!bitmapData) {
        fprintf(stderr, "oh no!\n");
        clearMemory();
        exit(1);
    }

    return bitmapData;
}

#endif
