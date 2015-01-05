#include "tr485.h"
#include <stdio.h>

struct struct_list
{
	char name[52];
	int len;
	int fact_len;
};

struct pro_list
{
	char name[52];
	int len;
};


// 移植到其他平台，首先检查数据结构长度是否正确
int TestStruct()
{
	printf("**********************************************\n");
	struct struct_list list[] = {
		{"struct tr485_frame", TR485_FRAME_H, sizeof(struct tr485_frame)},
		{"struct tr485_alloc", TR485_ALLOC_H, sizeof(struct tr485_alloc)},
		{"struct tr485_rand",  TR485_RAND_H,  sizeof(struct tr485_rand)},
		{"struct tr485_dev",   TR485_DEV_H,   sizeof(struct tr485_dev)},

		{"struct tr485_base",  TR485_BASE_H,  sizeof(struct tr485_base)},
		{"struct tr485_data",  TR485_DATA_H,  sizeof(struct tr485_data)},
	};
	int arrlen = sizeof(list) / sizeof(struct struct_list);
	bool success = true;

	printf("\nChecking platform\n");
	for (int i = 0; i < arrlen; i++) {
		printf("check \"%s\"\t len must be   %2d fact   %2d \t", list[i].name, list[i].len, list[i].fact_len);
		if (list[i].len != list[i].fact_len) {
			printf("[NO!!!]\n");
			success = false;
		}
		else {
			printf("[OK]\n");
		}
	}


	if (success == true) {
		printf("Success!!!\n");
		return 0;
	}
	else {
		printf("Fail!!!\n");	
		return 1;
	}

}

int TestDataFill()
{
	// printf("**********************************************\n");
	return 0;
}


// 打印内存内容
void PrintfMemory(uint8_t *buf,uint32_t len)
{
	for (int i = 0; i < len; i++) {
		if (i & 0xf) {
			printf(" ");
		}
		else {
			printf("\n");
		}
		printf("%2.2x",(unsigned char)*(buf+i) );
	}
	printf("\n");
}


// 测试构造协议包
void TestBuildPacket()
{
	printf("**********************************************\n");
	uint8_t buf[30000] = {0};
	uint8_t data[16] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10};
	struct ptag_t t;

	printf("\nbuild packet\n");
	// memset(buf, 0, 256);
	tr485_Init();
	// tr485_BuildToken(buf, 0x21, NULL);
	tr485_BuildRetransfer(buf, 0x21, NULL);
	// PrintfMemory(buf, 256);

	t.data = data;
	t.len = 16;
	tr485_BuildData(buf, 0xff, TR485_CMD_DATAPART, &t);
	PrintfMemory(buf, 256);


	printf("\nsend packet\n");
	int len = tr485_Send(0, data, TR485_MAX_DATA*3+2);
	// printf("len = %d\n",len);
}

// 显示协议宏长度
int ShowProtocolInfo()
{
	printf("**********************************************\n");
	struct pro_list list[] = {
		{"TR485_NODE_COUNT ",TR485_NODE_COUNT},
		{"TR485_MTU        ",TR485_MTU},
		{"TR485_CACHE_COUNT",TR485_CACHE_COUNT},
		{"TR485_HEAD_LEN   ",TR485_HEAD_LEN},
		{"TR485_TAIL_LEN   ",TR485_TAIL_LEN},
		{"TR485_BASE_H     ",TR485_BASE_H},
		{"TR485_MAX_DATA   ",TR485_MAX_DATA},
		{"TR485_MAX_NAME   ",TR485_MAX_NAME},
	};

	int arrlen = sizeof(list) / sizeof(struct pro_list);
	for (int i = 0; i < arrlen; i++) {
		printf("%0.30s \t:%d\n", list[i].name, list[i].len);
	}
	

}

#include <stdlib.h>
unsigned int RAND_SEED = 0;
unsigned int testrand()
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
	srandom(RAND_SEED);
	return (RAND_SEED);
}

// 检查平台是否能用内存堆栈产生随机数，保存文件seed.txt
void TestSeed()
{
	printf("**********************************************\n");
	// testrand();
	unsigned int  r1,r2;
	FILE *fd;

	printf("Waiting create seed.txt\n");
	fd = fopen("seed.txt","w");
	fseek(fd,SEEK_SET,0);

	for (int i = 0; i < 1000000; i++) {
		r1 = tr485_Seed(0);
		fprintf(fd,"%8.8x\n",r1);
	}
	fclose(fd);
	printf("Please open seed.txt and Checking\n");
	sleep(3);
}
unsigned int myrandom(void)
{
	static char k = 59;
	unsigned char r1,r2;
	// 生成256~65536范围随机数，但是其中很大一部分数值永远得不到
	RAND_SEED = (RAND_SEED * 15323 + k) % 65536;
	// 修改常量，随机数能成保证原永远得不到的值也能得到
	k += 3;

	r1 = RAND_SEED % 256;

	RAND_SEED = (RAND_SEED * 15323 + 123) % 65536;
	// 修改常量，随机数能成保证原永远得不到的值也能得到

	r2 = RAND_SEED % 256;
	return r2 + (r1 << 8) ;
}



// 测试TR485.c移植是否正确
int main()
{
	
	if (TestStruct()) {
		return 1;
	}
	// testrand();	
	ShowProtocolInfo();

	TestSeed();
	TestBuildPacket();
}

