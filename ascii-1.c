#include <stdio.h>
#define MAX 100

int main()
{

	// Displays program description
	FILE *prompt = fopen("prompt.txt", "r");
	char line[MAX];
	for (unsigned int i = 0; i < 12; i++)
	{
		fgets(line, MAX, prompt);
		printf("%s", line);
	}
	putchar('\n');

	// Opens cover image
	char coverName[MAX];
	printf("|=^_^=| Input a cover Image (.bmp file format):");
	scanf("%s",coverName);
	FILE *cover = fopen (coverName, "r");
	if (cover == NULL)
	{
		printf(" File does not exist or cannot be opened.\n");
	}

	// Opens secret text
	char secretName[MAX];
    printf("|=0_0=| Input a Secret Text (.txt file format):");
	scanf("%s",secretName);
	FILE *secret = fopen (secretName, "r");
	if (secret == NULL)
	{
		printf(" File does not exist or cannot be opened.\n");
	}

	// Opens stego name
	char stegoName[MAX];
    printf("|=*_*=| Input a Stego Image (.bmp file format):");
	scanf("%s",stegoName);
	FILE *stego = fopen (stegoName, "w");
	if (stego == NULL)
	{
		printf(" File does not exist or cannot be opened.\n");
	}

	return 0;
}
