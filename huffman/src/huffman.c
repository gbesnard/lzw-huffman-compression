#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>
#include <limits.h>

/*
 * -> Read in.bin -> build tree -> encode text -> write compressed text 
 * -> read compressed text -> write uncompressed text
 * 
 * TODO :   Large file : malloc by chunk ?
 *          Iterative tree search ? Will introduce other malloc pb 
 *          Write huffman tree into the compressed file, so that it can be read by
 *          a second independant execution.
 *          Use same API as lzw software, and test on the same test files
 *          Try to compress lzw output dictionary with a huffman second pass
 */
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

struct node
{
    int value;
    int ascii_code;
    struct node *left;
    struct node *right;
};

/* global var to be incorporated into the compressed file */
struct node *huffmantree;
int shiftcount = 0;

void destroy_tree(struct node *tree)
{
    if (tree != 0) {
        if (tree->left != NULL) {
            destroy_tree(tree->left);
        }
        if (tree->right != NULL) {
            destroy_tree(tree->right);
        }
        free(tree);
    }
}

int max(int a, int b) 
{
    if (a > b) { 
        return a;
    }
    else {
        return b;
    }
}

int print_t_rec(struct node *tree, int is_left, int offset, int depth, char s[20][255])
{
    char b[20];
    int width = 5;

    if (!tree) return 0;

    sprintf(b, "(%03d)", tree->value);
    // sprintf(b, "(%03d)", tree->ascii_code);

    int left  = print_t_rec(tree->left,  1, offset,                depth + 1, s);
    int right = print_t_rec(tree->right, 0, offset + left + width, depth + 1, s);

    for (int i = 0; i < width; i++)
        s[2 * depth][offset + left + i] = b[i];

    if (depth && is_left) {

        for (int i = 0; i < width + right; i++)
            s[2 * depth - 1][offset + left + width/2 + i] = '-';

        s[2 * depth - 1][offset + left + width/2] = '+';
        s[2 * depth - 1][offset + left + width + right + width/2] = '+';

    } else if (depth && !is_left) {

        for (int i = 0; i < left + width; i++)
            s[2 * depth - 1][offset - width/2 + i] = '-';

        s[2 * depth - 1][offset + left + width/2] = '+';
        s[2 * depth - 1][offset - width/2 - 1] = '+';
    }

    return left + width + right;
}

void print_t(struct node *tree)
{
    char s[20][255];
    for (int i = 0; i < 20; i++)
        sprintf(s[i], "%80s", " ");

    print_t_rec(tree, 0, 0, 0, s);

    for (int i = 0; i < 20; i++)
        printf("%s\n", s[i]);
}

int height_tree_rec(struct node *tree, int h) 
{
    int lft_max = 0;
    int rgt_max = 0;

    if (tree->left == 0 && tree->right == 0) {
        return h;
    }
    
    if (tree->left != 0) {
        lft_max = height_tree_rec(tree->left, h + 1);
    }

    if (tree->right != 0) {
        rgt_max = height_tree_rec(tree->right, h + 1); 
    }

    return max(lft_max, rgt_max);
}

int height_tree(struct node *tree) 
{
    return height_tree_rec(tree, 0);
}

void print_level(struct node *tree, int level) 
{
    if (tree != 0) {
        if (level == 1) {
            printf("%i ", tree->value);
        }
        else {
            print_level(tree->left, level-1);
            print_level(tree->right, level-1);
        }
    }
}

void print_tree(struct node *tree)
{
    int h = height_tree(tree);
    for (int lvl = 1; lvl <= h+1; lvl++) {
        print_level(tree, lvl);
        printf("\n");
    }
    printf("\n");
}

int path_tree(int ascii_code, struct node *tree, int coding[], int i)
{
    if (tree != 0) {
        if (tree->ascii_code != -1 && tree->ascii_code == ascii_code) {
            return 1;
        }
        
        /* 0 : left, 1 : right */
        if (path_tree(ascii_code, tree->left, coding, i+1)) {
            coding[i] = 0;
            return 1;
        }

        if (path_tree(ascii_code, tree->right, coding, i+1)) {
            coding[i] = 1;
            return 1;
        }
    }
    else {
        return 0;
    }
}

void insert_tree(int value, int ascii_code, struct node **tree)
{
    if(*tree == 0) {
        *tree = (struct node*) malloc(sizeof(struct node));
        (*tree)->value = value;
        (*tree)->ascii_code = ascii_code;
        /* initialize the children to null */
        (*tree)->left = 0;    
        (*tree)->right = 0;  
    }
    else if(value < (*tree)->value) {
        insert_tree(value, ascii_code, &(*tree)->left);
    }
    else if(value > (*tree)->value) {
        insert_tree(value, ascii_code, &(*tree)->right);
    }
}

void find_mins_trees(int *min1, int *min2, struct node *trees[]) 
{
    /* add two min tree's head */
    *min1 = -1;
    *min2 = -1;
    int tmp;

    for (int i = 0; i < 256; i++) {
        if (trees[i] != 0) {
            /* update min1 */
            if (*min1 == -1 || trees[i]->value < trees[*min1]->value) {
                tmp = *min1;
                *min1 = i;
                /* test if we should copy previous min1 to min2 */
                if (*min2 == -1 || trees[*min2]->value > trees[tmp]->value) {
                    *min2 = tmp;
                }
            } 
            /* update min2 */
            else if (*min2 == -1 || trees[i]->value < trees[*min2]->value) {
                *min2 = i;
            }
        }
    }
}

int decompress(char * output_filename, char * compressed_filename) 
{
	FILE *fd;
    long fsize;

    /* read compressed data */
    if ((fd = fopen(compressed_filename, "rb")) == NULL) {
        fprintf(stderr, "fopen returned %s\n", strerror(errno));
        exit(0);
    }
    
    fseek(fd, 0, SEEK_END);
    fsize = ftell(fd);
    fseek(fd, 0, SEEK_SET); 

    uint32_t *buf = malloc(fsize*64);
    fread(buf, 4, fsize/4, fd); 
    fclose(fd);

    /* uint32_t : 4 bytes */
    int bpath;
    int sub_loop_bound;
    int charcounter = 0;
    unsigned char decoded_char[fsize*10]; // TODO: same, use realloc ? can't really know size in advance : no it's not fsize !
    struct node *tmptree = huffmantree;

#ifdef DEBUG
    printf("\n============ READ DATA =============\n\n");
#endif

    for (int i = 0; i < (fsize/4); i++) {

#ifdef DEBUG    
        printf("====================i:%i\tfisze/4:%i\n==============\n", i, fsize/4);
#endif

        if (i+1 == fsize/4) {
            sub_loop_bound = shiftcount-1; /* all bits in last byte might not mean something */
        }
        else {
            sub_loop_bound = 32 - 1;
        }

        /*
         * From 1010 we want to get a list -> 1:0:1:0 
         * For example, to get the last 1 (10'1'0 >> 1)&1 -> (0101)&1 -> 1 
         */
        for (int j = sub_loop_bound; j >= 0; j--) {
            //printf("==YOLO shiftcounts : %i\n", shiftcount);
            bpath = (buf[i] >> j) & 1;
            //printf("%i", bpath);
            if (bpath == 0) {
                tmptree = tmptree->left;
            }
            else if (bpath == 1) {
                tmptree = tmptree->right;
            }

            // printf("ascii tmptree : %i\n", tmptree->ascii_code);
            if (tmptree->ascii_code != -1) {
                // printf("\t%c\n", tmptree->ascii_code);
                decoded_char[charcounter] = tmptree->ascii_code;
                charcounter++;
                tmptree = huffmantree;
            }
        }
        // printf("\n\n");
    }

    /* write */
    if ((fd = fopen(output_filename,"wb+")) == NULL) {
        fprintf(stderr, "fopen returned %s\n", strerror(errno));
        exit(0);
    }

    fwrite(decoded_char, 1, charcounter, fd); 
    fclose(fd);

    /* clean up */
    free(buf);
    destroy_tree(huffmantree);
    return 0;   
}


int compress(char * input_filename, char * compressed_filename) 
{
    srand(time(NULL));
   
    /* read */
    FILE *fd;
    if ((fd = fopen(input_filename, "rb")) == NULL) {
        fprintf(stderr, "fopen returned %s\n", strerror(errno));
        exit(0);
    }
    
    fseek(fd, 0, SEEK_END);
    long fsize = ftell(fd);
    fseek(fd, 0, SEEK_SET); 

    int occ[256] = {0}; /* ASCII size */
    struct node *trees[256] = {0};

    unsigned char *buffer = malloc(fsize);
    int i = (int)fread(buffer, 1, fsize, fd); 
    fclose(fd);

    /* count char occurences */
#ifdef DEBUG
    if (fsize < 20) {
        printf("\n========== CHAR READ ===========\n\n");
    }
#endif

    int to_analyze = fsize;
    
    for(int i = 0; i < to_analyze; i++) {
        occ[buffer[i]]++; 

#ifdef DEBUG
        if (fsize < 20) {
            printf("%c\t%x\t", buffer[i], buffer[i]);
            printf(BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(buffer[i]));
        }
#endif
    }
    
    /* make a small forest of one elem trees */
    for(int i = 0; i < 256; i++) {
        if (occ[i] > 0) {
           insert_tree(occ[i], i, &trees[i]);
        }
    }
    
    /* build Huffman tree */ 
#ifdef DEBUG
    printf("\n=========== HUFFMAN ============\n\n");
#endif

    int min1, min2;
    int iter = 0;

    do {
        iter++;
#ifdef DEBUG
        if (fsize < 20) {
            printf("ITERATION %i\n\n", iter);
            for (int i = 0; i < 256; i++) {
                if (trees[i] != 0) {
                    print_tree(trees[i]);
                }
            }
        }
#endif

        int val = 0;
        find_mins_trees(&min1, &min2, trees);
       
        if (min1 != -1 && min2 != -1) {
            struct node *treefusion = (struct node*) malloc(sizeof(struct node));
            treefusion->left = 0;
            treefusion->right = 0;

            val = val + trees[min1]->value;
            treefusion->left = trees[min1];
            
            val = val + trees[min2]->value;
            treefusion->right = trees[min2];

            treefusion->value = val;
            treefusion->ascii_code = -1;

            trees[min1] = 0;
            trees[min2] = 0;
            trees[min1] = treefusion;
            huffmantree = treefusion;
        }
        
    } while(min1 != -1 && min2 != -1);

#ifdef DEBUG
    if (fsize < 20) {
        /* crash on big trees */
        print_t(huffmantree); 
    }
    else {
        print_tree(huffmantree);
    }
#endif

#ifdef DEBUG
    printf("\n=========== CHAR TABLE =============\n\n");
#endif

    int coding[10]; // 10 is the max size for a code, considering all 256 ascii char are present */
    uint32_t bin[409600] = {0}; // TODO : SIZE : alloc by chunk ?
    int bincount = 0;

    if (fsize < 20) { 
        for (int c = 0; c < 256; c++) {
            for (int k = 0; k < 256; k++) {
                coding[k] = -1;
            }

            if (path_tree(c, huffmantree, coding, 0)) {
#ifdef DEBUG
                printf("%c \t", c);
#endif
                for (int i = 0; i < 256; i ++) {
                    if (coding[i] == -1) {
                        break;
                    }

#ifdef DEBUG
                    printf("%i", coding[i]);
#endif
                }
#ifdef DEBUG
                printf("\n");
#endif
            }
        }
    }

#ifdef DEBUG
    printf("\n============= SEQ GEN ==============\n\n");
#endif

    for (int i = 0; i < fsize; i++) {
        for (int k = 0; k < 256; k++) {
            coding[k] = -1;
        }
        
        // printf("i : %i\tfsize : %i=============\n==========\n==========\n", i, fsize);
        if (path_tree(buffer[i], huffmantree, coding, 0)) {
            for (int i = 0; i < 256; i ++) {
                if (coding[i] == -1) {
                    break;
                }
                
                /*
                 * if we already have 1100 and we get a 1 we want to have 11001
                 * (1101 << 1) | 1 -> 11010 | 1 -> 11010 | 00001 -> 11011
                 */
                bin[bincount] = bin[bincount] << 1; /* first bitwise with 0 won't matter */
                bin[bincount] = bin[bincount] | (unsigned int)coding[i];
                shiftcount++;

                /* add bit to != byte one current is "full" */
                if (shiftcount == 32) {
                    shiftcount = 0;
                    bincount++;
                }
            }
        }
    }

#ifdef DEBUG
    if (fsize < 20) {
        for (int i = 0; i < bincount+1; i++) {
            printf("bin[i] \t%u\t0x%04x\t", bin[i], bin[i]);
            printf(BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(bin[i]));
        }
    }
#endif

    /* write */
    if ((fd = fopen(compressed_filename, "wb+")) == NULL) {
        fprintf(stderr, "fopen returned %s\n", strerror(errno));
        exit(0);
    }

    fwrite(bin, 4, bincount+1, fd); 
    fclose(fd);

    /* clean up */
    free(buffer);
    // destroy_tree(huffmantree); TODO: restore when both compress and decompress function separated
    return 0;   
}

int main(int argc, char *argv[])
{
    /* read from stdin if there's no file provided */
    if (argc != 4) {
        fprintf(stderr, "Usage : ./huffman input_filename compressed_filename output_filename\n");
        exit(0);
    }
    else if (argc == 4)
	{
        compress(argv[1], argv[2]);
		decompress(argv[3], argv[2]);
	}

    return 0;
}
