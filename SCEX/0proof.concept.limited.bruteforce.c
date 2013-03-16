/* 
 * File:   main.c
 * Author: peon
 *
 * Created on 15 mars 2013, 19:22
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> //gcc -lm check linker
#include <time.h>

#define POLY1 0xEF75L
#define POLY2 0x58C2FL
#define POLY3 0x739583L

#define MASK1 0x1FFFFL
#define MASK2 0x7FFFFL
#define MASK3 0x7FFFFFL

//typedef unsigned __int32 u32;
typedef unsigned long int u32;
typedef unsigned short u16;
typedef unsigned char u8;

unsigned int motpar(u32 w) {
    //w^=(w>>32);
    w ^= (w >> 16);
    w ^= (w >> 8);
    w ^= (w >> 4);
    w ^= (w >> 2);
    w ^= (w >> 1);
    return (unsigned char) (w & 1);
}

char name[]="Mr DUPUY";

u8 stringLength = 0;
int displayCaseFound = 1;
FILE *fout;
char BITS_REG1 = 17;
int NB_OPS = 0;
unsigned long long caseSolved = 0;
unsigned long long globalCaseCounter = 0;

typedef struct deep_tree deep_tree;

struct deep_tree {
    int count;
    int output_req;
    struct deep_tree *next;
};

inline signed long long nodeValue(deep_tree *origin) {
    int i = 0;
    signed long long value = 0;
    deep_tree* current = origin;
    while (current != NULL) {
        value = value + current->count * (signed long long) pow(4.0f, i);
        current = current->next;
        i++;
    }
    return value;
}

inline deep_tree* createTree() {
    int i;
    deep_tree *next = NULL;
    deep_tree *current = NULL;
    deep_tree *origin = (deep_tree*) calloc(1, sizeof (deep_tree));
    current = origin;
    for (i = 0; i < BITS_REG1 - 1; i++) {
        next = (deep_tree*) calloc(1, sizeof (deep_tree));
        current->next = next;
        current = next;
    }
    return origin;
}

inline int incrementTree(deep_tree *origin, long long nb_off) {
    int boolean;
    if (nb_off) {
        if (origin->count == 3) {
            origin->count = 0;
            if (origin->next != NULL) {
                boolean = incrementTree(origin->next, 1);
                boolean = boolean && incrementTree(origin, nb_off - 1);
                return boolean;
            } else {
                return 0;
            }
        } else {
            origin->count++;
            return incrementTree(origin, nb_off - 1);
        }
    }
    return 1;
}

inline void updateRegs(deep_tree *tree, u32 *regs) {
    regs[0] = 0;
    regs[1] = 0;
    regs[2] = 0;
    deep_tree *current = tree;
    char tab_zero[4][3] = {
        {0, 0, 0},
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 0}
    }; //all case if outblock bit=0
    char tab_one[4][3] = {
        {1, 1, 1},
        {1, 0, 1},
        {0, 1, 1},
        {1, 1, 0}
    }; //all case if outblock bit=1
    char node = 0;
    char keep = 1;
    NB_OPS++;
    if (NB_OPS == 5000000) {
        long long currentState = nodeValue(tree);
        float state = currentState / 17179869184.0f;
        time_t rawtime;
        struct tm * timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        printf("************************************************************\n");
        printf("current node value %lld \n", currentState + 1);
        printf("number of every single combinations evalued %lld \n", globalCaseCounter);
        printf("program has analysed %.5f %% of total cases \n", state * 100);
        printf("program has found %lld cases matching requested output \n", caseSolved);
        printf("Current local time and date: %s", asctime(timeinfo));
        printf("************************************************************\n\n");

        /*
        fprintf(fout,"************************************************************\n");
        fprintf(fout,"current node value %lld \n", currentState+1);
        fprintf(fout,"number of every single combinations evalued %lld \n", globalCaseCounter);
        fprintf(fout,"program has analysed %.3f %% of total cases \n", state*100);
        fprintf(fout,"program has found %lld cases matching requested output \n", caseSolved);
        fprintf(fout,"Current local time and date: %s",asctime(timeinfo));
        fprintf(fout,"************************************************************\n\n");
        fflush(fout);
         */

        NB_OPS = 0;
    }
    while (keep) {

        if (current->output_req) {
            regs[0] |= tab_one[current->count][0] << node;
            regs[1] |= tab_one[current->count][1] << node;
            regs[2] |= tab_one[current->count][2] << node;
        } else {
            regs[0] |= tab_zero[current->count][0] << node;
            regs[1] |= tab_zero[current->count][1] << node;
            regs[2] |= tab_zero[current->count][2] << node;
        }

        if (current->next == NULL) {
            keep = 0;
        } else {
            current = current->next;
        }
        node++;
    }
}

inline void readName(char* name, deep_tree* tree) {
    deep_tree *current = tree;
    char i, keep = 1;
    unsigned int j;
    for (j = 0; keep; j++) {
        for (i = 0; i < 8 && keep; i++) {
            char charToRead = name[j];
            int valueToSet = charToRead >> i;
            current->output_req = valueToSet & 1;
            //current->output_req=1;
            if (current->next == NULL) {
                keep = 0;
            } else {
                current = current->next;
            }
        }
    }
}

inline void printRegisters(u32 regs[3]) {
    char j;
    for (j = 0; j < 3; j++) {
        printf("reg%d= 0x%lx;\n", j, regs[j]);
    }
}

u8 getCharFromIndexAndTree(u32 *regs) {
    u8 f[8] = {0, 0, 0, 1, 0, 1, 1, 1};
    u8 x,outblock,j,i;

    u32 reb = 0;
    register u32 reg1 = regs[0];
    register u32 reg2 = regs[1];
    register u32 reg3 = regs[2];


    unsigned char tmp = 0;
    for (j = 0; j < stringLength; j++) { //string
        outblock = 0;
        for (i = 0; i < 8; i++) {

            x = f[ (reg1 & 1) | ((reg2 & 1) << 1) | ((reg3 & 1) << 2) ];
            outblock |= (x << i);


            tmp = (name[j] >> i)&1;

            if (x != tmp) {
                return 0;
            }

            reb = motpar(reg1 & POLY1);
            reg1 >>= 1;
            reg1 |= reb ? 0x10000L : 0L;

            reb = motpar(reg2 & POLY2);
            reg2 >>= 1;
            reg2 |= reb ? 0x40000L : 0L;

            reb = motpar(reg3 & POLY3);
            reg3 >>= 1;
            reg3 |= reb ? 0x400000L : 0L;
        }
    }
    return outblock;
}

int main(int argc, char *argv[]) //argv[1] = cpu_offset; argv[2]=cpu_count; argv[3]=output_file_path
{
    deep_tree *tree = NULL;
    char path[250];
    int nb_cpu, nb_step;
    u32 regs[3] = {0, 0, 0};
    u32 reg0, reg1, reg2,a,b,c;
    tree = createTree();
    readName(name, tree);
    stringLength = 5; //fix str size fast recover
    /* 
     stringLength = strlen(name);
     nb_step = atoi(argv[1]);
     nb_cpu = atoi(argv[2]);     
     char* folderPath = argv[3];
     strcpy(path, folderPath);
     strcat(path, argv[1]); //add offset number
     fout = fopen(path, "w");
     */
    nb_cpu = atoi("1");
    nb_step = atoi("0");
    fout = fopen("/home/peon/test", "w");
    
    incrementTree(tree, nb_step);

    
    signed long long maxNodeValue = -1;
    u32 regSave0, regSave1, regSave2;
    char computedChar = 0;
    do {
        signed long long newNodeValue = nodeValue(tree);
        if (newNodeValue <= maxNodeValue) {
            printf("fatal error, should not happen");
        }
        maxNodeValue = newNodeValue;
        updateRegs(tree, regs);
        reg0 = regs[0];
        reg1 = regs[1];
        reg2 = regs[2];
        for (b = 0; b <= MASK2; b += 0x20000) {
            
            regs[1] += 0x20000;
            regs[2] = reg2;
                        
            for (c = 0; c <= MASK3; c += 0x20000) {
                regs[2] = regs[2] + 0x20000;
                computedChar = getCharFromIndexAndTree(regs);
                globalCaseCounter++;
                
                if (computedChar != 0) {
                    regSave0 = regs[0];
                    regSave1 = regs[1];
                    regSave2 = regs[2];
                    caseSolved++;
                    if (stringLength > 2 && displayCaseFound) {
                        printf("************************************************************\n");
                        printf("reg0=%lx;\nreg1=%lx;\nreg2=%lx;\n", regSave0, regSave1, regSave2);
                        printf("last character: %c,0x%x \n", computedChar, computedChar);
                        printf("************************************************************\n");
                        printf("\n");
                    }
                    fprintf(fout, "************************************************************\n");
                    fprintf(fout, "reg0=0x%lx;\nreg1=0x%lx;\nreg2=0x%lx;\n", regSave0, regSave1, regSave2);
                    fprintf(fout, "last character: %c, 0x%x \n", computedChar, computedChar);
                    fprintf(fout, "************************************************************\n\n");
                    fflush(fout);
                }
                
            }
            
        }
        regs[0] = reg0;
    } while (incrementTree(tree, nb_cpu));
    printf("END");
    getchar();

}



