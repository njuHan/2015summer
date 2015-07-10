#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <pcap.h>

#include <dirent.h> //opendir
#include <assert.h>

#define SIZE 15120


/*
 *    readme
 * (1) install libpcap  参考： http://blog.csdn.net/qinggebuyao/article/details/7715843
 * (2) gcc -o tcplog tcplog.c -lpcap
 * (3) sudo ./tcplog 
 * (4) 如果有libpcap.so.1 链接错误，请建立软链接，参考：http://blog.csdn.net/cfjtaishan/article/details/7096085
 * 
 * 
 */

int work(char *filename)
{
	int id=2;
	int tempnum=0;
	// char filename[50];
	int reval;   
	struct pcap_pkthdr* header;
	u_char *pkt_data;
	pcap_t *pcap_handle;
	char error_content[PCAP_ERRBUF_SIZE];
	int count;
	int index;
	char tempname[256];
	count=4;
	index=26;
	FILE *fd;
	strcpy(tempname,filename);
	strcat(tempname,"_out.txt");
	//w+ 打开可读写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。
	fd=fopen(tempname,"w+");
	pcap_handle=pcap_open_offline(filename,error_content);
	if(!pcap_handle)
	{
		fprintf(stderr, "Error in opening savefile, %s, for reading: %s\n",filename,error_content );
		exit(1);
	}
	reval = pcap_next_ex(pcap_handle, &header, (const u_char **)&pkt_data);

	
	unsigned int src,dst;
	//inet_aton("192.168.1.126",(struct in_addr*)&src);
	//inet_aton("130.104.230.45",(struct in_addr*)&dst);
	
	inet_aton("192.168.1.126",(struct in_addr*)&dst);
	inet_aton("130.104.230.45",(struct in_addr*)&src);
	
	while(pkt_data!=NULL && reval > 0)
	{
		
		if(*(pkt_data+12)==0x08 && *(pkt_data+13)==0x00 && *(unsigned int*)(pkt_data+26)==src && *(unsigned int*)(pkt_data+30)==dst) //MAC type==IP ip.src==client, ip.dst=server
		{

			if(*(pkt_data+23)==0x06) // TCP 6
			{
			
				fprintf(fd,"%ld.%ld\t",header->ts.tv_sec,header->ts.tv_usec);
				
				//assert(header->len-*(pkt_data+46)/4-34 != 0);
				fprintf(fd,"%d\n",header->len-*(pkt_data+46)/4-34);

			}
		}
		
		reval = pcap_next_ex(pcap_handle, &header, (const u_char **)&pkt_data);
		id++;
	}
	fclose(fd);
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
		while( pDirEntry = readdir(pDir) )
		{
			//判断是否为指定类型的文件
			//if(strstr(pDirEntry->d_name, ".cap") )
			if (pDirEntry->d_name[strlen(pDirEntry->d_name)-4]=='.'
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-3]=='c'
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-2]=='a' 
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-1]=='p')
			{
                printf("索引节点:%d\t 偏移量：%d\t 文件名长：%d\t文件类型：%d\t 文件名：%s\n",
                    (int)(pDirEntry->d_ino), (int)(pDirEntry->d_off),
                    (int)(pDirEntry->d_reclen),(int)(pDirEntry->d_type),pDirEntry->d_name);
				work(pDirEntry->d_name);
			}
		}
		closedir(pDir);

	}       
	
	
	
	return 0;
}
