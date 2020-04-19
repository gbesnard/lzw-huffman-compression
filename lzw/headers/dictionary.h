#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "binio.h"

#define DICO_SIZE 4096
#define LZW_EOF 256
#define NB_INSTR 1

struct _seq {
	struct _seq *prefix; /* set to NULL if no prefix */
    char curr; /* current mono byte */
};
typedef struct _seq seq;

struct _dictionary {
    unsigned int nbit; /* number of bit for one sequence */
    uint32_t index; /* current index of the dictionary */
    seq* array[DICO_SIZE]; /* dico with fixed size : reinit if length is reached */
    int flag_reinit;
};
typedef struct _dictionary dictionary;
dictionary dico;

void init_dico();
void free_dico();
int dico_get_seq(seq s);
char first_byte(seq s);
void dico_add_seq(seq s);
void print_seq(seq s);

#endif /* DICTIONARY_H */
