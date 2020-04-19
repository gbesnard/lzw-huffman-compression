#include "dictionary.h"

/* init dico with ascii table and our own instructions */
void init_dico()
{
    /* full array allocation */
    for (int i = 0; i < DICO_SIZE; i++) {
        dico.array[i] = malloc (sizeof (seq));
        dico.array[i]->prefix = NULL;
        dico.array[i]->curr = 0; // useless
    }
    
    /* init ascii table */
    for (int i = 0; i < 256; i++) {
        dico.array[i]->prefix = NULL;
        dico.array[i]->curr = i;
    }

    dico.flag_reinit = 0;
    // dico.nbit = 9; /* all seq are coded on 9 bit until index reach 512 */
    dico.index = 255 + NB_INSTR; /* add also our instruction : EOF, REINIT */
}

void free_dico()
{
    for (int i = 0; i < DICO_SIZE; i++) {
        free(dico.array[i]);
    }
}

char first_byte(seq s)
{
    while (s.prefix != NULL) s = *(s.prefix);
    return s.curr;
}

/* 
 * return index to found seq, or -1 if not found 
 * TODO : improve algorithm readability
 */
int dico_get_seq(seq s)
{
    unsigned int found = 0;
    unsigned int i = 0;
    seq *tmp_pre_dico, *tmp_pre_curr;
    int to_return = -1;
    int try_next;;

    while (i < dico.index && !found) {
        if (dico.array[i]->curr == s.curr) { /* current char is the same, try to check prefix */
            tmp_pre_dico = dico.array[i]->prefix;
            tmp_pre_curr = s.prefix;

            try_next = 0;
            while (tmp_pre_curr != NULL) {
                if (tmp_pre_dico == NULL) {try_next = 1; break;} /* prefix too short */
                if (tmp_pre_dico->curr != tmp_pre_curr->curr) {try_next = 1; break;} /* not same prefix */
                tmp_pre_dico = tmp_pre_dico->prefix;
                tmp_pre_curr = tmp_pre_curr->prefix;    
            }
            if (tmp_pre_dico != NULL) { try_next = 1; } // seq too long

            if (!try_next) {
                to_return = i;
                found = 1;
            }
        }
        i++;
    }

    return to_return;
}

void dico_add_seq(seq s)
{
    if (dico_get_seq(s) == -1) { /* add the sequence if it doesn't exist yet */
        // TODO : REINIT dico 
        if (dico.index == DICO_SIZE - 1) { fprintf(stderr, "dico.index = %"PRIu32" TODO : handle dico reinit\n", dico.index); exit(0); }

        dico.index++;
        *(dico.array[dico.index]) = s;
#ifdef DEBUG
        printf("\nADDED : prefix = %i, value %i(%c), index %"PRIu32"\n", dico_get_seq(*(s.prefix)), s.curr, s.curr, dico.index);
#endif
        if (dico.index == DICO_SIZE - 1) {
            free_dico();
            init_dico();
            dico.flag_reinit = 1;
        }
    }
}
