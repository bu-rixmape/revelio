/*
 *  fname:
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

int main(int argc, char **argv)
{
    char *cover = NULL;   // Filename of cover image
    char *stego = NULL;   // Filename of stego image
    char *message = NULL; // Filename of secret text

    int option; // Next option argument in the argument list

    // Parses command line arguments
    while ((option = getopt(argc, argv, ":c:s:m:")) != -1)
    {
        switch (option)
        {
        case 'c':
            cover = optarg; // Filename of cover image
            break;

        case 's':
            stego = optarg; // Filename of stego image
            break;

        case 'm':
            message = optarg; // Filename of secret message
            break;

        case ':': // Required option with missing option argument
            printf("missing '%c' command line argument\n", optopt);
            break;

        case '?': // Unknown option character
            printf("unknown '%c' option character\n", optopt);
            break;
        }
    }

    // Check if required option arguments are obtained
    if (cover == NULL)
    {
        fprintf(stderr, "%s", "no cover image provided in main()\n");
        exit(1);
    }

    if (stego == NULL)
    {
        fprintf(stderr, "%s", "no stego image provided in main()\n");
        exit(1);
    }

    if (message == NULL)
    {
        message = "secretMessage.txt"; // Default filename for secret message
    }

    BMP *coverImage = loadImage(cover); // Open original image
    BMP *stegoImage = loadImage(stego); // Open stego image

    decodeText(*coverImage, *stegoImage, message);
    freeImage(coverImage);
    freeImage(stegoImage);
}