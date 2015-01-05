/**
 ******************************************************************************
 * @file	randmac.c
 * @brief	
 *		自动生成mac地址

 	自动生成mac地址，并将其写入启动项目
 *
*/


#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include <string.h>

unsigned int RAND_SEED = 0;

// 用内存和系统时间生成随机数
unsigned int makeseed()
{
	int c[1024];
	int *p = c;
	static int d1,d2,d3,d4,index = 0;
	for (int i = 0;i < sizeof(c) / sizeof(int); i++) {
		// printf("i = %d\n",i);
		RAND_SEED += *p++;

	}
	
	// printf("rand %8.8x %x %x %x %x\n",RAND_SEED, d1, d2, d3, d4);
	RAND_SEED = RAND_SEED ^ (RAND_SEED >> 8) + 
				RAND_SEED ^ (RAND_SEED >> 16) + 
				RAND_SEED ^ (RAND_SEED >> 24) + 
				RAND_SEED ^ (RAND_SEED >> 4) +
				RAND_SEED ^ (RAND_SEED >> 12) + 
				RAND_SEED ^ (RAND_SEED >> 20);
	
    time_t now;

	struct tm *timenow;
	time(&now);
	timenow = localtime(&now);


	RAND_SEED += timenow->tm_mon + timenow->tm_mday +
				timenow->tm_hour + timenow->tm_min + timenow->tm_sec;

	srandom(RAND_SEED);
	return (RAND_SEED);
}

int getdate(int *year,int *mon,int *day)
{
	time_t now;

	struct tm *timenow;
	time(&now);
	timenow = localtime(&now);


	RAND_SEED += timenow->tm_mon + timenow->tm_mday +
				timenow->tm_hour + timenow->tm_min + timenow->tm_sec;

	
	*year = 1900+timenow->tm_year;
	*mon  = timenow->tm_mon + 1;
	*day  = timenow->tm_mday;
}

int main(int argc,char **argv)
{
	char path[256] = "./randmac.sh";
	unsigned char mac_addr[6];
	FILE *fd;

	// 判断参数有效性
	if (argc == 2) {
		strcpy(path,argv[1]);
	}
	else {
		printf("Usage:\n");
		printf("\trandmac <file>: output file\n");
		printf("\tdefault file randmac.sh\n");
	}
	
	// 生成脚本
	fd = fopen(path,"w");
	if (fd == 0) {
		printf("path invaild!!!\n");
		return -1;
	}
	makeseed();
	for (int i = 0; i < 6; i++) {
		mac_addr[i] = rand();
	}
	mac_addr[0] &= 0xfe;
	int y,m,d;
	getdate(&y,&m,&d);
	printf("%d %d %d\n",y,m,d);

	mac_addr[1] = y - 2000;
	mac_addr[2] = m;
	mac_addr[3] = d;
	fprintf(fd,"ifconfig eth1 down\n");
	fprintf(fd,"ifconfig eth1 hw ether ");
	fprintf(fd,"%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
		mac_addr[0],mac_addr[1],mac_addr[2],
		mac_addr[3],mac_addr[4],mac_addr[5]);
	fprintf(fd,"ifconfig eth1 up\n");
	
	fclose(fd);
	return 0;
}