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
register u32 reg1, reb,save_reg1;
u32 i, j,k,count,max_count;
FILE *fin, *fout;
u8 outblock, lettre, f[8]={0,0,0,1,0,1,1,1}, x;

fin=fopen(argv[2],"r");
fout=fopen(argv[3],"w");

//gene registre




save_reg1=0L;
max_count=0L;
reg1=0L;
for(k=0;k<=MASK1;k++){ 
reg1=k;
#ifdef DEBUG
printf("reg1 : %lx \n",reg1);
#endif

j=0L;
count=0;
while(fscanf(fin,(argv[1][0]=='e')? "%c" : "%02X" , &lettre), !feof(fin)) {
	j++;
	outblock=0;
	//printf(" %c ",lettre);
	for(i=0;i<8;i++){ 
		x =  (reg1 & 1) ;
		count+= (x ^ ( (lettre>>i) & 1)) ^1;
		//count+=!(x^((lettre>>i)&1));

		reb= motpar(reg1 & POLY1);
		reg1>>=1;
		reg1 |= reb ? 0x10000L : 0L;
		
		}
	

}
rewind(fin);
if(max_count<count){
printf("\n count = %d \n",count);
max_count=count;
save_reg1=k;
}

}

printf("\n reg1= 0x%lx\n",save_reg1);
printf("\n max count = %d \n",max_count);
fclose(fin);
fclose(fout);
}
