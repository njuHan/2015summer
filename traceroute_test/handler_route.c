#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <dirent.h> //opendir

#define MAX_LINE_LEN 512

// gcc -o route handler_route.c 


char pre_ip[32];
double pre_time;

char next_ip[32];
double next_time;




void handler_line(char* line)
{
	//提取括号内ip
	char* ip = (char*)malloc(sizeof(char)*32);
	int len = strlen(line);
	
	int i;
	for (i=0; i<len; i++)
	{
		if (line[i]=='(')
			break;
	}
	i++;
	
	int j=0;
	while(line[i]!=')')
	{
		next_ip[j] = line[i];
		i++;
		j++;
	}
	next_ip[j] = '\0';
	
	//提取时间
	i++;
	j=0;
	char temp[16];
	while(line[i]!='\n')
	{		
		if (line[i]=='m') //ms
			break;
		
		if (line[i]!=' ')
		{
			temp[j]=line[i];
			j++;
		}
		
		i++;
		
		
	}
	temp[j]='\0';
	//printf("time: %s\n", temp);
	next_time =  strtod(temp, NULL);
	
	
}


void handler_file(char* filename, char* outfilename)
{
	FILE* fread = fopen(filename,"r");
	
	FILE* fwrite = fopen(outfilename,"w+");
	
	char line[MAX_LINE_LEN];
	
	int is_first_line = 1;
	
	while(1)
	{
		fgets(line, MAX_LINE_LEN, fread);
		if (feof(fread))
			break;
		
		
		if (strchr(line, '*')!=NULL)
		{
			/*
			strcpy(next_ip, "*");
			next_time = 0;
			
			strcpy(pre_ip, next_ip);
			pre_time = next_time;
			*/
			continue;
		}
		
		handler_line(line);
		
		if  (is_first_line)
		{
			strcpy(pre_ip, next_ip);
			pre_time = next_time;
			
			is_first_line = 0;
		}
		else
		{
			double delay = next_time - pre_time;
			
			delay = (delay>=0)?(delay): 0;
			
			if (strcmp(pre_ip, next_ip)!=0 && strcmp(pre_ip,"*")!=0 && strcmp(next_ip, "*")!=0 )
				fprintf(fwrite,"%s\t%s\t%f\n", pre_ip, next_ip, delay);
			
			strcpy(pre_ip, next_ip);
			pre_time = next_time;
		}
		
		//printf("ip: %s, time: %f\n", next_ip, next_time);
		
	}
	

	
	fclose(fwrite);
	fclose(fread);
}



int main(int argc, char *argv[])
{

	struct dirent *pDirEntry = NULL;
    DIR *pDir = NULL;
    if( (pDir = opendir("./")) == NULL )
    {
		printf("opendir failed!\n");
		return 1;
	}
    else
    {
		char outfilename[16];
		int i = 1;
		
		while( pDirEntry = readdir(pDir) )
		{
			
			if ( strstr(pDirEntry->d_name, "route")
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-4]=='.'
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-3]=='t'
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-2]=='x' 
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-1]=='t')
			{
				
				sprintf(outfilename, "out%d", i);
				i++;
				
                printf("输入文件：%s\t输出文件：%s \n",pDirEntry->d_name, outfilename);
				
				handler_file(pDirEntry->d_name, outfilename);
			
				printf("------------------------------------\n");
			}
		}
		closedir(pDir);

	}       
	
	
	
	return 0;
}