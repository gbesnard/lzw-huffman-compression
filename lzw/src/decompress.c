#include "decompress.h"

void write_seq(seq s)
{
    seq *tmp = &s;
    if (tmp->prefix != NULL)
        write_seq(*(s.prefix));
    fwrite(&(s.curr), 1, 1, stdout);
}

/* unused : for debug purpose */
void print_seq(seq s)
{
    seq *tmp = &s;
    do {
        printf("%c", tmp->curr);
        tmp = tmp->prefix;
    }
    while (tmp != NULL);
    printf("\n");
}

/* decompression algo : file already opened */
void decompress(FILE *file)
{
    char a;
    uint32_t i, i2;
    seq w, w2, wa;

    init_dico();
    dico.nbit=9;
    init_binio ();

    i = read_input(file);
    i2 = i,
    a = dico.array[i]->curr;

    w.prefix = NULL; w.curr = a;

    write_seq(w);
    while(i2 != LZW_EOF) { 
        if (dico.index == (1 << dico.nbit)-3) {
                dico.nbit++; /* increment nbit if the maximum with this power of 2 is reached */
        }
        
        i2 = read_input(file);
        
        // printf("\n==\nchar : %"PRIu32"\n==\n", i2);
        if (i2 != LZW_EOF) {
            if (i2 > dico.index) {
                w2 = *(dico.array[i]);
                w2.prefix = dico.array[i];
                w2.curr = a;
            }
            else {
                w2 = *(dico.array[i2]);
            }
            write_seq(w2);
         
            a = first_byte(w2);
            wa.prefix = dico.array[dico_get_seq(w)];
            wa.curr = a;
            dico_add_seq(wa); 
            i = i2;
            w = *(dico.array[i]);
        }
    }
    free_dico();
}
