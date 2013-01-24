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

typedef unsigned __int32 u32;
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


void main (void){
register u32 reg1, reg2, reg3, reb;
u32 i, j;
u8 outblock,  f[8]={0,0,0,1,0,1,1,1}, x;

reg1 = 0x1fffd;
reg2 = 0x2724f;
reg3 = 0x200001;
printf("Etats registre init : %lx %lx %lx\n",reg1,reg2,reg3);

j=0L;
for(j=0;j<8;j++) {
	outblock=0;

	for(i=0;i<8;i++){ 
		x = f[ (reg1 & 1) | ((reg2&1) << 1) | ((reg3&1) << 2) ];
		outblock |= (x << i);

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
		
		printf("%c (%lx) ",outblock,outblock);


}

getchar();

}
