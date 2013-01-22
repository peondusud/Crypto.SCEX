#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define DEBUG
//#define POLY1 0x2000000002D
//#define POLY2 0x8000000332B
//#define POLY3 0x800000000069
#define MASK1 0x3FFFFFFFFFFL
#define MASK2 0xFFFFFFFFFFFL
#define MASK3 0xFFFFFFFFFFFFL
#define POLY1 0x2D
#define POLY2 0x332B
#define POLY3 0x69



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
u8 outblock, lettre, f1[8]={0,0,0,1,0,1,1,1},f2[8]={0,1,0,0,1,0,1,1},f3[8]={0,0,1,1,0,1,0,1}, x,y,z,aaa;

fin=fopen(argv[2],"r");
fout=fopen(argv[3],"w");

reg1 = (u8) (argv[4][0])  ;
reg1 &= MASK1;

reg2 = (u8) (argv[4][0]>>6) | ((u8) (argv[4][2]) << 2) | ((u8) ((argv[4][3])&0xF )<< 10) ;
reg2 &= MASK2;

reg3 = (u8) (argv[4][3]>>4) | ((u8) (argv[4][4]&0x3) << 4)  ;
reg3 &= MASK3;


#ifdef DEBUG
reg1 = 0x3130;
reg2 = 0x21999;
reg3 = 0x66C6A6;
printf("Etats registre init : %lx %lx %lx\n",reg1,reg2,reg3);
#endif
j=0;
while(fscanf(fin,(argv[1][0]=='e')? "%c" : "%02X" , &lettre), !feof(fin)) {
	j++;
	outblock=0;

	for(i=0;i<8;i++){ 

		x = f1[ (reg1 & 1) | ((reg2&1) << 1) | ((reg3&1) << 2) ];
		y = f2[ (reg1 & 1) | ((reg2&1) << 1) | ((reg3&1) << 2) ];
		z = f3[ (reg1 & 1) | ((reg2&1) << 1) | ((reg3&1) << 2) ];

		aaa=x^y^z;
		outblock |= (aaa << i);

		reb= motpar(reg1 & POLY1);
		reg1>>=1;
		reg1 |= reb ? 0x10000L : 0L;

		reb= motpar(reg2 & POLY2);
		reg2>>=1;
		reg2 |= reb ? 0x40000L : 0L;


		reb= motpar(reg3 & POLY3);
		reg3>>=1;
		reg3 |= reb ? 0x400000L : 0L;		
	}
	
		#ifdef DEBUG
		printf(" - %lx",outblock);
		#endif

if(argv[1][0]=='e')
	fprintf(fout,(!(j%32)) ? "%02X\n" : "%02X", lettre^outblock);
else
	fprintf(fout,"%c", lettre^outblock);
}

fclose(fin);
fclose(fout);
}
