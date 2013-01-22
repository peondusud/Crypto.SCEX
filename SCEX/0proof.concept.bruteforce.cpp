#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_TAB 10
#define POLY1 0xEF75L
#define POLY2 0x58C2FL
#define POLY3 0x739583L

#define MASK1 0x1FFFFL
#define MASK2 0x7FFFFL
#define MASK3 0x7FFFFFL

typedef __int32 u32;
//typedef unsigned long int u32;
typedef unsigned short 	u16;
typedef unsigned char 	u8;	

struct regs {
	u32 reg1;
	u32 reg2;
	u32 reg3;
	regs *next_ptr ;
};
typedef struct regs regs;

u8 motpar(u32  w){
	//w^=(w>>32);
	w^=(w>>16);
	w^=(w>>8);
	w^=(w>>4);
	w^=(w>>2);
	w^=(w>>1);
	return (u8)(w & 1);
}


int main (int argc, char * argv[]){
	register u32 reg1, reg2, reg3, reb;
	u32 i, j,a,b,c,z;
	FILE  *fout;
	u8 outblock, lettre, f[8]={0,0,0,1,0,1,1,1}, x,name[7]={'M',' ','D','u','p','u','y'},loop;
	regs *save_buff,*tmp,*tmp2;

	fout=fopen("result_DUP.txt","w");
	reg1 = 0;
	reg2 = 0;
	reg3 = 0;
	save_buff=(regs*)calloc(1,sizeof(regs)*1);
	tmp=save_buff;
	loop=1;
	for(a=0;a<=MASK1;a++){
		for(b=0;b<=MASK2;b++){
			for(c=((b>>18)&1);c<=MASK3;c+=2){ 
				reg1=a;
				reg2=b;
				reg3=c;
				loop=1;

				for(j=0;j<sizeof(name);j++){	//string
					if(loop==0)
						break;
					outblock=0;					
					for(i=0;i<8;i++){	// bit a bit
						x = f[ (reg1 & 1) | ((reg2&1) << 1) | ((reg3&1) << 2) ];
						outblock |= (x << i);

						if(x!=(name[j]>>i)&1){
							loop=0;
							break;
						}
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
				}
				if(loop)
				{
					printf("\n\n********reg1=%lx reg2=%lx reg3=%lx**********\n",a,b,c);
					printf(" - %lx",outblock);
					fprintf(fout,"\n\n********reg1=%lx reg2=%lx reg3=%lx**********\n",a,b,c);
					fprintf(fout," - %lx",outblock);
					fflush(fout);
					tmp2=(regs*)calloc(1,sizeof(regs)*1);
					tmp2->next_ptr=NULL;
					tmp->reg1=a;
					tmp->reg2=b;
					tmp->reg3=c;
					tmp->next_ptr=tmp2;					
				}
			}
		}
	}
	printf("END");
	fclose(fout);
	return 0;
}
