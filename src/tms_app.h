#ifndef _TMS_APP_H_
#define _TMS_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	跟踪输出缓存
 
 只有 offset < limit 时才能对缓存添加内容，填充函数必须保证每次填充的长度不得大于
 empty - limit，
 */

struct trace_cache
{
	char *strout;	///< 输出执法车
	int offset;		///< 输出字符串偏移
	int empty;		///< 最大空余
	int limit;		///< 填充极限
};


void tms_SetCB(void *fun);
void tms_Callback(struct tms_callback *ptcb);
#ifdef __cplusplus
}
#endif

#endif