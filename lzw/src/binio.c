#include "binio.h"

void init_binio ()
{
	io.buffer = '0'; /* init value doesn't matter */
	io.posi = 0;
}

void write_output(FILE *file, uint32_t index)
{
    uint32_t index_save = index;
	for (int i = 0; i < dico.nbit; i++) {	
		//to know whether it's a 1 or a 0
        if (index & 1) {//1
            io.buffer = io.buffer | (1 << (io.posi));
        }
        else {//0
            io.buffer = io.buffer & ~(1 << (io.posi));
        }

        io.posi++;
        index = index >> 1;

        /* write if byte is full or if eof */ 
		if (io.posi == 8 || (i == (dico.nbit - 1) && index_save == LZW_EOF)) {
			fwrite(&(io.buffer), 1, 1, file);
			io.posi = 0;
		}

        if (i == (dico.nbit -1) && dico.flag_reinit) {
            dico.nbit = 9;
            dico.flag_reinit = 0;
        }
	}	
}

uint32_t read_input(FILE *file)
{
    if (dico.flag_reinit) {
        dico.nbit = 9;
        dico.flag_reinit = 0;
    }
	uint32_t temp = 0;
	for (int i = 0; i < dico.nbit; i++){	
		if (io.posi == 0){
			if (fread(&(io.buffer), 1 , 1, file) != 1) { 
				fprintf(stderr, "fread size error in read input"); 
				exit(0); 
			}
			io.posi = 8;
		}
		
		temp |= ((io.buffer & 1) << i);
		
		io.buffer = io.buffer >> 1;
		io.posi--;
	}
	return temp;
}
