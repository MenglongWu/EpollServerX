#include "tr485.h"
#include "bipbuffer.h"
#include <string.h>


// 接收缓存
struct bipbuffer sg_bbrecv = {0};

// 发送缓存，485缓冲，用户重发检测
// 连续发送TR485_CACHE_COUNT x TR485_MTU不应超过协议最大传输时间
struct tr485_cache
{
	uint8_t frame[TR485_MTU];
	int32_t state;
};
struct tr485_cache sg_cachesend[TR485_CACHE_COUNT];


struct tr485_inf
{
	int8_t name[TR485_MAX_NAME];
	uint8_t addr;
	uint8_t rand;
};
// 本设备信息
struct tr485_inf sg_myinf;

// 所有网络节点的信息，从机可注释掉该行，节省内存
struct tr485_inf sg_netinf[TR485_NODE_COUNT];





////////////////////////////////////////////////////////////
unsigned int sg_seed = 0;
/**
 * @brief	生成随机数种子，采用随机内存方式生成或者用户输入的seed值
 * @param	seed 一个随机数作为种子
 * @retval	返回随机种子，用于验证随机内存值随机序列是否够长
 * @remarks	内存随机值的生成原理：由于操作系统每次分配的栈地址偏移有关，不同的偏移
 地址可能得到的随机值不同，但这种方法有极强的重复性,当内存长度为10x4字节时，
 Windows环境能生成1600个4Byte随机序列Linux环境能生成260个4Byte随机序列；当内存长度
 为1024x4字节时，Windows和Linux均能生成1000万个随机序列。保证一般使用
 * @remarks	但对于裸机环境，每次上电后栈地址相同，栈的内存地址可能全1可能全0，
 也可能是未知，所以只有内存未知的裸机环境才可调用该函数，通常随机种子是读取当前
 系统时间、鼠标滑动轨迹、用户按键历史等
 */
unsigned int tr485_Seed(unsigned int seed)
{
	// 累加若干次，c数组长度越长可能得到的随机长度越长，但太长编译时会报告堆溢
	int c[1024];
	int *p = c;

	if (seed == 0) {	
		for (int i = 0;i < sizeof(c) / sizeof(int); i++) {
			sg_seed += *p++;
		}
		// 带操作系统的程序sg_seed值的高4位和低8位通常相同，
		// 所以用当中的其余字节与这12位异或，如下异或算法可任意，
		// 只要保证高4位和低8位是随机即可
		// 这种方法生成的随机种子可以保证生成1000以上随机种子
		sg_seed = sg_seed ^ (sg_seed >> 8) + 
					sg_seed ^ (sg_seed >> 16) + 
					sg_seed ^ (sg_seed >> 24) + 
					sg_seed ^ (sg_seed >> 4) +
					sg_seed ^ (sg_seed >> 12) + 
					sg_seed ^ (sg_seed >> 20);
	}
	else {
		sg_seed = seed;
	}
	return (sg_seed);
}
/**
 * @brief	生成随机数
 * @param	null
 * @retval	null
 * @remarks	只是为了减小某些内存只有几百K的处理器使用，math库毕竟有些大，通过宏定
 义可以替换成mach的random函数
 */
unsigned int tr485_Random(void)
{
	static char k = 59;
	unsigned char r1,r2;
	// 生成256~65536范围随机数，但是其中很大一部分数值永远得不到
	sg_seed = (sg_seed * 15323 + k) % 65536;
	// 修改常量，随机数能成保证原永远得不到的值也能得到
	k += 3;

	r1 = sg_seed % 256;

	sg_seed = (sg_seed * 15323 + k) % 65536;
	// 修改常量，随机数能成保证原永远得不到的值也能得到
	k += 3;

	r1 = sg_seed % 256;
	return sg_seed % 256;
}

/**
 * @brief	token ring协议初始化
 */

void tr485_Init()
{
	// sg_myinf.name
	sg_myinf.addr = 0;
	sg_myinf.rand = 0;
	bipbuffer_Init(&sg_bbrecv);
	// bipbuffer_AllocateBuffer(&sg_bbrecv,TR485_MTU*TR485_CACHE_COUNT);
	bipbuffer_AllocateBuffer(&sg_bbrecv,TR485_MTU * 2);
}

/**
 * @brief	校验和，用于二进制取反求和
 * @param	data 待校验内容
 * @param	size 校验内存长度占多少字节
 * @retval	校验和 长度1Byte
 * @remarks	校验前将校验data里的check sum字段清0，校验结束后将校验内容填入check 
 sum，下次在校验该data返回结果为0表示数据无误
 */
uint8_t tr485_Checksum(uint16_t *data, uint16_t size)
{
	register unsigned long chksum = 0;
	while(size) {
		chksum += *data++;
		size--;
	}
	// 加上进位
	chksum  = (chksum>>8) + (chksum&0xff); 
	chksum += (chksum>>8);                 
	return (unsigned char)(~chksum);
}
// bipbuffer 实现
// uint8_t *tr485_strstr( uint8_t *s1, uint8_t *s2,int len);
// bipbuffer 实现
// void tr485_Copy(uint8_t *buf,struct terminal_hdr *hdr);
struct ptag_t *tr485_AutoBuild(uint8_t *buf,/*struct terminal_hdr *hdr,*/struct ptag_t *t)
{
	return NULL;
}
// bipbuffer 实现
// uint8_t *tr485_FindFrame(struct ptag_t *t);

// int32_t tr485_Init(void *p,struct comhandle *com)

// 具体平台实现
// int32_t tr485_Sendframe(struct comhandle *com,uint8_t *buf,int32_t len);
// 具体平台实现
// int32_t tr485_RecvFrame(struct comhandle *com,uint8_t *buf,int32_t len);

/**
 * @brief	构造tr485基本帧
 * @param	buf 构造的帧传入buf
 * @param	token 令牌地址
 * @param	cmd 帧命令
 * @param	t struct ptag_t，借鉴libnet库
 * @retval	查询struct ptag_t的err值
 * @remarks	tr485_BuildToken、tr485_BuildRetransfer两个宏实际上调用tr485_BuildBase
 * @see	
 */

struct ptag_t *tr485_BuildBase(uint8_t *buf, uint8_t token, uint8_t cmd, struct ptag_t *t)
{
	uint8_t len = sizeof(struct tr485_frame) + sizeof(struct tr485_base);
	uint8_t *phead,*ptail;
	struct tr485_base *pdata;
	struct bipbuffer tbb;
	int reserved;

	// 从data里划分指针位置
	phead = buf;
	pdata = (struct tr485_base*)(buf + TR485_HEAD_LEN);
	ptail = buf + len - TR485_TAIL_LEN;

	// 填充各个段
	phead[0] = '#',phead[1] = 'S';
		pdata->len    = len;
		pdata->chksum = 0;
		pdata->cmd    = cmd;
		pdata->token  = token;
	ptail[0] = '#',ptail[1] = 'E';

	pdata->chksum = tr485_Checksum((uint16_t*)phead, len);

	// TODO 发送
	// for (int i = 0; i < len; i++) {
	// 	printf("%2.2x ",(uint8_t)buf[i]);
	// }
	// printf("\n");
	return NULL;
}

/**
 * @brief	构造tr485数据帧
 * @param	buf 构造的帧传入buf
 * @param	token 令牌地址
 * @param	cmd 帧命令
 * @param	t struct ptag_t，借鉴libnet库，其中t->data表示数据内容内，t->len表示数据长度
 * @retval	查询struct ptag_t的err值
 * @remarks	tr485_BuildDataPart、tr485_BuildDataEnd两个宏实际上调用tr485_BuildData
 * @see	
 */
struct ptag_t *tr485_BuildData(uint8_t *buf, uint8_t token, uint8_t cmd, struct ptag_t *t)
{
	uint8_t len = sizeof(struct tr485_frame) + sizeof(struct tr485_base) + t->len;
	uint8_t *phead,*ptail;
	struct tr485_data *pdata;
	struct bipbuffer tbb;
	int reserved;

	// 从data里划分指针位置
	phead = buf;
	pdata = (struct tr485_data*)(buf + TR485_HEAD_LEN);
	ptail = buf + len;

	// 填充各个段
	phead[0] = '#',phead[1] = 'S';
		pdata->len    = len;
		pdata->chksum = 0;
		pdata->cmd    = cmd;
		pdata->token  = token;
		pdata->data   = buf + sizeof(struct tr485_frame) + sizeof(struct tr485_data) - 4;
		// printf("pdata %x\n",pdata->data);
		// printf("t %x\n",t->data);
		// printf("len %d\n",t->len);

		memcpy(pdata->data, t->data, t->len);
	ptail[0] = '#',ptail[1] = 'E';
	pdata->chksum = tr485_Checksum((uint16_t*)phead, len);

	// TODO 发送
	// for (int i = 0; i < len; i++) {
	// 	printf("%2.2x ",(uint8_t)buf[i]);
	// }
	// printf("\n");
	return NULL;
}

/**
 * @brief	发送一个485数据帧
 * @param	fd 设备描述符号，对于不同平台自行处理，裸机通常平台可忽略该参数
 * @param	buf 数据内容
 * @param	len 数据内容的长度
 * @retval	实际发送的数据长度
 * @remarks	对于上层来说，调用tr485_Send的方式如同socket标准函数send一样。发送若干
 TR485_CMD_DATAPART和一个TR485_CMD_DATAEND帧。
 tr485协议有错误重传检测，所以当帧缓存已满，并且里的内容未得到接收端确认前，不能再发送。
 只要收到一个允许该地址发送的令牌，则表示缓存里的TR485_CMD_DATAEND帧之前内容得到确认，
 而不是每个帧都需要确认，通过修改TR485_CACHE_COUNT可以扩大帧缓存
 */
int32_t tr485_Send(int32_t fd, uint8_t *buf, int32_t len)
{
	struct ptag_t t;
	register int32_t offset = 0;
	register int32_t frame = 0;
	register int32_t ret;
	//TODO 检查缓存，最大可以发送多少帧
	// tlen = ???
	// 接收缓存


	// 小数据单帧发送
	if (len <= TR485_MAX_DATA) {
		//TODO 构造 end帧并发送
		// printf("frame end\n");
		
		t.data = buf;
		t.len  = len;
		tr485_BuildData(sg_cachesend[0].frame, sg_myinf.addr, TR485_CMD_DATAEND, &t);
		// TODO 发送
		return len;
	}

	// 多帧发送
	if (len > TR485_MAX_DATA * TR485_CACHE_COUNT) {
		len = TR485_MAX_DATA * TR485_CACHE_COUNT;
	}
	while (offset < len) {
		// printf("offset = %3d len = %3
		// 接收缓存d ",offset,len);
		if (frame == TR485_CACHE_COUNT) {
			break;
		}
		if (frame != TR485_CACHE_COUNT - 1 && 
			offset + TR485_MAX_DATA < len) {

			//TODO 构造 part帧并发送
	// 接收缓存
			t.data = buf + offset;
			t.len  = TR485_MAX_DATA;
			tr485_BuildData(sg_cachesend[frame].frame, sg_myinf.addr, TR485_CMD_DATAPART, &t);
			// printf("frame part %d\n",t.len);
		}
		else {
			//TODO 构造 end帧并发送
			t.data = buf + offset;
			t.len  = len - offset;
			tr485_BuildData(sg_cachesend[frame].frame, sg_myinf.addr, TR485_CMD_DATAEND, &t);
			// printf("frame end %d\n",t.len);
		}
		// TODO 发送，应该保证len和ret的值相同
		ret = tr485_HwSend(fd, buf, len);
		frame++;
		offset += t.len;
	}
	return offset;
}


uint32_t tr485_FindFrame(int32_t fd, struct bipbuffer *bb)
{
	static uint8_t *pheadcache[TR485_ANALYSE_COUNT];
	static uint8_t *ptailcache[TR485_ANALYSE_COUNT];
	uint8_t *phead, *ptail;
	bool frameErr = false;
	struct bipbuffer tbb = *bb;
	struct tr485_base *pbase_hdr;
	// static uint8_t rbuf[TR485_MTU];
	uint8_t *ptlen,*pblock;
	int size;

	// tr485_HwRecv();
	// Step 1:在临时bipbuffer长度大于TR485_LTU前提下找到Frame Head
	// 未找到，标记帧错误，则释放所有bipbuffer

	// 只存在A块
	if (bipbuffer_IsSequenceAll(&tbb)) {
		if (bipbuffer_GetCommittedSize(&tbb) >= TR485_LTU) {
			phead  = (uint8_t*)bipbuffer_DataData(&tbb, (char*)"#S", TR485_HEAD_LEN);
			pblock = (uint8_t*)bipbuffer_GetContiguousBlock(&tbb, &size);
			// 找到帧头
			if (phead) {
				if (phead != pblock) {
					bipbuffer_DecommitTo(&tbb, phead+TR485_HEAD_LEN);
				}
			}
			// 未找到帧头、释放空间，释放后的空间大于帧头
			else {
				bipbuffer_DecommitBlock(&tbb, bipbuffer_GetCommittedSize(&tbb) - TR485_HEAD_LEN);
			}
		}
	}
	else {// 尽量避免进入这里，缓存不连续开销大
		printf("Unsequence\n");
	}
	
	
	// pbase_hdr = 
	// Step 2:得到基本帧首部len，在bipbuffer长度大于tlen前提下得到偏移len地址
	// 偏移地址值为Frame Tail：找到一帧
	// 否则：标记帧错误（或者忽略），bipbuffer释放Frame Head长度，tlen复位，返回Step 1
	if (size >= TR485_LTU) {
		pbase_hdr = (struct tr485_base *)(phead + TR485_HEAD_LEN);
		// if (size >= pbase_hdr.len) 
		{
			
		}
	}
	
	// Step 3:计算Frame Head到Frame Tail的校验和
	// 校验真确：找到一合法帧
	// 否则：标记帧错误，bipbuffer释放Frame Head长度，tlen复位，返回Step 1

	// Step 4:当最后一帧是TR485_CMD_DATAEND并且无任何标记错误，
	// 无错误：投递给tr485_Analyse，回收bipbuffer 1帧内存
	// 否则：令牌桶顶端为重发帧

	// Step 

	// Step 

	// Step 
	return 0;
}

/**
 * @brief	分析收到的tr485帧
 * @param	fd 设备描述符号，对于不同平台自行处理，裸机通常平台可忽略该参数
 * @param	buf 数据内容
 * @param	len 数据内容的长度
 * @retval	null
 * @remarks	只有TR485_CMD_DATAPART和TR485_CMD_DATAEND帧会给上层处理，其余均协议
 内部自行处理，调用tr485_Recv获取数据
 * @see	tr485_Recv
 */

int32_t tr485_Analyse(int32_t fd, uint8_t *buf, int32_t len)
{
	

	// 接收数据直到数据远端不发送，或者
	// 从环形缓冲里找出合法帧
	

	// 属于控制层
	// 遍历命令
	switch(0) {
	case TR485_CMD_TOKEN:
		break;
	case TR485_CMD_DATAEND:
		// 传递到应用层环形缓冲
		break;
	case TR485_CMD_DATAPART:
		// 传递到应用层环形缓冲
		break;

	case TR485_CMD_ALLOC:
		break;
	case TR485_CMD_RAND:
		break;
	case TR485_CMD_DEV:
		break;



	case TR485_CMD_RETRANSFER:
		break;
	}

	// 遍历类型
	switch(0) {
	case TR485_TYPE_REQUEST:
		break;
	case TR485_TYPE_ANSWER:
		break;
	case TR485_TYPE_SET:
		break;
	}
	
	
	return 0;
}




/**
 * @brief	获取一串由tr485合并的数据帧
 * @param	fd 设备描述符号，对于不同平台自行处理，裸机通常平台可忽略该参数
 * @param	buf 数据内容
 * @param	len 数据内容的长度
 * @retval	实际接收的数据长度
 * @remarks	对于上层来说，调用tr485_Recv的方式如同socket标准函数recv一样。
 */
int32_t tr485_Recv(int32_t fd, uint8_t *buf, int32_t len)
{
	//从环形缓冲里拿出数据	
	return 0;
}



// 上层事件接口
/**
 * @brief	动态配置当前设备tr485设备名，由上层将名称保存到存储设备
 * @param	name 设备名，设备名长度在32Byte以内
 * @retval	null
 * @remarks	
 * @see	
 */

int32_t tr485_OnDynamicName(int8_t *name)
{
	return 0;
}

/**
 * @brief	动态配置当前设备tr485设备地址，只有允许软件自动分配地址时才有效，
 由上层将地址保存到存储设备
 * @param	addr 设备地址
 * @retval	null
 * @remarks	
 * @see	
 */
int32_t tr485_OnDynamicAddr(uint8_t addr)
{
	return 0;
}

// 上层API
/**
 * @brief	设置地址和名称映射表，如同TCP/IP协议里的ARP表
 * @param	null
 * @retval	null
 * @remarks	
 * @see	
 */

int32_t tr485_SetMap()
{
	return 0;
}

/**
 * @brief	获取地址和设备名称映射表，如同TCP/IP协议里的ARP表
 * @param	null
 * @retval	null
 * @remarks	
 * @see	
 */

int32_t tr485_GetMap()
{
	return 0;
}

/**
 * @brief	设置本地tr485设备信息
 * @param	name 设备名，32Byte以内
 * @param	addr 设备地址
 * @param	enableDynamic 是否允许动态分配地址
 * @retval	null
 * @remarks	当enableDynamic为0时表示禁止动态分配地址，否则允许。如同TCP/IP里允许
 DHCP协议
 * @see	tr485_GetLocal tr485_OnDynamicAddr
 */

int32_t tr485_SetLocal(int8_t *name, uint8_t addr, bool enableDynamic)
{
	return 0;
}

/**
 * @brief	获取本地tr485设备信息
 * @param	name 设备名，32Byte以内
 * @param	addr 设备地址
 * @param	enableDynamic 是否允许动态分配地址
 * @retval	null
 * @remarks	
 * @see	tr485_GetLocal
 */
int32_t tr485_GetLocal(int8_t *name, uint8_t *addr, bool *enableDynamic)
{
	return 0;
}

/**
 * @brief	tr485令牌调度规则，主机必须实现
 * @param	null
 * @retval	null
 * @remarks	
 * @see	
 */

int32_t tr485_rule()
{
	return 0;
}