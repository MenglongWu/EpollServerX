/**
 ******************************************************************************
 * @file	tr485.h
 * @brief	token ring 485

一套485令牌环通信协议
 *

- 2015-3-1,MenglongWu,DreagonWoo@163.com
 	- 以至于

*/


#ifndef __TR485_H__
#define __TR485_H__

#include "bipbuffer.h"
//TODO:添加于具体平台相关的底层函数接口声明头文件
//#define _EMLUATOR //仿真

// #include <stdint.h>

/* exact-width signed integer types */
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef   signed     long long int64_t;
// typedef   signed           int bool;


/* exact-width unsigned integer types */
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned     long long uint64_t;

#define bool int 
#define true  1
#define false 0

#define NULL 0
#define _ARM7_

#ifdef WIN32
	#include "stdafx.h"
	#include "WinBase.h"
	#include "SerialPort.h"
#else //_ARM7_
	#include "stdio.h"
	// #include "usart.h"
	#define TRACE
#endif
/************************************************************************
            Terminal protocol header
|	       Header	4 byte  	|
|  Len   2 byte |   Win 2 byte  |
|      CheckSum |   unuse       |
|  Target       |   Flag        |
|                               |
|                               |
|          Data max 128 byte    |
|                               |
|	       End	4 byte		  	|
************************************************************************/




// #define TER_START  "[S-*"//帧开始标志
// #define TER_END    "[E-*"//帧结束标志


// #define ter_getdata(point) (((uint8_t*)point) + 16)

// #define ter_getlen(point) (((struct terminal_hdr*)point)->len)

// //最大传输单元
// #define TER_MTU    120   
// //帧不带任何数据内容时候的长度sizeof(struct terminal_hdr) - sizeof(uint8_t*)
// //纯terminal_hdr协议头长度（24Btye），不包括*data（4Byte）内容
// #define TER_FRAME_MIN 20
// //一帧最大数据量
// #define TER_MAX_DATA (TER_MTU - TER_FRAME_MIN)




// //关系到底层缓存的大小设置，底层缓存大小绝对不能小于TER_MTU
// #define TER_RWIN   500   //实际接收缓存大小，必须大于TER_MTU，建议是TER_MTU的3倍以上
// #define TER_90_WIN   (TER_RWIN * 0.9)   //90%接收缓存大小



// //返回值类型
// #define TRUE 1
// #define FALSE 0

// #define TER_SUCCESS			0//成功
// #define TER_FAIL			1//失败
// #define TER_FRAME_PART		2//只接收到一部分
// #define TER_FRAME_SMALL		3//帧太短


// //具体设备协议类型，高字节表示设备类型，低字节表示，
// #define TER_TARGET_TS100  0x0100
// #define TER_TARGET_TS600  0x0200
//TODO：后续带添加
#define TR485_NODE_COUNT 	256	///<最大节点数

#define TR485_CACHE_COUNT 	4	///<发送缓存数，用于错误重发
#define TR485_ANALYSE_COUNT 16	///<建议TR485_ANALYSE_COUNT > TR485_CACHE_COUNT*2

#define TR485_HEAD_LEN 		2	///<帧头长度
#define TR485_TAIL_LEN 		2	///<帧尾长度

#define TR485_FRAME_H 		(TR485_HEAD_LEN + TR485_TAIL_LEN)	///<帧首、尾部长度
#define TR485_ALLOC_H 		8	///<分配地址首部长度
#define TR485_RAND_H  		8	///<随机数首部长度
#define TR485_DEV_H 		12	///<设备帧首部长度
#define TR485_BASE_H 		4	///<基本帧首部长度
#define TR485_DATA_H 		8	///<数据帧首部长度

#define TR485_MTU 			64								///<最大帧长
#define TR485_LTU			(TR485_FRAME_H + TR485_BASE_H)	///<最小帧长


#define TR485_MAX_DATA 			(TR485_MTU - TR485_HEAD_LEN - TR485_TAIL_LEN - TR485_BASE_H) ///<最长数据
#define TR485_MAX_NAME 			32  ///<最长设备名

#define TR485_CMD_ALLOC 		'a' ///<地址分配帧
#define TR485_CMD_RAND 			'r' ///<随机数帧
#define TR485_CMD_DEV 			'N' ///<设备信息帧

#define TR485_CMD_TOKEN 		'T' ///<令牌帧
#define TR485_CMD_RETRANSFER 	'R' ///<重传帧
#define TR485_CMD_DATAPART 		'D' ///<部分数据帧
#define TR485_CMD_DATAEND 		'd' ///<数据传输完毕帧，放弃时间片

#define TR485_TYPE_REQUEST		'r' ///<命令请求
#define TR485_TYPE_ANSWER		'a' ///<命令应答类型
#define TR485_TYPE_SET			's' ///<命令设置




/************************************************************************
串口描述符句柄
它用于保证合法调用情况下（调用commsend、commrecv）发送的数据任何时候不会超过
对方硬件的缓存大小，硬件接收缓存大多数情况下（对方传递控制信号：checksum错误包）
都略有空余。
例如：
1、本地接收缓存是500，那么握手连接时候告诉对方本地缓存大小450（50 * 90%）。
2、只要对方缓存还有富余，就发送能发送的量，每次发送一串数据后，计算对方剩余的缓存大小。
3、当己方接收的缓存都被读出来后，告知对方本地缓存已经复位（或告知还剩余多少缓存）
4、当收到对方告知缓存大小后，重新计算对方缓存大小，位下一次发送做准备
注意：实际上一个合法的Terminal帧都包括缓存大小
************************************************************************/


//描述缓存的标签，用户不直接访问它
struct ptag_t
{
	uint8_t *data;
	uint16_t len;
	uint16_t err;
};


#pragma pack (2)


struct tr485_frame
{
	uint8_t head[TR485_HEAD_LEN];
	uint8_t tail[TR485_TAIL_LEN];
};
/////////////////////////////////////////////////////////////////////
struct tr485_alloc
{
	uint8_t len;		//完整数据长度
	uint8_t chksum;		//窗口大小
	
	uint8_t  cmd;		//
	uint8_t  token;		//
	uint8_t  reserve1;
	uint8_t  addr;

	uint16_t  rand;
};

struct tr485_rand
{
	uint8_t len;		//完整数据长度
	uint8_t chksum;		//窗口大小
	
	uint8_t  cmd;		//
	uint8_t  token;		//
	uint8_t  type;
	uint8_t  reserve1;
	
	uint16_t  rand;
};

struct tr485_dev
{
	uint8_t len;		//完整数据长度
	uint8_t chksum;		//窗口大小
	
	uint8_t  cmd;		//
	uint8_t  token;		//
	uint8_t  type;
	uint8_t  reserve1;
	
	uint16_t  rand;
	// uint16_t  unuse_align1;

	uint8_t  *data;

	// uint16_t  unuse_align2;
};




/////////////////////////////////////////////////////////////////////
struct tr485_base
{
	uint8_t len;		//完整数据长度
	uint8_t chksum;		
	
	uint8_t  cmd;		
	uint8_t  token;		//
};


struct tr485_data
{
	uint8_t len;		//完整数据长度
	uint8_t chksum;		
	
	uint8_t  cmd;		
	uint8_t  token;		//
	// uint16_t  unuse_align1;
	
	uint8_t  *data;

	// uint16_t  unuse_align2;
};





// 协议实现
struct ptag_t  *tr485_BuildBase(uint8_t *buf, uint8_t token, uint8_t cmd, struct ptag_t *t);
	#define 	tr485_BuildToken(buf,token,t)      tr485_BuildBase (buf, token, TR485_CMD_TOKEN,      t)
	#define 	tr485_BuildRetransfer(buf,token,t) tr485_BuildBase (buf, token, TR485_CMD_RETRANSFER, t)

struct ptag_t  *tr485_BuildData(uint8_t *buf, uint8_t token, uint8_t cmd, struct ptag_t *t);
	#define 	tr485_BuildDataPart(buf,token,t)   tr485_BuildData(buf, token, TR485_CMD_DATAPART, &t)
	#define 	tr485_BuildDataEnd(buf,token,t)    tr485_BuildData(buf, token, TR485_CMD_DATAEND, &t)

int32_t 		tr485_Analyse(int32_t fd, uint8_t *buf, int32_t len);

// 上层事件接口
int32_t tr485_OnDynamicName(int8_t *name);
int32_t tr485_OnDynamicAddr(uint8_t addr);

// 上层API
void tr485_Init();
int32_t tr485_SetMap();
int32_t tr485_GetMap();
int32_t tr485_SetLocal(int8_t *name, uint8_t addr, bool enableDynamic);
int32_t tr485_GetLocal(int8_t *name, uint8_t *addr, bool *enableDynamic);
int32_t tr485_Send(int32_t fd, uint8_t *buf, int32_t len);
int32_t tr485_Recv(int32_t fd, uint8_t *buf, int32_t len);
// 硬件相关接口
extern int32_t tr485_HwSend(int32_t fd, uint8_t *buf, int32_t len);
extern int32_t tr485_HwRecv(int32_t fd, uint8_t *buf, int32_t len);

#endif 

