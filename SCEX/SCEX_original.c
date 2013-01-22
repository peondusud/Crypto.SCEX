#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define DEBUG
#define POLY1 0xEF75L
#define POLY2 0x58C2FL
#define POLY3 0x739583L

#define MASK1 0x1FFFFL
#define MASK2 0x7FFFFL
#define MASK3 0x7FFFFFL

typedef unsigned long int u32;
typedef unsigned short 	u16;
typedef unsigned char 	u8;	

u8 motpar(u32  w){
//w^=(w>>32);
w^=(w>>16);
w^=(w>>8);
w^=(w>>4);
w^=(w>>2);
w^=(w>>1);
return (u8)(w & 1);
}


void main (int argc, char * argv[]){
register u32 reg1, reg2, reg3, reb;
u32 i, j;
FILE *fin, *fout;
u8 outblock, lettre, f[8]={0,0,0,1,0,1,1,1}, x;

fin=fopen(argv[2],"r");
fout=fopen(argv[3],"w");

reg1 = (u8) (argv[4][0]) | ((u8) (argv[4][1]) << 8) | ((u8) (argv[4][2]) << 16) ;
reg1 &= MASK1;

reg2 = (u8) (argv[4][2]) | ((u8) (argv[4][3]) << 8) | ((u8) (argv[4][4]) << 16) ;
reg2 >>= 1;
reg2 &= MASK2;

reg3 = (u8) (argv[4][4]) | ((u8) (argv[4][5]) << 8) | ((u8) (argv[4][6]) << 16) | ((u8) (argv[4][7]) << 24) ;
reg3 >>= 3;
reg3 &= MASK3;


#ifdef DEBUG
reg1 = 0x3130;
reg2 = 0x21999;
reg3 = 0x66C6A6;
printf("Etats registre init : %lx %lx %lx\n",reg1,reg2,reg3);
#endif

j=0L;
while(fscanf(fin,(argv[1][0]=='e')? "%c" : "%02X" , &lettre), !feof(fin)) {
	j++;
	outblock=0;

	for(i=0;i<8;i++){ 
		x = f[ (reg1 & 1) | ((reg2&1) << 1) | ((reg3&1) << 2) ];
		outblock |= (x << i);

		reb= motpar(reg1 & POLY1);
		reg1>>=1;
		reg1 |= reb ? 0x10000L : 0L;
		//reg1 |= (reb<<16);


		reb= motpar(reg2 & POLY2);
		reg2>>=1;
		reg2 |= reb ? 0x40000L : 0L;
		//reg2 |= (reb<<18);


		reb= motpar(reg3 & POLY3);
		reg3>>=1;
		reg3 |= reb ? 0x400000L : 0L;
		//reg3 |= (reb<<22);
	}
		#ifdef DEBUG
		printf(" - %lx",outblock);
		#endif

if(argv[1][0]=='e')
	fprintf(fout,(!(j%32)) ? "%02X\n" : "%02X", lettre^outblock);
else
	fprintf(fout,"%c", lettre^outblock);
}

printf("\n nombre de lettre traitees : %d\n",j);
fclose(fin);
fclose(fout);
}
