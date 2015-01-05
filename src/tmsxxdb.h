#ifndef _TMSXX_DB_H_
#define _TMSXX_DB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <protocol/tmsxx.h>


struct tdb_common
{
	int id;				///< 自增长
	int val1;			///< 用户数据
	int val2;			///< 用户数据
	int val3;			///< 用户数据
	int val4;			///< 用户数据
	int val5;			///< 用户数据
	int val6;			///< 用户数据
	int val7;			///< 用户数据
	int val8;			///< 用户数据
	int val9;			///< 用户数据
	int val10;			///< 用户数据
	int val11;			///< 用户数据
	int val12;			///< 用户数据
	int lenpdata;		///< 二进制数据长度
	const void *pdata;		///< 自定义二进制
};
typedef struct tdb_common tdb_common_t;					 ///< tdb_common 通用表数据结构

typedef struct tms_sn                  tdb_sn_t;		 ///< tb_sn 表数据结构

struct tdb_sms
{
	int id;
	int32_t time;
	int8_t  phone[TLE_LEN];
	int32_t type;
	int32_t level;
};
typedef struct tdb_sms tdb_sms_t;		 ///< tb_sms 表数据结构


struct tdb_composition 
{
	int id;
	int32_t frame;
	int32_t slot;
	int32_t type;
	int32_t port;
};
typedef struct tdb_composition tdb_composition_t;///< tb_composition 表数据结构

struct tdb_dev_map    
{
	int id;
	int32_t frame;
	int32_t slot;
	int32_t type;
	int32_t port;
	uint8_t dev_name[64];
	uint8_t cable_name[64];
	uint8_t start_name[64];
	uint8_t end_name[64];
};
typedef struct tdb_dev_map tdb_dev_map_t;	 ///< tb_dev_map 表数据结构



struct tdb_any_unit_osw
{
	int id;
	// 参考 tmsxx.h的定义 struct tms_cfg_mcu_u_any_osw
	int32_t any_frame;	///<OPM/OLP机框编号
	int32_t any_slot;	///< OPM/OLP槽位编号
	int32_t any_type;	///< OPM/OLP设备类型

	// 参考 struct tms_cfg_mcu_u_any_osw_val
	int32_t any_port;	///< OPM/OLP端口编号
	int32_t osw_frame;	///< OSW机框编号
	int32_t osw_slot;	///< OSW槽位编号
	int32_t osw_type;	///< OSW设备类型，无用函数里自动设置成DEV_OSW
	int32_t osw_port;	///< OSW端口编号
};
typedef struct tdb_any_unit_osw tdb_any_unit_osw_t;///< tb_any_unit_osw 表数据结构

struct tdb_osw_cyc
{
	int id;
	// 参考 tmsxx.h的定义 struct tms_cfg_mcu_osw_cycle
	int32_t  frame;		///< OSW机框编号
	int32_t  slot;		///< OSW槽位编号
	int32_t  type;		///< OSW设备类型

	// 参考 tmsxx.h的定义 struct tms_cfg_mcu_osw_cycle_val
	int32_t port;		///< OSW端口编号
	int32_t iscyc;		///< 是否周期测试
	int32_t interval;	///< 周期测试间隔
};
typedef struct tdb_osw_cyc tdb_osw_cyc_t;		///< tb_osw_cyc 表数据结构

struct tdb_otdr_rollcall
{
	int id;
	struct tms_getotdr_test_hdr    *ptest_hdr;
	struct tms_getotdr_test_param *ptest_param;
};
typedef struct tdb_otdr_rollcall tdb_otdr_rollcall_t;


struct tdb_otdr_ref
{
	int id;
	struct tms_otdr_ref_hdr       *pref_hdr; 		
	struct tms_retotdr_test_param *ptest_param; 

	struct tms_retotdr_data_hdr   *pdata_hdr; 
	struct tms_retotdr_data_val   *pdata_val; 

	struct tms_retotdr_event_hdr  *pevent_hdr; 
	struct tms_retotdr_event_val  *pevent_val; 

	struct tms_retotdr_chain      *pchain; 
	struct tms_cfg_otdr_ref_val   *pref_data;
};
typedef struct tdb_otdr_ref tdb_otdr_ref_t;		///< tb_otdr_ref 表数据结构

struct tdb_otdr_his_data
{
	int id;
	struct tms_retotdr_test_hdr   *ptest_hdr; 		
	struct tms_retotdr_test_param *ptest_param; 

	struct tms_retotdr_data_hdr   *pdata_hdr; 
	struct tms_retotdr_data_val   *pdata_val; 

	struct tms_retotdr_event_hdr  *pevent_hdr; 
	struct tms_retotdr_event_val  *pevent_val; 

	struct tms_retotdr_chain      *pchain;
};
typedef struct tdb_otdr_his_data tdb_otdr_his_data_t;		///< tb_otdr_his_data 表数据结构


int tmsdb_CheckDb();
int tmsdb_Insert_common(
		tdb_common_t *pcondition, 
		tdb_common_t *pmask,
		int count);
int tmsdb_Insert_sn(
		tdb_sn_t *pcondition, 
		tdb_sn_t *pmask,
		int count);
int tmsdb_Insert_sms(
		tdb_sms_t *pcondition, 
		tdb_sms_t *pmask,
		int count);
int tmsdb_Insert_composition(
		tdb_composition_t *pcondition, 
		tdb_composition_t *pmask,
		int count);
int tmsdb_Insert_dev_map(
		tdb_dev_map_t *pcondition, 
		tdb_dev_map_t *pmask,
		int count);
int tmsdb_Insert_any_unit_osw(
		tdb_any_unit_osw_t *pcondition, 
		tdb_any_unit_osw_t *pmask,
		int count);
int tmsdb_Insert_osw_cyc(
		tdb_osw_cyc_t *pcondition, 
		tdb_osw_cyc_t *pmask,
		int count);
int tmsdb_Insert_otdr_rollcall(
		tdb_otdr_rollcall_t *pcondition, 
		tdb_otdr_rollcall_t *pmask,
		int count);
int tmsdb_Insert_otdr_ref(
		tdb_otdr_ref_t *pcondition, 
		tdb_otdr_ref_t *pmask,
		int count);
int tmsdb_Insert_otdr_his_data(
		tdb_otdr_his_data_t *pcondition, 
		tdb_otdr_his_data_t *pmask,
		int count);
int tmsdb_Delete_common(
		tdb_common_t *pcondition, 
		tdb_common_t *pmask);
int tmsdb_Delete_sn(
		tdb_sn_t *pcondition, 
		tdb_sn_t *pmask);
int tmsdb_Delete_sms(
		tdb_sms_t *pcondition, 
		tdb_sms_t *pmask);

int tmsdb_Delete_composition(
		tdb_composition_t *pcondition, 
		tdb_composition_t *pmask);
int tmsdb_Delete_dev_map(
		tdb_dev_map_t *pcondition, 
		tdb_dev_map_t *pmask);
int tmsdb_Delete_any_unit_osw(
		tdb_any_unit_osw_t *pcondition, 
		tdb_any_unit_osw_t *pmask);
int tmsdb_Delete_osw_cyc(
		tdb_osw_cyc_t *pcondition, 
		tdb_osw_cyc_t *pmask);
int tmsdb_Delete_otdr_ref(
		tdb_otdr_ref_t *pcondition, 
		tdb_otdr_ref_t *pmask);
int tmsdb_Delete_otdr_rollcall(
		struct tdb_otdr_rollcall *pcondition, 
		struct tdb_otdr_rollcall *pmask);
int tmsdb_Delete_otdr_his_data(
		tdb_otdr_his_data_t *pcondition, 
		tdb_otdr_his_data_t *pmask);
int tmsdb_Select_common(
		tdb_common_t *pcondition, 
		tdb_common_t *pmask,
		// tdb_common_t **ppout,
		int (*pcallback)(tdb_common_t *cbptr, void *ptr, int len),
		void *ptr);
int tmsdb_Select_sn(
		tdb_sn_t *pcondition, 
		tdb_sn_t *pmask,
		tdb_sn_t **ppout);
int tmsdb_Select_sms(
		tdb_sms_t *pcondition, 
		tdb_sms_t *pmask,
		tdb_sms_t **ppout);
int tmsdb_Select_composition(
		tdb_composition_t *pcondition, 
		tdb_composition_t *pmask,
		tdb_composition_t **ppout);
int tmsdb_Select_dev_map(
		tdb_dev_map_t *pcondition, 
		tdb_dev_map_t *pmask,
		tdb_dev_map_t **ppout);
int tmsdb_Select_any_unit_osw(
		tdb_any_unit_osw_t *pcondition, 
		tdb_any_unit_osw_t *pmask,
		tdb_any_unit_osw_t **ppout);
int tmsdb_Select_osw_cyc(
		tdb_osw_cyc_t *pcondition, 
		tdb_osw_cyc_t *pmask,
		tdb_osw_cyc_t **ppout);
int tmsdb_Select_otdr_rollcall(
		tdb_otdr_rollcall_t *pcondition, 
		tdb_otdr_rollcall_t *pmask,
		int (*pcallback)(tdb_otdr_rollcall_t *output, void *ptr), 
		void *ptr);
int tmsdb_Select_otdr_ref(
		tdb_otdr_ref_t *pcondition, 
		tdb_otdr_ref_t *pmask,
		int (*pcallback)(tdb_otdr_ref_t *output, void *ptr), 
		void *ptr);
int tmsdb_Select_otdr_his_data(
		tdb_otdr_his_data_t *pcondition, 
		tdb_otdr_his_data_t *pmask,
		int (*pcallback)(tdb_otdr_his_data_t *cbptr, void *ptr),
		void *ptr);
#ifdef __cplusplus
}
#endif

#endif