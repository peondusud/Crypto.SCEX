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
register u32 reg1, reg2, reg3, reb,save_reg1,save_reg2,save_reg3;
u32 i, j,k,count_reg1,count_reg2,count_reg3, max_count_reg1, max_count_reg2, max_count_reg3;
FILE *fin, *fout;
u8 outblock, lettre, f[8]={0,0,0,1,0,1,1,1}, x;

fin=fopen(argv[1],"r");
fout=fopen(argv[2],"w");

//gene registre


save_reg1=0L;
save_reg2=0L;
save_reg3=0L;
max_count_reg1=0L;
max_count_reg2=0L;
max_count_reg3=0L;
reg1=0L;
reg2=0L;
reg3=0L;
fprintf(fout,"TEXT =************** %s ************\n",argv[1]);
for(k=0;k<=MASK3;k++){ 

reg1=k&MASK1;
reg2=k&MASK2;
reg3=k&MASK3;


//printf("reg1 : %lx \n",reg1);


j=0L;
count_reg1=0;
count_reg2=0;
count_reg3=0;
while(fscanf(fin, "%02X" , &lettre), !feof(fin)) {
	j++;
	outblock=0;
	//printf(" %c ",lettre);
	for(i=0;i<8;i++){ 
		
		if(k<MASK1){
			count_reg1+= ( (reg1 & 1) ^ ( (lettre>>i) & 1)) ^1; //count+=!(x^((lettre>>i)&1));
			reb= motpar(reg1 & POLY1);
			reg1>>=1;
			reg1 |= reb ? 0x10000L : 0L;
		}
				
		if(k<MASK2){
			count_reg2+= ( (reg2 & 1) ^ ( (lettre>>i) & 1)) ^1;
			reb= motpar(reg2 & POLY2);
			reg2>>=1;
			reg2 |= reb ? 0x40000L : 0L;
		}
		
		count_reg3+= ( (reg3 & 1) ^ ( (lettre>>i) & 1)) ^1;

		reb= motpar(reg3 & POLY3);
		reg3>>=1;
		reg3 |= reb ? 0x400000L : 0L;
		
		}
	

}
rewind(fin);
	if(max_count_reg1<count_reg1){
	printf("\n count_reg1 = %lx \n",count_reg1);
	printf("\n reg1= 0x%lx\n",save_reg1);
	fprintf(fout,"reg1= 0x%lx\n",save_reg1);
	max_count_reg1=count_reg1;
	save_reg1=k&MASK1;
	}
	if(max_count_reg2<count_reg2){
	printf("\n count_reg2 = %lx \n",count_reg2);	
	printf("\n reg2= 0x%lx\n",save_reg2);
	fprintf(fout,"reg2= 0x%lx\n",save_reg2);
	max_count_reg2=count_reg2;
	save_reg2=k&MASK2;
	}
	if(max_count_reg3<count_reg3){
	printf("\n count_reg3 = %lx \n",count_reg3);
	printf("\n reg3= 0x%lx\n",save_reg3);
	fprintf(fout,"reg3= 0x%lx\n",save_reg3);
	max_count_reg3=count_reg3;
	save_reg3=k&MASK3;
	}

}

printf("reg1= 0x%lx\n",save_reg1);
printf("reg2= 0x%lx\n",save_reg2);
printf("reg3= 0x%lx\n",save_reg3);
fprintf(fout,"\n reg1= 0x%lx\n",save_reg1);
fprintf(fout,"\n reg2= 0x%lx\n",save_reg2);
fprintf(fout,"\n reg3= 0x%lx\n",save_reg3);
fclose(fin);
fclose(fout);
}
