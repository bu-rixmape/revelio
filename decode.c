#include "stegano.h"

int main(void)
{
    BMP *origImage = loadImage("sources\\img001.bmp"); // Open original image
    BMP *newImage = loadImage("new.bmp"); // Open stego image

    puts("COVER IMAGE PROPERTIES");
    printProperties(*origImage);
    puts("\nSTEGO IMAGE PROPERTIES");
    printProperties(*newImage);

    decodeMessage(*origImage, *newImage);

    freeImage(origImage);
    freeImage(newImage);
}