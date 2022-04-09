#include "stegano.h"

int main(void)
{
    BMP *origImage = loadImage("orig.bmp"); // Open original image
    BMP *newImage = loadImage("new.bmp"); // Open stego image

    decodeMessage(origImage, newImage);

    freeImage(origImage);
    freeImage(newImage);
}