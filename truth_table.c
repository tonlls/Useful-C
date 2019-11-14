#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define COMPILER "gcc"
#define OUT_FILE_C "t.c"
#define OUT_FILE "t"

void build_function(const int n_vars,const char *function){
	char c;
	char str[5]="\0";
	FILE* f;
	
	f=fopen(OUT_FILE_C,"w+");
	fprintf(f,"#include <stdlib.h>\nint main(int argc,char **argv){int a[%i+2];for(int i=1;i<argc;i++){a[i]=atoi(argv[i]);}",n_vars);
	fprintf(f,"return ");
	for(int i=0;i!=strlen(function);i++){
		c=function[i];
		if(isdigit(c))
			fprintf(f,"a[%i]",c-'0');
		else{
			switch(c){
				case '^':strcpy(str,"&&");break;
				case 'v':strcpy(str,"||");break;
				case -62:strcpy(str,"!");i++;break;
				case '[':strcpy(str,"(");break;
				case ']':strcpy(str,")");break;
				default:continue;
			}
			fprintf(f,"%s",str);
		}
	}
	fprintf(f,";}");
	fclose(f);
}
void compile_function(){
	char str[strlen(COMPILER)+1+strlen(OUT_FILE_C)+1+strlen(OUT_FILE)+1];
	sprintf(str,"%s %s -o %s",COMPILER,OUT_FILE_C,OUT_FILE);
	system(str);
	//todo check errors
}
int run_function(const int n_vars,int *vars){
	int ret;
	char str[50];
	char str1[10];
	sprintf(str,"./%s ",OUT_FILE);
	for(int i=0;i<n_vars;i++){
		sprintf(str1,"%i ",vars[i]);
		strcat(str,str1);
	}
	strcat(str,"\0");
	ret=system(str);
	if(ret>0)
		ret=1;
	return ret;
}
void get_results(const int n_vars,const int combinacions,int **table,int *result){
	for(int i=0;i<combinacions;i++){
		result[i]=run_function(n_vars,table[i]);
	}
}
void build_table(int n_vars,int combinacions,int **table){
	int val;
	int k=1,l;
	
	for(int i=n_vars-1;i>=0;i--){
		val=0;
		l=0;
		for(int j=0;j<combinacions;j++){
			table[j][i]=val;
			l++;
			if(l==k){
				l=0;
				val=(val+1)%2;
			}
		}
		k=k*2;
	}
}

void print_table(int fd,int n_vars,int combinations,int **table,int *res){
	for(int i=0;i<n_vars;i++)
		dprintf(df,"| a%i ",i+1);
	dprintf(fd,"|| s ||\n");
	for(int i=0;i<combinations;i++){
		for(int j=0;j<n_vars;j++){
			dprintf(fd,"| %i  ",table[i][j]);
		}
		dprintf(fd,"|| %i ||\n",res[i]);
	}
}

int init_table(int n_vars,int combinations,int ***table){
	(*table)=(int**)malloc(combinations*sizeof(int *));
	for(int i=0;i<combinations;i++){
		(*table)[i]=(int*)malloc(n_vars*sizeof(int));
	}
	return 1;
}
int init_results(int combinations,int *results){
	results=(int*)malloc(combinations*sizeof(int));
	return 1;
}
int main(int argc,char **argv){
	
	int variables,combinations;
	char c;
	int **table;
	int *result;
	
	if(argc<2){
		printf("arg error");
	}
	variables=atoi(argv[1]);
	combinations=(int)pow(2,variables);

	init_table(variables,combinations,&table);
	init_results(combinations,result);
	
	build_function(variables,argv[2]);
	compile_function();
	
	build_table(variables,combinations,table);

	get_results(variables,combinations,table,result);

	print_table(1,variables,combinations,table,result);
	
	remove(OUT_FILE);
	remove(OUT_FILE_C);
}
