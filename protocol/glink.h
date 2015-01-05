/**
 ******************************************************************************
 * @file	glink.h
 * @brief	glink协议格式

	TODO：详细描述
 *


- 2015-3-25,Menglong Wu,DreagonWoo@163.com
 	- 编写预留接口
*/

#ifndef _GLINK_H_
#define _GLINK_H_




#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
// /* exact-width signed integer types */
// typedef   signed          char int8_t;
// typedef   signed short     int int16_t;
// typedef   signed           int int32_t;
// typedef   signed     long long int64_t;
// // typedef   signed           int bool;


// /* exact-width unsigned integer types */
// typedef unsigned          char uint8_t;
// typedef unsigned short     int uint16_t;
// typedef unsigned           int uint32_t;
// typedef unsigned     long long uint64_t;

#define FRAME_HEAD_TAG 	0xffffeeee
#define FRAME_TAIL_TAG 	0xeeeeffff
#define FRAME_VERSION	0x001A0001	///< 各个应用层采用不同版本

#define GLINK_FRAME_H 8
#define GLINK_HREAD_H 4
#define GLINK_END_H   4
#define GLINK_BASE_H  32
#define GLINK_OFFSET_DATA 36
#define GLINK_OFFSET_USER 32
#define GLINK_OFFSET_CMDID 28


#define GLINK_MANAGE_ADDR	0x3e	///< 网管地址（无实际用途用）
#define GLINK_CU_ADDR      0x0e	///< MCU地址（无实际用途用）
#define GLINK_4412_ADDR 0x0a
#define GLINK_DEV_ADDR 0x0b


struct glink_context
{
	int fd;
	// TODO more
};
// #define aaaaaaaaaa 0x0a
struct glink_frame
{
	uint32_t head;		///< 起始帧标志
	uint32_t tail;		///< 结尾帧标志
};



struct glink_addr
{
	uint32_t src;		///< 本地地址
	uint32_t dst;		///< 目的地址
};



struct glink_base
{
	uint32_t pklen;		///< 帧总长
	uint32_t version;	///< 版本号
	uint32_t src;		///< 原始地址
	uint32_t dst;		///< 目的地址
	uint16_t type;		///< 帧类型
	uint16_t pkid;		///< 流水号
	uint32_t reserve;	///< 保留字节
	uint32_t cmdid;		///< 命令码ID
	uint32_t datalen;	///< 数据长度
};

void glink_htonlMore(uint8_t *pdata, uint32_t len);
int32_t glink_SendSerial(int fd, uint8_t *pdata, uint32_t len);
void glink_Build(
		struct glink_base  *pbase_hdr,
		uint32_t cmdid,
		uint32_t datalen);


int32_t glink_Send(
		int fd,  
		struct glink_base  *pbase_hdr,
		uint8_t *pdata,
		uint32_t len);
int32_t glink_SendHead(
		int fd,  
		struct glink_base  *pbase_hdr);
int32_t glink_SendTail(
		int fd);
int32_t glink_FindFrame(int8_t *pdata, int32_t *recvlen, int32_t *retframelen);

#ifdef __cplusplus
}
#endif

#endif