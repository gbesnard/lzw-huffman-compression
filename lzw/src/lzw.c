#include "compress.h"
#include "decompress.h"

/*
 * Main file. 
 */
int main(int argc, char *argv[])
{
    FILE *file; 
       
    /* read from stdin if there's no file provided */
    if (argc == 1) {
        fprintf(stderr, "Usage : -c to compress\n");
        fprintf(stderr, "Usage : -d to decompress\n");
        fprintf(stderr, "Optional : filename (default stdin)\n");
        exit(0);
    }
    else if (argc == 2)
        file = stdin;
    else 
        file = fopen(argv[2], "r");

    if (file == NULL) { fprintf(stderr, "Error, could not open file\n"); exit(0); }

    if (strcmp("-c", argv[1]) == 0)
        compress(file);
    else if (strcmp("-d", argv[1]) == 0)
        decompress(file);
    else  { fprintf(stderr, "Unknown option\n"); exit(0); }
		

    if (argc == 3) fclose(file);
    return 0;
}
