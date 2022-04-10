#include "stegano.h"

int main(void)
{
    BMP *origImage = loadImage("130x200.bmp"); // Open original image
    BMP *newImage = loadImage("new.bmp"); // Open stego image

    // puts("COVER IMAGE PROPERTIES");
    // printProperties(*origImage);
    // puts("\nSTEGO IMAGE PROPERTIES");
    // printProperties(*newImage);

    decodeMessage(*origImage, *newImage);

    freeImage(origImage);
    freeImage(newImage);
}