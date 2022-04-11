/*
 *  fname:
 *      encode.c
 *
 *  Purpose:
 *      To create an steganographic image by encoding secret message
 *      into a cover image.
 *
 *  Modifications:
 *      07 April 2022 - created
 *      08 April 2022 - added stegano.h header
 *                    - file renamed from pixel_array.c to encode.c
 *      09 April 2022 - added argument parsing
 *      10 April 2022 - modified argument parsing
 */

#include "stegano.h"

int main(int argc, char **argv)
{
    char *input = NULL;   // Filename of cover image
    char *message = NULL; // Filename of secret message
    char *output = NULL;  // Filename of stego image

    int option; // Next option argument in the argument list

    // Parses command line arguments
    while ((option = getopt(argc, argv, ":i:m:o:")) != -1)
    {
        switch (option)
        {
        case 'i':
            input = optarg; // Filename of cover image
            break;

        case 'm':
            message = optarg; // Filename of secret message
            break;

        case 'o':
            output = optarg; // Filename of stego image
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
    if (input == NULL)
    {
        fprintf(stderr, "%s", "no cover image provided in main()\n");
        exit(1);
    }

    if (message == NULL)
    {
        fprintf(stderr, "%s", "no secret message provided in main()\n");
        exit(1);
    }

    if (output == NULL)
    {
        output = "stegoImage.bmp"; // Default filename of stego image
    }

    BMP *imagePtr = loadImage(input); // Open cover image
    printProperties(*imagePtr);       // Display image properties

    encodeMessage(message, imagePtr); // Hide secret message
    createStego(output, *imagePtr);  // Create new image
    freeImage(imagePtr);              // Close cover image
}