#include "graphics.h"
#include "game.h"

#include <fcntl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/** FileDescriptor for framebuffer */
static int fdFramebuffer = -1;
/** Contains information about the screen (pixels, size, etc) */
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;
/** Contains the part of the screen to update */
struct fb_copyarea rect;
uint8_t *mmapFrameBuffer;

BitmapInfoHeader *bitmapInfoHeader;
BitmapFileHeader *bitmapFileHeader;

/** Render the background screen presented to user at start of the game
 * 
 * @return 0 upon success, -1 if something when wrong.
 */
int initializeBackgroundScreen()
{
    unsigned char *bitmapData;

    bitmapInfoHeader = (BitmapInfoHeader *) malloc(sizeof(BitmapInfoHeader));
    bitmapFileHeader = (BitmapFileHeader *) malloc(sizeof(BitmapInfoHeader));

    renderHealthbar(PLAYERONE_STARTING_HEALTH);

    // drawPrimitive(1, 0x07E0, 20, 80, 5, 5);
    //

    bitmapData = loadBMPToMemory(IMAGE_FOLDER "GameOverlay.bmp");
    if (!bitmapData) {
        fprintf(stderr, "Could not load screen background\n");
        return -1;
    }
    loadBMPToFrameBuffer(bitmapData, 0, 1, 0, 0);
    updateFrameBuffer();

    free(bitmapData);
    bitmapData = NULL;

    return 0;
}

/** Load a BMP to heap memory
 *
 * @param FileName the full path of the image to load
 * @return pointer to image memory location
 */
unsigned char *loadBMPToMemory(char *FileName)
{
    FILE *FilePtr;
    unsigned char *bitmapImage;
    int __attribute__ ((unused)) BitMapSize;

    /** Open the BMP file */
    FilePtr = fopen(FileName, "rb");

    if (FilePtr == NULL) {
        printf("Failed to open BMP image %s.\n", FileName);
        return (unsigned char *)0;
    }

    /** Read the BMP file header */
    if (fread(bitmapFileHeader, sizeof(BitmapFileHeader), 1, FilePtr) <= 0) {
        fprintf(stderr, "Error when using fread: empty image %s", FileName);
        fclose(FilePtr);
        return (unsigned char *)0;
    };

    /** Verify that file is indeed a BMP */
    if (bitmapFileHeader->bfType != 0x4D42) {
        fprintf(stderr, "File not recognized as a BMP.\n");
        return (unsigned char *)0;
    }

    /** read the BMP info header */
    if (fread(bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, FilePtr) <= 0) {
        perror("error when using fread for info-header");
        return (unsigned char *)0;
    };

    BitMapSize = (bitmapInfoHeader->biWidth) * abs(bitmapInfoHeader->biHeight);

    bitmapImage = (unsigned char *)malloc(bitmapInfoHeader->biSizeImage);

    if (!bitmapImage) {
        perror("Memory allocation failed!");
        fclose(FilePtr);
        return (unsigned char *)0;
    }

    /** Move file point to the beginning of the BMP data */
    if (fseek(FilePtr, bitmapFileHeader->bfOffBits, SEEK_SET) < 0) {
        perror("Error when using fseek");
        fclose(FilePtr);
        return (unsigned char *)0;
    };

    /** Load the BMP image data */
    if (fread(bitmapImage, bitmapInfoHeader->biSizeImage, 1, FilePtr) <= 0) {
        fprintf(stderr, "Error when using fread: empty image %s", FileName);
        fclose(FilePtr);
        return (unsigned char *)0;
    }

    if (bitmapImage == NULL) {
        fprintf(stderr, "Failed to read BMP data.\n");
        fclose(FilePtr);
        return (unsigned char *)0;
    }

    if (fclose(FilePtr) != 0) {
        perror("fclose");
        return (unsigned char *)0;
    }

    return bitmapImage;
}

/** Initialize the framebuffer so that it can be read/written to by other methods
 *
 * @return nonzero on error
 */
int initFrameBuffer()
{
    int temp;
    int frameBufferSize;

    /** Disable cursor in LCD screen */
    system("echo -e -n '\033[?25l' > /dev/tty0");

    /** Open the framebuffer "file" in read/write mode */
    fdFramebuffer = open("/dev/fb0", O_RDWR);
    if (fdFramebuffer < 0) {
        perror("Failed to open framebuffer");
        close(fdFramebuffer);
        return 1;
    }

    /** Retrieve the fixed screen information */
    temp = ioctl(fdFramebuffer, FBIOGET_FSCREENINFO, &finfo);
    if (temp < 0) {
        printf("Failed to retrieve fixed framebuffer information.\n");
        close(fdFramebuffer);
        return 1;
    }

    /** Retrieve the variable screen information */
    temp = ioctl(fdFramebuffer, FBIOGET_VSCREENINFO, &vinfo);

    if (temp < 0) {
        perror("Failed to retrieve variable framebuffer information");
        close(fdFramebuffer);
        return 1;
    }

    frameBufferSize = finfo.smem_len;

    mmapFrameBuffer = mmap(NULL, frameBufferSize, PROT_READ | PROT_WRITE,
                           MAP_SHARED, fdFramebuffer, (off_t) 0);

    if (mmapFrameBuffer == NULL) {
        perror("mmap failed.");
        close(fdFramebuffer);
        return 1;
    }

    return 0;
}

/** Close the framebuffer file descriptor
 */
void closeFrameBuffer()
{
    if (munmap(mmapFrameBuffer, finfo.smem_len) == -1)
        perror("Error when freeing framebuffer memory");
    if (close(fdFramebuffer) == -1)
        perror("Error when closing framebuffer file descriptor");
}

/** Update the entire framebuffer, painting whatever objects are put in memory
 */
void updateFrameBuffer()
{
    int yr, xr;

    rect.width = DRAWTILE_XWIDTH;
    rect.height = DRAWTILE_YHEIGHT;

    /** Iterate over the entire screen, in tiles */
    for (yr = 0; yr < (vinfo.yres / DRAWTILE_YHEIGHT); yr++) {
        rect.dy = (yr * DRAWTILE_YHEIGHT);
        for (xr = 0; xr < (vinfo.xres / DRAWTILE_XWIDTH); xr++) {
            rect.dx = xr * DRAWTILE_XWIDTH;
            if (ioctl(fdFramebuffer, 0x4680, &rect) < 0) {
                perror("Error when updating framebuffer");
                clearMemory();
                exit(1);
            }

        }
    }
}

/** Update a region of the framebuffer, painting whatever objects are put in memory
 *
 * @param xoffset x position on screen to start paiting from
 * @param yoffset y position on screen to start paiting from
 * @param xsize x width of paint-rectangle
 * @param ysize height of paint-rectangle
 */
void updateFrameBufferPosition(int xoffset, int yoffset, int xsize, int ysize)
{
    rect.width = xsize;
    rect.height = ysize;
    rect.dy = yoffset;
    rect.dx = xoffset;
    if (ioctl(fdFramebuffer, 0x4680, &rect) < 0) {
        perror("error when updating framebuffer position");
        clearMemory();
        exit(1);
    }
}

/** Load parts of an image to a framebuffer
 *
 * @param bitmapData pointer to a malloced memory region containing an image
 * @param rotation the desired rotation of the image, specified as either 0,1,2,3 for 0,90,180,270 degrees repsectively
 * @param type whether or not draw background colors white and black
 * @param xsize width of the region to draw
 * @param ysize height of the region to draw
 * @param xoffset xpos to start fetching region from in image
 * @param yoffset ypos to start fetching region from in image
 */
int *loadPartialBMPToFrameBuffer(unsigned char *bitmapData, int rotation,
                                 int type, int xsize, int ysize, int xoffset,
                                 int yoffset)
{
    int x = 0;
    int y = 0;
    int padding = 0;
    int BMPlength = (bitmapInfoHeader->biWidth);
    int BMPheight = (bitmapInfoHeader->biHeight);

    /** Determine padding: each pixel is 3 bytes, each line must be divisible by 4 */
    padding = (BMPlength * 6) % 4;

    /** For all pixels.. */
    for (x = ysize; x < ysize + yoffset; x++) {
        for (y = xsize; y < xsize + xoffset; y++) {
            long location =
                (y) * (vinfo.bits_per_pixel / 8) + (x) * finfo.line_length;
            int position = (BMPheight - x) * (BMPlength) * 2 + y * 2 - x * padding; //0 degrees

            if (type == 1) {
                /** Filter white(ish) and black */
                if ((PixelColor
                     (bitmapData[0 + position],
                      bitmapData[1 + position]) < 0xFEFE)
                    &&
                    (PixelColor
                     (bitmapData[0 + position],
                      bitmapData[1 + position]) > 0x0000)) {
                    *((uint32_t *) (mmapFrameBuffer + location)) =
                        PixelColor(bitmapData[0 + position],
                                   bitmapData[1 + position]);
                }
            }
            /** Just paint the image */
            else {
                *((uint32_t *) (mmapFrameBuffer + location)) =
                    PixelColor(bitmapData[0 + position],
                               bitmapData[1 + position]);
            }
        }
    }

    return 0;
}

/** Set flash text in memory */
void setFlashText(int type)
{
    unsigned char *bitmapdata;

    switch (type) {

    case MISS:
        bitmapdata = loadBMPToMemory(IMAGE_FOLDER "Miss.bmp");
        break;

    case BOO:
        bitmapdata = loadBMPToMemory(IMAGE_FOLDER "Boo.bmp");
        break;

    case GREAT:
        bitmapdata = loadBMPToMemory(IMAGE_FOLDER "Great.bmp");
        break;

    case PERFECT:
        bitmapdata = loadBMPToMemory(IMAGE_FOLDER "Perfect.bmp");
        break;

    default:
        bitmapdata = loadBMPToMemory(IMAGE_FOLDER "Miss.bmp");
        break;
    }
    if (!bitmapdata) {
        fprintf(stderr, "text\n");
        clearMemory();
        exit(1);
    }
    loadBMPToFrameBuffer(bitmapdata, 0, 1,
                         (vinfo.xres) / 2 - (bitmapInfoHeader->biWidth) / 2,
                         (vinfo.yres) / 2 - 35);

    free(bitmapdata);
    bitmapdata = NULL;
}

/** Load an image to a framebuffer
 *
 * @param bitmapData pointer to a malloced memory region containing an image
 * @param rotation the desired rotation of the image, specified as either 0,1,2,3 for 0,90,180,270 degrees repsectively
 * @param type whether or not draw background colors white and black
 * @param xoffset xpos to start fetching region from in image
 * @param yoffset ypos to start fetching region from in image
 */
void loadBMPToFrameBuffer(unsigned char *bitmapData, int rotation, int type,
                          int xoffset, int yoffset)
{
    int x = 0;
    int y = 0;
    int padding = 0;
    int BMPlength = (bitmapInfoHeader->biWidth);
    int BMPheight = (bitmapInfoHeader->biHeight);

    /** Determine padding: each pixel is 3 bytes, each line must be divisible by 4 */
    padding = (BMPlength * 6) % 4;

    for (x = 0; x < BMPheight; x++) {
        for (y = 0; y < BMPlength; y++) {
            long location =
                (y + xoffset) * (vinfo.bits_per_pixel / 8) + (x +
                                                              yoffset) *
                finfo.line_length;
            int position = (BMPheight - x) * (BMPlength) * 2 + y * 2 - x * padding; //0 degrees

            switch (rotation) {
            case 0:
                position = (BMPheight - x) * (BMPlength) * 2 + y * 2 - x * padding; //0 degrees
                break;
            case 1:
                position = BMPlength * y * 2 + x * 2 + y * padding; //90 degrees
                break;
            case 2:
                position = BMPlength * x * 2 + y * 2 + x * padding; //180 degrees   
                break;
            case 3:
                position = (BMPheight - y) * (BMPlength) * 2 + x * 2 - y * padding; //270 degrees

                break;
            default:
                position = (BMPheight - x) * (BMPlength) * 2 + y * 2 - x * padding; //0 degrees
                break;
            }

            if (type == 1) {
                if ((PixelColor
                     (bitmapData[0 + position],
                      bitmapData[1 + position]) < 0xFEFE)
                    &&
                    (PixelColor
                     (bitmapData[0 + position],
                      bitmapData[1 + position]) > 0x0000)) {
                    *((uint32_t *) (mmapFrameBuffer + location)) =
                        PixelColor(bitmapData[0 + position],
                                   bitmapData[1 + position]);
                }
            } else {
                *((uint32_t *) (mmapFrameBuffer + location)) =
                    PixelColor(bitmapData[0 + position],
                               bitmapData[1 + position]);
            }
        }
    }
}

/** Draw a simple shape (single color)
 *
 * @param color fillcolor of shape
 * @param xsize width of shape to draw
 * @param ysize height of shape to draw
 * @param xoffset xpos to start drawing shape
 * @param yoffset ypos to start drawing shape
 *
 * @return 0 on success
 */
int drawPrimitive(int color, int xsize, int ysize, int xoffset, int yoffset)
{
    int x = 0;
    int y = 0;

    for (x = 0; x < ysize; x++) {
        for (y = 0; y < xsize; y++) {
            long location =
                (y + xoffset) * (vinfo.bits_per_pixel / 8) + (x +
                                                              yoffset) *
                finfo.line_length;
            *((uint32_t *) (mmapFrameBuffer + location)) = color;
        }
    }

    return 0;

}

/** Get the nth digit of a number
 * e.g. getDigit(123, 1) => 2,  getDigit(123, 2) => 3, 
 * 
 * @param number the number to use
 * @param position 0-based index of number to fetch digit from
 * @return nth digit of a number
 */
int getDigit(int number, int position)
{

    int digit = 0;

    do {
        digit = number % 10;
        number /= 10;
        position -= 1;
    } while ((number > 0) && (position >= 0));

    if (position >= 0) {
        return 0;
    }

    return digit;

}

/** Load the current score to memory as an image
 */
void renderScore()
{
    int number;
    unsigned char *bitmapdata;
    char filestring[35] = IMAGE_FOLDER "1.bmp";

    /** For each digit in our score (max 8) */
    for (number = 0; number < 8; number++) {
        filestring[29] = (char)('0' + getDigit(PlayerOneScore, number));

        bitmapdata = loadBMPToMemory(filestring);
        if (!bitmapdata) {
            fprintf(stderr, "(An error happened in renderScore)\n");
            clearMemory();
            exit(1);
        }

        loadBMPToFrameBuffer(bitmapdata, 0, 1,
                             5 + (8 - number) * 10 - number * 2, 215);

        free(bitmapdata);
        bitmapdata = NULL;
    }
}

/** Render the healthbar 
 */
void renderHealthbar()
{
    unsigned char *bitmapdata;
    int color = GREEN;

    if (PlayerOneHealth < 90) {
        color = YELLOW;
    }

    if (PlayerOneHealth < 40) {
        color = RED;
    }

    drawPrimitive(color, PlayerOneHealth, 10, 4, 11);
    drawPrimitive(0x0000, LENGTH_OF_HEALTHBAR - PlayerOneHealth, 10,
                  PlayerOneHealth + 4, 11);

    /** Render the wobbly things that makes the health bar look like a non-rectangular shape */
    bitmapdata = loadBMPToMemory(IMAGE_FOLDER "GameOverlay.bmp");
    if (!bitmapdata) {
        fprintf(stderr, "error when setting health\n");
        clearMemory();
        exit(1);
    }
    loadPartialBMPToFrameBuffer(bitmapdata, 0, 1, 3, 9, 131, 16);

    free(bitmapdata);
    bitmapdata = NULL;
}
