/**
 ******************************************************************************
 * @file	bipbuffer.c
 * @brief	The Circular Buffer with a Twist

reference:http://www.codeproject.com/Articles/3479/The-Bip-Buffer-The-Circular-Buffer-with-a-Twist
 *

- 2003,Simon Cooke
 	- raw C++ code BipBuffer.hpp

- 2015-3-1,Menglong Wu,DreagonWoo@163.com
 	- Change to c code
 	- Add find a frame in buffer,bipbuffer_DataData()
 	- Add bipbuffer_FrameLen()
- 2015-4-13,Menglong Wu,DreagonWoo@163.com
 	- Fix bipbuffer_Reserve when return NULL,reserved is't zero
 	- Add bipbuffer_GetUnContiguousBlock
 	- Add bipbuffer_GetUsedSize
*/

#include "bipbuffer.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

static char *datacmp(char *buf,unsigned int buflen, char *sub, unsigned int len);
static char *datacmppart(char *buf, int buflen, char *sub, int len,int *part);
static int   datamemcmp(char *s1, char *s2, int len);

#define GetSpaceAfterA(bb) (bb->buflen - bb->ixa - bb->sza)
#define GetBFreeSpace(bb) (bb->ixa - bb->ixb - bb->szb)
#define OUT


/**
 * @brief	初始化struct bipbuffer，将其清零
 */

void bipbuffer_Init(struct bipbuffer *bb)
{
	memset(bb, 0, sizeof(struct bipbuffer));
}


/**
 * @brief	分配内存
 * @param	bb struct bipbuffer指针
 * @retval	buffersize 缓存大小
 * @remarks	建议分配2倍所需内存，申请空间越大越能保证在使用过程中缓存不会填满，提高效率
 * @see	bipbuffer_FreeBuffer
 */

bool bipbuffer_AllocateBuffer(struct bipbuffer *bb, int buffersize)
{
	if (buffersize <= 0) return false;

	if (bb->pBuffer != NULL) bipbuffer_FreeBuffer(bb);

	bb->pBuffer = (char*)malloc(buffersize);
	if (bb->pBuffer == NULL) return false;

	bb->buflen = buffersize;
	memset(bb->pBuffer, 0, buffersize);
	return true;
}



/**
 * @brief	清楚所有缓存
 * @param	bb struct bipbuffer指针
 * @remarks	只是设置struct bipbuffer结构内部参数为0，并不将缓存清零
 * @see	
 */

void bipbuffer_Clear(struct bipbuffer *bb)
{
	bb->ixa = bb->sza = bb->ixb = bb->szb = bb->ixResrv = bb->szResrv = 0;
}



/**
 * @brief	释放所申请的内存
 * @param	bb struct bipbuffer指针
 * @remarks	
 * @see	bipbuffer_AllocateBuffer
 */

void bipbuffer_FreeBuffer(struct bipbuffer *bb)
{
	if (bb->pBuffer == NULL) return;

	bb->ixa = bb->sza = bb->ixb = bb->szb = bb->buflen = 0;

	free(bb->pBuffer);

	bb->pBuffer = NULL;
}


/**
 * @brief	预从缓存里申请多少内存用于写入
 * @param	bb struct bipbuffer指针
 * @param	reserved 需要申请的内存，返回实际申请到的内存大小
 * @retval	返回内存地址
 * @remarks	bipbuffer_Reserve和bipbuffer_Commit必须成对出现，调用bipbuffer_Commit
 后才真正记录到缓存中。如果当前环形缓存的内存形式，前段部分不空闲大于后段部分，如下\n
 |--------dddd---|\n
 而输入的reserved太大，导致实际返回的指针不是dddd后面，而是环形缓存首地址，于是创建了B区域。
 为避免少创建B区域，保证数据连续，建议reserved为bipbuffer_AllocateBuffer的1/4~1/16
 \n
 详细：http://www.codeproject.com/Articles/3479/The-Bip-Buffer-The-Circular-Buffer-with-a-Twist
 * @see	bipbuffer_Commit
 */

char* bipbuffer_Reserve(struct bipbuffer *bb, int size, OUT int	*reserved)
{
	// We always allocate on B if B exists; this means we have two blocks and our buffer is filling.
	if (bb->szb)
	{
		int freespace = GetBFreeSpace(bb);

		if (size < freespace) freespace = size;

		if (freespace == 0) {
			*reserved = 0;
			return NULL;
		}

		bb->szResrv = freespace;
		*reserved = freespace;
		bb->ixResrv = bb->ixb + bb->szb;
		return bb->pBuffer + bb->ixResrv;
	}
	else
	{
		// Block b does not exist, so we can check if the space AFTER a is bigger than the space
		// before A, and allocate the bigger one.

		int freespace = GetSpaceAfterA(bb);
		if (freespace >= bb->ixa)
		{
			if (freespace == 0) return NULL;
			if (size < freespace) freespace = size;

			bb->szResrv = freespace;
			*reserved = freespace;
			bb->ixResrv = bb->ixa + bb->sza;
			return bb->pBuffer + bb->ixResrv;
		}
		else
		{
			if (bb->ixa == 0) return NULL;
			if (bb->ixa < size) size = bb->ixa;
			bb->szResrv = size;
			*reserved = size;
			bb->ixResrv = 0;
			return bb->pBuffer;
		}
	}
}


/**
 * @brief	提交缓存大小
 * @param	bb struct bipbuffer指针
 * @param	size 提交缓存代销
 * @remarks	bipbuffer_Reserve和bipbuffer_Commit必须成对出现\n
 详细：http://www.codeproject.com/Articles/3479/The-Bip-Buffer-The-Circular-Buffer-with-a-Twist
 * @see	bipbuffer_Commit
 */
void bipbuffer_Commit(struct bipbuffer *bb, int size)
{
	// 强制转换防止size为负数溢出
	unsigned int tsize = (unsigned int)size;

	if (size < 0) {
		return;
	}
	if (tsize == 0)
	{
		
		// decommit any reservation
		bb->szResrv = bb->ixResrv = 0;
		return;
	}

	// If we try to commit more space than we asked for, clip to the size we asked for.

	if (tsize > (unsigned int)bb->szResrv)
	{
		
		tsize = bb->szResrv;
	}

	// If we have no blocks being used currently, we create one in A.

	if (bb->sza == 0 && bb->szb == 0)
	{
		
		bb->ixa = bb->ixResrv;
		bb->sza = tsize;

		bb->ixResrv = 0;
		bb->szResrv = 0;
		return;
	}


	if (bb->ixResrv == bb->sza + bb->ixa)
	{
		
		bb->sza += tsize;
	}
	else
	{
		
		bb->szb += tsize;
	}

	bb->ixResrv = 0;
	bb->szResrv = 0;
}



/**
 * @brief	获取一个连续块长度
 * @param	bb struct bipbuffer指针
 * @param	size 块长度
 * @retval	块指针
 * @remarks	当buffer只有A区域是size为环形缓存实际接收长度，如果存在B区域则值表示A区域的长度
 * @see	
 */

char* bipbuffer_GetContiguousBlock(struct bipbuffer *bb,OUT int *size)
{
	if (bb->sza == 0)
	{
		*size = 0;
		return NULL;
	}

	*size = bb->sza;
	return bb->pBuffer + bb->ixa;	
}

char *bipbuffer_GetUnContiguousBlock(struct bipbuffer *bb,OUT int *size)
{
	printf("bb->sza %d bb->szb %d\n",bb->sza,bb->szb);
	if (bb->sza == 0)
	{
		*size = 0;
		return NULL;
	}

	if (bb->szb == 0) {
		*size = bb->sza;
		return bb->pBuffer + bb->ixa;	
	}
	else {
		char *ptmp;
		printf("copy........\n");
		ptmp = (char*)malloc(bb->buflen);
		if (ptmp == NULL) return NULL;
		memcpy(ptmp,           bb->pBuffer + bb->ixa,    bb->sza);
		memcpy(ptmp + bb->sza, bb->pBuffer + bb->ixb,    bb->szb);

		memcpy(bb->pBuffer, ptmp, bb->sza + bb->szb);

		bb->sza = bb->sza + bb->szb;
		bb->ixa = 0;
		bb->ixb = 0;
		bb->szb = 0;
		bipbuffer_PrintMemory(bb);
		free(ptmp);
		return bb->pBuffer;
	}
}
/**
 * @brief	回收缓存
 * @param	bb struct bipbuffer指针
 * @param	size 回收大小
 * @retval	null
 * @remarks	如果size大于A区域大小，清除A区域所有缓存，B区域不变
 * @see	bipbuffer_DecommitMulti bipbuffer_DecommitTo
 */

void bipbuffer_DecommitBlock(struct bipbuffer *bb,int size)
{
	unsigned int tsize = (unsigned int)size;
	if (tsize >= (unsigned int)bb->sza)
	{
		bb->ixa = bb->ixb;
		bb->sza = bb->szb;
		bb->ixb = 0;
		bb->szb = 0;
	}
	else
	{
		bb->sza -= tsize;
		bb->ixa += tsize;
	}
}



/**
 * @brief	回收缓存，属于bipbuffer_DecommitBlock的扩展
 * @param	bb struct bipbuffer指针
 * @param	size 回收大小
 * @retval	null
 * @remarks	如果size大于A区域大小，清除A区域所有缓存，多余的部分清除B缓存
 * @see	bipbuffer_DecommitBlock bipbuffer_DecommitTo
 */

void bipbuffer_DecommitMulti(struct bipbuffer *bb,int size)
{
	int b = size - bb->sza;

	bipbuffer_DecommitBlock(bb,size);
	if (b > 0) {
		bipbuffer_DecommitBlock(bb,b);	
	}

}


/**
 * @brief	回收缓存，属于bipbuffer_DecommitBlock的扩展
 * @param	bb struct bipbuffer指针
 * @param	ptrto 一个合法指针，该指针必须在A区域到B区域之间
 * @retval	null
 * @remarks	警告ptrto必须是bipbuffer.pBuffer指针所只内存区域范围，并且在A区域和B
 区域之间，释放的内存长度有负数的可能，这有可能导致程序不可预料的崩溃，为取得一个
 合法的指针，可以通过调用bipbuffer_DataData等bipbuffer内部函数
 * @see	bipbuffer_DecommitBlock bipbuffer_DecommitMulti
 */
void bipbuffer_DecommitTo(struct bipbuffer *bb,void *ptrto)
{
	int size;

	size = bipbuffer_FrameLen(bb, bb->pBuffer+bb->ixa, ptrto);
	bipbuffer_DecommitMulti(bb, size);
}

/**
 * @brief	判断两个指针是否在缓存中连续
 * @param	bb struct bipbuffer指针
 * @param	pfrom 环形缓存内有效地址
 * @param	pto 环形缓存内有效地址
 * @retval	true 连续
 * @retval	false 不连续
 * @remarks	应用层保证地址pfrom > pto
 * @see	
 */

bool  bipbuffer_IsSequence(struct bipbuffer *bb, void *pfrom, void *pto)
{
	// pfrom pto 在A区域
	if 		((unsigned int)bb->pBuffer + bb->ixa <= (unsigned int)pfrom && 
			 (unsigned int)bb->pBuffer + bb->ixa + bb->sza >= (unsigned int)pto) {
		return true;
	}
	// pfrom pto 在B区域
	else if ((unsigned int)bb->pBuffer + bb->ixb <= (unsigned int)pfrom && 
			 (unsigned int)bb->pBuffer + bb->ixb + bb->szb >= (unsigned int)pto) {
		return true;
	}
	else {
		return false;
	}
}

bool  bipbuffer_IsSequenceAll(struct bipbuffer *bb)
{
	if (bb->szb == 0) {
		return true;
	}
	else {
		return false;
	}
}


//offset 只能为正数
char *bipbuffer_Offset(struct bipbuffer *bb, void *ref, unsigned int offset)
{
	struct bipbuffer tbb = *bb;

	bipbuffer_DecommitTo(&tbb, ref);
	if ((unsigned int)tbb.sza >= offset) {
		return tbb.pBuffer + tbb.ixa + offset;
	}
	else {
		return NULL;
	}
}
/**
 * @brief	从缓冲里拷贝数据
 * @param	bb struct bipbuffer指针
 * @param	puser 拷贝目的地址
 * @param	size 预备拷贝的长度
 * @retval	实际拷贝的长度
 * @remarks	通常bipbuffer_Copy2User用于处理一段连续的内容存储在A区域和B区域之间
 为使得内存不连续，于是将其拷贝出来。实际应用最求效率应尽量避免拷贝，尽量保证只
 存在A区域
 * @see	
 */

void bipbuffer_Copy2User(struct bipbuffer *bb, int offset, char *puser, int size)
{
	struct bipbuffer tbb = *bb;
	//unsigned char *pbuf;
	// int total;
	// total = bb->sza + bb->szb;
	bipbuffer_DecommitBlock(&tbb,offset);
	if (size > bb->sza + bb->szb) {
		size = bb->sza + bb->szb;
	}
	
	if (tbb.sza >= size) {
		memcpy(puser,           tbb.pBuffer + tbb.ixa,    size);
	}
	else {
		memcpy(puser,           tbb.pBuffer + tbb.ixa,    tbb.sza);
		memcpy(puser + tbb.sza, tbb.pBuffer + tbb.ixb,    size - tbb.sza);
	}

}


int bipbuffer_GetCommittedSize(struct bipbuffer *bb) 
{
	return bb->sza + bb->szb;
}
int bipbuffer_GetReservationSize(struct bipbuffer *bb) 
{
	return bb->szResrv;
}
int bipbuffer_GetUsedSize(struct bipbuffer *bb)
{
	return bb->sza + bb->szb;	
}
int bipbuffer_GetBufferSize(struct bipbuffer *bb) 
{
	return bb->buflen;
}
bool bipbuffer_IsInitialized(struct bipbuffer *bb) 
{
	return bb->pBuffer != NULL;
}


static int   datamemcmp(char *s1, char *s2, int len)
{
    for (int i = 0;i < len; i++) {
        if (*s1++ != *s2++) {
            return 1;
        }
    }
    return 0;
}


static char *datacmp(char *buf,unsigned int buflen, char *sub, unsigned int len)
{
    register char *pbuf;
    register unsigned int tbuflen;
    
    tbuflen = buflen;
    if (tbuflen >= len) {
        tbuflen = tbuflen - len+1;
    }
    else {
        return NULL;
    }

    pbuf = buf;
    while (tbuflen--) {
        if (datamemcmp(pbuf,sub,len) == 0) {
            return pbuf;
        }            
        pbuf++;
    }  
    return NULL;
}

char *datacmppart(char *buf, int buflen, char *sub, int len,int *part)
{
    register int tlen;
    register char *pbuf;
    register char *psub;
    register char *tbuf;

    pbuf = buf;
    while (buflen--) {
        psub = sub;
        tbuf = pbuf;
        tlen = len;
		tlen--;
        *part = 0;
        for (;*tbuf++ == *psub++; --tlen) {
            (*part)++;
             if (tlen == 0) {
                 return tbuf - len;
             }

        }
		if (buflen <= *part) {
			return pbuf;
		}
        pbuf++;

    }  
    return NULL;
}



/**
 * @brief	在缓存里搜索与sub相同的内容，返回首次匹配的内存地址
 * @param	bb struct bipbuffer指针
 * @param	sub 需要搜索内存的内容
 * @param	len sub内存长度
 * @retval	如果找到则返回非0值，否则返回NULL
 * @remarks	
 * @see	bipbuffer_FrameLen
 */

char *bipbuffer_DataData(struct bipbuffer *bb, char *sub,int len)
{
    int  partL,partR;
    char *pdataA;
    char *pdataB;
    int sizeL,sizeR;

    // Only A
    if ((pdataA = datacmp(bb->pBuffer+bb->ixa, bb->sza, sub, len))) {
		return pdataA;
	}	
    if (bb->szb != 0 ) {
        // A and B
        // 在区域A里找sub左半部分数据
        sizeL  = bb->sza > (len - 1) ? (len - 1) : bb->sza;
        pdataA = datacmppart(
                    bb->pBuffer + bb->ixa + bb->sza - sizeL,sizeL,
                    sub, sizeL,
                    &partL);
        // 在区域A里找sub右半部分数据
        if(partL) {
            sizeR  = bb->szb > (len - partL) ? (len - partL) : bb->szb;
            pdataB = datacmppart(
                        bb->pBuffer + bb->ixb, sizeR,
                        sub + partL, len - partL,
                        &partR);
            if(partL + partR == len && pdataB == bb->pBuffer+bb->ixb) {
                return pdataA;
            }    
        }
        // Only B
        return datacmp(bb->pBuffer+bb->ixb,bb->szb,sub,len);
    }
    return NULL;
}


char *bipbuffer_DataDataOffset(struct bipbuffer *bb, void *pstart, char *sub,int len)
{
    int  partL,partR;
    char *pdataA;
    char *pdataB;
    int sizeL,sizeR;
	struct bipbuffer tbb = *bb;
	
	bipbuffer_DecommitTo(&tbb, pstart);

    // Only A
    if ((pdataA = datacmp(tbb.pBuffer+tbb.ixa, tbb.sza, sub, len))) {
		return pdataA;
	}	
    if (tbb.szb != 0 ) {
        // A and B
        // 在区域A里找sub左半部分数据
        sizeL  = tbb.sza > (len - 1) ? (len - 1) : tbb.sza;
        pdataA = datacmppart(
                    tbb.pBuffer + tbb.ixa + tbb.sza - sizeL,sizeL,
                    sub, sizeL,
                    &partL);
        // 在区域A里找sub右半部分数据
        if(partL) {
            sizeR  = tbb.szb > (len - partL) ? (len - partL) : tbb.szb;
            pdataB = datacmppart(
                        tbb.pBuffer + tbb.ixb, sizeR,
                        sub + partL, len - partL,
                        &partR);
            if(partL + partR == len && pdataB == tbb.pBuffer+tbb.ixb) {
                return pdataA;
            }    
        }
        // Only B
        return datacmp(tbb.pBuffer+tbb.ixb,tbb.szb,sub,len);
    }
    return NULL;
}



// unsigned int bipbuffer_FrameLen(struct bipbuffer *bb, unsigned int saddr, unsigned int eaddr)
/**
 * @brief	获取某帧长度，用于具体协议
 * @param	bb struct bipbuffer指针
 * @param	saddr 帧起始地址
 * @param	eaddr 帧末尾地址
 * @retval	该帧的长度
 * @remarks	saddr与eaddr必须是bipbuffer的内存地址，如果某帧为\n
 |AA########BB-----|\n
 其中saddr为AA的地址，eaddr为BB的地址，这一点需要应用层处理，建议调用bipbuffer_DataData
 寻找AA和BB
 * @see	bipbuffer_DataData
 */

unsigned int bipbuffer_FrameLen(struct bipbuffer *bb, void *saddr, void *eaddr)
{
    if (saddr == NULL || eaddr == NULL) {
        return 0;
    }
    // Only in A or B
    if (saddr < eaddr) {
        return (unsigned int)eaddr - (unsigned int)saddr;
    }
    // in A and B
    // return  (eaddr - ((unsigned int)bb->pBuffer + bb->ixb)) + 
    // 		(((unsigned int)bb->pBuffer + bb->ixa+bb->sza) - saddr);
    // return (unsigned int)((eaddr - (bb->pBuffer + bb->ixb)) + 
    // 		(bb->pBuffer + bb->ixa + bb->sza - (unsigned int*)saddr));
    return (unsigned int)eaddr - (unsigned int)(bb->pBuffer + bb->ixb) + 
    	(unsigned int)(bb->pBuffer + bb->ixa + bb->sza - (unsigned int)saddr);
}

#ifdef _DEBUG
void bipbuffer_PrintMemory(struct bipbuffer *bb)
{
	printf("Print Memory-----------------------");
	for (int i = 0; i < bb->buflen; i++) {
		if (i & 0xf) {
			printf(" ");
		}
		else {
			printf("\n");
		}
		printf("%2.2x",(unsigned char)*(bb->pBuffer+i) );
	}
	printf("\nixa %d sza %d ixb %d szb %d buflen %d ixResrv %d szResrv %d\n",
		 bb->ixa,
		 bb->sza,
		 bb->ixb,
		 bb->szb,
		 bb->buflen,
		 bb->ixResrv,
		 bb->szResrv);
}
#else
void bipbuffer_PrintMemory(struct bipbuffer *bb)
{

}
#endif

#ifdef __cplusplus
}
#endif

