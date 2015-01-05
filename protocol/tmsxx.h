/**
 ******************************************************************************
 * @file	tmsxx.h
 * @brief	TMSxx协议内容

	依据《网管与MCU通信协议20150325.doc》
 *


- 2015-3-25,Menglong Wu,DreagonWoo@163.com
 	- 编写预留接口
*/

#ifndef _TMSXX_H_
#define _TMSXX_H_
#include "glink.h"



#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib.h"
/*
-------------------------------------------------------------------------------------------------
|   struct unknow1   |<- struct_A -> | <- struct_B->|<- struct_B->|<- struct_B->|   struct unknow2|
-------------------------------------------------------------------------------------------------
                     ^ptrA           |              B_count                                       ^ptrEnd

*/

#define CHECK_PTR(ptrA, struct_A, struct_B, B_Count, PtrEnd) 	\
 				( ((uint8_t*)(ptrA)) + sizeof(struct_A) + \
				   sizeof(struct_B) * (int32_t)(B_Count-1) <= ((uint8_t*)(PtrEnd)-sizeof(struct_B)) )



////////////////////////////////////////////////////////////////////////////////
// Section 1 返回值
#define RET_ERROR 				(-1)///< 错误
#define RET_SUCCESS 			0///< 0 --- 命令成功（成功执行无返回数据命令时的返回值）
#define RET_UNEXIST 			1///< 1 --- 业务板卡不存在
#define RET_COMMU_ABORT 						2///< 2 --- MCU与业务板卡通信异常
#define RET_UNMATCH_FRAME 			3///< 3 --- 命令中机框编号与业务板卡本身所处的机框编号不一致
#define RET_UNMATCH_SLOT 			4///< 4 --- 命令中槽位编号与业务板卡本身所处的槽位编号不一致
#define RET_UNMATCH_TYPE 			5///< 5 ---  命令中设备类型与业务板卡本身的设备类型不一致
#define RET_PARAM_INVALID 			6///< 6 --- 命令参数出现非法值
#define RET_IGNORE_SAVE 			7///< 7 --- 命令参数无法保存
#define RET_WAIT 					8///< 8 --- 已经存在一条命令，请稍后重试
#define RET_OTDR_ILLEGAL 			9///< 9 ---  OTDR测量异常
#define RET_OTDR_TIMEOUT 			10///< 10 --- OTDR测量超时
#define RET_UPDATE_ILLEGAL 			11///< 11 ---  RTU升级过程出现异常。
#define RET_CMD_INVALID 			12///< 12 --- 无效命令
#define RET_OLP_CANT_SWITCH     	13///<olp不能切换到指定线路
#define RET_OSW_SWITCH_ABORT    	14///<光开关切换异常
#define RET_SEND_CMMD_TIMEOUT       15///<发送命令超时
#define RET_UNEXIST_ROW				16///<不存在行

////////////////////////////////////////////////////////////////////////////////
// Section  数据包处理方式
#define	PROCCESS_COPY2USE	0	///<00--MCU本地文处理	
#define	PROCCESS_CALLBACK	1	///<01--MCU本地吴处理	
#define	PROCCESS_2DEV	2	///<02-->下发到板卡	
#define	PROCCESS_2MANAGE	3	///<03<--上传到网管	
#define	PROCCESS_2DEV_AND_COPY2USE	4	///<04-->下发到板卡、自身也处理	
#define	PROCCESS_2MANAGE_AND_COPY2USE	5	///<05--上传网管、自身也处理	
#define	PROCCESS_2DEV_OR_COPY2USE	6	///<06-->下发或自身处理	
#define	PROCCESS_2MANAGE_OR_COPY2USE	7	///<07-->上传或自身处理	
#define	PROCCESS_DONOT	8	///<08--不做任何处理	
#define	PROCCESS_SPECAIAL	9	///<09--特殊处理	


////////////////////////////////////////////////////////////////////////////////
// Section 2 设备类型
#define DEV_PWU					1	///<电源板
#define DEV_MCU					2	///<主控板
#define DEV_OPM					3	///<光功率模块板
#define DEV_OSW					4	///<光开关板
#define DEV_OTDR				5	///<OTDR板
#define DEV_OLS					6	///<光源板
#define DEV_OLP					7	///<光功率板
#define DEV_SMS					8	///<短信模块板

#define LEVEL_UI 3
#define LEVEL_TC 0

#define OLP_SWITCH_MODE_UNBACK       0	///<保护不返回，ID_CFG_OLP_MODE
#define OLP_SWITCH_MODE_BACK        1	///<表示保护返回，ID_CFG_OLP_MODE

#define OLP_SWITCH_ACTION_PERSION  0	///<人工切换，ID_REPORT_OLP_ACTION
#define OLP_SWITCH_ACTION_AUTO     1	///<自动保护倒换，ID_REPORT_OLP_ACTION
#define OLP_SWITCH_ACTION_BACK     2	///<保护返回，ID_REPORT_OLP_ACTION

#define OLP_SWITCH_ACTION_MASTER       0	///<切换到主光路，ID_REPORT_OLP_ACTION
#define OLP_SWITCH_ACTION_SLAVE        1	///<切换到备光路，ID_REPORT_OLP_ACTION

#define OTDR_SAMPLE_16000 16000			///< OTDR数据点个数
#define OTDR_SAMPLE_32000 32000			///< OTDR数据点个数
#define OTDR_EVENT_MAX 1024

#define MANAGE_COUNT 4

#define MT_MANAGE 0 	///< 普通网管\n
#define MT_TRACE 1 		///< 支持命令行的网管


///< 缺省填充源地址，目的地址
#ifdef _MANAGE
#define TMS_DEFAULT_LOCAL_ADDR GLINK_MANAGE_ADDR
#define TMS_DEFAULT_RMOTE_ADDR GLINK_CU_ADDR
#else
#define TMS_DEFAULT_LOCAL_ADDR GLINK_4412_ADDR
#define TMS_DEFAULT_RMOTE_ADDR GLINK_CU_ADDR
#endif

////////////////////////////////////////////////////////////////////////////////
// Section 3 命令ID


#define 	ID_TICK			0x10000000	///< 心跳
#define 	ID_UPDATE			0x10000001	///< 在线升级
#define 	ID_TRACE0			0x10000002	///< MCU打印调试信息
#define 	ID_TRACE1			0x10000003	///< MCU打印调试信息
#define 	ID_TRACE2			0x10000004	///< MCU打印调试信息
#define 	ID_TRACE3			0x10000005	///< MCU打印调试信息
#define 	ID_COMMAMD			0x10000006	///< 网管下发字符串命令

////////////////////////////////////////////////////////////////////////////////
// 命令类型:网管与MCU之间的通信
#define ID_GET_DEVTYPE			0x60000000	///<工控板查询设备板卡类型信息
#define ID_RET_DEVTYPE			0x60000001	///<返回设备类型信息
#define ID_CU_NOTE_NET			0x60000002///<工控板与CU通信的网段选择切换通知
#define ID_CU_NOTE_MANAGE_CONNECT		0x60000003///<CU通知工控板网管的连接状态

#define ID_GET_OPM_OLP_RAYPOWER	0x60000004///<工控板查询某槽位上OPM或OLP模块总的光功率告警
#define ID_CMD_OSW_SWITCH		0x60000005///<工控板通知OSW模块切换到某路
#define ID_CMD_OLP_REQ_OTDR		0x60000006///<OLP模块向工控机请求OTDR测试
#define ID_CMD_OLP_START_OTDR	0x60000007///<工控板通知OLP模块OTDR测试开始
#define ID_CMD_OLP_FINISH_OTDR	0x60000008///<工控板通知OLP模块OTDR测试完成
#define ID_GET_ALARM_TEST	0x60000009
#define ID_RET_ALARM_TEST	0x60000010

////////////////////////////////////////////////////////////////////////////////
// 命令类型:网管与MCU之间的通信
///<网管查询MCU的设备序列号
#define ID_CHANGE_ADDR			0x80000000	///<网管修改RTU的IP地址(RTU做TCP服务器，端口号6000)
#define ID_GET_SN				0x80000001	///<网管激活RTU
#define ID_RET_SN				0x80000002	///<MCU返回设备序列号
#define ID_CFG_SMS				0x80000003	///<网管发送告警短信发送权限到RTU
#define ID_CFG_SMS_CLEAR		0x80000004	///<网管发送清除告警短信发送权限到MCU

#define ID_GET_COMPOSITION		0x80000005	///<网管查询板卡组成信息
#define ID_RET_COMPOSITION		0x80000006	///<MCU返回板卡组成信息s

#define ID_CFG_MCU_OSW_PORT			0x80000007	///<网管发送OSW模块各光端口关联光缆信息到MCU
#define ID_CFG_MCU_OSW_PORT_CLEAR		0x80000008	///<网管发送清除OSW模块各光端口关联光缆信息到MCU
#define ID_CFG_MCU_OLP_PORT	0x80000009	///<网管发送OLP模块各光端口关联光缆信息到MCU
#define ID_CFG_MCU_OLP_PORT_CLEAR	0x80000010	///<网管发送清除OLP模块各光端口关联光缆信息到MCU


#define ID_CFG_MCU_U_OPM_OSW	0x80000011	///<网管发送OPM光端口与OSW光端口联动关系到MCU
#define ID_CFG_MCU_U_OPM_OSW_CLEAR	0x80000012	///<网管发送清除OPM光端口与OSW光端口联动关系到MCU
#define ID_CFG_MCU_U_OLP_OSW	0x80000013	///<网管发送OLP模块光端口与OSW模块光端口联动关系到MCU
#define ID_CFG_MCU_U_OLP_OSW_CLEAR	0x80000014	///<网管发送清除OLP模块光端口与OSW模块光端口联动关系到MCU



//////////////////////

#define ID_CFG_OPM_REF_LEVEL	0x80000015	///<网管发送OPM模块各光端口关联光缆的参考光功率及告警门限
#define ID_GET_OPM_OP			0x80000016	///<网管查询OPM模块各光端口当前功率
#define ID_RET_OPM_OP			0x80000017	///<MCU返回OPM模块各光端口当前功率

#define ID_CFG_OLP_REF_LEVEL	0x80000018	///<网管发送OLP模块各光端口关联光缆的光功率及告警门限到MCU
#define ID_GET_OLP_OP			0x80000019	///<网管查询OLP模块各光端口当前功率
#define ID_RET_OLP_OP			0x80000020	///<MCU返回OLP模块各光端口当前功率

#define ID_CFG_OTDR_REF				0x80000021	///<网管发送各监测光路的OTDR参考曲线数据到MCU

#define ID_CFG_MCU_OSW_CYCLE	0x80000022	///<网管发送OSW各光端口需要周期测试的光缆以及周期间隔到MCU
#define ID_CFG_OSW_CYCLE ID_CFG_MCU_OSW_CYCLE 

#define ID_GET_OTDR_TEST		0x80000023	///<网管对OTDR点名测试
#define ID_RET_OTDR_TEST		0x80000024	///<MCU返回OTDR测量曲线

#define ID_CFG_OLP_MODE			0x80000025	///<网管对OLP模块的工作模式和返回时间设定
#define ID_CMD_OLP_SWITCH		0x80000026	///<网管对OLP模块的指令倒换命令
#define ID_REPORT_OLP_ACTION 	0x80000027 ///<MCU上报OLP人工切换或自动切换的动作信息

#define ID_ALARM_OPM			0x80000028	///<MCU上报某槽位总的光功率告警
#define ID_ALARM_OPM_CHANGE		0x80000029	///<MCU上报某槽位变化的光功率告警
#define ID_ALARM_LINE			30	///<MCU返回线路告警
#define ID_ALARM_HW				0x80000031	///<MCU返回硬件告警
#define ID_RET_OTDR_CYC			0x80000032	///<MCU返回OTDR周期性测试曲线

#define ID_CMD_SMS_TEXT			0x80000033	///<网管发送短信内容到MCU
#define ID_CMD_SMS_ERROR		0x80000034	///<MCU返回短信操作返回码
#define ID_GET_VERSION			0x80000035	///<网管查询板卡版本号
#define ID_RET_VERSION			0x80000036	///<MCU返回板卡版本号
#define ID_ADJUST_TIME				0x80000037	///<网管给MCU校时
#define ID_CMD_ACK				0x80000038	///<MCU返回应答错误码
#define	ID_GET_OTDR_TEST_CYC    0x80000039
#define ID_RET_OTDR_TEST_CYC    0x80000040






////////////////////////////////////////////////////////////////////////////////
// Section 4 TMSxx通信各种包结构
struct tms_dev_slot
{
	int32_t frame;
	int32_t slot;
};
struct tms_dev_type
{
	int32_t frame;
	int32_t slot;
	int32_t type;
};

struct tms_dev_version
{
	int32_t frame;
	int32_t slot;
	int32_t type;
	uint8_t vsersion[256];
};

struct tms_dev_update_hdr
{
	int32_t frame;
	int32_t slot;
	int32_t type;
	uint8_t fname[256];
	int32_t flen;
};
struct tms_dev_md5
{
	uint8_t md5[32];
};



// ID_CHANGE_ADDR			2	
// #define TMS_NAME_LEN 64
struct tms_change_addr
{
	uint8_t ip[16];
	uint8_t mask[16];
	uint8_t gw[16];
};

// // ID_CFG_SMS				3
struct tms_sms_alarm
{
	int32_t type;
	int32_t level;
};
#define TLE_LEN 16
struct tms_sms_clerk
{
	int32_t alarm_count;
	int8_t  phone[TLE_LEN];
	struct tms_sms_alarm *alarm;
};

struct tms_sms_duty
{
	int32_t time;
	int32_t clerk_count;
	struct tms_sms_clerk *clerk;
};

// struct tms_cfg_sms
// {
// 	int32_t time;
// 	int8_t  phone[TLE_LEN];
// 	int32_t type;
// 	int32_t level;
// };

struct tms_sn
{
	int8_t sn[128];
};
//ID_CFG_SMS
struct tms_cfg_sms
{
	int32_t count;
};

struct tms_cfg_sms_val
{
	int32_t time;
	int8_t  phone[TLE_LEN];
	int32_t type;
	int32_t level;
};


// // ID_GET_DEVICE		4
// // struct tms_
// // {

// // };

// // ID_SET_DEVICE		5
// struct tms_SET_device
// {
// 	char 	device_name[TMS_NAME_LEN];
// 	int 	frame_count;
// 	int 	frame;
// 	int 	slot;
// 	int 	slot_type;
// 	int 	port_num;
// };
//ID_CFG_MCU_OSW_PORT 7
struct tms_cfg_mcu_osw_port
{
	int32_t frame;
	int32_t slot;
	int32_t type;
	int32_t port;
	uint8_t dev_name[64];
	uint8_t cable_name[64];
	uint8_t start_name[64];
	uint8_t end_name[64];
};

// ID_CFG_OSW_PORT			6
struct tms_dev_port
{
	int32_t frame;
	int32_t slot;
	int32_t type;
	int32_t port;
};

// #define tms_dev_base tms_dev_port 
	
struct tms_dev_composition
{
	int32_t count;//frame
	// struct tms_dev_port *dev;
};

struct tms_dev_composition_val
{
	int32_t frame;
	int32_t slot;
	int32_t type;
	int32_t port;
};


// // ID_CFG_OLP_PORT			7
// struct tms_
// {
// int32_t frame		;
// int32_t slot		;
// int32_t type;
// int32_t port_num		;
// int8_t cable_name	[TMS_NAME_LEN]	;
// int32_t fiber_num		;
// uint8_t a_station_name	[TMS_NAME_LEN]	;
// int8_t landmark_name	[TMS_NAME_LEN]	;
// char	Z Station Name	[TMS_NAME_LEN]	;
// char	  ZLandmark ame	[TMS_NAME_LEN]	;
// int	A Station Position		;
// int	Cable  Length		;

// };
/////////////////////////////////////
// OPM/OLP 与 OSW关联数据结构大体相同
// ID_CFG_MCU_U_OPM_OSW  ID_CFG_MCU_U_OLP_OSW
struct tms_cfg_mcu_u_any_osw
{
	int32_t any_frame;	///< OPM/OLP机框编号
	int32_t any_slot;	///< OPM/OLP槽位编号
	int32_t any_type;	///< OPM/OLP设备类型
	int32_t	count;		

};
struct tms_cfg_mcu_u_any_osw_val
{
	int32_t any_port;	///< OPM/OLP端口编号
	int32_t osw_frame;	///< OSW机框编号
	int32_t osw_slot;	///< OSW槽位编号
	int32_t osw_type;	///<OSW设备类型，无用函数里自动设置成DEV_OSW
	int32_t osw_port;	///< OSW端口编号
};
// ID_CFG_MCU_U_OPM_OSW_CLEAR  ID_CFG_MCU_U_OLP_OSW_CLEAR
struct tms_cfg_mcu_any_port_clear_val
{
	int32_t any_port;
};




// ID_CFG_MCU_U_OLP_OSW_CLEAR






// struct tms_8_2
// {
// 	int32_t port_num		;
// 	int	Relation Num		;

// };
// struct tms_8_3
// {
// 	int32_t frame		;
// 	int32_t slot		;
// 	int32_t type;
// 	int32_t port_num		;
// };

// // ID_CFG_OLP_UNITE_OSW	9	
// struct tms_
// {
// 	int32_t frame		;
// 	int32_t slot		;
// 	int32_t type;
// 	int	Relation Num		;
// };

// struct tms_9_2
// {
// 	int32_t port_num		;
// 	int32_t frame		;
// 	int32_t slot		;
// 	int32_t type;
// 	int32_t port_num		;

// };
// // ID_CFG_OPM_REF_LEVEL	10	
struct tms_cfg_any_power_ref
{
	int32_t frame;
	int32_t slot;
	int32_t type;
	int32_t count;
};

struct tms_cfg_opm_ref_val
{
	int32_t port;
	int32_t isminitor;
	int32_t wave;
	int32_t ref_power;
	int32_t leve0; // 紧急告警门限
	int32_t leve1; // 主要告警门限
	int32_t leve2; // 次要告警门限
};

struct tms_cfg_olp_ref_val
{
	int32_t port;
	int32_t isminitor;
	int32_t wave;
	int32_t ref_power;
	int32_t leve0; // 紧急告警门限
	int32_t leve1; // 主要告警门限
	int32_t leve2; // 次要告警门限
};


struct tms_any_op
{
	int32_t frame;
	int32_t slot;
	int32_t type;
	int32_t count;
};
struct tms_any_op_val
{
	int32_t port;
	int32_t power;
};
// struct tms_10_2
// {
// 	int32_t port_num		;
// 	int	Is Monitor		;
// 	int	   Wave Length		;
// 	int	Ref Power		;
// 	int	Serious Alarm Th		;
// 	int	Primary Alarm Th		;
// 	int	Slight Alarm Th		;
// };
// // ID_GET_OPM_OP		11	
// struct tms_
// {
// 	int32_t frame		;
// 	int32_t slot		;
// 	int32_t type;
// };
// // ID_SET_OPM_OP		12	
// struct tms_
// {
// 	int32_t frame		;
// 	int32_t slot		;
// 	int32_t type;
// };
// struct tms_
// {
// 	int32_t port_num		;
// 	int	Current Power		;

// };

// // ID_CFG_OLP_REF_LEVEL	13	
// struct tms_
// {
// 	int32_t frame		;
// 	int32_t slot		;
// 	int32_t type;
// };
// struct tms_
// {
// 	int32_t port_num		;
// 	int	 Power		;
// 	int	Serious Alarm Th		;
// 	int	Primary Alarm Th		;
// 	int	Slight Alarm Th		;


// };
// // ID_GET_OLP_OP		14	
// struct tms_
// {
// 	int32_t frame		;
// 	int32_t slot		;
// 	int32_t type;
// };
// // ID_SET_OLP_OP		15	
// struct tms_
// {
// 	int32_t frame		;
// int32_t slot		;
// int32_t type;
// char	Monitor Port Num		;

// };
// struct tms_
// {

// int32_t port_num		;
// int	Transmit Power		;

// };



// //ID_CFG_OTDR				16	///<
// struct tms_
// {
// 	int32_t frame		;
// int32_t slot		;
// int32_t type;
// int32_t port_num		;
// char	Pipe Serial umber		;
// char	OTDRTest Parameter onfig[20];
// char	RANG		;
// char	WL		;
// char	PW		;
// char	TIME		;
// float	GI		;
// float	END HRESHOLD		;
// float	NONE EFLECTTHRESHOLD		;
// int	 Reserved		;
// int	 Reserved		;
// int	 Reserved		;


// };
// struct tms_
// {
// 	char	DPID(Data Points Block ID)		[12];
// 	char	TNDP(Number of Data Points)		;
// 	short data[20000];
// };


// // TODO (c)事件点信息单元
// struct tms_
// {
// };

// struct tms_
// {
// 	char	OTDR Test Result Information[20] 		;
// 	float	 Chain Range		;
// 	float	Chain Loss		;
// 	float	Chain Attenuation		;

// };

// struct tms_
// {
// 	int	Serious Alarm Th		;
// 	int	Primary Alarm Th		;
// 	int	Slight Alarm Th		;

// };










// //ID_CFG_MCU_OSW_CYCLE		22	
struct tms_cfg_mcu_osw_cycle
{
	int32_t  frame;		///< OSW机框编号
	int32_t  slot;		///< OSW槽位编号
	int32_t  type;		///< OSW设备类型
	uint32_t count;		///< 有多少个 struct tms_cfg_mcu_osw_cycle_val 配置项
};
struct tms_cfg_mcu_osw_cycle_val
{
	int32_t port;		///< OSW端口编号
	int32_t iscyc;		///< 是否周期测试
	int32_t interval;	///< 周期测试间隔
};


// ID_CFG_OLP_MODE
struct tms_cfg_olp_mode
{
	int32_t  frame;		///< 
	int32_t  slot;		///< 
	int32_t  type;		///< 
	int32_t  mode;		///< 0表示保护不返回，1表示保护返回
	int32_t  backtime;	///< 返回时间，单位分钟。
	int32_t  protect;	///< 保护线路，0表示保护线路为主路，1表示保护线路为备路
};
// struct tms_
// {
// 	char	Port Number		;
// 	char	Is CycTest		;
// 	char	CycTest Interval		;
// 	char	delay		;
// };
//ID_GET_OTDR_TEST	18	

// 不同平台hdr不同
struct tms_getotdr_test_hdr
{
	int32_t frame;				///< OSW机框号
	int32_t slot;				///< OSW槽位号
	int32_t type;				///< OSW设备类型
	int32_t port;				///< OSW端口号
	int32_t	reserve0;			///< OSW保留
};

// 不同型号获取OTDR参数保持一致
struct tms_getotdr_test_param
{
	int32_t	rang;					///< 测试量程
	int32_t	wl;						///< 测试波长
	int32_t	pw;						///< 测试脉宽
	int32_t	time;					///< 测试时间
	float	gi;						///< 群折射率
	float	end_threshold;			///< 结束门限
	float	none_reflect_threshold; ///< 非反射门限
	int32_t reserve0;				///< 保留 
	int32_t reserve1;				///< 保留
	int32_t reserve2;				///< 保留
};


// OTDR参考曲线头
struct tms_otdr_ref_hdr
{
	int32_t osw_frame;		///< osw机框号
	int32_t osw_slot;		///< osw槽位号
	int32_t osw_type;		///< osw设备类型
	int32_t osw_port;		///< osw端口
	int32_t otdr_port;		///< otdr端口
	int8_t  strid[20];		///< OTDR测量参数的识别ID，
	
};

struct tms_cfg_otdr_ref_val
{
	int32_t leve0; // 紧急告警门限
	int32_t leve1; // 主要告警门限
	int32_t leve2; // 次要告警门限
};
// OTDR返回信息A部分，不同TMS系列可能不同
struct tms_retotdr_test_hdr
{
	int32_t osw_frame;	///< osw机框号
	int32_t osw_slot;	///< osw槽位号
	int32_t osw_type;	///< osw设备类型
	int32_t osw_port;	///< osw端口
	int8_t  time[20];	///< 测量时间，格式如下2015-03-19 14:02:15。

	int32_t otdr_frame;	///< otdr机框号
	int32_t otdr_slot;	///< otdr槽位号
	int32_t otdr_type;	///< otdr设备类型
	int32_t otdr_port;	///< otdr端口
};
struct tms_retotdr_test_param
{
	int32_t	rang;					///< 测试量程
	int32_t	wl;						///< 测试波长
	int32_t	pw;						///< 测试脉宽
	int32_t	time;					///< 测试时长
	float	gi;						///< 群折射率
	float	end_threshold;			///< 结束门限
	float	none_reflect_threshold; ///< 非反射门限
	int32_t sample;					///< 采样速率
	int32_t reserve0;				///< 保留 
	int32_t reserve1;				///< 保留
	//int32_t reserve2;				///< 保留
};
struct tms_retotdr_data_hdr
{
	int8_t  dpid[12];					///< 测试数据单元名称
	int32_t count;					///< 数据点个数
};

// OTDR返回信息B部分
struct tms_retotdr_data_val
{
	uint16_t data;
};

struct tms_retotdr_event_hdr
{
	int8_t eventid[12];
	int32_t count;
};
// OTDR返回信息C部分
struct tms_retotdr_event_val
{
	int32_t distance;		///< 该事件点距离
	int32_t event_type;		///< 该事件点类型
	float   att;			///< 该事件点两事件点与前事件点之间光纤衰减系数
	float   loss;			///< 该事件点连接损耗
	float   reflect;		///< 该事件点反射损耗
	float   link_loss;		///< 该事件点累计损耗
};

// OTDR返回信息D部分
struct tms_retotdr_chain
{
	int8_t inf[20];			///< 测试结果
	float range;		///< 链长
	float loss;		///< 链损耗
	float att;		///< 链衰减系数
};


// //ID_SET_OTDR_TEST	19	
// struct tms_
// {
// int32_t frame		;
// int32_t slot		;
// int32_t type;
// int32_t port_num		;
// char	Test Time		;
// int32_t frame		;
// int32_t slot		;
// int32_t type;
// char	Pipe Number		;
// char	RANG		;
// char	WL		;
// char	PW		;
// char	TIME		;
// float	GI		;
// float	END THRESHOLD		;
// float	NONE REFLECT THRESHOLD		;
// int	 Sampleing Rate		;
// int	 Reserved		;
// int	 Reserved		;

// };

// struct tms_
// {
// 	char	DPID(Data Points Block ID)		[12];
// 	char	TNDP(Number of Data Points)		;
// 	short data[20000];

// };

// struct tms_
// {
// 	char	 Key Events Block ID		;
// 	char	 Number of Key Events		;
// };

// struct tms_event
// {
// 	int	Event Distance		;
// 	int	 Event Number		;
// 	float	 Attenuation Coefficient		;
// 	float	 Event Loss		;
// 	float	 Event Reflectance		;
// 	float	 Event Total Link Loss		;

// };

// struct tms_
// {
// char	OTDR Test Result Information 		;
// float	 Chain Range		;
// float	Chain Loss		;
// float	Chain Attenuation		;

// };
// //ID_CMD_OSW_SWITCH111111111111111111111111111		20	
// struct tms_
// {
// int32_t frame		;
// int32_t slot		;
// int	Device Type		;
// int	Switch Type		;
// int	Switch		;

// };
// //ID_CMD_OSW_SWITCH2222222222222222222222222222		21	
// struct tms_
// {
// int32_t frame		;
// int32_t slot		;
// int	Device Type		;
// int	Switch Type		;
// int	Switch		;

// };


// //ID_ALARM_OPM			22	
struct tms_alarm_opm
{
	int	alarm_type;
	int32_t frame;
	int32_t slot;
	int32_t	count;
};
struct tms_alarm_opm_val
{
	int32_t port;
	int32_t	levelx;
	int32_t	power;
	uint8_t	time[20];
};

struct tms_alarm_opm_change
{
	int	alarm_type;
	int32_t frame;
	int32_t slot;
	int32_t alarm_cnt;
	int32_t	count;
};

// ID_REPORT_OLP_ACTION
struct tms_report_olp_action
{
	int32_t  frame;
	int32_t  slot;
	int32_t  type;
	int32_t  sw_type;///< 0表示人工切换，1表示自动保护倒换，2表示保护返回。
	int32_t  sw_port;     ///< 0表示切换到主路，1表示切换到备路
};
// // 与上面的合并
// struct tms_
// {
// int	Alarm Type		;
// int32_t frame		;
// int32_t slot		;
// int	Num		;
// };

// struct tms_
// {
// int32_t port_num		;
// int	Alarm Level		;
// int	Current Power		;
// char	Time[20]		;
// };

// //ID_ALARM_LINE			23	
// struct tms_
// {
// int	Alarm Type		;
// int	Alarm Level		;
// int32_t frame		;
// int32_t slot		;
// int32_t port_num		;
// int	Alarm Position		;
// char	Time[20]		;
// int	Reserved		;

// };
//ID_ALARM_HW				24	
struct tms_alarm_hw
{
	int	alarm_type;
	int	count;
};

struct tms_alarm_hw_val
{
	int32_t level;
	int32_t frame;
	int32_t slot;
	uint8_t	reason[128];
	uint8_t	time[20];
};
// //ID_CMD_OTDR_CYC			25	
// struct tms_
// {
// 	int CycTest Curve Num[2000];
// 	// TODO
// };

// //ID_CMD_SMS_TEXT			26	
// struct tms_sms_text
// {
// 	char       phone_num[16];
// 	uint32_t   len;
// 	short      msg[70];
// };
// //ID_CMD_SMS_ERROR		27	
struct tms_sms_state
{
	char     phone[16];
	uint32_t state;
};
struct tms_adjust_tm
{
	char     tm[20];
};
// //ID_GET_VERSION		28	
// struct tms_
// {
// int32_t frame		;
// 	int32_t slot		;
// 	int32_t type;
// };
// //ID_SET_VERSION		29	
// struct tms_
// {
// int32_t frame		;
// 	int32_t slot		;
// 	int32_t type;
// 	char version;

// };
// //ID_CMD_TIME				30	
// struct tms_
// {
// 	char Current Time[20];
// };
//ID_CMD_ACK				31	
struct tms_ack
{
	int32_t errcode;
	int32_t cmdid;
	int32_t reserve1;
	int32_t reserve2;
	int32_t reserve3;
	int32_t reserve4;
};
// //ID_CMD_TICK				32	
// struct tms_
// {

// };




///< 设备基本类型描述
// 用于刷新设备连接状态图形界面，
struct tms_devbase
{
	int fd;
	int32_t  frame;
	int32_t  slot;
	int32_t type;
	int32_t port;
};


////////////////////////////////////////////////////////////////////////
// Section 5 TMSxx管理算法相关数据结构

///< 网关表
struct tms_manage
{
	int fd[MANAGE_COUNT];       ///< 第三方开发的网管
	int fdtc[MANAGE_COUNT];		///< 支持控制台的网管数
};

///< 设备上下文描述
// 主要处理心跳计数，用与当设备长时间断开连接后能快速响应
// 应用层“刷新”命令读取tick，如果tick一直不变表示死链接
struct tms_context
{
	int fd;          ///<socket描述符
	uint32_t frame;  ///<机框号
	uint32_t slot;   ///<槽位号
	uint32_t tick;   ///<心跳计数
	struct tms_callback *ptcb;
	// void     *ptr_analyse_arr;  ///<应用层不要读取任何值，tmsxx协议内部使用
	struct tms_analyse_array *ptr_analyse_arr;
};


#include "bipbuffer.h"
///< 应用程序
struct tmsxx_app
{
	// int    fd;                   ///<socket fd
	int    morebyte;             ///<为防止bipbuffer环形缓存浪费
	struct bipbuffer bb;         ///<glink接收环形缓存，长度初始化后永远不变

	int enable_lbb;
	int lbyte;					 ///<
	struct bipbuffer lbb;		 ///<临时环形缓存，仅用于接收大于bb长度的帧
	struct tms_context context;  ///<tms包描述符

};

struct tms_callback
{
	int32_t (*ptrfun)(int fd, int cmdid, void *ptr);
	int32_t (*ptrfun1)(int fd, int cmdid, void *ptr);
	int32_t (*pf_OnCopy2Use)(char *data, int32_t datalen, int msec);
	int32_t (*pf_OnGetDeviceComposition)(struct tms_context *pcontext, int8_t *pdata, int32_t len);
	int32_t (*pf_OnRetDeviceComposition)(struct tms_context *pcontext, int8_t *pdata, int32_t len,
			struct tms_dev_composition *pcfg_hr,
			struct tms_dev_composition_val *plist);
	int32_t (*pf_OnCommand)(struct tms_context *pcontext, char *strcmd, int32_t slen);
	int32_t (*pf_OnCUNoteNet)(void);
	int32_t (*pf_OnCUNoteManageConnect)(int32_t state);
	int32_t (*pf_OnRetSerialNumber)(uint8_t (*psn)[128]);
	int32_t (*pf_OnSetIPAddress)(uint8_t (*ip)[16], uint8_t (*mask)[16], uint8_t (*gw)[16]);
	int32_t (*pf_OnRetOTDRTest)(struct tms_context *pcontext,
			struct tms_retotdr_test_hdr   *ptest_hdr,
			struct tms_retotdr_test_param *ptest_param,
			struct tms_retotdr_data_hdr   *pdata_hdr,
			struct tms_retotdr_data_val   *pdata_val,
			struct tms_retotdr_event_hdr  *pevent_hdr,
			struct tms_retotdr_event_val  *pevent_val,
			struct tms_retotdr_chain      *pchain);

	int32_t (*pf_OnCfgOTDRRef)(struct tms_context *pcontext,
			struct tms_retotdr_test_param *ptest_param,
			struct tms_retotdr_data_hdr   *pdata_hdr,
			struct tms_retotdr_data_val   *pdata_val,
			struct tms_retotdr_event_hdr  *pevent_hdr,
			struct tms_retotdr_event_val  *pevent_val,
			struct tms_retotdr_chain      *pchain,
			struct tms_cfg_otdr_ref_val   *pref_data);
};


///< 回调函数列表结构，采用函数指针方法，避免多个if、switch的低效率
struct tms_analyse_array
{
	int (*ptrfun)(struct tms_context *pcontext, int8_t *pdata, int32_t len);
	int dowhat;
};



////////////////////////////////////////////////////////////////////////
// Section 6 MCU与业务板通信接口
int32_t tms_Tick(int fd,struct glink_addr *paddr);
int32_t tms_Update(
		int fd,	
		struct glink_addr *paddr,
		int32_t frame, 
		int32_t slot,
		int32_t type,
		uint8_t (*fname)[256],
		int32_t flen,
		uint8_t *pdata);
void tms_Trace(struct glink_addr *paddr,char *strout, int32_t len, int level);
int32_t tms_Command(
	int fd, 
	struct glink_addr *paddr,
	char *strcmd, 
	int32_t len);

// 应用层不需要调用该函数
int32_t tms_MCUtoDevice(
	int     fd, 
	struct glink_addr *paddr,
	int32_t frame, 
	int32_t slot,
	int32_t type,
	int32_t port,
	int32_t cmdID,
	int32_t len);
// 一下函数间接调用tms_MCUtoDevice
int32_t tms_MCU_GetDeviceType(
	int fd,
	struct glink_addr *paddr);//, int32_t frame, int32_t slot);

int32_t tms_MCU_RetDeviceType(
	int     fd,
	struct glink_addr *paddr,
	int32_t frame, 
	int32_t slot, 
	int32_t type, 
	int32_t port);
// int32_t tms_MCU_GetOPMRayPower(
int32_t tms_MCU_GetOPMAlarm(
	int     fd, 
	struct glink_addr *paddr,
	int32_t frame, 
	int32_t slot);
// int32_t tms_MCU_GetOLPRayPower(
int32_t tms_MCU_GetOLPAlarm(
	int     fd, 
	struct glink_addr *paddr,
	int32_t frame, 
	int32_t slot);
int32_t tms_MCU_OSWSwitch(
	int     fd, 
	struct glink_addr *paddr,
	int32_t frame, 
	int32_t slot,
	int32_t port);
int32_t tms_MCU_OLPReqOTDRTest(
	int     fd, 
	struct glink_addr *paddr,
	int32_t frame, 
	int32_t slot,
	int32_t port);
int32_t tms_MCU_OLPStartOTDRTest(
	int     fd, 
	struct glink_addr *paddr,
	int32_t frame, 
	int32_t slot,
	int32_t port);
int32_t tms_MCU_OLPFinishOTDRTest(
	int     fd, 
	struct glink_addr *paddr,
	int32_t frame, 
	int32_t slot,
	int32_t port);
int32_t tms_Tick(int fd,struct glink_addr *paddr);




////////////////////////////////////////////////////////////////////////
// 网管与MCU之间的通信
int32_t tms_SetIPAddress(
	int     fd, 
	struct glink_addr *paddr,
	uint8_t *ip, 
	uint8_t *mask, 
	uint8_t *gw);
int32_t tms_GetSerialNumber(int fd, struct glink_addr *paddr);
int32_t tms_RetSerialNumber(int fd, struct glink_addr *paddr, uint8_t (*psn)[128]);
int32_t tms_CfgSMSAuthorization(
		int fd,
		struct glink_addr *paddr,
		int32_t count,
		struct tms_cfg_sms_val *smslist);
uint32_t tms_RetDeviceComposition(
		int      fd, 
		struct glink_addr *paddr,
		uint32_t count, 
		struct tms_dev_composition_val *list);

uint32_t tms_GetDeviceComposition(
		int     fd,
		struct glink_addr *paddr);
int32_t tms_RetVersion(
		int fd,	
		struct glink_addr *paddr,
		int32_t frame, 
		int32_t slot,
		int32_t type,
		uint8_t  *version);
int32_t tms_Update(
		int fd,	
		struct glink_addr *paddr,
		int32_t frame, 
		int32_t slot,
		int32_t type,
		uint8_t (*fname)[256],
		int32_t flen,
		uint8_t *pdata);
int32_t tms_Ack(
	int fd,
	struct glink_addr *addr,
	int errcode,
	int cmdid);
int32_t tms_AckEx(
	int fd, 
	struct glink_addr *paddr, 
	struct tms_ack *pACK);

int32_t tms_CfgOPMRefLevel(
		int fd,
		struct glink_addr *paddr,
		int32_t frame,
		int32_t slot,
		int32_t count,
		struct tms_cfg_opm_ref_val *list);
int32_t tms_CfgOLPRefLevel(
		int fd, 
		struct glink_addr *paddr, 
		int32_t frame, 
		int32_t slot, 
		int32_t count, 
		struct tms_cfg_olp_ref_val *list);

int32_t tms_GetOPMOP(
		int fd, 
		struct glink_addr *paddr,
		int32_t frame,  
		int32_t slot);
// int32_t tms_RetOPMOP(
// 		int fd,
// 		int32_t frame,
// 		int32_t slot,
// 		int32_t count,
// 		struct tms_any_op_val *opmList);
int32_t tms_CfgOLPMode(
		int fd, 
		struct glink_addr *paddr, 
		int32_t frame, 
		int32_t slot, 
		int32_t mode, 
		int32_t backtime, 
		int32_t protect);
int32_t tms_RetOPMOP(
		int fd,
		struct glink_addr *paddr,
		int32_t frame,
		int32_t slot,
		int32_t count,
		struct tms_any_op_val *list);
int32_t tms_MCU_OLPSwitch(
	int     fd, 
	struct glink_addr *paddr, 
	int32_t frame, 
	int32_t slot, 
	int32_t sw_port);
int32_t tms_ReportOLPAction(
		int fd, 
		struct glink_addr *paddr, 
		int32_t frame, 
		int32_t slot, 
		int32_t sw_type, 
		int32_t sw_port);
int32_t tms_AlarmOPM(
		int fd,
		struct glink_addr *paddr,
		int32_t frame,
		int32_t slot,
		// int     alarm_type,
		int32_t count,
		struct tms_alarm_opm_val *list);

int32_t tms_AlarmOPMChange(
		int     fd,
		struct glink_addr *paddr,
		int32_t frame,
		int32_t slot,
		// int     alarm_type,
		int 	alarm_cnt,
		int32_t count,
		struct  tms_alarm_opm_val *list);
int32_t tms_GetVersion(
		int fd,	
		struct glink_addr *paddr,
		int32_t frame, 
		int32_t slot,
		int32_t type);

void tms_Trace(struct glink_addr *paddr,char *strout, int32_t len, int level);
#define tms_Trace3(a,b) tms_Trace(a,b,LEVEL_UI)
// int32_t tms_Analyse(int fd, int8_t *pdata, int32_t len);
int32_t tms_Analyse(struct tms_context *pcontext, int8_t *pdata, int32_t len);


int32_t tms_GetSockFD(int32_t frame, int32_t slot);
int32_t tms_GetDevBaseByFd(int fd, struct tms_devbase *pout);
int32_t tms_GetDevBaseByLocation(int32_t frame, int32_t slot,struct tms_devbase *pout);
int32_t tms_GetTCManageFd(
		int (*fd)[MANAGE_COUNT]);
int32_t tms_GetManageFd();
int32_t tms_RemoveDev(int fd);
int32_t tms_ManageCount();
int32_t tms_AddManage(int fd, int32_t type);
int32_t tms_DelManage(int fd);
// int32_t tms_GetFrame(int32_t frame, struct tms_devbase pdev[12]);
int32_t tms_GetFrame(int32_t frame, struct tms_devbase (*pdev)[12]);

void tms_Init();
int32_t tms_CfgMCUOSWCycle(
		int fd, 
		struct glink_addr *paddr, 
		int32_t frame, 
		int32_t slot, 
		int32_t count, 
		struct tms_cfg_mcu_osw_cycle_val *oswCycleList);
int32_t tms_CfgMCUOSWPort(
		int fd,
		struct glink_addr *paddr,		
		int32_t frame,
		int32_t slot,
		int32_t port,
		uint8_t (*dev_name)[64],
		uint8_t (*cable_name)[64],
		uint8_t (*start_name)[64],
		uint8_t (*end_name)[64]);
int32_t tms_CfgMCUOLPPort(
		int fd,
		struct glink_addr *paddr,		
		int32_t frame,
		int32_t slot,
		int32_t port,
		uint8_t (*dev_name)[64],
		uint8_t (*cable_name)[64],
		uint8_t (*start_name)[64],
		uint8_t (*end_name)[64]);
int32_t tms_CfgMCUUniteAnyOSW(
		int fd,
		struct glink_addr *paddr,
		int32_t any_frame,
		int32_t any_slot,
		int32_t any_type,
		int32_t cmdID,
		int32_t	count,
		struct tms_cfg_mcu_u_any_osw_val *list);

int32_t tms_CfgMCUOSWPortClear(
		int fd, 
		struct glink_addr *paddr, 
		int32_t frame, 
		int32_t any_slot, 
		int32_t	count, 
		struct tms_cfg_mcu_any_port_clear_val *list);
int32_t tms_CfgMCUOPMPortClear(
		int fd,
		struct glink_addr *paddr,
		int32_t frame,
		int32_t any_slot,
		int32_t	count,
		struct tms_cfg_mcu_any_port_clear_val *list);
int32_t tms_CfgMCUOLPPortClear(
		int fd,
		struct glink_addr *paddr,
		int32_t frame,
		int32_t any_slot,
		int32_t	count,
		struct tms_cfg_mcu_any_port_clear_val *list);
int32_t tms_CfgMCUUniteOPMOSW(
		int fd, 
		struct glink_addr *paddr, 
		int32_t frame, 
		int32_t slot, 
		int32_t	count, 
		struct tms_cfg_mcu_u_any_osw_val *list);
int32_t tms_CfgMCUUniteOPMOSWClear(
		int fd,
		struct glink_addr *paddr,
		int32_t any_frame,
		int32_t any_slot,
		int32_t	count,
		struct tms_cfg_mcu_any_port_clear_val *list);
int32_t tms_CfgMCUUniteOLPOSWClear(
		int fd,
		struct glink_addr *paddr,
		int32_t frame,
		int32_t slot,
		int32_t	count,
		struct tms_cfg_mcu_any_port_clear_val *list);
int32_t tms_CfgMCUUniteOLPOSW(
		int fd,
		struct glink_addr *paddr,
		int32_t frame,
		int32_t slot,
		int32_t	count,
		struct tms_cfg_mcu_u_any_osw_val *list);


void tms_SetDoWhat(int cmdh, int count, int *arr);
int32_t tms_SendSMS(
		int fd,
		struct glink_addr *paddr,
		char (*phone)[16],
		int32_t count,
		wchar_t *text);
int32_t tms_GetOTDRTest(
		int fd,
		struct glink_addr *paddr,
		int32_t frame,
		int32_t slot,
		// int32_t type,
		int32_t port,
		struct tms_getotdr_test_param *val);
int32_t tms_GetOTDRTestCycle(
		int fd,
		struct glink_addr *paddr,
		int32_t frame,
		int32_t slot,
		// int32_t type,
		int32_t port,
		struct tms_getotdr_test_param *val);
int32_t tms_AnyRetOTDRTest(
		int fd,
		struct glink_addr *paddr,
		struct tms_retotdr_test_hdr   *ptest_hdr,		
		struct tms_retotdr_test_param *ptest_param,

		struct tms_retotdr_data_hdr   *pdata_hdr,
		struct tms_retotdr_data_val   *pdata_val,

		struct tms_retotdr_event_hdr  *pevent_hdr,
		struct tms_retotdr_event_val  *pevent_val,

		struct tms_retotdr_chain      *pchain,
		int32_t cmdID);
int32_t tms_RetOTDRTest(
		int fd,
		struct glink_addr *paddr,
		struct tms_retotdr_test_hdr *ptest_hdr,		
		struct tms_retotdr_test_param *ptest_param,

		struct tms_retotdr_data_hdr *pdata_hdr,
		struct tms_retotdr_data_val *pdata_val,

		struct tms_retotdr_event_hdr *pevent_hdr,
		struct tms_retotdr_event_val *pevent_val,

		struct tms_retotdr_chain    *pchain);
int32_t tms_RetOTDRTestCycle(
		int fd,
		struct glink_addr *paddr,
		struct tms_retotdr_test_hdr   *ptest_hdr,		
		struct tms_retotdr_test_param *ptest_param,

		struct tms_retotdr_data_hdr   *pdata_hdr,
		struct tms_retotdr_data_val   *pdata_val,

		struct tms_retotdr_event_hdr  *pevent_hdr,
		struct tms_retotdr_event_val  *pevent_val,

		struct tms_retotdr_chain      *pchain);

int32_t tms_CfgOTDRRef(
		int fd,
		struct glink_addr *paddr,
		struct tms_otdr_ref_hdr       *pref_hdr,		
		struct tms_retotdr_test_param *ptest_param,

		struct tms_retotdr_data_hdr   *pdata_hdr,
		struct tms_retotdr_data_val   *pdata_val,

		struct tms_retotdr_event_hdr  *pevent_hdr,
		struct tms_retotdr_event_val  *pevent_val,

		struct tms_retotdr_chain      *pchain,
		struct tms_cfg_otdr_ref_val   *pref_date);
uint32_t tms_GetDeviceComposition(
		int     fd,
		struct glink_addr *paddr);
int32_t tms_Command(int fd, struct glink_addr *paddr, char *strcmd, int32_t len);
int32_t tms_AdjustTime(
	int fd,
	struct glink_addr *paddr,
	int8_t (*time)[20]);

int32_t tms_GetOPMOP(
		int fd, 
		struct glink_addr *paddr, 
		int32_t frame,  
		int32_t slot);
int32_t tms_GetOLPOP(
		int fd, 
		struct glink_addr *paddr, 
		int32_t frame,  
		int32_t slot);
int32_t tms_RetOLPOP(
		int fd, 
		struct glink_addr *paddr, 
		int32_t frame, 
		int32_t slot, 
		int32_t count, 
		struct tms_any_op_val *list);
int32_t tms_RetOTDRCycle(
		int    fd,
		struct glink_addr *paddr,
		struct tms_retotdr_test_hdr   *ptest_hdr,		
		struct tms_retotdr_test_param *ptest_param,

		struct tms_retotdr_data_hdr   *pdata_hdr,
		struct tms_retotdr_data_val   *pdata_val,

		struct tms_retotdr_event_hdr  *pevent_hdr,
		struct tms_retotdr_event_val  *pevent_val,

		struct tms_retotdr_chain      *pchain);
void tms_Print_tms_cfg_opm_ref_val(struct tms_cfg_opm_ref_val *pref_hdr,int count);
void tms_Print_tms_cfg_olp_ref_val(struct tms_cfg_olp_ref_val *pref_hdr,int count);
void tms_Print_tms_otdr_ref_hdr(struct tms_otdr_ref_hdr *pref_hdr);
void tms_Print_tms_getotdr_test_hdr(struct tms_getotdr_test_hdr *ptest_hdr);
void tms_Print_tms_retotdr_test_hdr(struct tms_retotdr_test_hdr *ptest_hdr);
void tms_Print_tms_retotdr_event(struct tms_retotdr_event_hdr *pevent_hdr, struct tms_retotdr_event_val *pevent_val);
void tms_Print_tms_retotdr_test_param(struct tms_retotdr_test_param *ptest_param);
void tms_Print_tms_retotdr_chain(struct tms_retotdr_chain *pchain);
void tms_Print_tms_cfg_otdr_ref_val(struct tms_cfg_otdr_ref_val *pref_data);
void tms_SaveOTDRData(
			struct tms_retotdr_test_hdr   *ptest_hdr,
			struct tms_retotdr_test_param *ptest_param,
			struct tms_retotdr_data_hdr   *pdata_hdr,
			struct tms_retotdr_data_val   *pdata_val,
			struct tms_retotdr_event_hdr  *pevent_hdr,
			struct tms_retotdr_event_val  *pevent_val,
			struct tms_retotdr_chain      *pchain,
			char *path,
			int32_t flag);
/**
 * @brief	得到OTDR返回数据的指针，pdata是连续的缓存区，如果采用环形缓存必须保证
 数据起始部分不在环形缓存末尾，结束部分在环形缓存开头。pdata的数据内容是glink协议中
 命令ID、数据长度、数据包内容三部分
 * @remarks	<h2><center>TMS_PTR_OTDRTest的使用方法建议查看下面的例程链接</center></h2>\n
 */

#define TMS_PTR_OTDRTest(pdata, ptest_hdr, ptest_param, pdata_hdr, pdata_val, pevent_hdr, pevent_val, pchain) \
	ptest_hdr   = (struct tms_retotdr_test_hdr   *)(pdata + 8 ); \
	ptest_param = (struct tms_retotdr_test_param *)(((char*)ptest_hdr)   + sizeof(struct tms_retotdr_test_hdr)); \
	pdata_hdr   = (struct tms_retotdr_data_hdr   *)(((char*)ptest_param) + sizeof(struct tms_retotdr_test_param)); \
	pdata_val   = (struct tms_retotdr_data_val   *)(((char*)pdata_hdr) + sizeof(struct tms_retotdr_data_hdr)); \
	pevent_hdr  = (struct tms_retotdr_event_hdr  *)(((char*)pdata_val) + sizeof(struct tms_retotdr_data_val) * (pdata_hdr->count)); \
	pevent_val  = (struct tms_retotdr_event_val  *)(((char*)pevent_hdr) + sizeof(struct tms_retotdr_event_hdr)); \
	pchain      = (struct tms_retotdr_chain      *)(((char*)pevent_val) + sizeof(struct tms_retotdr_event_val) * (pevent_hdr->count));

int32_t tms_GetTempFd();
int32_t tms_ClearSMSAuthorization(
	int     fd, 
	struct glink_addr *paddr);
// int32_t tms_AddManage(int fd, int32_t type);
#ifdef __cplusplus
}
#endif

#endif