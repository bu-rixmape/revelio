/*
 *  Filename:
 *      decode.c
 *
 *  Purpose:
 *      To obtain secret text hidden in a stego image.
 *
 *  Modifications:
 *      07 April 2022 - created
 *      12 April 2022 - added argument parsing
 */

#include "stegano.h"

int main(void)
{
    // Displays program description
    FILE *prompt = fopen("prompt.txt", "r");
    char line[FNAME_MAX];
    for (unsigned int i = 0; i < 12; i++)
    {
        fgets(line, FNAME_MAX, prompt);
        printf("%s", line);
    }
    putchar('\n');

    // Obtains filename of cover image
    char cover[FNAME_MAX];
    printf("|=^_^=| Input a cover Image (.bmp file format):");
    scanf("%s", cover);

    // Obtains filename of stego name
    char stego[FNAME_MAX];
    printf("|=*_*=| Input a stego Image (.bmp file format):");
    scanf("%s", stego);

    // Obtains filename of decoded text
    char decoded[FNAME_MAX];
    printf("|=0_0=| Input a decoded text (.txt file format):");
    scanf("%s", decoded);

    BMP *coverImage = loadImage(cover); // Opens cover image
    BMP *stegoImage = loadImage(stego); // Opens stego image

    decodeText(*coverImage, *stegoImage, decoded);
    freeImage(coverImage);
    freeImage(stegoImage);
}