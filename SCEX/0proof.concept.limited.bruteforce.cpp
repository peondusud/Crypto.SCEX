#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char BITS_REG1=17;
int  NB_OPS=0;

#define POLY1 0xEF75L
#define POLY2 0x58C2FL
#define POLY3 0x739583L

#define MASK1 0x1FFFFL
#define MASK2 0x7FFFFL
#define MASK3 0x7FFFFFL


//typedef __int32 u32;
typedef unsigned long int u32;
typedef unsigned short 	u16;
typedef unsigned char 	u8;	

typedef struct deep_tree deep_tree;

struct deep_tree{
	char count;
	char output_req;
	struct deep_tree *next;
};


inline u8 motpar(u32  w){
	//w^=(w>>32);
	w^=(w>>16);
	w^=(w>>8);
	w^=(w>>4);
	w^=(w>>2);
	w^=(w>>1);
	return (u8)(w & 1);
}


deep_tree* createTree(){
	char i;
	deep_tree *next=NULL;
	deep_tree *current=NULL;
	deep_tree * origin=(deep_tree*)calloc(1,sizeof(deep_tree));
	current=origin;
	for(i=0;i<BITS_REG1-1;i++){
		next=(deep_tree*)calloc(1,sizeof(deep_tree));
		current->next=next;
		current=next;
	}
	return origin;
}


char incrementTree(deep_tree *origin,int nb_off){

	if(nb_off){
		if(origin->count==3){
			origin->count=0;
			if(origin->next!=NULL){
				incrementTree(origin->next,1);
				incrementTree(origin,nb_off-1);
			}
			else
				return 0;
		}
		else{
			origin->count++;
			incrementTree(origin,nb_off-1);
		}	
	}
	return 1;
}

inline void updateRegs(deep_tree *tree,u32 regs[3]){
	deep_tree *current = tree;
	char tab_zero[4][3] ={{1,0,0},{0,1,0},{0,0,1},{0,0,0}}; //all case if outlock bit=0
	char tab_one[4][3] ={{1,1,0},{0,1,1},{1,0,1},{1,1,1}}; //all case if outlock bit=1
	char node=0;
	char keep=1;
	NB_OPS++;
	if(NB_OPS==10000000){
		printf("10M OPS \n");
		NB_OPS=0;
	}
	while(keep){

		//printf("node = %d \t output_req = %d \t count = %d\n",node,current->output_req,current->count);
		//for(char i=0;i<3;i++)
		//	(current->output_req)?regs[i]|=tab_one[current->count][i]<<node:regs[i]|=tab_zero[current->count][i]<<node;

		if(current->output_req){
			regs[0]|=tab_one[current->count][0]<<node;
			regs[1]|=tab_one[current->count][1]<<node;
			regs[2]|=tab_one[current->count][2]<<node;
		}
		else{
			regs[0]|=tab_zero[current->count][0]<<node;
			regs[1]|=tab_zero[current->count][1]<<node;
			regs[2]|=tab_zero[current->count][2]<<node;
		}

		if(current->next==NULL){
			keep=0;
		}
		else{
			current=current->next;
		}
		node++;
	}

}

void readName(u8* name,deep_tree* tree){
	deep_tree *current = tree;
	char i,keep=1;
	int j;
	for(j=0;j<sizeof(name) && keep ;j++){
		for(i=0;i<8 && keep ;i++)
		{ 
			current->output_req=(name[j]>>i)&1;
			if(current->next==NULL)	{
				keep=0;
			}
			else{
				current=current->next;
			}
		}
	}
}

void printRegisters(u32 regs[3]){
	char j;
	for(j=0;j<3;j++)
		printf("reg= %lx\n",regs[j]);
}

//void setTree(int val,int nb_cpu,deep_tree *tree){
//	deep_tree *current = tree;
//	__int64 nb_ops_max=0x400000000;
//	__int64 nb_ops_thread=nb_ops_max/nb_cpu+nb_ops_max%nb_cpu;
//	__int64 start_ops=nb_ops_thread*val;
//	char keep=1;
//	for(int i=0;i<BITS_REG1 && keep;i++){
//		current->output_req=((start_ops>>(i*2))&3);
//		if(current->next==NULL)	{
//			keep=0;
//		}
//		else{
//			current=current->next;
//		}
//	}
//}

				
//	args[1]=current thread (for nb_cpu=8, current thread goes 0-7)
//	args[2]=number of cpu

int main(int argc,char *argv[]){
	FILE  *fout;
	deep_tree *tree=NULL;
	char path[250];
	char tab_zero[4][3] ={{1,0,0},{0,1,0},{0,0,1},{0,0,0}};
	char tab_one[4][3] ={{1,1,0},{0,1,1},{1,0,1},{1,1,1}};
	u8 name[8]={'M','r',' ','D','u','p','u','y'},loop,outblock, f[8]={0,0,0,1,0,1,1,1}, x,i;
	u32 rec_a,rec_b,b,rec_c,c;
	int nb_cpu,nb_step,j;
	register u32 regs[3]={0,0,0},reb;
	tree = createTree();
	readName(name,tree);
	nb_cpu=atoi(argv[2]);
	nb_step=atoi(argv[1]);
	incrementTree(tree,nb_step);
	updateRegs(tree,regs); //regen regs
	printRegisters(regs);
	strcpy (path,"C:\\Users\\X\\Desktop\\result_DUP");
	strcat(path,argv[1]);
	fout=fopen(path,"w");

	do{
		//printRegisters(regs);
		updateRegs(tree,regs);

		rec_a=regs[0];
		rec_b=regs[1];
		rec_c=regs[2];


		for(b=0;b<=MASK2;b+=0x20000){
			for(c=0;c<=MASK3;c+=0x20000){ 
				regs[0]=rec_a;
				regs[1]=rec_b|b;
				regs[2]=rec_c|c;
				//printf("\n\n********reg1=%lx reg2=%lx reg3=%lx**********\n",regs[0],regs[1],regs[2]);
				loop=1;
				for(char j=0;j<sizeof(name);j++){  //string
					if(loop==0)
						break;
					outblock=0;

					for( i=0;i<8;i++){ // bit a bit
						x = f[ (regs[0] & 1) | ((regs[1]&1) << 1) | ((regs[2]&1) << 2) ];
						outblock |= (x << i);
						int toto =x;
						int tata=(name[j]>>i)&1;

						if(!(toto==tata)){
							loop=0;
							break;
						}
						reb= motpar(regs[0] & POLY1);
						regs[0]>>=1;
						regs[0]|= reb ? 0x10000L : 0L;

						reb= motpar(regs[1] & POLY2);
						regs[1]>>=1;
						regs[1] |= reb ? 0x40000L : 0L;

						reb= motpar(regs[2] & POLY3);
						regs[2]>>=1;
						regs[3] |= reb ? 0x400000L : 0L;
					}
					//printf(" - %c",outblock);
					//printf("\n**********\nreg1=%lx \nreg2=%lx \nreg3=%lx\n**********\n",rec_a,rec_b|b,rec_c|c);
				}
				if(loop){ //
					printf("\n\n********reg1=%lx reg2=%lx reg3=%lx**********\n",rec_a,rec_b|b,rec_c|c);
					printf(" - %c",outblock);
					fprintf(fout,"\n\n********reg1=%lx reg2=%lx reg3=%lx**********\n",rec_a,rec_b|b,rec_c|c);
					fprintf(fout," - %c",outblock);
					fflush(fout);

				}

			}

		}
	}
	while(incrementTree(tree,nb_cpu));
	printf("END");
	getchar();

}