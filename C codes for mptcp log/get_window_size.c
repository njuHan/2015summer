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
 * (2) gcc -o programname filename.c -lpcap
 * (3) sudo ./programname 
 * (4) 如果有libpcap.so.1 链接错误，请建立软链接，参考：http://blog.csdn.net/cfjtaishan/article/details/7096085
 * 
 * 
 */

//最大tcp目标端口数，和mptcp子流数相关
#define MAX_PORT_NUM 20

//ip address 用于过滤数据包
#define dst_ip "192.168.1.126" //本次实验的 w0网卡
//#define dst_ip "192.168.1.166" //本次实验的 w4网卡
//#define dst_ip "192.168.1.167" //w1
//#define dst_ip "192.168.8.100" // e1

#define src_ip "130.104.230.45"

unsigned short tcp_dst_port_table[MAX_PORT_NUM];
int port_count = 0;

//端口号 查表/填表
int search_table(unsigned short port)
{
	int i=0;
	for (i=0; i<port_count; i++)
	{
		if (port == tcp_dst_port_table[i])
			break;
	}
	if (i==port_count)
	{
		port_count++;
		tcp_dst_port_table[i] = port;
		return 0;
	}
	else
	{
		return 1;
	}
}

//打印tcp目标端口号表
void display_table()
{
	int i=0;
	for (i=0; i<port_count; i++)
	{
		printf("port %d: %u\n",i, tcp_dst_port_table[i]);
	}
}

//表初始化
void table_init()
{
	port_count = 0;
}

//遍历文件，获取tcp目的端口号信息
void load_table(char* filename)
{
	int reval;   
	struct pcap_pkthdr* header;
	u_char *pkt_data;
	pcap_t *pcap_handle;
	char error_content[PCAP_ERRBUF_SIZE];
	
	pcap_handle=pcap_open_offline(filename,error_content);
	reval = pcap_next_ex(pcap_handle, &header, (const u_char **)&pkt_data);
	
	
	unsigned int src,dst;
	
	inet_aton(src_ip,(struct in_addr*)&src);
	inet_aton(dst_ip,(struct in_addr*)&dst);
	
	
	unsigned short tcp_dst_port;
	
	
	while(pkt_data!=NULL && reval > 0)
	{
		
		if(*(pkt_data+12)==0x08 && *(pkt_data+13)==0x00 && *(unsigned int*)(pkt_data+26)==src && *(unsigned int*)(pkt_data+30)==dst) //MAC type==IP ip.src==client, ip.dst=server
		{

			if(*(pkt_data+23)==0x06) // TCP 6
			{
				tcp_dst_port = *(unsigned short*)(pkt_data+36);		
				tcp_dst_port = ntohs(tcp_dst_port);
				search_table(tcp_dst_port);
			}
		}
		
		reval = pcap_next_ex(pcap_handle, &header, (const u_char **)&pkt_data);
		
	}
	
	
	
	return;
}

//打印tcp时间戳和数据字段长度（区分不同的tcp目的端口）
int get_tcp_info(char *filename)
{
	int reval;   
	struct pcap_pkthdr* header;
	u_char *pkt_data;
	pcap_t *pcap_handle;
	char error_content[PCAP_ERRBUF_SIZE];
	
	char tempname[256];
	char dst_port_info[32];
	
	FILE *fd;
	
	int i=0;
	
	//循环输出多个端口tcp 数据信息到不同的文件中
	for (i=0; i<port_count; i++)
	{
		
		dst_port_info[0]='\0';
		tempname[0] = '\0';
	
		sprintf(dst_port_info, "_port%d_window.txt",i);
		strcpy(tempname,filename);
		strcat(tempname,dst_port_info);
	
		printf ("port%d 端口号: %u, 输出文件：%s\n",i, tcp_dst_port_table[i],tempname);
	
		
	
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
	
		inet_aton(src_ip,(struct in_addr*)&src);
		inet_aton(dst_ip,(struct in_addr*)&dst);
	
		
		unsigned short tcp_dst_port;
		
		unsigned short tcp_src_port;
		
		unsigned int seq, ack;
	
		unsigned int current_ack = 1;
	
		unsigned int first_ack_flag = 0;
		unsigned int first_seq_flag = 0;
		
		while(pkt_data!=NULL && reval > 0)
		{
			
			//下行数据包,服务器到本地，从服务器上下载，提取seq的值
			if(*(pkt_data+12)==0x08 && *(pkt_data+13)==0x00 && *(unsigned int*)(pkt_data+26)==src && *(unsigned int*)(pkt_data+30)==dst) //MAC type==IP ip.src==client, ip.dst=server
			{
				
				
				if(*(pkt_data+23)==0x06 && first_ack_flag!=0) // TCP 6
				{	
					tcp_dst_port = *(unsigned short*)(pkt_data+36);		
					tcp_dst_port = ntohs(tcp_dst_port);
					
					seq = *(unsigned int*)(pkt_data+38);
					seq = ntohl(seq);
					
					
					if(tcp_dst_port == tcp_dst_port_table[i])
					{
						
						if (first_seq_flag==0)
						{	
							first_seq_flag = seq;
							printf("seq: %u\n", first_seq_flag);
						}
						fprintf(fd,"%ld.%ld\t",header->ts.tv_sec,header->ts.tv_usec);
						fprintf(fd,"%u\n",seq - current_ack);
						
						
						
					}
				}
			}		
			
			//上行，本地到服务器，提取ack值
			else if (*(pkt_data+12)==0x08 && *(pkt_data+13)==0x00 && *(unsigned int*)(pkt_data+26)==dst && *(unsigned int*)(pkt_data+30)==src)
			{
				
				
				
				if(*(pkt_data+23)==0x06) // TCP 6
				{	
					
					
					tcp_src_port = *(unsigned short*)(pkt_data+34);		
					tcp_src_port = ntohs(tcp_src_port);
					
					ack = *(unsigned int*)(pkt_data+42);
					ack = ntohl(ack);
					
				
					
					if(tcp_src_port == tcp_dst_port_table[i])	
					{
						
						if (first_ack_flag==0)
						{
							first_ack_flag = ack;
							printf("ack:%u\n", first_ack_flag);
						}
						
						//fprintf(fd,"%ld.%ld\t",header->ts.tv_sec,header->ts.tv_usec);
						//fprintf(fd,"%d\n",header->len-*(pkt_data+46)/4-34);
						current_ack = ack;

						
					}
				}
				

				
			}
			reval = pcap_next_ex(pcap_handle, &header, (const u_char **)&pkt_data);	
		}
		fclose(fd);
	
	}
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
			/*
			//判断是否为指定类型的.cap file
			if (pDirEntry->d_name[strlen(pDirEntry->d_name)-4]=='.'
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-3]=='c'
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-2]=='a' 
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-1]=='p')
			*/
			
			//.pcap files
			if (pDirEntry->d_name[strlen(pDirEntry->d_name)-5]=='.'
    			&&pDirEntry->d_name[strlen(pDirEntry->d_name)-4]=='p'
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-3]=='c'
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-2]=='a' 
				&& pDirEntry->d_name[strlen(pDirEntry->d_name)-1]=='p')
				
			{
                printf("输入文件：%s\n",pDirEntry->d_name);
				
				load_table(pDirEntry->d_name);
				get_tcp_info(pDirEntry->d_name);
				//display_table();
				table_init();
				printf("------------------------------------\n");
			}
		}
		closedir(pDir);

	}       
	
	
	
	return 0;
}
