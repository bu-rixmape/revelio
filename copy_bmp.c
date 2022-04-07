#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE *streamIn; // Pointer to input file
    unsigned char header[54]; // Array for image header
    unsigned char colorTable[1024]; // Array for color table, if it exists.

    // Read input file
    if ((streamIn = fopen("sources\\img (3).bmp", "r")) == NULL)
    {
        printf("File opening error ocurred. Exiting program.\n");
        exit(0);
    }

    // Read header
    for (unsigned int i = 0; i < 54; i++)
    {
        fread(&header[i], sizeof(unsigned char), 1, streamIn);
    }

    // Read content of image header
    int width = *(int *)&header[22];
    int height = *(int *)&header[22];
    int bitDepth = (int) header[28];

    // Read color table
    if (bitDepth <= 8)
        fread(colorTable, sizeof(unsigned char), 1024, streamIn);

    // Print content image header
    printf("width: %d\n", width);
    printf("height: %d\n", height);
    printf("depth: %d\n", bitDepth);

    // Read pixel array
    unsigned char pixels[height * width];
    fread(pixels, sizeof(unsigned char), (height * width), streamIn);

    FILE *fo = fopen("out.bmp", "wb"); // Output File name
    fwrite(header, sizeof(unsigned char), 54, fo); // write the image header to output file

    if (bitDepth <= 8)
        fwrite(colorTable, sizeof(unsigned char), 1024, fo);

    fwrite(pixels, sizeof(unsigned char), (height * width), fo);

    // Close input and output file
    fclose(fo);
    fclose(streamIn);
}