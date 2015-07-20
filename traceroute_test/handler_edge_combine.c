#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <dirent.h> //opendir

#define MAX_LINE_LEN 512

//gcc -o edge handler_edge_combine.c 

typedef struct Edge
{
	char preip[32];
	char nextip[32];
	double sum_delay;
	int count;
}Edge;


struct Edge edge_table[1024];

typedef struct IP
{
	char ip[32];
}IP;

struct IP IP_table[1024];

int ip_count = 0;
int edge_count = 0;

int search_ip_table(char* ip)
{
	/*
	int i;
	for (i=0; i<ip_count; i++)
	{
		if (strcmp(IP_table[i].ip, ip)==0)
		{
			return i;
		}
	}
	
	if (i==ip_count)
	{
		strcpy(IP_table[ip_count].ip, ip);
		ip_count ++;
		return i;
	}
	*/
	if (strcmp(ip,"130.104.230.45")==0)
	{
		if (ip_count==0)
		{
			strcpy(IP_table[ip_count].ip, "start");
			ip_count ++;
			return 0;
		}
		else
			return 0;
	}
		
	
	int i=0;
	for (i=0; i<ip_count; i++)
	{
		int j=0;
		char* p1 = strchr(ip, '.');
		char* p2 = strchr(IP_table[i].ip, '.');
		if ( (p1-ip)==(p2-IP_table[i].ip) )
		{
			if (strncmp(ip, IP_table[i].ip, p1-ip)==0)
				return i;
		}
	
	}
	
	if (i==ip_count)
	{
		strcpy(IP_table[ip_count].ip, ip);
		ip_count ++;
		return i;
	}
}


int search_table(char* ip1, char* ip2, double delay)
{
	search_ip_table(ip1);
	search_ip_table(ip2);
	
	int i=0;
	for (i=0; i<edge_count; i++)
	{
		if (strcmp(edge_table[i].preip, ip1)==0
			&& strcmp(edge_table[i].nextip, ip2)==0 )
		{
			edge_table[i].sum_delay += delay;
			edge_table[i].count ++;
			
			break;
		}
	}
	
	if (i==edge_count)
	{
		strcpy(edge_table[edge_count].preip, ip1);
		strcpy(edge_table[edge_count].nextip, ip2);
		edge_table[edge_count].sum_delay = delay;
		edge_table[edge_count].count = 1;
		
		edge_count ++;
		
	}
}


void display_table(FILE* f, FILE* f2)
{
	int i;
	for (i=0; i<edge_count; i++)
	{
		//printf("%s\t%s\t%f\t%d\n", edge_table[i].preip, edge_table[i].nextip, edge_table[i].sum_delay, edge_table[i].count);
		fprintf(f, "%s\t%s\t%f\t%d\n", edge_table[i].preip, edge_table[i].nextip, edge_table[i].sum_delay, edge_table[i].count);
		
		//fprintf(f2, "\t%s -> %s [ label = \"%f(%d)\"];\n", edge_table[i].preip, edge_table[i].nextip, edge_table[i].sum_delay, edge_table[i].count);
		
		fprintf(f2, "\trouter%d -> router%d [ label = \"%f(%d)\"];\n", search_ip_table(edge_table[i].preip), search_ip_table(edge_table[i].nextip), edge_table[i].sum_delay, edge_table[i].count);
		
	}
}

void load_edge(char* filename)
{
	FILE* fread = fopen(filename,"r");
	
	char line[MAX_LINE_LEN];
	while(1)
	{
		fgets(line, MAX_LINE_LEN, fread);
		if (feof(fread))
			break;
		
		char* tab1, *tab2;
		
		char ip1[32];
		char ip2[32];
		
		double delay;
		
		int i=0;
		while(line[i]!='\t')
		{
			ip1[i] = line[i];
			i++;
		}
		ip1[i]='\0';
		
		int j=0;
		i++;
		while (line[i]!='\t')
		{
			ip2[j] = line[i];
			j++;
			i++;
		}
		ip2[j]='\0';
		
		delay =  strtod(line+i, NULL);
		
		
		
		//printf("%s\t%s\t%f\n", ip1,ip2,delay);
		
		search_table(ip1,ip2,delay);
		
	}
	
	
	fclose(fread);
}

void print_router(FILE* f)
{
	int i;
	
	for (i=0; i<ip_count; i++)
	{
		fprintf(f, "\trouter%d [label=\"%s \"]\n", i, IP_table[i].ip);
	}
}


void get_metrics(char* filename, FILE* fmetric)
{
	FILE* fread = fopen(filename,"r");
	
	int table[64];
	int i;
	for (i=0; i<64; i++)
		table[i] = 0;
	
	char line[MAX_LINE_LEN];
	while(1)
	{
		fgets(line, MAX_LINE_LEN, fread);
		if (feof(fread))
			break;
		
		char* tab1, *tab2;
		
		char ip1[32];
		char ip2[32];
		
		double delay;
		
		int i=0;
		while(line[i]!='\t')
		{
			ip1[i] = line[i];
			i++;
		}
		ip1[i]='\0';
		
		int j=0;
		i++;
		while (line[i]!='\t')
		{
			ip2[j] = line[i];
			j++;
			i++;
		}
		ip2[j]='\0';
		
		table[search_ip_table(ip1)]=1;
		table[search_ip_table(ip2)]=1;
		
	}
	
	//fprintf(fmetric, "%s\t", filename);
	//printf("%s\t",filename);
	
	
	fprintf(fmetric, "%d",table[0]);
	for (i=1; i<ip_count; i++)
	{
		fprintf(fmetric, ",%d", table[i]);
		//printf(",%d",table[i]);
	}
	fprintf(fmetric,"\n");
	//printf("\n");
	
	fclose(fread);

}

int cluster()
{
	FILE* fmetric = fopen("metrics","w+");
	fprintf(fmetric,"@relation topology\n");
	int i=0;
	for(i=0; i<ip_count; i++)
	{
		fprintf(fmetric, "@attribute router%d {0,1}\n", i);
	}
	fprintf(fmetric, "@data\n");
	
	struct dirent *pDirEntry = NULL;
    DIR *pDir = NULL;
    if( (pDir = opendir("./")) == NULL )
    {
		printf("opendir failed!\n");
		return 1;
	}
    else
    {
		
		
		
		while( pDirEntry = readdir(pDir) )
		{
			
			if ( strstr(pDirEntry->d_name, "out")
				&& pDirEntry->d_name[0]=='o'
				&& pDirEntry->d_name[1]=='u'
				&& pDirEntry->d_name[2]=='t')
			{
				
				get_metrics(pDirEntry->d_name , fmetric);
				
				
			}
		}
		closedir(pDir);

	}    
	
	fclose(fmetric);
	return 0;
}


int main(int argc, char *argv[])
{
	
	FILE* fwrite = fopen("topology","w+");
	FILE* fdraw = fopen("draw_combine.dot","w+");

	struct dirent *pDirEntry = NULL;
    DIR *pDir = NULL;
    if( (pDir = opendir("./")) == NULL )
    {
		printf("opendir failed!\n");
		return 1;
	}
    else
    {
		
		
		
		while( pDirEntry = readdir(pDir) )
		{
			
			if ( strstr(pDirEntry->d_name, "out")
				&& pDirEntry->d_name[0]=='o'
				&& pDirEntry->d_name[1]=='u'
				&& pDirEntry->d_name[2]=='t')
			{
                printf("输入文件：%s \n",pDirEntry->d_name);
				
				load_edge(pDirEntry->d_name);
			
				printf("------------------------------------\n");
			}
		}
		closedir(pDir);

	}       
	
	fprintf(fdraw, "digraph G {\n\t rankdir=LR;\n");
	
	display_table(fwrite,fdraw);
	
	print_router(fdraw);
	
	fprintf(fdraw, "}");
	
	fclose(fwrite);
	fclose(fdraw);
	
	cluster();
	
	return 0;
}