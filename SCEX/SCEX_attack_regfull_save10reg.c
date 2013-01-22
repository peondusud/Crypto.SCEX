// crypto.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TAB 10
#define DEBUG
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

u8 motpar(u32  w){
	//w^=(w>>32);
	w^=(w>>16);
	w^=(w>>8);
	w^=(w>>4);
	w^=(w>>2);
	w^=(w>>1);
	return (u8)(w & 1);
}

int checkvalueIntab(u32 tab_count[],u32 value){
	int ret=-1;
	int i=0;
	for(i=MAX_TAB-1;i>=0;i--){
		if(tab_count[i]<value){
			ret=i;
			break;
		}
	}

	return ret;
}

void updateTab(u32 tab_reg[],u32 tab_count[],u32 count_value,u32 reg){
	int i,j;
	i=checkvalueIntab( tab_count, count_value);
	if(i!=-1){
		for(j=0;j<i;j++){
			tab_count[j]=tab_count[j+1];
			tab_reg[j]=tab_reg[j+1];
		}
		tab_reg[i]=reg;
		tab_count[i]=count_value;
	}
}

void prinTabs(u32 tab_reg[],u32 tab_count[],FILE *fout){
	for(int i=0;i<MAX_TAB;i++){
		printf("reg[%d] =  %lx  \t count[%d] = %lx\n",i,tab_reg[i],i,tab_count[i]);
		fprintf(fout,"reg[%d] =  %lx  \t count[%d] = %lx\n",i,tab_reg[i],i,tab_count[i]);
	}
	printf("\n");
}
void clearTabs(u32 tab_reg[],u32 tab_count[]){
	for(int i=0;i<MAX_TAB;i++){
		tab_reg[i]=0;
		tab_count[i]=0;}
}



void decrypt(u32 reg1,u32 reg2,u32 reg3,FILE *fin,FILE *fout){
	register u32 reb;
	u32 i, j;
	u8 outblock, lettre, f[8]={0,0,0,1,0,1,1,1}, x;

	rewind(fin);
	while(fscanf(fin, "%02X" , &lettre), !feof(fin)) {
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


		fprintf(fout,"%c", lettre^outblock);
	}

}






void findRegs(char * argv[]){
	register u32 reg1, reg2, reg3, reb,save_reg1,save_reg2,save_reg3;
	u32 i,j,k,len,count_reg1,count_reg2,count_reg3, max_count_reg1, max_count_reg2, max_count_reg3;
	u32 tab_count1[10]={0,0,0,0,0,0,0,0,0,0};
	u32 tab_reg1[10]={0,0,0,0,0,0,0,0,0,0};
	u32 tab_count2[10]={0,0,0,0,0,0,0,0,0,0};
	u32 tab_reg2[10]={0,0,0,0,0,0,0,0,0,0};
	u32 tab_count3[10]={0,0,0,0,0,0,0,0,0,0};
	u32 tab_reg3[10]={0,0,0,0,0,0,0,0,0,0};
	FILE *fin, *fout,*ftemp;
	u8 *buffer, lettre, f[8]={0,0,0,1,0,1,1,1},a,b,c,*path;

	fin=fopen("C:\\cipher\\crypto5","r");
	fout=fopen("C:\\Users\\X\\Desktop\\result_reg5.txt","w");
	ftemp=fopen("C:\\Users\\X\\Desktop\\plain_ciper5.txt","w");
	//fin=fopen(argv[1],"r");
	//fout=fopen(argv[2],"w");

	fseek(fin, 0, SEEK_END);
	len=ftell(fin);
	fseek(fin, 0, SEEK_SET);

	//Allocate memory
	buffer=(u8 *)malloc(len*sizeof(char));
	len=0;
	while(fscanf(fin, "%02X" , &lettre), !feof(fin)) {
		buffer[len]=lettre;
		len++;
	}

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
	for(k=0;k<=MASK1;k++){ 

		reg1=k;
		count_reg1=0;
		count_reg2=0;
		for(j=0;j<len;j++) {
			lettre=buffer[j];
			for(i=0;i<8;i++){ 

				count_reg1+= ( (reg1 & 1) ^ ( (lettre>>i) & 1)) ^1; 
				reb= motpar(reg1 & POLY1);
				reg1>>=1;
				reg1 |= reb ? 0x10000L : 0L;
			}
		}

		updateTab(tab_reg1, tab_count1,count_reg1,k);
		/*if(max_count_reg1<count_reg1){
		max_count_reg1=count_reg1;
		save_reg1=k;			
		printf("\n reg1= 0x%lx\n",save_reg1);
		}*/
	}
	fprintf(fout,"************reg1***************\n");
	prinTabs(tab_reg1, tab_count1,fout);

	for(k=0;k<=MASK2;k++){  
		reg2=k;
		count_reg2=0;

		for(j=0;j<len;j++){
			lettre=buffer[j];

			for(i=0;i<8;i++){ 

				count_reg2+= ( (reg2 & 1) ^ ( (lettre>>i) & 1)) ^1;
				reb= motpar(reg2 & POLY2);
				reg2>>=1;
				reg2 |= reb ? 0x40000L : 0L;
			}	

		}
		updateTab(tab_reg2, tab_count2,count_reg2,k);
		/*	if(max_count_reg2<count_reg2){
		max_count_reg2=count_reg2;
		save_reg2=k;			
		printf("\n reg2= 0x%lx\n",save_reg2);
		fprintf(fout,"reg2= 0x%lx\n",save_reg2);
		}*/

	}

	fprintf(fout,"************reg2***************\n");
	prinTabs(tab_reg2, tab_count2,fout);

	for(k=((save_reg2>>18)&1);k<=MASK3;k+=2){ 
		reg3=k;
		count_reg3=0;

		for(j=0;j<len;j++){
			lettre=buffer[j];

			for(i=0;i<8;i++){ 

				count_reg3+= ( (reg3 & 1) ^ ( (lettre>>i) & 1)) ^1;
				reb= motpar(reg3 & POLY3);
				reg3>>=1;
				reg3 |= reb ? 0x400000L : 0L;		
			}	

		}
		updateTab(tab_reg3, tab_count3,count_reg3,k);
		/*if(max_count_reg3<count_reg3){
		max_count_reg3=count_reg3;
		save_reg3=k;			
		printf("\n reg3= 0x%lx\n",save_reg3);
		}
		*/
	}
	fprintf(fout,"************reg3***************\n");
	prinTabs(tab_reg3, tab_count3,fout);
	printf("reg1= 0x%lx\n",save_reg1);
	printf("reg2= 0x%lx\n",save_reg2);
	printf("reg3= 0x%lx\n",save_reg3);
	fprintf(fout,"\n reg1= 0x%lx\n",save_reg1);
	fprintf(fout,"\n reg2= 0x%lx\n",save_reg2);
	fprintf(fout,"\n reg3= 0x%lx\n",save_reg3);	
	fclose(fout);


	for(a=MAX_TAB-1;a>=5;a--){
		for(b=MAX_TAB-1;b>=5;b--){
			for(c=MAX_TAB-1;c>=5;c--){
				fprintf(ftemp,"***********reg1 = %lx  -  reg2 = %lx  -  reg3 = %lx**************\n",tab_reg1[a],tab_reg2[b],tab_reg3[c]);
				decrypt(tab_reg1[a],tab_reg2[b],tab_reg3[c],fin,ftemp);
				fprintf(ftemp,"\n\n");				
			}
			fflush(ftemp);
		}
	}
	fclose(ftemp);
	fclose(fin);
}





void main (int argc, char * argv[]){

	findRegs(argv);
	printf("END\n");
	getchar();
}
