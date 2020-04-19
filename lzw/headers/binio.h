#ifndef BINIO_H
#define BINIO_H

#include "dictionary.h"

struct _binio{
	char buffer;
	int posi;
};
typedef struct _binio binio;

binio io;


void init_binio();
void write_output(FILE *file, uint32_t index);
uint32_t read_input(FILE *file);

#endif /* BINIO */
