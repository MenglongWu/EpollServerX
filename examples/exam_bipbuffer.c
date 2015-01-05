/**
 ******************************************************************************
 * @file	exam_test.c
 * @brief	Test bipbuffer.c
 ******************************************************************************
*/


#include "stdio.h"
#include "string.h"
#include "bipbuffer.h"

#ifdef WIN32
#else
#include <sys/time.h> 
#include <stdio.h> 
#include <math.h>

#define MEM_LEN 64

struct timeval tpstart,tpend; 
float timeuse; 
#endif

void ResetConst()
{
#ifdef WIN32
#else
	gettimeofday(&tpstart,NULL); 
#endif
}
void PrintCost()
{
#ifdef WIN32
#else
	gettimeofday(&tpend,NULL); 
	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+ 
	tpend.tv_usec-tpstart.tv_usec; 
	timeuse/=1000000; 
	printf("Used Time:%f\n",timeuse);
#endif
}

struct framehelloword
{
	char head[11];
	char data[MEM_LEN-11-11];
	char tail[11];
};

void PrintFrame(struct framehelloword *pframe)
{
	pframe->head[10] = '\0';
	printf("head:\"%s\"\n", pframe->head);
	pframe->data[12] = '\0';
	printf("data:\"%s\"\n", pframe->data);
}

void Test()
{
	char *pdata;
	int reserved;
	struct bipbuffer bipb;
	char readdata[MEM_LEN] = {0};
	struct framehelloword *pframe;

	// 初始化内存，并至少生气2倍最大长度内存，实际应用
	// 中应尽量申请大的内存，保证环形缓冲不被填满，保证
	// 读取速度大于写入速度
	printf("\n\n###################################################################\n");
	printf("Frame at 2 area\n");
	bipbuffer_Init(&bipb);
	bipbuffer_AllocateBuffer(&bipb,MEM_LEN*2);
	
	// 连续写入
	printf("commint 0x01 32 byte,write 0x02 32 byte\n");
	if (pdata = bipbuffer_Reserve(&bipb,32,&reserved)) {	
		memset(pdata, 0x01, 32);
		bipbuffer_Commit(&bipb,reserved);	
	}
	if (pdata = bipbuffer_Reserve(&bipb,32,&reserved)) {
		memset(pdata, 0x02, 32);
		bipbuffer_Commit(&bipb,reserved);
	}
	



	
	// 写入Frame Head [Hello Word] Frame Tail
	printf("commint \"Frame Hea\" 9 byte,uninclude \"\\0\"\n");
	if (pdata = bipbuffer_Reserve(&bipb,MEM_LEN,&reserved)) {
		memcpy(pdata, "Frame Hea", 9);
		bipbuffer_Commit(&bipb,9);	
	}

	// 释放前64字节
	printf("decommint 64 byte,but not set zero for free\n");
	bipbuffer_DecommitBlock(&bipb,64);

	// 现在A前面的空间大于后面的空间，如果申请一个大于A后面空间大小的内存
	// 将在A前面创建B
	printf("commint \"d [Hello Word] Frame Tail\" 26 byte,uninclude \"\\0\"\n");
	if (pdata = bipbuffer_Reserve(&bipb,MEM_LEN,&reserved)) {
		memcpy(pdata, "d [Hello Word] Frame Tail", 26);
		bipbuffer_Commit(&bipb,26);	
	}
	printf("\n|<          area B              >|<area A>|<  free  >|\n");
	printf(  "d [Hello Word] Frame Tail########Frame Hea############\n");
	bipbuffer_PrintMemory(&bipb);
	

	
	char *phead;
	char *ptail;
	int framelen;
	phead    = bipbuffer_DataData(&bipb,"Frame Head", 10);
	ptail    = bipbuffer_DataData(&bipb,"Frame Tail", 10);
	framelen = bipbuffer_FrameLen(&bipb, phead, ptail);
	
	// 拷贝在Frame Head和Frame Tail之间的内容
	printf("frame head addr 0x%8.8x data:\"%s\"\n", phead, phead);
	printf("frame tail addr 0x%8.8x data:\"%s\"\n", ptail, ptail);
	printf("Frame len:%d \n", framelen);
	if (phead && ptail) {
		if (phead > ptail) {
			bipbuffer_Copy2User(&bipb, 0, readdata, framelen);
			printf("copy to user:\"%s\"\n",readdata);
			pframe = (struct framehelloword*)readdata;
		}
		else {
			printf("uncopy:\"%s\"\n",phead);
			pframe = (struct framehelloword*)phead;
		}
		PrintFrame(pframe);	
	}
	


	

	printf("Decommit some space from \"Frame Head\" to \"Frame Tail\"\n");
	// bipbuffer_DecommitTo(&bipb, (unsigned int)ptail);
	bipbuffer_DecommitTo(&bipb, ptail);
	pdata = bipbuffer_GetContiguousBlock(&bipb, &reserved);
	printf("after Decommit:\"%s\"\n",pdata);
	printf("Clear all\n");
	bipbuffer_Clear(&bipb);


	printf("\n\n###################################################################\n");
	printf("Frame at 1 area\n");
	if (pdata = bipbuffer_Reserve(&bipb, 35, &reserved)) {
		memcpy(pdata, "Frame Head [Hello Word] Frame Tail", 35);
		bipbuffer_Commit(&bipb,35);	
	}
	printf("\n|<          area A              >|<       free      >|\n");
	printf(  "Frame Head [Hello Word] Frame Tail####################\n");
	bipbuffer_PrintMemory(&bipb);
	// bipbuffer_PrintMemory(&bipb);
	phead    = bipbuffer_DataData(&bipb,"Frame Head", 10);
	ptail    = bipbuffer_DataData(&bipb,"Frame Tail", 10);
	framelen = bipbuffer_FrameLen(&bipb, phead, ptail);
	
	// 拷贝在Frame Head和Frame Tail之间的内容
	printf("frame head addr 0x%8.8x data:\"%s\"\n", phead, phead);
	printf("frame tail addr 0x%8.8x data:\"%s\"\n", ptail, ptail);
	printf("Frame len:%d \n", framelen);
	if (phead && ptail) {
		if (phead > ptail) {
			bipbuffer_Copy2User(&bipb, 0, readdata, framelen);
			printf("copy to user:\"%s\"\n",readdata);
			pframe = (struct framehelloword*)readdata;
		}
		else {
			printf("uncopy:\"%s\"\n",phead);
			pframe = (struct framehelloword*)phead;
		}
		PrintFrame(pframe);
	}
}


int main()
{

	Test();
	while(1)
		sleep(1);
	return 0;
	
}