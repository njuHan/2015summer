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


void handler_file(char* filename, char* outfilename, FILE* fsort)
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
	
	printf("total delay:%f\n", next_time);

	fprintf(fsort,"%f\n", next_time);
		
	fclose(fwrite);
	fclose(fread);
}

struct SortElement
{
	char name[8];
	double delay;
};

struct SortElement arr[102];
int elem_count = 0;

void sort()
{
	FILE* f1 = fopen("sort","r");
	
	char line[MAX_LINE_LEN];
	
	
	//load elements
	while(1)
	{
		fgets(line, MAX_LINE_LEN, f1);
		if (feof(f1))
			break;
		
		int i=0;
		
		while(line[i]!='\t')
		{
			arr[elem_count].name[i] = line[i]; 
			i++;
		}
		arr[elem_count].name[i] = '\0';
		
		i++;
		
		arr[elem_count].delay = strtod(line+i, NULL);
		
		printf("%s\t%f\n", arr[elem_count].name, arr[elem_count].delay );
		
		elem_count ++;
		
	}
	fclose(f1);
	
	//sort elements
	int i,j;
	for(i=0; i<elem_count; i++)
	{
		double min = arr[i].delay; 
		struct SortElement temp;
		int index = i;
		
		for(j=i+1;j<elem_count;j++)
		{
			if(arr[j].delay < min)
			{ 
				min = arr[j].delay; 
				index = j;
			}       
		}       
        
		temp.delay = arr[i].delay; 
		strcpy(temp.name, arr[i].name);
		
		
		arr[i].delay = min;
		strcpy(arr[i].name, arr[index].name);
		
		arr[index].delay = temp.delay;
		strcpy(arr[index].name, temp.name);
	}       
	
	
	
	FILE* f2 = fopen("sort","w+");
	
	for (i=0; i<elem_count; i++)
	{
		fprintf(f2, "%s\t%f\n", arr[i].name, arr[i].delay);
	}
	
	fclose(f2);
}

int main(int argc, char *argv[])
{
	FILE* fsort = fopen("sort","w+");
	
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
				
				fprintf(fsort,"%s\t", outfilename);
				
				handler_file(pDirEntry->d_name, outfilename, fsort);
			
				printf("------------------------------------\n");
			}
		}
		closedir(pDir);

	}       
	
	fclose(fsort);
	
	sort();
	
	return 0;
}