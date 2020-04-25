#include "compress.h"

/* compression algo : file already opened */
void compress(FILE *file)
{
    char a;
    seq w, wa;
    int i_w_found,i_wa_found; 
    int scanf_ret;

    w.prefix = NULL; 
    init_dico();
    dico.nbit=9;
    init_binio ();

    scanf_ret = fscanf(file, "%c", &(w.curr)); 
    while (scanf_ret != EOF) {
           
        scanf_ret = fscanf(file, "%c", &a);

        /* concat w.a */
        if ((i_w_found = dico_get_seq(w)) == -1) { 
			fprintf(stderr, "Error, w not found in dico\n"); 
			exit(0); 
		}
        wa.prefix = (dico.array[i_w_found]);
        wa.curr = a;

        if ((i_wa_found = dico_get_seq(wa)) != -1) { /* search for a longer substring */
#ifdef DEBUG
            fprintf(stderr, "\nFOUND : prefix = %i, value %i(%c), index %"PRIu32"\n", 
					dico_get_seq(*(wa.prefix)), a, a, i_wa_found);
#endif
            /* w <- w.a */
            w = *(dico.array[i_wa_found]); 
            if (scanf_ret == EOF) write_output(stdout, i_w_found); 
        }
        else { /* longer substring */
#ifdef DEBUG
            fprintf(stderr, "\nNOT FOUND : prefix = %i, value %i(%c), send %i \n", 
					dico_get_seq(*(wa.prefix)), a, a, dico_get_seq(w));
#endif

            write_output(stdout, i_w_found); 
            dico_add_seq(wa); 
            w.prefix = NULL; w.curr = a;
            if (dico.index == (1 << dico.nbit) - 2) {
                dico.nbit++; /* increment nbit if the maximum with this power of 2 is reached */
            }
        }
    }
    write_output(stdout, LZW_EOF); 
    free_dico();
}
