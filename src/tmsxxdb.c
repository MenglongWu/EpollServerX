/**
 ******************************************************************************
 * @file	tmsxxdb.c
 * @brief	TMSxx 光缆监控系统数据库操作
	
	只实现简单的插入、删除、查询功能，没有任何事务处理、回滚、触发器\n
	select语句返回永远是能查询到的所有行，优化需要加入 select  * from tb_name limit xn offset yn;
	对返回行分页，目前任何select表单操作都没有分页\n
	根据表的大小对应用程序内存管理轻微优化，select有两种操作方式 sqlite3_get_table 和 sqlite3_step\n
	对于内容短小的表用sqlite3_get_table，tmsdb_Select_xxx 返回找到的行数，并将每行信息赋予output指针，
	调用者必须调用free(output)释放内存\n
	对于内容可能有数百条，每行耗费34K的表如 tb_otdr_ref 采用sqlite3_prepare、sqlite3_step结合回调函数
	返回给调用者，调用者在回调函数内操作表记录，这种方式调用者无任何free操作\n
	无论采用以上2种方式的哪一种，都避免不了 sqlite3 查询过程的内存开销，除非加入分页操作



 @section 函数参数规则简述
 	关于 pcondition（条件）和 pmask（掩码）作用：\n
 	很多函数都有 pcondition 和 pmask ，两个是相同的结构体，只有当mask对于位不为0，此时
 	pcondition才有效。
 	例如：对 tb_demo 表有3列col1、col2、col3，用下面结构体描述\n
 	struct demo				\n
 	{ 						\n
		int col1;			\n
		int col2;			\n
		int col3;			\n
 	};						\n
	struct demo condition,mask;										\n
	bzero(&mask,  sizeof(struct demo));								\n
																	\n
	mask->col1 = 1;mask->col2 = 1;									\n
	condition.col1 = 10;condition.col2 = 20;condition.col3 = 30;	\n
 	select_fun(&condition, &mask);									\n
 	此时会select_fun会生成如下sql语句								\n
 	select * from tb_demo where (col1 = 10 and col2 = 20);			\n
	condition.col3 的内容被忽略
	\n\n
	<h3>不同的表可用的掩码数不同，若某表由20列，但大多数时应用程序只以其中5列作为
	搜索、删除关键字，于是在函数实现内部仅为这5列提供操作掩码，至于那些列有掩码
	功能，请直接查阅相应函数调用规则</h3>

 *
|     Version    |     Author        |      Date      |    Content   
| :------------: | :---------------: | :------------: | :------------
|     V1.0       |    Name           |   xxxx-xx-xx   | 1.xxxxx    
|                |                   |                | 2.xxxxx

 @section Platform
	-# Linux 2.6.35-22-generic #33-Ubuntu SMP Sun Sep 19 20:34:50 UTC 2010 i686 GNU/Linux
	-# gcc-4.7.4 gcc/g++
 @section Library
	-# libsqlite3.so

- 2015-5-27, Menglong Wu, DreagonWoo@163.com
 	- table tb_common , \n
	 	table tb_sn , \n
		table tb_sms ,\n
		table tb_composition ,\n
		table tb_dev_map ,\n
		table tb_any_unit_osw ,\n
		table tb_osw_cyc ,\n
		table tb_otdr_rollcall ,\n
		table tb_otdr_ref ,\n
		table tb_otdr_his_data ,\n
	- 数据表细节查看 《tmsxxdb_create.sql》和《sqlite3数据库设计.xlsx》


*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <sqlite3.h>
#include <protocol/tmsxx.h>
#include <tmsxxdb.h>
#include <string.h>
#include <errno.h>

#define DB_PATH "/etc/tmsxx.db"
////////////////////////////////////////////////////////////////////////////////
// 数据库创建

// 创建表 tb_sn
static int _tmsdb_CreateTable_common()
{
	// TODO: 创建表 tb_sn
	sqlite3 *db;
	int rc;
	char *errMsg = NULL;
	char sql[] = "create table tb_common ( \n\
	 id INTEGER PRIMARY KEY ,\n\
	 val1 INT ,\n\
	 val2 INT ,\n\
	 val3 INT ,\n\
	 val4 INT ,\n\
	 val5 INT ,\n\
	 val6 INT ,\n\
	 val7 INT ,\n\
	 val8 INT ,\n\
	 val9 INT ,\n\
	 val10 INT ,\n\
	 val11 INT ,\n\
	 val12 INT ,\n\
	 bin_len INT ,\n\
	 bin_data BLOB \n\
	);";


	rc = sqlite3_open(DB_PATH , &db);
	printf("_tmsdb_CreateTable_common :%d\n", rc);
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	
	if (errMsg) {
		printf("%s\n", errMsg);
		sqlite3_free(errMsg);	
	}
	
	sqlite3_close(db);
	return 0;
}

// 创建表 tb_sn
static int _tmsdb_CreateTable_sn()
{
	// TODO: 创建表 tb_sn
	sqlite3 *db;
	int rc;
	char *errMsg = NULL;
	char sql[] = "create table tb_sn ( \n\
	sn CHAR(128) \n\
	);   ";


	rc = sqlite3_open(DB_PATH , &db);
	printf("_tmsdb_CreateTable_sn :%d\n", rc);
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	
	if (errMsg) {
		printf("%s\n", errMsg);
		sqlite3_free(errMsg);	
	}
	
	sqlite3_close(db);
	return 0;
}
// 创建表 tb_sms
static int _tmsdb_CreateTable_sms()
{
	// TODO: 创建表 tb_sms
	sqlite3 *db;
	int rc;
	char *errMsg = NULL;
	char sql[] = "create table tb_sms (  \n\
	id INTEGER PRIMARY KEY ,\n\
	date INT , \n\
	phone CHAR(16), \n\
	type INT , \n\
	level INT \n\
	);";


	rc = sqlite3_open(DB_PATH , &db);
	printf("_tmsdb_CreateTable_sms :%d\n", rc);
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	if (errMsg) {
		printf("%s\n", errMsg);
		sqlite3_free(errMsg);
	}
	sqlite3_close(db);
	return 0;
}
// 创建表 tb_composition
static int _tmsdb_CreateTable_composition()
{
	// TODO: 创建表 tb_composition
	sqlite3 *db;
	int rc;
	char *errMsg = NULL;
	char sql[] = "create table tb_composition (  \n\
	id INTEGER PRIMARY KEY ,\n\
	frame INT , \n\
	slot INT , \n\
	type INT , \n\
	port INT \n\
	);   ";


	rc = sqlite3_open(DB_PATH , &db);
	printf("_tmsdb_CreateTable_composition :%d\n", rc);
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	if (errMsg) {
		printf("%s\n", errMsg);
		sqlite3_free(errMsg);
	}
	sqlite3_close(db);
	return 0;
}
// 创建表 tb_dev_map
static int _tmsdb_CreateTable_dev_map()
{
	// TODO: 创建表 tb_dev_map
	sqlite3 *db;
	int rc;
	char *errMsg = NULL;
	char sql[] = "create table tb_dev_map (  \n\
	id INTEGER PRIMARY KEY ,\n\
	any_frame INT , \n\
	any_slot INT , \n\
	any_type INT , \n\
	any_port INT , \n\
	dev_name CHAR(64) , \n\
	cable_name CHAR(64) , \n\
	start_name CHAR(64) , \n\
	end_name CHAR(64)  \n\
	);   ";


	rc = sqlite3_open(DB_PATH , &db);
	printf("_tmsdb_CreateTable_dev_map :%d\n", rc);
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	if (errMsg) {
		printf("%s\n", errMsg);
		sqlite3_free(errMsg);
	}
	sqlite3_close(db);
	return 0;
}
// 创建表 tb_any_unit_osw
static int _tmsdb_CreateTable_any_unit_osw()
{
	// TODO: 创建表 tb_any_unit_osw
	sqlite3 *db;
	int rc;
	char *errMsg = NULL;
	char sql[] = "create table tb_any_unit_osw (   \n\
	id INTEGER PRIMARY KEY ,\n\
	any_frame INT ,  \n\
	any_slot INT ,  \n\
	any_type INT ,  \n\
	any_port INT ,  \n\
	osw_frame INT ,  \n\
	osw_slot INT ,  \n\
	osw_type INT ,  \n\
	osw_port INT   \n\
	);   ";


	rc = sqlite3_open(DB_PATH , &db);
	printf("_tmsdb_CreateTable_any_unit_osw :%d\n", rc);
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	if (errMsg) {
		printf("%s\n", errMsg);
		sqlite3_free(errMsg);
	}
	sqlite3_close(db);
	return 0;
}
// 创建表 tb_osw_cyc
static int _tmsdb_CreateTable_osw_cyc()
{
	// TODO: 创建表 tb_osw_cyc
	sqlite3 *db;
	int rc;
	char *errMsg = NULL;
	char sql[] = "create table tb_osw_cyc ( \n\
	id INTEGER PRIMARY KEY ,\n\
	osw_frame INT ,\n\
	osw_slot INT ,\n\
	osw_type INT ,\n\
	osw_port INT ,\n\
	iscyc INT ,\n\
	interval INT \n\
	);   ";


	rc = sqlite3_open(DB_PATH , &db);
	printf("_tmsdb_CreateTable_osw_cyc :%d\n", rc);
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	if (errMsg) {
		printf("%s\n", errMsg);
		sqlite3_free(errMsg);
	}
	sqlite3_close(db);
	return 0;
}
// 创建表 tb_otdr_rollcall
static int _tmsdb_CreateTable_otdr_rollcall()
{
	// TODO: 创建表 tb_otdr_rollcall
	sqlite3 *db;
	int rc;
	char *errMsg = NULL;
	char sql[] = "create table tb_otdr_rollcall (  \n\
	id INTEGER PRIMARY KEY ,\n\
	osw_frame INT , \n\
	osw_slot INT , \n\
	osw_type INT , \n\
	osw_port INT , \n\
	hdr_reserve0 INT , \n\
	\n\
	rang INT , \n\
	wl INT , \n\
	pw INT , \n\
	time INT , \n\
	gi REAL , \n\
	end_threshold REAL , \n\
	none_reflect_threshold REAL , \n\
	param_reserve0 INT , \n\
	param_reserve1 INT , \n\
	param_reserve2 INT  \n\
	);";


	rc = sqlite3_open(DB_PATH , &db);
	printf("_tmsdb_CreateTable_otdr_rollcall :%d\n", rc);
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	if (errMsg) {
		printf("%s\n", errMsg);
		sqlite3_free(errMsg);
	}
	sqlite3_close(db);
	return 0;
}
// 创建表 tb_otdr_ref
static int _tmsdb_CreateTable_otdr_ref()
{
	// TODO: 创建表 tb_otdr_ref
	sqlite3 *db;
	int rc;
	char *errMsg = NULL;
	char sql[] = " create table tb_otdr_ref (     \n\
	id INTEGER PRIMARY KEY ,\n\
	osw_frame INT ,    \n\
	osw_slot INT ,    \n\
	osw_type INT ,    \n\
	osw_port INT ,    \n\
	otdr_port INT ,    \n\
	strid CHAR(20) ,    \n\
	   \n\
	rang INT ,    \n\
	wl INT ,    \n\
	pw INT ,    \n\
	time INT ,    \n\
	gi REAL ,    \n\
	end_threshold REAL ,    \n\
	none_reflect_threshold REAL ,    \n\
	param_reserve0 INT ,    \n\
	param_reserve1 INT ,    \n\
	param_reserve2 INT ,    \n\
	   \n\
	dpid CHAR(12) ,    \n\
	sample_count INT ,    \n\
	   \n\
	sample_data BLOB ,    \n\
	   \n\
	eventid CHAR(12) ,    \n\
	event_count INT ,    \n\
	   \n\
	event_data BLOB ,    \n\
	   \n\
	   \n\
	   \n\
	   \n\
	   \n\
	   \n\
	   \n\
	ch_inf	CHAR(20)	, \n\
	ch_range	REAL	, \n\
	ch_loss	REAL	, \n\
	ch_att	REAL	, \n\
			 \n\
	leve0	INT	, \n\
	leve1	INT	, \n\
	leve2	INT	 \n\
	);";


	rc = sqlite3_open(DB_PATH , &db);
	printf("_tmsdb_CreateTable_otdr_ref :%d\n", rc);
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	if (errMsg) {
		printf("%s\n", errMsg);
		sqlite3_free(errMsg);
	}
	sqlite3_close(db);
	return 0;
}
// 创建表 tb_otdr_his_data
static int _tmsdb_CreateTable_otdr_his_data()
{
	// TODO: 创建表 tb_otdr_his_data
	sqlite3 *db;
	int rc;
	char *errMsg = NULL;
	char sql[] = "create table tb_otdr_his_data ( \n\
	id INTEGER PRIMARY KEY ,\n\
	 osw_frame INT ,\n\
	 osw_slot INT ,\n\
	 osw_type INT ,\n\
	 osw_port INT ,\n\
	 date CHAR(20) ,\n\
	 otdr_frame INT ,\n\
	 otdr_slot INT ,\n\
	 otdr_type INT ,\n\
	 otdr_port INT ,\n\
	   \n\
	 rang INT ,\n\
	 wl INT ,\n\
	 pw INT ,\n\
	 time INT ,\n\
	 gi REAL ,\n\
	 end_threshold REAL ,\n\
	 none_reflect_threshold REAL ,\n\
	 sample INT ,\n\
	 param_reserve1 INT ,\n\
	 param_reserve2 INT ,\n\
	   \n\
	 dpid CHAR(12) ,\n\
	 sample_count INT ,\n\
	   \n\
	 sample_data BLOB ,\n\
	   \n\
	 eventid CHAR(12) ,\n\
	 event_count INT ,\n\
	   \n\
	 event_data BLOB ,\n\
	 inf CHAR(20) ,\n\
	 range REAL ,\n\
	 loss REAL ,\n\
	 att REAL\n\
	);   ";


	rc = sqlite3_open(DB_PATH , &db);
	printf("_tmsdb_CreateTable_otdr_his_data :%d\n", rc);
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	if (errMsg) {
		printf("%s\n", errMsg);
		sqlite3_free(errMsg);
	}
	sqlite3_close(db);
	return 0;
}

/**
 * @brief	创建tmsxx数据库
 */
int tmsdb_CheckDb()
{
	// TODO: 检查数据库是否存在，不存在则创建
	sqlite3 *db;
	int rc;
	rc = sqlite3_open(DB_PATH, &db);
	printf("tmsdb_CheckDb rc = %d\n", rc);
	sqlite3_close(db);
	
	_tmsdb_CreateTable_common();
	_tmsdb_CreateTable_sn();
	_tmsdb_CreateTable_sms();
	_tmsdb_CreateTable_composition();
	_tmsdb_CreateTable_dev_map();
	_tmsdb_CreateTable_any_unit_osw();
	_tmsdb_CreateTable_osw_cyc();
	_tmsdb_CreateTable_otdr_rollcall();
	_tmsdb_CreateTable_otdr_ref();
	_tmsdb_CreateTable_otdr_his_data();
	errno = 0;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief	检查操作sqlite3数据库返回值
 * @param	rc 返回值
 * @param	szInfo 输出信息
 * @param	serrMsg sqlite3返回的错误文本
 * @param	db 打开的sqlite3数据库
 */
#define CHECK_RC(rc, szInfo, serrMsg, db)   \
		if (rc != SQLITE_OK)                 \
		{                                 \
			fprintf(stdout, "%s error: %s\n", szInfo, serrMsg); \
			sqlite3_free(serrMsg);       \
			sqlite3_close(db);            \
			return -1;	                  \
		}

/**
 * @brief	安全拷贝blob字符
 * @param	pstmt sqlite3_stmt指针
 * @param	index 第几列
 * @param	dst 目的地址
 * @param	max 最大拷贝数
 */

#define BLOB_COPY_S(pstmt, index, dst, max) \
{ \
		const void *pcol_data; \
		int col_byte; \
		pcol_data          = sqlite3_column_blob(pstmt,index); \
		if (pcol_data) { \
			col_byte = sqlite3_column_bytes(pstmt, index); \
			col_byte = col_byte > max ? max : col_byte; \
			memcpy(dst, pcol_data, col_byte); \
		} \
}

#ifdef _DEBUG
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	
	printf("count = %d\n", argc);
	for(i=0; i<argc; i++){ 
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}
#endif







/**
 * @brief	插入多行到 tb_common
 * @param	pcondition 插入内容，当pcondition不为NULL时插入
 * @param	pmask 搜索掩码 \n
 		无意义
 * @param	count 有多少行，pcondition必须是个数组
 * @retval	0 成功
 * @retval	-1 失败
 */
int tmsdb_Insert_common(
		tdb_common_t *pcondition, 
		tdb_common_t *pmask,
		int count)
{
	sqlite3 *db;

	// char **dbResult;
	// int row, col;
	// char *errMsg = NULL;
	int rc;
	// int index;

	char sql[2048];
	// char sqlid[36];
	char sqlval1[36];
	char sqlval2[36];
	char sqlval3[36];
	char sqlval4[36];
	char sqlval5[36];
	char sqlval6[36];
	char sqlval7[36];
	char sqlval8[36];
	char sqlval9[36];
	char sqlval10[36];
	char sqlval11[36];
	char sqlval12[36];
	char strlenpdata[36];
	char strpdata[36];

	sqlite3_stmt *pstmt;
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	
	if (pcondition == NULL) {
		snprintf(sql, 1024, "delete from tb_common;");
	}


	else if (pcondition != NULL) 
	{
		for (int i = 0; i < count; i++) {
			// 构造各个sql字段
			
			// snprintf(sqlid, 36, "%d",pcondition->id);
			snprintf(sqlval1, 36, "%d",pcondition->val1);
			snprintf(sqlval2, 36, "%d",pcondition->val2);
			snprintf(sqlval3, 36, "%d",pcondition->val3);
			snprintf(sqlval4, 36, "%d",pcondition->val4);
			snprintf(sqlval5, 36, "%d",pcondition->val5);
			snprintf(sqlval6, 36, "%d",pcondition->val6);
			snprintf(sqlval7, 36, "%d",pcondition->val7);
			snprintf(sqlval8, 36, "%d",pcondition->val8);
			snprintf(sqlval9, 36, "%d",pcondition->val9);
			snprintf(sqlval10, 36, "%d",pcondition->val10);
			snprintf(sqlval11, 36, "%d",pcondition->val11);
			snprintf(sqlval12, 36, "%d",pcondition->val12);
			snprintf(strlenpdata, 36, "%d",pcondition->lenpdata);
			snprintf(strpdata, 36, "?");

			// 汇总各sql字段
			snprintf(sql, 1024, "insert into tb_common values(null,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s);",
				sqlval1,sqlval2,sqlval3,sqlval4,sqlval5,sqlval6,
				sqlval7,sqlval8,sqlval9,sqlval10,sqlval11,sqlval12,
				strlenpdata,strpdata);
			
			printf("sql %s\n",sql);
			// 准备执行语句
			sqlite3_prepare(db, sql, -1, &pstmt, 0);
			// 插入pdata二进制数据
			if (sqlite3_bind_blob(pstmt, 1, pcondition->pdata, pcondition->lenpdata, NULL) != SQLITE_OK) {
				fprintf(stdin, "sqlite3_bind_blob");
			}
			if(sqlite3_step(pstmt) !=SQLITE_DONE) {
                		fprintf(stdin, "sqlite3_step error:%s", sqlite3_errmsg(db));
            }
			sqlite3_finalize(pstmt);
			pcondition++;
		}
	}
	sqlite3_close(db);

	return 0;
}

/**
 * @brief	插入 tb_sn
 * @see	
 */

int tmsdb_Insert_sn(
		tdb_sn_t *pcondition, 
		tdb_sn_t *pmask,
		int count)
{
	sqlite3 *db;

	// char **dbResult;
	// int row, col;
	char *errMsg = NULL;
	int rc;
	// int index;

	char sql[1024];

	
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	if (pcondition != NULL) {
		for (int i = 0; i < count; i++) {
			snprintf(sql, 1024, "insert into tb_sn (sn) values('%s');",
				pcondition->sn);
			rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
			CHECK_RC(rc, "tmsdb_Insert_sn", errMsg, db);
			pcondition++;
		}
	}
	printf("sql:%s\n", sql);
	sqlite3_close(db);

	return 0;
}

/**
 * @brief	插入多行到 tb_sms
 * @param	pcondition 插入内容，当pcondition不为NULL时插入
 * @param	pmask 搜索掩码 \n
 		无意义
 * @param	count 有多少行，pcondition必须是个数组
 * @retval	0 成功
 * @retval	-1 失败
 */
int tmsdb_Insert_sms(
		tdb_sms_t *pcondition, 
		tdb_sms_t *pmask,
		int count)
{
	sqlite3 *db;

	// char **dbResult;
	// int row, col;
	char *errMsg = NULL;
	int rc;
	// int index;

	char sql[1024];

	
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	if (pcondition != NULL) {
		for (int i = 0; i < count; i++) {
			snprintf(sql, 1024, "insert into tb_sms values(null,%d , '%s' , %d , %d);",
				pcondition->time,pcondition->phone,pcondition->type,pcondition->level);


			rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
			CHECK_RC(rc, "tmsdb_Insert_sms", errMsg, db);
			pcondition++;
		}
	}
	printf("sql:%s\n", sql);
	sqlite3_close(db);

	return 0;
}


/**
 * @brief	插入多行到 tb_composition
 * @param	pcondition 插入内容，当pcondition不为NULL时插入
 * @param	pmask 搜索掩码 \n
 		无意义
 * @param	count 有多少行，pcondition必须是个数组
 * @retval	0 成功
 * @retval	-1 失败
 */
int tmsdb_Insert_composition(
		tdb_composition_t *pcondition, 
		tdb_composition_t *pmask,
		int count)
{
	sqlite3 *db;

	// char **dbResult;
	// int row, col;
	char *errMsg = NULL;
	int rc;
	// int index;

	char sql[1024];

	
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	if (pcondition != NULL) {
		for (int i = 0; i < count; i++) {
			snprintf(sql, 1024, "insert into tb_composition values(null,%d , %d , %d , %d);",
				pcondition->frame, pcondition->slot, pcondition->type, pcondition->port);	

			rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
			CHECK_RC(rc, "tmsdb_Insert_composition", errMsg, db);
			pcondition++;
		}
	}
	printf("sql:%s\n", sql);
	sqlite3_close(db);

	return 0;
}


/**
 * @brief	插入多行到 tb_dev_map
 * @param	pcondition 插入内容，当pcondition不为NULL时插入
 * @param	pmask 搜索掩码 \n
 		无意义
 * @param	count 有多少行，pcondition必须是个数组
 * @retval	0 成功
 * @retval	-1 失败
 */
int tmsdb_Insert_dev_map(
		tdb_dev_map_t *pcondition, 
		tdb_dev_map_t *pmask,
		int count)
{
	sqlite3 *db;
	char *errMsg = NULL;
	int rc;
	char sql[1024];

	
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		// fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}


	if (pcondition != NULL) {
		for (int i = 0; i < count; i++) {
			snprintf(sql, 1024, "insert into tb_dev_map values(null,%d , %d , %d , %d, '%s', '%s', '%s', '%s');",
				pcondition->frame, pcondition->slot, pcondition->type, pcondition->port,
				pcondition->dev_name, pcondition->cable_name, pcondition->start_name, pcondition->end_name);
			printf("sql:%s\n",sql);
			rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
			CHECK_RC(rc, "tmsdb_Insert_dev_map", errMsg, db);
			pcondition++;
		}
	}
	
	sqlite3_close(db);
	return 0;
}
/**
 * @brief	插入多行到 tb_any_unit_osw
 * @param	pcondition 插入内容，当pcondition不为NULL时插入
 * @param	pmask 搜索掩码 \n
 		无意义
 * @param	count 有多少行，pcondition必须是个数组
 * @retval	0 成功
 * @retval	-1 失败
 */

int tmsdb_Insert_any_unit_osw(
		tdb_any_unit_osw_t *pcondition, 
		tdb_any_unit_osw_t *pmask,
		int count)
{
	sqlite3 *db;
	char *errMsg = NULL;
	int rc;
	// int index;

	char sql[1024];

	
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		// fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	printf("rc = %d\n", rc);
	if (pcondition != NULL) {
		for (int i = 0; i < count; i++) {
			snprintf(sql, 1024, "insert into tb_any_unit_osw values(null,%d , %d , %d , %d, %d, %d, %d, %d);",
				pcondition->any_frame, pcondition->any_slot, pcondition->any_type, pcondition->any_port,
				pcondition->osw_frame, pcondition->osw_slot, pcondition->osw_type, pcondition->osw_port);
			printf("sql:%s\n",sql);
			rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
			CHECK_RC(rc, "tmsdb_Insert_any_unit_osw", errMsg, db);
			pcondition++;
		}
	}
	
	sqlite3_close(db);
	return 0;
}


/**
 * @brief	插入多行到 tb_osw_cyc
 * @param	pcondition 插入内容，当pcondition不为NULL时插入
 * @param	pmask 搜索掩码 \n
 		无意义
 * @param	count 有多少行，pcondition必须是个数组
 * @retval	0 成功
 * @retval	-1 失败
 */
int tmsdb_Insert_osw_cyc(
		tdb_osw_cyc_t *pcondition, 
		tdb_osw_cyc_t *pmask,
		int count)
{
	sqlite3 *db;
	char *errMsg = NULL;
	int rc;
	char sql[1024];

	
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		// fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	if (pcondition != NULL) {
		for (int i = 0; i < count; i++) {
			snprintf(sql, 1024, "insert into tb_osw_cyc values(null,%d , %d, %d, %d, %d, %d);",
				pcondition->frame, pcondition->slot, pcondition->type, pcondition->port,
				pcondition->iscyc, pcondition->interval);
			printf("sql:%s\n",sql);
			rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
			CHECK_RC(rc, "tmsdb_Insert_osw_cyc", errMsg, db);
			pcondition++;
		}
	}
	
	sqlite3_close(db);
	return 0;

	return 0;
}

/**
 * @brief	插入多行到 tb_otdr_rollcall
 * @param	pcondition 插入内容，当pcondition不为NULL时插入
 * @param	pmask 搜索掩码 \n
 		无意义
 * @param	count 有多少行，pcondition必须是个数组
 * @retval	0 成功
 * @retval	-1 失败
 */
int tmsdb_Insert_otdr_rollcall(
		tdb_otdr_rollcall_t *pcondition, 
		tdb_otdr_rollcall_t *pmask,
		int count)
{
	sqlite3 *db;
	int rc;

	char sql[1024];
	char sqlvalGroup1[256];
	char sqlvalGroup2[256];


	sqlite3_stmt *pstmt;
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	
	printf("hello\n");
	if (pcondition != NULL) 
	{
		for (int i = 0; i < count; i++) {
			// 构造各个sql字段
			snprintf(sqlvalGroup1, 256, "%d, %d, %d, %d, %d",
					pcondition->ptest_hdr->frame, 
					pcondition->ptest_hdr->slot, 
					pcondition->ptest_hdr->type, 
					pcondition->ptest_hdr->port,
					pcondition->ptest_hdr->reserve0);
			snprintf(sqlvalGroup2, 256, "%d, %d, %d, %d, %f, %f, %f, %d, %d, %d",
					pcondition->ptest_param->rang,
					pcondition->ptest_param->wl,
					pcondition->ptest_param->pw,
					pcondition->ptest_param->time,
					pcondition->ptest_param->gi,
					pcondition->ptest_param->end_threshold,
					pcondition->ptest_param->none_reflect_threshold,
					pcondition->ptest_param->reserve0,
					pcondition->ptest_param->reserve0,
					pcondition->ptest_param->reserve1);
			
			// 汇总各sql字段
			snprintf(sql, 1024, "insert into tb_otdr_rollcall values(null,%s, %s);",
					sqlvalGroup1,sqlvalGroup2);


			printf("sql:%s\n",sql);
			// 准备执行语句
			sqlite3_prepare(db, sql, -1, &pstmt, 0);
			if(sqlite3_step(pstmt) !=SQLITE_DONE) {
                		fprintf(stdin, "sqlite3_step error:%s", sqlite3_errmsg(db));
            }
			sqlite3_finalize(pstmt);
			pcondition++;
		}
	}
	sqlite3_close(db);

	return 0;
}

/**
 * @brief	插入多行到 tb_otdr_ref
 * @param	pcondition 插入内容，当pcondition不为NULL时插入
 * @param	pmask 搜索掩码 \n
 		无意义
 * @param	count 有多少行，pcondition必须是个数组
 * @retval	0 成功
 * @retval	-1 失败
 */
int tmsdb_Insert_otdr_ref(
		tdb_otdr_ref_t *pcondition, 
		tdb_otdr_ref_t *pmask,
		int count)
{
	sqlite3 *db;
	int rc;

	char sql[2048];
	char sqlvalGroup1[256];
	char sqlvalGroup2[256];
	char sqlvalGroup3[256];
	char sqlvalGroup4[8];
	char sqlvalGroup5[256];
	char sqlvalGroup6[8];
	char sqlvalGroup7[256];
	char sqlvalGroup8[64];

	sqlite3_stmt *pstmt;
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	
	if (pcondition != NULL) 
	{
		for (int i = 0; i < count; i++) {
			// 构造各个sql字段
			snprintf(sqlvalGroup1, 256, "%d, %d, %d, %d, %d, \'%s\'",
				pcondition->pref_hdr->osw_frame, 
				pcondition->pref_hdr->osw_slot, 
				pcondition->pref_hdr->osw_type, 
				pcondition->pref_hdr->osw_port,
				pcondition->pref_hdr->otdr_port,
				pcondition->pref_hdr->strid);
			snprintf(sqlvalGroup2, 256, "%d, %d, %d, %d, %f, %f, %f, %d, %d, %d",
				pcondition->ptest_param->rang,
				pcondition->ptest_param->wl,
				pcondition->ptest_param->pw,
				pcondition->ptest_param->time,
				pcondition->ptest_param->gi,
				pcondition->ptest_param->end_threshold,
				pcondition->ptest_param->none_reflect_threshold,
				pcondition->ptest_param->sample,
				pcondition->ptest_param->reserve0,
				pcondition->ptest_param->reserve1);
			snprintf(sqlvalGroup3, 256, "\'%s\', %d",
				pcondition->pdata_hdr->dpid,
				pcondition->pdata_hdr->count);
			snprintf(sqlvalGroup4, 8, "?");
			snprintf(sqlvalGroup5, 256, "\'%s\', %d",
				pcondition->pevent_hdr->eventid,
				pcondition->pevent_hdr->count);
			snprintf(sqlvalGroup6, 8, "?");
			snprintf(sqlvalGroup7, 256, "\'%s\', %f, %f, %f",
				pcondition->pchain->inf,
				pcondition->pchain->range,
				pcondition->pchain->loss,
				pcondition->pchain->att);
			snprintf(sqlvalGroup8, 64, "%d, %d, %d",
				pcondition->pref_data->leve0,
				pcondition->pref_data->leve1,
				pcondition->pref_data->leve2);
			// 汇总各sql字段
			snprintf(sql, 1024, "insert into tb_otdr_ref values(null,%s, %s, %s, %s, %s, %s, %s, %s);",
				sqlvalGroup1,sqlvalGroup2,sqlvalGroup3,sqlvalGroup4,
				sqlvalGroup5,sqlvalGroup6,sqlvalGroup7,sqlvalGroup8);
			printf("sql:%s\n",sql);
			// printf("val = %s\n",pcondition->pevent_val);
			// 准备执行语句
			sqlite3_prepare(db, sql, -1, &pstmt, 0);
			// 插入OTDR曲线
			if (SQLITE_OK != sqlite3_bind_blob(
						pstmt, 
						1, 
						pcondition->pdata_val, 
						pcondition->pdata_hdr->count * sizeof(struct tms_retotdr_data_val), 
						NULL) ) {

				fprintf(stdin, "sqlite3_bind_blob");
			}

			// 插入事件列表
			if (SQLITE_OK != sqlite3_bind_blob(
						pstmt, 
						2, 
						pcondition->pevent_val , 
						pcondition->pevent_hdr->count * sizeof(struct tms_retotdr_event_val), 
						NULL) ) {

				fprintf(stdin, "sqlite3_bind_blob");
			}
			if(sqlite3_step(pstmt) !=SQLITE_DONE) {
                		fprintf(stdin, "sqlite3_step error:%s", sqlite3_errmsg(db));
            }
			sqlite3_finalize(pstmt);
			pcondition++;
		}
	}
	sqlite3_close(db);

	return 0;
}

/**
 * @brief	插入多行到 tb_otdr_rollcall
 * @param	pcondition 插入内容，当pcondition不为NULL时插入
 * @param	pmask 搜索掩码 \n
 		无意义
 * @param	count 有多少行，pcondition必须是个数组
 * @retval	0 成功
 * @retval	-1 失败
 */
int tmsdb_Insert_otdr_his_data(
		tdb_otdr_his_data_t *pcondition, 
		tdb_otdr_his_data_t *pmask,
		int count)
{
	sqlite3 *db;
	// char *errMsg = NULL;
	int rc;

	char sql[1024];
	char sqlvalGroup1[256];
	char sqlvalGroup2[256];
	char sqlvalGroup3[256];
	char sqlvalGroup4[8];
	char sqlvalGroup5[256];
	char sqlvalGroup6[8];
	char sqlvalGroup7[256];
	

	sqlite3_stmt *pstmt;
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	
	if (pcondition != NULL) 
	{
		for (int i = 0; i < count; i++) {
			// 构造各个sql字段
			snprintf(sqlvalGroup1, 256, "%d, %d, %d, %d,\'%s\',%d,%d,%d,%d",
				pcondition->ptest_hdr->osw_frame, 
				pcondition->ptest_hdr->osw_slot, 
				pcondition->ptest_hdr->osw_type, 
				pcondition->ptest_hdr->osw_port,
				pcondition->ptest_hdr->time,
				pcondition->ptest_hdr->otdr_frame,
				pcondition->ptest_hdr->otdr_slot,
				pcondition->ptest_hdr->otdr_type,
				pcondition->ptest_hdr->otdr_port);
			snprintf(sqlvalGroup2, 256, "%d, %d, %d, %d, %f, %f, %f, %d, %d, %d",
				pcondition->ptest_param->rang,
				pcondition->ptest_param->wl,
				pcondition->ptest_param->pw,
				pcondition->ptest_param->time,
				pcondition->ptest_param->gi,
				pcondition->ptest_param->end_threshold,
				pcondition->ptest_param->none_reflect_threshold,
				pcondition->ptest_param->sample,
				pcondition->ptest_param->reserve0,
				pcondition->ptest_param->reserve1);
			snprintf(sqlvalGroup3, 256, "\'%s\', %d",
				pcondition->pdata_hdr->dpid,
				pcondition->pdata_hdr->count);
			snprintf(sqlvalGroup4, 8, "?");
			snprintf(sqlvalGroup5, 256, "\'%s\', %d",
				pcondition->pevent_hdr->eventid,
				pcondition->pevent_hdr->count);
			snprintf(sqlvalGroup6, 8, "?");
			snprintf(sqlvalGroup7, 256, "\'%s\', %f, %f, %f",
				pcondition->pchain->inf,
				pcondition->pchain->range,
				pcondition->pchain->loss,
				pcondition->pchain->att);
			// 汇总各sql字段
			snprintf(sql, 1024, "insert into tb_otdr_his_data values(null,%s, %s, %s, %s, %s, %s, %s);",
				sqlvalGroup1,sqlvalGroup2,sqlvalGroup3,sqlvalGroup4,
				sqlvalGroup5,sqlvalGroup6,sqlvalGroup7);
			printf("sql:%s\n",sql);
			// 准备执行语句
			sqlite3_prepare(db, sql, -1, &pstmt, 0);
			// 插入OTDR曲线
			if (SQLITE_OK != sqlite3_bind_blob(
						pstmt, 
						1, 
						pcondition->pdata_val, 
						pcondition->pdata_hdr->count * sizeof(struct tms_retotdr_data_val), 
						NULL) ) {

				fprintf(stdin, "sqlite3_bind_blob");
			}
			// 插入事件列表
			if (SQLITE_OK != sqlite3_bind_blob(
						pstmt, 
						2, 
						pcondition->pevent_val , 
						pcondition->pevent_hdr->count * sizeof(struct tms_retotdr_event_val), 
						NULL) ) {

				fprintf(stdin, "sqlite3_bind_blob");
			}
			if(sqlite3_step(pstmt) !=SQLITE_DONE) {
                		fprintf(stdin, "sqlite3_step error:%s", sqlite3_errmsg(db));
            }
			sqlite3_finalize(pstmt);
			pcondition++;
		}
	}
	sqlite3_close(db);

	return 0;
}



int _tmsdb_Delete_any(
	const char *dbpath,
	const char *sql,
	const char *info)
{
	sqlite3 *db;
	char *errMsg = NULL;
	int rc;
	
	rc = sqlite3_open(dbpath, &db);
	if ( rc ){
		// fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
	if ( rc ){
		fprintf(stderr, "%s: %s\n", info, sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	sqlite3_close(db);
	return 0;
}

/**
 * @brief	删除 tb_common 表某些行
 * @param	null
 * @retval	null
 * @remarks	
 * @see	
 */

int tmsdb_Delete_common(
		tdb_common_t *pcondition, 
		tdb_common_t *pmask)
{
	char sql[1024];
	char sqlid[36]    = "\0";
	char sqlval1[36]  = "\0";
	char sqlval2[36]  = "\0";
	char sqlval3[36]  = "\0";
	char sqlval4[36]  = "\0";
	char sqlval5[36]  = "\0";
	char sqlval6[36]  = "\0";
	char sqlval7[36]  = "\0";
	char sqlval8[36]  = "\0";
	char sqlval9[36]  = "\0";
	char sqlval10[36] = "\0";
	char sqlval11[36] = "\0";
	char sqlval12[36] = "\0";

	// 构造SQL语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "delete from tb_common;");
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 36, "and id=%d", pcondition->id);
		}
		if (pmask->val1) {
			snprintf(sqlval1, 36, "and val1=%d", pcondition->val1);
		}
		if (pmask->val2) {
			snprintf(sqlval2, 36, "and val2=%d", pcondition->val2);
		}
		if (pmask->val3) {
			snprintf(sqlval3, 36, "and val3=%d", pcondition->val3);
		}
		if (pmask->val4) {
			snprintf(sqlval4, 36, "and val4=%d", pcondition->val4);
		}
		if (pmask->val5) {
			snprintf(sqlval5, 36, "and val5=%d", pcondition->val5);
		}
		if (pmask->val6) {
			snprintf(sqlval6, 36, "and val6=%d", pcondition->val6);
		}
		if (pmask->val7) {
			snprintf(sqlval7, 36, "and val7=%d", pcondition->val7);
		}
		if (pmask->val8) {
			snprintf(sqlval8, 36, "and val8=%d", pcondition->val8);
		}
		if (pmask->val9) {
			snprintf(sqlval9, 36, "and val9=%d", pcondition->val9);
		}
		if (pmask->val10) {
			snprintf(sqlval10, 36, "and val10=%d", pcondition->val10);
		}
		if (pmask->val11) {
			snprintf(sqlval11, 36, "and val11=%d", pcondition->val11);
		}
		if (pmask->val12) {
			snprintf(sqlval12, 36, "and val12=%d", pcondition->val12);
		} 
		snprintf(sql, 1024, "delete from tb_common where(1 %s %s %s %s %s %s %s %s %s %s %s %s %s);",
				sqlid,
				sqlval1,sqlval2,sqlval3,sqlval4,sqlval5,sqlval6,
				sqlval7,sqlval8,sqlval9,sqlval10,sqlval11,sqlval12);
		printf("sql : %s\n",sql);
	}

	_tmsdb_Delete_any(DB_PATH, sql, "tmsdb_Delete_sms");
	return 0;
}
/**
 * @brief	删除 tb_sn 表某些行
 * @param	null
 * @retval	null
 * @remarks	
 * @see	
 */
int tmsdb_Delete_sn(
		tdb_sn_t *pcondition, 
		tdb_sn_t *pmask)
{
	char sql[64];

	// 构造SQL语句
	// if (pcondition == NULL) {
	// 	snprintf(sql, 64, "delete * from tb_sn;");
	// }
	snprintf(sql, 64, "delete from tb_sn;");
	printf("sql:%s\n",sql);
	_tmsdb_Delete_any(DB_PATH, sql, "tmsdb_Delete_sn");
	return 0;
}

/**
 * @brief	删除 tb_sms 表某些行
 * @param	null
 * @retval	null
 * @remarks	
 * @see	
 */
int tmsdb_Delete_sms(
		tdb_sms_t *pcondition, 
		tdb_sms_t *pmask)
{
	char sql[1024];
	char sqlid[64]    = "\0";
	char sqlval1[64]  = "\0";
	char sqlval2[64]  = "\0";
	char sqlval3[64]  = "\0";
	char sqlval4[64]  = "\0";
	// 构造SQL语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "delete from tb_sms;");
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		if (pmask->time) {
			snprintf(sqlval1, 64, "and date=%d", pcondition->time);
		}
		if (pmask->phone[0]) {
			snprintf(sqlval2, 64, "and phone='%s'", pcondition->phone);
		}
		if (pmask->type) {
			snprintf(sqlval3, 64, "and type=%d", pcondition->type);
		}
		if (pmask->level) {
			snprintf(sqlval4, 64, "and level=%d", pcondition->level);
		}
		snprintf(sql, 1024, "delete from tb_sms where(1 %s %s %s %s %s);",
							sqlid, sqlval1, sqlval2, sqlval3, sqlval4);
	}
	printf("sql:%s\n",sql);
	_tmsdb_Delete_any(DB_PATH, sql, "tmsdb_Delete_sms");
	return 0;
}


/**
 * @brief	搜索 tb_composition 表某些行
 * @param	pcondition 搜索条件
 * @param	pmask 搜索掩码 \n
 		有效范围从id、frame、slot、type、port
 * @retval	>0 成功
 * @retval	-1 失败
 */
int tmsdb_Delete_composition(
		tdb_composition_t *pcondition, 
		tdb_composition_t *pmask)
{
	char sql[1024];
	char sqlid[64]    = "\0";
	char sqlval1[64]  = "\0";
	char sqlval2[64]  = "\0";
	char sqlval3[64]  = "\0";
	char sqlval4[64]  = "\0";
	// 构造SQL语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "delete from tb_composition;");
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		if (pmask->frame) {
			snprintf(sqlval1, 64, "and frame=%d", pcondition->frame);
		}
		if (pmask->slot) {
			snprintf(sqlval2, 64, "and slot=%d", pcondition->slot);
		}
		if (pmask->type) {
			snprintf(sqlval3, 64, "and type=%d", pcondition->type);
		}
		if (pmask->port) {
			snprintf(sqlval4, 64, "and port=%d", pcondition->port);
		}

		snprintf(sql, 1024, "delete from tb_composition where(1 %s %s %s %s %s);",
				sqlid, sqlval1, sqlval2, sqlval3, sqlval4);

	}
	printf("sql:%s\n",sql);
	_tmsdb_Delete_any(DB_PATH, sql, "tmsdb_Delete_composition");
	return 0;
}


/**
 * @brief	搜索 tb_composition 表某些行
 * @param	pcondition 搜索条件\n
 * @param	pmask 搜索条件掩码，当掩码不为0时，对应pcondition字段有效\n
		有效范围 any_frame、any_slot、any_type、any_port
 * @retval	>0 成功
 * @retval	-1 失败
 */
int tmsdb_Delete_dev_map(
		tdb_dev_map_t *pcondition, 
		tdb_dev_map_t *pmask)
{
	char sql[1024];
	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64]  = "\0";
	char sqlval3[64]  = "\0";
	char sqlval4[64]  = "\0";

	// 构造SQL语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "delete from tb_composition;");
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		if (pmask->frame) {
			snprintf(sqlval1, 64, "and any_frame=%d", pcondition->frame);
		}
		if (pmask->slot) {
			snprintf(sqlval2, 64, "and any_slot=%d", pcondition->slot);
		}
		if (pmask->type) {
			snprintf(sqlval3, 64, "and any_type=%d", pcondition->type);
		}
		if (pmask->port) {
			snprintf(sqlval4, 64, "and any_port=%d", pcondition->port);
		}
		snprintf(sql, 1024, "delete from tb_dev_map where(1 %s %s %s %s %s);",
							sqlid, sqlval1, sqlval2, sqlval3, sqlval4);


	}
	printf("sql: %s\n",sql);
	_tmsdb_Delete_any(DB_PATH, sql, "tmsdb_Delete_dev_map");

	return 0;
}

/**
 * @brief	搜索 tb_any_unit_osw 表某些行
 * @param	pcondition 搜索条件\n
 * @param	pmask 搜索条件掩码，当掩码不为0时，对应pcondition字段有效\n
		有效范围 any_frame、any_slot、any_type、any_port、osw_frame、osw_slot、osw_type、osw_port
 * @retval	>0 成功
 * @retval	-1 失败
 */
int tmsdb_Delete_any_unit_osw(
		tdb_any_unit_osw_t *pcondition, 
		tdb_any_unit_osw_t *pmask)
{

	char sql[1024];
	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64] = "\0";
	char sqlval3[64] = "\0";
	char sqlval4[64] = "\0";
	char sqlval5[64] = "\0";
	char sqlval6[64] = "\0";
	char sqlval7[64] = "\0";
	char sqlval8[64] = "\0";

	// 构造SQL语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "delete from tb_any_unit_osw;");
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		if (pmask->any_frame) {
			snprintf(sqlval1, 64, "and any_frame=%d", pcondition->any_frame);
		}
		if (pmask->any_slot) {
			snprintf(sqlval2, 64, "and any_slot=%d", pcondition->any_slot);
		}
		if (pmask->any_type) {
			snprintf(sqlval3, 64, "and any_type=%d", pcondition->any_type);
		}
		if (pmask->any_port) {
			snprintf(sqlval4, 64, "and any_port=%d", pcondition->any_port);
		}
		if (pmask->osw_frame) {
			snprintf(sqlval5, 64, "and osw_frame=%d", pcondition->osw_frame);
		}
		if (pmask->osw_slot) {
			snprintf(sqlval6, 64, "and osw_slot=%d", pcondition->osw_slot);
		}
		if (pmask->osw_type) {
			snprintf(sqlval7, 64, "and osw_type=%d", pcondition->osw_type);
		}
		if (pmask->osw_port) {
			snprintf(sqlval8, 64, "and osw_port=%d", pcondition->osw_port);
		}
		snprintf(sql, 1024, "delete from tb_any_unit_osw where(1 %s %s %s %s %s %s %s %s %s);",
			sqlid,
			sqlval1, sqlval2, sqlval3, sqlval4,
			sqlval5, sqlval6, sqlval7, sqlval8);
	}
	printf("sql:%s\n",sql);
	_tmsdb_Delete_any(DB_PATH, sql, "tmsdb_Delete_any_unit_osw");
	

	return 0;
}

/**
 * @brief	搜索 tb_osw_cyc 表某些行
 * @param	pcondition 搜索条件\n
 * @param	pmask 搜索条件掩码，当掩码不为0时，对应pcondition字段有效\n
		有效范围 osw_frame、osw_slot、osw_type、osw_port、iscyc、interval
 * @retval	>0 成功
 * @retval	-1 失败
 */
int tmsdb_Delete_osw_cyc(
		tdb_osw_cyc_t *pcondition, 
		tdb_osw_cyc_t *pmask)
{
	char sql[1024];
	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64] = "\0";
	char sqlval3[64] = "\0";
	char sqlval4[64] = "\0";
	char sqlval5[64] = "\0";
	char sqlval6[64] = "\0";

	// 构造SQL语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "delete from tb_osw_cyc;");
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		if (pmask->frame) {
			snprintf(sqlval1, 64, "and osw_frame=%d", pcondition->frame);
		}
		if (pmask->slot) {
			snprintf(sqlval2, 64, "and osw_slot=%d", pcondition->slot);
		}
		if (pmask->type) {
			snprintf(sqlval3, 64, "and osw_type=%d", pcondition->type);
		}
		if (pmask->port) {
			snprintf(sqlval4, 64, "and osw_port=%d", pcondition->port);
		}
		if (pmask->iscyc) {
			snprintf(sqlval5, 64, "and iscyc=%d", pcondition->iscyc);
		}
		if (pmask->interval) {
			snprintf(sqlval6, 64, "and interval=%d", pcondition->interval);
		}
		snprintf(sql, 1024, "delete from tb_osw_cyc where(1 %s %s %s %s %s %s %s);",
			sqlid,sqlval1, sqlval2, sqlval3, sqlval4, sqlval5, sqlval6);
	}

	_tmsdb_Delete_any(DB_PATH, sql, "tmsdb_Delete_any_unit_osw");
	

	return 0;
}

/**
 * @brief	搜索 tb_otdr_ref 表某些行
 * @param	pcondition 搜索条件\n
 * @param	pmask 搜索条件掩码，当掩码不为0时，对应pcondition字段有效\n
		有效范围 osw_frame、osw_slot、osw_type、osw_port
 * @retval	>0 成功
 * @retval	-1 失败
 */
int tmsdb_Delete_otdr_ref(
		tdb_otdr_ref_t *pcondition, 
		tdb_otdr_ref_t *pmask)
{
	char sql[1024];
	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64] = "\0";
	char sqlval3[64] = "\0";
	char sqlval4[64] = "\0";


	// 构造SQL语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "delete from tb_otdr_ref;");
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		if (pmask->pref_hdr->osw_frame) {
			snprintf(sqlval1, 64, "and osw_frame=%d", pcondition->pref_hdr->osw_frame);
		}
		if (pmask->pref_hdr->osw_slot) {
			snprintf(sqlval2, 64, "and osw_slot=%d", pcondition->pref_hdr->osw_slot);
		}
		if (pmask->pref_hdr->osw_type) {
			snprintf(sqlval3, 64, "and osw_type=%d", pcondition->pref_hdr->osw_type);
		}
		if (pmask->pref_hdr->osw_port) {
			snprintf(sqlval4, 64, "and osw_port=%d", pcondition->pref_hdr->osw_port);
		}
		snprintf(sql, 1024, "delete from tb_otdr_ref where(1 %s %s %s %s %s);",
			sqlid, sqlval1, sqlval2, sqlval3, sqlval4 );
	}
	printf("sql%s\n",sql);
	_tmsdb_Delete_any(DB_PATH, sql, "tmsdb_Delete_otdr_ref");
	return 0;
}

/**
 * @brief	搜索 tb_otdr_rollcall 表某些行
 * @param	pcondition 搜索条件\n
 * @param	pmask 搜索条件掩码，当掩码不为0时，对应pcondition字段有效\n
		有效范围 osw_frame、osw_slot、osw_type、osw_port
 * @retval	>0 成功
 * @retval	-1 失败
 */
int tmsdb_Delete_otdr_rollcall(
		struct tdb_otdr_rollcall *pcondition, 
		struct tdb_otdr_rollcall *pmask)
{
	char sql[1024];
	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64] = "\0";
	char sqlval3[64] = "\0";
	char sqlval4[64] = "\0";
	

	// 构造SQL语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "delete from tb_otdr_rollcall;");
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		if (pmask->ptest_hdr->frame) {
			snprintf(sqlval1, 64, "and osw_frame=%d", pcondition->ptest_hdr->frame);
		}
		if (pmask->ptest_hdr->slot) {
			snprintf(sqlval2, 64, "and osw_slot=%d", pcondition->ptest_hdr->slot);
		}
		if (pmask->ptest_hdr->type) {
			snprintf(sqlval3, 64, "and osw_type=%d", pcondition->ptest_hdr->type);
		}
		if (pmask->ptest_hdr->port) {
			snprintf(sqlval4, 64, "and osw_port=%d", pcondition->ptest_hdr->port);
		}
		snprintf(sql, 1024, "delete from tb_otdr_rollcall where(1 %s %s %s %s %s);",
			sqlid, sqlval1, sqlval2, sqlval3, sqlval4);
	}
	_tmsdb_Delete_any(DB_PATH, sql, "tmsdb_Delete_otdr_rollcall");
	

	return 0;
}

/**
 * @brief	搜索 tb_otdr_his_data 表某些行
 * @param	pcondition 搜索条件\n
 * @param	pmask 搜索条件掩码，当掩码不为0时，对应pcondition字段有效\n
		有效范围 osw_frame、osw_slot、osw_type、osw_port、otdr_frame、otdr_slot、otdr_type、otdr_port
 * @retval	>0 成功
 * @retval	-1 失败
 */
int tmsdb_Delete_otdr_his_data(
		tdb_otdr_his_data_t *pcondition, 
		tdb_otdr_his_data_t *pmask)
{
	char sql[1024];
	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64] = "\0";
	char sqlval3[64] = "\0";
	char sqlval4[64] = "\0";
	char sqlval5[64] = "\0";
	char sqlval6[64] = "\0";
	char sqlval7[64] = "\0";
	char sqlval8[64] = "\0";

	// 构造SQL语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "delete from tb_otdr_his_data;");
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		if (pmask->ptest_hdr->osw_frame) {
			snprintf(sqlval1, 64, "and osw_frame=%d", pcondition->ptest_hdr->osw_frame);
		}
		if (pmask->ptest_hdr->osw_slot) {
			snprintf(sqlval2, 64, "and osw_slot=%d", pcondition->ptest_hdr->osw_slot);
		}
		if (pmask->ptest_hdr->osw_type) {
			snprintf(sqlval3, 64, "and osw_type=%d", pcondition->ptest_hdr->osw_type);
		}
		if (pmask->ptest_hdr->osw_port) {
			snprintf(sqlval4, 64, "and osw_port=%d", pcondition->ptest_hdr->osw_port);
		}
		if (pmask->ptest_hdr->otdr_frame) {
			snprintf(sqlval5, 64, "and otdr_frame=%d", pcondition->ptest_hdr->otdr_frame);
		}
		if (pmask->ptest_hdr->otdr_slot) {
			snprintf(sqlval6, 64, "and otdr_slot=%d", pcondition->ptest_hdr->otdr_slot);
		}
		if (pmask->ptest_hdr->otdr_type) {
			snprintf(sqlval7, 64, "and otdr_type=%d", pcondition->ptest_hdr->otdr_type);
		}
		if (pmask->ptest_hdr->otdr_port) {
			snprintf(sqlval8, 64, "and otdr_port=%d", pcondition->ptest_hdr->otdr_port);
		}
		snprintf(sql, 1024, "delete from tb_otdr_his_data where(1 %s %s %s %s %s %s %s %s %s);",
			sqlid,
			sqlval1, sqlval2, sqlval3, sqlval4,
			sqlval5, sqlval6, sqlval7, sqlval8);
	}
	printf("sql:%s\n",sql);
	_tmsdb_Delete_any(DB_PATH, sql, "tmsdb_Delete_any_unit_osw");


	return 0;
}



int _tmsdb_Select_any(
	const char *dbpath,
	const char *table,
	const char *sql,
	const char *info,
	char ***pdbResult,
	int *prow,
	int *pcol)
{
	sqlite3 *db;
	char   **dbResult;
	char    *errMsg = NULL;
	int row, col;
	int rc;
	

	// 打开数据库
	rc = sqlite3_open(dbpath , &db);
	if ( rc ){
		// fprintf(stderr, "Can't open database: ");//%s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	row = 0;
	*prow = 0;
	*pcol = 0;
	// 构造sql语句并执行
	rc = sqlite3_get_table(db, sql, &dbResult, &row, &col, &errMsg);
	CHECK_RC(rc, "info", errMsg, db);

	// 当行数大于0时才分配内存
	if (row > 0) {
		*pdbResult = dbResult;
		*prow = row;
		*pcol = col;
		sqlite3_close(db);
		return row;
	}
	sqlite3_close(db);
	return 0;
}

#ifdef _DEBUG
// 调试函数
static int _cb_Select_common(void *NotUsed, int argc, char **argv, char **azColName)
{

	int i;
	
	printf("count = %d\n", argc);
	for(i=0; i<argc; i++){ 
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");

	return 0;
}
#endif

/**
 * @brief	搜索 tb_common
 * @param	pcondition 搜索条件
 * @param	pmask 搜索掩码 \n
 		有效范围从id、val1~val12
 * @param	ppout 搜索的行（目前没有任何用）
 * @param	pcallback 搜索到的行回调函数，有三个参数\n
 		out 搜索到的行指针，指向一个 tdb_common_t 结构体\n
 		ptr 调用者的参数指针\n
 		len cbptr->pdata参数的长度
 * @param	ptr 用于传递给pcallback函数，调用者自定义\n
 * @retval	null
 * @remarks	
 * @see	
 */

int tmsdb_Select_common(
		tdb_common_t *pcondition, 
		tdb_common_t *pmask,
		int (*pcallback)(tdb_common_t *output, void *ptr, int len),
		void *ptr)
{
	sqlite3 *db;
	int rc;

	char sql[1024];
	char sqlid[36]    = "\0";
	char sqlval1[36]  = "\0";
	char sqlval2[36]  = "\0";
	char sqlval3[36]  = "\0";
	char sqlval4[36]  = "\0";
	char sqlval5[36]  = "\0";
	char sqlval6[36]  = "\0";
	char sqlval7[36]  = "\0";
	char sqlval8[36]  = "\0";
	char sqlval9[36]  = "\0";
	char sqlval10[36] = "\0";
	char sqlval11[36] = "\0";
	char sqlval12[36] = "\0";

	sqlite3_stmt *pstmt;
	tdb_common_t out;
	
	if (pcallback == NULL) {
		return -1;
	}
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	
	if (pcondition == NULL) {
		snprintf(sql, 1024, "select id,val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,val11,val12,bin_len,bin_data from tb_common;");
	}

	else if (pcondition != NULL && pmask != NULL) {
		// 构造SQL语句
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}

		if (pmask->val1) {
			snprintf(sqlval1, 36, "and val1=%d", pcondition->val1);
		}
		if (pmask->val2) {
			snprintf(sqlval2, 36, "and val2=%d", pcondition->val2);
		}
		if (pmask->val3) {
			snprintf(sqlval3, 36, "and val3=%d", pcondition->val3);
		}
		if (pmask->val4) {
			snprintf(sqlval4, 36, "and val4=%d", pcondition->val4);
		}
		if (pmask->val5) {
			snprintf(sqlval5, 36, "and val5=%d", pcondition->val5);
		}
		if (pmask->val6) {
			snprintf(sqlval6, 36, "and val6=%d", pcondition->val6);
		}
		if (pmask->val7) {
			snprintf(sqlval7, 36, "and val7=%d", pcondition->val7);
		}
		if (pmask->val8) {
			snprintf(sqlval8, 36, "and val8=%d", pcondition->val8);
		}
		if (pmask->val9) {
			snprintf(sqlval9, 36, "and val9=%d", pcondition->val9);
		}
		if (pmask->val10) {
			snprintf(sqlval10, 36, "and val10=%d", pcondition->val10);
		}
		if (pmask->val11) {
			snprintf(sqlval11, 36, "and val11=%d", pcondition->val11);
		}
		if (pmask->val12) {
			snprintf(sqlval12, 36, "and val12=%d", pcondition->val12);
		}		

		snprintf(sql, 1024, "select id,val1,val2,vla3,val4,val5,val6,val7,val8,val9,val10,val11,val12,bin_len,bin_data\n\
				from  tb_common where(1 %s %s %s %s %s %s %s %s %s %s %s %s %s);",
			sqlid,sqlval1,sqlval2,sqlval3,sqlval4,sqlval5,sqlval6,sqlval7,sqlval8,sqlval9,sqlval10,sqlval11,sqlval12);
		printf("sql:%s\n",sql);
		// snprintf(sql, 1024, "select * from tb_common");
	}
	// if (pcallback) 
	// {
	// 	rc = sqlite3_exec(db, sql, _cb_Select_common, NULL, &errMsg);	
	// 	CHECK_RC(rc, "tmsdb_Select_common", errMsg, db);
	// }
	sqlite3_prepare(db, sql, -1, &pstmt, NULL);

	
	out.pdata = NULL;	
	while(sqlite3_step(pstmt) == SQLITE_ROW) {
		
		out.id    = sqlite3_column_int(pstmt, 0);
		out.val1  = sqlite3_column_int(pstmt, 1);
		out.val2  = sqlite3_column_int(pstmt, 2);
		out.val3  = sqlite3_column_int(pstmt, 3);
		out.val4  = sqlite3_column_int(pstmt, 4);
		out.val5  = sqlite3_column_int(pstmt, 5);
		out.val6  = sqlite3_column_int(pstmt, 6);
		out.val7  = sqlite3_column_int(pstmt, 7);
		out.val8  = sqlite3_column_int(pstmt, 8);
		out.val9  = sqlite3_column_int(pstmt, 9);
		out.val10 = sqlite3_column_int(pstmt, 10);
		out.val11 = sqlite3_column_int(pstmt, 11);
		out.val12 = sqlite3_column_int(pstmt, 12);
		out.lenpdata = sqlite3_column_int(pstmt, 13);
		out.pdata = (void*)sqlite3_column_blob(pstmt,14);
		out.lenpdata = sqlite3_column_bytes(pstmt, 14);
		
		
		if (-1 == pcallback(&out, ptr, out.lenpdata)) {
			break;
		}
		
		out.pdata = NULL;
		
	}
		
	sqlite3_finalize(pstmt);
	sqlite3_close(db);
	return 0;
}

int tmsdb_Select_sn(
		tdb_sn_t *pcondition, 
		tdb_sn_t *pmask,
		tdb_sn_t **ppout)
{
	char     sql[1024];
	char   **dbResult;
	int row, col;
	int index;

	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64]  = "\0";
	char sqlval3[64]  = "\0";
	char sqlval4[64]  = "\0";


	tdb_sn_t *pout;
	// 构造sql语句
	// if (pcondition == NULL) {
	// 	snprintf(sql, 1024, "select * from tb_sn;");	
	// }
	snprintf(sql, 1024, "select sn from tb_sn;");	

	printf("sql:%s\n",sql);
	// 执行sql语句
	_tmsdb_Select_any(DB_PATH, "tb_sn", sql, "tmsdb_Select_sn", &dbResult,&row, &col);
	printf("row = %d\n",row);

	if (row > 0) {
		pout = (tdb_sn_t*)malloc( sizeof(tdb_sn_t) * row);
		if (pout == NULL) {
			return -1;
		}
		bzero(pout,  sizeof(tdb_sn_t) * row);
		*ppout = pout;

		// dbResult是(row+1 * col)字符串，
		// 0~col是“列名”
		// 剩下的(row * col)是数据矩阵字符串
		// index偏移到有效数据
		index = col;
		for (int r = 0; r < row; r++) {
			if (dbResult[index]) {
				memcpy(pout->sn, dbResult[index], 128);
			}
			index++;
			pout++;
		}

		// 释放表
		sqlite3_free_table(dbResult);
		return row;
	}
	return 0;
}
int tmsdb_Select_sms(
		tdb_sms_t *pcondition, 
		tdb_sms_t *pmask,
		tdb_sms_t **ppout)
{
	char     sql[1024];
	char   **dbResult;
	int row, col;
	int index;

	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64]  = "\0";
	char sqlval3[64]  = "\0";
	char sqlval4[64]  = "\0";


	tdb_sms_t *pout;
	// 构造sql语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "select id,date,phone,type,level from tb_sms;");	
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		if (pmask->time) {
			snprintf(sqlval1, 64, "and date=%d", pcondition->time);
		}
		if (pmask->phone[0]) {
			snprintf(sqlval2, 64, "and phone='%s'", pcondition->phone);
		}
		if (pmask->type) {
			snprintf(sqlval3, 64, "and type=%d", pcondition->type);
		}
		if (pmask->level) {
			snprintf(sqlval4, 64, "and level=%d", pcondition->level);
		}
		snprintf(sql, 1024, "select id,date,phone,type,level from tb_sms where(1 %s %s %s %s %s);",
			sqlid, sqlval1, sqlval2, sqlval3, sqlval4);
	}
	printf("sql:%s\n",sql);
	// 执行sql语句
	_tmsdb_Select_any(DB_PATH, "tb_sms", sql, "tmsdb_Select_sms", &dbResult,&row, &col);
	printf("row = %d\n",row);

	if (row > 0) {
		pout = (tdb_sms_t*)malloc( sizeof(tdb_sms_t) * row);
		if (pout == NULL) {
			return -1;
		}
		bzero(pout,  sizeof(tdb_sms_t) * row);
		*ppout = pout;

		// dbResult是(row+1 * col)字符串，
		// 0~col是“列名”
		// 剩下的(row * col)是数据矩阵字符串
		// index偏移到有效数据
		index = col;
		for (int r = 0; r < row; r++) {
			if (dbResult[index]) {
				pout->id 	= atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->time = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				memcpy(pout->phone,   dbResult[index],16);
			}
			index++;
			if (dbResult[index]) {
				pout->type = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->level = atoi(dbResult[index]);
			}
			index++;
			pout++;
		}

		// 释放表
		sqlite3_free_table(dbResult);
		return row;
	}
	return 0;
}

/**
 * @brief	搜索 tb_composition 表某些行
 * @param	pcondition 搜索条件
 * @param	pmask 搜索掩码 \n
 		有效范围从id、frame、slot、type、port
 * @param	ppout 搜索到的行指针
 * @retval	>0 返回的行数，数据存在ppout，调用需要在使用完后调用free(ppout)
 */
int tmsdb_Select_composition(
		tdb_composition_t *pcondition, 
		tdb_composition_t *pmask,
		tdb_composition_t **ppout)
{
	char     sql[1024];
	char   **dbResult;
	int row, col;
	int index;

	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64]  = "\0";
	char sqlval3[64]  = "\0";
	char sqlval4[64]  = "\0";


	tdb_composition_t *pout;
	// 构造sql语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "select id,frame,slot,type,port from tb_composition;");	
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		if (pmask->frame) {
			snprintf(sqlval1, 64, "and frame=%d", pcondition->frame);
		}
		if (pmask->slot) {
			snprintf(sqlval2, 64, "and slot=%d", pcondition->slot);
		}
		if (pmask->type) {
			snprintf(sqlval3, 64, "and type=%d", pcondition->type);
		}
		if (pmask->port) {
			snprintf(sqlval4, 64, "and port=%d", pcondition->port);
		}
		snprintf(sql, 1024, "select id,frame,slot,type,port from tb_composition where(1 %s %s %s %s %s);",
			sqlid, sqlval1, sqlval2, sqlval3, sqlval4);
	}
	printf("sql:%s\n",sql);
	// 执行sql语句
	_tmsdb_Select_any(DB_PATH, "tb_composition", sql, "tmsdb_Select_composition", &dbResult,&row, &col);
	if (row > 0) {
		pout = (tdb_composition_t*)malloc( sizeof(tdb_composition_t) * row);
		if (pout == NULL) {
			return -1;
		}
		bzero(pout,  sizeof(tdb_composition_t) * row);
		*ppout = pout;

		// dbResult是(row+1 * col)字符串，
		// 0~col是“列名”
		// 剩下的(row * col)是数据矩阵字符串
		// index偏移到有效数据
		index = col;
		for (int r = 0; r < row; r++) {
			if (dbResult[index]) {
				pout->id 	= atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->frame = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->slot = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->type = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->port = atoi(dbResult[index]);
			}
			index++;
			pout++;
		}

		// 释放表
		sqlite3_free_table(dbResult);
		return row;
	}
	return 0;
}

/**
 * @brief	搜索 tb_composition 表某些行
 * @param	pcondition 搜索条件\n
 * @param	pmask 搜索条件掩码，当掩码不为0时，对应pcondition字段有效\n
		有效范围 any_frame、any_slot、any_type、any_port
 * @param	ppout 搜索到的行指针
 * @retval	>0 返回的行数，数据存在ppout，调用需要在使用完后调用free(ppout)
 */
int tmsdb_Select_dev_map(
		tdb_dev_map_t *pcondition, 
		tdb_dev_map_t *pmask,
		tdb_dev_map_t **ppout)
{
	char     sql[1024];
	char   **dbResult;
	int row, col;
	int index;
	tdb_dev_map_t *pout;

	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64]  = "\0";
	char sqlval3[64]  = "\0";
	char sqlval4[64]  = "\0";
	// 构造sql语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "select id,any_frame,any_slot,any_type,any_port,dev_name,cable_name,start_name,end_name from tb_dev_map;");	
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		if (pmask->frame) {
			snprintf(sqlval1, 64, "and any_frame=%d", pcondition->frame);
		}
		if (pmask->slot) {
			snprintf(sqlval2, 64, "and any_slot=%d", pcondition->slot);
		}
		if (pmask->type) {
			snprintf(sqlval3, 64, "and any_type=%d", pcondition->type);
		}
		if (pmask->port) {
			snprintf(sqlval4, 64, "and any_port=%d", pcondition->port);
		}
		snprintf(sql, 1024, "select id,any_frame,any_slot,any_type,any_port,dev_name,cable_name,start_name,end_name from tb_dev_map where(1 %s %s %s %s %s);",
			sqlid, sqlval1, sqlval2, sqlval3, sqlval4);
	}
	printf("sql:%s\n",sql);
	// 执行sql语句
	_tmsdb_Select_any(DB_PATH, "tb_dev_map", sql, "tmsdb_Select_dev_map", &dbResult,&row, &col);
	if ( row > 0 ) {
		pout = (tdb_dev_map_t*)malloc( sizeof(tdb_dev_map_t) * row);
		if (pout == NULL) {
			return -1;
		}
		bzero(pout, sizeof(tdb_dev_map_t) * row);
		*ppout = pout;

		// dbResult是(row+1 * col)字符串，
		// 0~col是“列名”
		// 剩下的(row * col)是数据矩阵字符串
		// index偏移到有效数据

		index = col;
		printf("row = %d\n", row);

		for (int r = 0; r < row; r++) {
			if (dbResult[index]) {
				pout->id     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->frame     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->slot     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->type     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->port     = atoi(dbResult[index]);
			}
			index++;

			if (dbResult[index]) {
				memcpy(pout->dev_name,   dbResult[index],64);
			}
			index++;
			if (dbResult[index]) {
				memcpy(pout->cable_name, dbResult[index],64);
			}
			index++;
			if (dbResult[index]) {
				memcpy(pout->start_name, dbResult[index],64);
			}
			index++;
			if (dbResult[index]) {
				memcpy(pout->end_name,   dbResult[index],64);
			}
			index++;
			pout++;
		}

		// 释放表
		sqlite3_free_table(dbResult);
		return row;
	}

	return 0;
}

/**
 * @brief	搜索 tb_any_unit_osw 表某些行
 * @param	pcondition 搜索条件\n
 * @param	pmask 搜索条件掩码，当掩码不为0时，对应pcondition字段有效\n
		有效范围 any_frame、any_slot、any_type、any_port、osw_frame、osw_slot、osw_type、osw_port
 * @param	ppout 搜索到的行指针
 * @retval	>0 返回的行数，数据存在ppout，调用需要在使用完后调用free(ppout)
 */
int tmsdb_Select_any_unit_osw(
		tdb_any_unit_osw_t *pcondition, 
		tdb_any_unit_osw_t *pmask,
		tdb_any_unit_osw_t **ppout)
{
	char     sql[1024];
	char   **dbResult;
	int row, col;
	int index;
	tdb_any_unit_osw_t *pout;

	char sqlval1[64]  = "\0";
	char sqlval2[64]  = "\0";
	char sqlval3[64]  = "\0";
	char sqlval4[64]  = "\0";
	char sqlval5[64]  = "\0";
	char sqlval6[64]  = "\0";
	char sqlval7[64]  = "\0";
	char sqlval8[64]  = "\0";
	// 构造sql语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "select id,any_frame,any_slot,any_type,any_port,osw_frame,osw_slot,osw_type,osw_port from tb_any_unit_osw;");	
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->any_frame) {
			snprintf(sqlval1, 64, "and any_frame=%d", pcondition->any_frame);
		}
		if (pmask->any_slot) {
			snprintf(sqlval2, 64, "and any_slot=%d", pcondition->any_slot);
		}
		if (pmask->any_type) {
			snprintf(sqlval3, 64, "and any_type=%d", pcondition->any_type);
		}
		if (pmask->any_port) {
			snprintf(sqlval4, 64, "and any_port=%d", pcondition->any_port);
		}

		if (pmask->osw_frame) {
			snprintf(sqlval5, 64, "and osw_frame=%d", pcondition->osw_frame);
		}
		if (pmask->osw_slot) {
			snprintf(sqlval6, 64, "and osw_slot=%d", pcondition->osw_slot);
		}
		if (pmask->osw_type) {
			snprintf(sqlval7, 64, "and osw_type=%d", pcondition->osw_type);
		}
		if (pmask->osw_port) {
			snprintf(sqlval8, 64, "and osw_port=%d", pcondition->osw_port);
		}
		snprintf(sql, 1024, "select id,any_frame,any_slot,any_type,any_port,osw_frame,osw_slot,osw_type,osw_port from tb_any_unit_osw where(1 %s %s %s %s %s %s %s %s);",
			sqlval1, sqlval2, sqlval3, sqlval4,
			sqlval8, sqlval6, sqlval7, sqlval8);
	}
	
	printf("sql:%s\n",sql);
	// 执行sql语句
	_tmsdb_Select_any(DB_PATH, "tb_any_unit_osw", sql, "tmsdb_Select_any_unit_osw", &dbResult,&row, &col);
	if ( row > 0 ) {
		pout = (tdb_any_unit_osw_t*)malloc( sizeof(tdb_any_unit_osw_t) * row );
		if (pout == NULL) {
			return -1;
		}
		bzero(pout,  sizeof(tdb_any_unit_osw_t) * row);
		*ppout = pout;

		// dbResult是(row+1 * col)字符串，
		// 0~col是“列名”
		// 剩下的(row * col)是数据矩阵字符串
		// index偏移到有效数据
		index = col;
		printf("row = %d\n", row);
		for (int r = 0; r < row; r++) {
			if (dbResult[index]) {
				pout->id     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->any_frame     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->any_slot     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->any_type     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->any_port     = atoi(dbResult[index]);
			}
			index++;

			if (dbResult[index]) {
				pout->osw_frame     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->osw_slot     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->osw_type     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->osw_port     = atoi(dbResult[index]);
			}
			index++;


			pout++;
		}

		// 释放表
		sqlite3_free_table(dbResult);
		return row;
	}

	return 0;
}


/**
 * @brief	搜索 tb_osw_cyc 表某些行
 * @param	pcondition 搜索条件\n
 * @param	pmask 搜索条件掩码，当掩码不为0时，对应pcondition字段有效\n
		有效范围 osw_frame、osw_slot、osw_type、osw_port、iscyc、interval
 * @param	ppout 搜索到的行指针
 * @retval	>0 返回的行数，数据存在ppout，调用需要在使用完后调用free(ppout)
 */
int tmsdb_Select_osw_cyc(
		tdb_osw_cyc_t *pcondition, 
		tdb_osw_cyc_t *pmask,
		tdb_osw_cyc_t **ppout)
{
	char     sql[1024];
	char   **dbResult;
	int row, col;
	int index;
	tdb_osw_cyc_t *pout;

	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64] = "\0";
	char sqlval3[64] = "\0";
	char sqlval4[64] = "\0";
	char sqlval5[64] = "\0";
	char sqlval6[64] = "\0";


	// 构造sql语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "select id,osw_frame,osw_slot,osw_type,osw_port,iscyc,interval from tdb_osw_cyc;");	
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and osw_frame=%d", pcondition->id);
		}
		if (pmask->frame) {
			snprintf(sqlval1, 64, "and osw_frame=%d", pcondition->frame);
		}
		if (pmask->slot) {
			snprintf(sqlval2, 64, "and osw_slot=%d", pcondition->slot);
		}
		if (pmask->type) {
			snprintf(sqlval3, 64, "and osw_type=%d", pcondition->type);
		}
		if (pmask->port) {
			snprintf(sqlval4, 64, "and osw_port=%d", pcondition->port);
		}
		if (pmask->iscyc) {
			snprintf(sqlval5, 64, "and iscyc=%d", pcondition->iscyc);
		}
		if (pmask->interval) {
			snprintf(sqlval6, 64, "and interval=%d", pcondition->interval);
		}
		snprintf(sql, 1024, "select id,osw_frame,osw_slot,osw_type,osw_port,iscyc,interval from tb_osw_cyc where(1 %s %s %s %s %s %s %s);",
			sqlid, sqlval1, sqlval2, sqlval3, sqlval4, sqlval5, sqlval6);
	}
	printf("sql:%s\n",sql);
	// 执行sql语句
	_tmsdb_Select_any(DB_PATH, "tdb_osw_cyc", sql, "tmsdb_Select_osw_cyc", &dbResult,&row, &col);
	if ( row > 0 ) {
		pout = (tdb_osw_cyc_t*)malloc( sizeof(tdb_osw_cyc_t) * row);
		if (pout == NULL) {
			return -1;
		}
		bzero(pout, sizeof(tdb_osw_cyc_t) * row);
		*ppout = pout;

		// dbResult是(row+1 * col)字符串，
		// 0~col是“列名”
		// 剩下的(row * col)是数据矩阵字符串
		// index偏移到有效数据
		index = col;
		printf("row = %d\n", row);
		for (int r = 0; r < row; r++) {
			pout->id = 0;
			pout->frame = 0;
			pout->slot = 0;
			pout->type = 0;
			pout->port = 0;
			pout->iscyc = 0;
			if (dbResult[index]) {
				pout->id     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->frame     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->slot      = atoi(dbResult[index]);
				printf("%d \n",pout->slot);
			}
			index++;
			if (dbResult[index]) {
				pout->type      = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->port      = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->iscyc     = atoi(dbResult[index]);
			}
			index++;
			if (dbResult[index]) {
				pout->interval  = atoi(dbResult[index]);
			}
			index++;

			pout++;
		}

		// 释放表
		sqlite3_free_table(dbResult);
		return row;
	}

	return 0;
}

/**
 * @brief	搜索 tb_otdr_rollcall 表某些行
 * @param	pcondition 搜索条件\n
 * @param	pmask 搜索条件掩码，当掩码不为0时，对应pcondition字段有效\n
		有效范围 osw_frame、osw_slot、osw_type、osw_port
 * @param	pcallback 搜索到的行回调函数，有三个参数\n
 		out 搜索到的行指针，指向一个 tdb_otdr_rollcall_t 结构体\n
 		ptr 调用者的参数指针\n
  		pcallback返回0是如果还有行则继续调用pcallback，其他值则退出，返回值为pcallback的值
 * @param	ptr 用于传递给pcallback函数，调用者自定义\n
 * @retval	0 正常退出
 * @retval	<0 异常
 * @retval	>0 pcallback返回值
 */
int tmsdb_Select_otdr_rollcall(
		tdb_otdr_rollcall_t *pcondition, 
		tdb_otdr_rollcall_t *pmask,
		int (*pcallback)(tdb_otdr_rollcall_t *output, void *ptr), 
		void *ptr)
{
	sqlite3 *db;
	char     sql[1024];
	int rc;


	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64] = "\0";
	char sqlval3[64] = "\0";
	char sqlval4[64] = "\0";
	
	sqlite3_stmt *pstmt;
	tdb_otdr_rollcall_t  out;
	struct tms_getotdr_test_hdr    test_hdr;
	struct tms_getotdr_test_param  test_param;

	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	// 构造sql语句
	if (pcondition == NULL) {
		snprintf(sql, 1024, "select id,osw_frame,osw_slot,osw_type,osw_port,hdr_reserve0,\n\
rang,wl,pw,time,gi,end_threshold,none_reflect_threshold,param_reserve0,param_reserve1,param_reserve2\n\
from tb_otdr_rollcall;");	
	}
	else if (pcondition != NULL && pmask != NULL) {
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		if (pmask->ptest_hdr->frame) {
			snprintf(sqlval1, 64, "and osw_frame=%d", pcondition->ptest_hdr->frame);
		}
		if (pmask->ptest_hdr->slot) {
			snprintf(sqlval2, 64, "and osw_slot=%d", pcondition->ptest_hdr->slot);
		}
		if (pmask->ptest_hdr->type) {
			snprintf(sqlval3, 64, "and osw_type=%d", pcondition->ptest_hdr->type);
		}
		if (pmask->ptest_hdr->port) {
			snprintf(sqlval4, 64, "and osw_port=%d", pcondition->ptest_hdr->port);
		}
		snprintf(sql, 1024, "select id,osw_frame,osw_slot,osw_type,osw_port,hdr_reserve0,\n\
rang,wl,pw,time,gi,end_threshold,none_reflect_threshold,param_reserve0,param_reserve1,param_reserve2\n\
from tb_otdr_rollcall where(1 %s %s %s %s %s);",
			sqlid, sqlval1, sqlval2, sqlval3, sqlval4);
	}
	printf("sql: %s\n", sql);
	// 准备执行语句
	sqlite3_prepare(db, sql, -1, &pstmt, NULL);
	

	out.ptest_hdr   = &test_hdr;
	out.ptest_param = &test_param;
	while(sqlite3_step(pstmt) == SQLITE_ROW) {
		out.id    = sqlite3_column_int(pstmt, 0);
		// 填充 struct tms_otdr_ref_hdr
		test_hdr.frame    = sqlite3_column_int(pstmt, 1);
		test_hdr.slot     = sqlite3_column_int(pstmt, 2);
		test_hdr.type     = sqlite3_column_int(pstmt, 3);
		test_hdr.port     = sqlite3_column_int(pstmt, 4);
		test_hdr.reserve0 = sqlite3_column_int(pstmt, 5);
		
		// 填充 struct tms_retotdr_test_param
		test_param.rang     = sqlite3_column_int(pstmt, 6);
		test_param.wl       = sqlite3_column_int(pstmt, 7);
		test_param.pw       = sqlite3_column_int(pstmt, 8);
		test_param.time     = sqlite3_column_int(pstmt, 9);
		test_param.gi                     = sqlite3_column_double(pstmt, 10);
		test_param.end_threshold          = sqlite3_column_double(pstmt, 11);
		test_param.none_reflect_threshold = sqlite3_column_double(pstmt, 12);
		test_param.reserve0   = sqlite3_column_int(pstmt, 13);
		test_param.reserve1 = sqlite3_column_int(pstmt, 14);
		test_param.reserve2 = sqlite3_column_int(pstmt, 15);

	
		if (-1 == pcallback(&out, ptr)) {
			break;
		}
	}
	sqlite3_finalize(pstmt);
	sqlite3_close(db);
	return 0;
}


/**
 * @brief	搜索 tb_otdr_ref 表某些行
 * @param	pcondition 搜索条件\n
 * @param	pmask 搜索条件掩码，当掩码不为0时，对应pcondition字段有效\n
		有效范围 osw_frame、osw_slot、osw_type、osw_port
 * @param	pcallback 搜索到的行回调函数，有三个参数\n
 		out 搜索到的行指针，指向一个 tdb_otdr_ref_t 结构体\n
 		ptr 调用者的参数指针\n
 		pcallback返回0是如果还有行则继续调用pcallback，其他值则退出，返回值为pcallback的值
 * @param	ptr 用于传递给pcallback函数，调用者自定义\n
 * @retval	0 正常退出
 * @retval	<0 异常
 * @retval	>0 pcallback返回值
 */
int tmsdb_Select_otdr_ref(
		tdb_otdr_ref_t *pcondition, 
		tdb_otdr_ref_t *pmask,
		int (*pcallback)(tdb_otdr_ref_t *output, void *ptr), 
		void *ptr)
{
	sqlite3 *db;
	int rc;

	char sql[2048];
	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64] = "\0";
	char sqlval3[64] = "\0";
	char sqlval4[64] = "\0";

	sqlite3_stmt *pstmt;
	tdb_otdr_ref_t out;

	struct tms_otdr_ref_hdr   	  ref_hdr;
	struct tms_retotdr_test_param test_param;

	struct tms_retotdr_data_hdr   data_hdr;
	// struct tms_retotdr_data_val   *pdata_val = NULL;

	struct tms_retotdr_event_hdr  event_hdr;
	// struct tms_retotdr_event_val  *pevent_val = NULL;

	struct tms_retotdr_chain      chain;
	struct tms_cfg_otdr_ref_val   ref_data;

	
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	
	if (pcondition == NULL) {
		snprintf(sql, 1024, "select id,\n\
osw_frame,osw_slot,osw_type,osw_port,otdr_port,strid,\n\
rang,wl,pw,time,gi,end_threshold,none_reflect_threshold,param_reserve0,param_reserve1,param_reserve2,\n\
dpid,sample_count,\n\
sample_data,\n\
eventid,\n\
event_count,\n\
event_data,\n\
ch_inf,ch_range,ch_loss,ch_att,\n\
leve0,leve1,leve2\n\
 from tb_otdr_ref;");
	}

	else if (pcondition != NULL && pmask != NULL) 
	{
		if (pmask->id) {
			snprintf(sqlid, 64, "and id=%d", pcondition->id);
		}
		// 构造SQL语句
		if (pmask->pref_hdr->osw_frame) {
			snprintf(sqlval1, 64, "and osw_frame=%d", pcondition->pref_hdr->osw_frame);
		}
		if (pmask->pref_hdr->osw_slot) {
			snprintf(sqlval2, 64, "and osw_slot=%d", pcondition->pref_hdr->osw_slot);
		}
		if (pmask->pref_hdr->osw_type) {
			snprintf(sqlval3, 64, "and osw_type=%d", pcondition->pref_hdr->osw_type);
		}
		if (pmask->pref_hdr->osw_port) {
			snprintf(sqlval4, 64, "and osw_port=%d", pcondition->pref_hdr->osw_port);
		}
		snprintf(sql, 1024, "select id,\n\
osw_frame,osw_slot,osw_type,osw_port,otdr_port,strid,\n\
rang,wl,pw,time,gi,end_threshold,none_reflect_threshold,param_reserve0,param_reserve1,param_reserve2,\n\
dpid,sample_count,\n\
sample_data,\n\
eventid,\n\
event_count,\n\
event_data,\n\
ch_inf,ch_range,ch_loss,ch_att,\n\
leve0,leve1,leve2\n\
 from  tb_otdr_ref where(1 %s %s %s %s %s);",
			sqlid, sqlval1, sqlval2, sqlval3, sqlval4 );
	}
	printf("sql: %s\n", sql);
	// 准备执行语句
	sqlite3_prepare(db, sql, -1, &pstmt, NULL);

	out.pref_hdr    = &ref_hdr;
	out.ptest_param = &test_param;
	out.pdata_hdr   = &data_hdr;
	out.pdata_val   = NULL;
	out.pevent_hdr  = &event_hdr;
	out.pevent_val  = NULL;
	out.pchain      = &chain;
	out.pref_data   = &ref_data;
	while(sqlite3_step(pstmt) == SQLITE_ROW) {
		out.id = sqlite3_column_int(pstmt, 0);
		// 填充 struct tms_otdr_ref_hdr
		ref_hdr.osw_frame = sqlite3_column_int(pstmt, 1);
		ref_hdr.osw_slot  = sqlite3_column_int(pstmt, 2);
		ref_hdr.osw_type  = sqlite3_column_int(pstmt, 3);
		ref_hdr.osw_port  = sqlite3_column_int(pstmt, 4);
		ref_hdr.otdr_port = sqlite3_column_int(pstmt, 5);
		ref_hdr.strid[0] = '\0';
		BLOB_COPY_S(pstmt,6, ref_hdr.strid, 20);

		
		// 填充 struct tms_retotdr_test_param
		test_param.rang     = sqlite3_column_int(pstmt, 7);
		test_param.wl       = sqlite3_column_int(pstmt, 8);
		test_param.pw       = sqlite3_column_int(pstmt, 9);
		test_param.time     = sqlite3_column_int(pstmt, 10);
		test_param.gi                     = sqlite3_column_double(pstmt, 11);
		test_param.end_threshold          = sqlite3_column_double(pstmt, 12);
		test_param.none_reflect_threshold = sqlite3_column_double(pstmt, 13);
		test_param.sample   = sqlite3_column_int(pstmt, 14);
		test_param.reserve0 = sqlite3_column_int(pstmt, 15);
		test_param.reserve1 = sqlite3_column_int(pstmt, 16);

		// 填充 struct tms_retotdr_data_hdr
		data_hdr.dpid[0] = '\0';
		BLOB_COPY_S(pstmt,17, data_hdr.dpid, 12);
		data_hdr.count = sqlite3_column_bytes(pstmt, 19) / sizeof(struct tms_retotdr_data_val);
		// 填充 struct tms_retotdr_data_val
		out.pdata_val = (struct tms_retotdr_data_val*)sqlite3_column_blob(pstmt,19);
		
		// 填充 struct tms_retotdr_event_hdr
		event_hdr.eventid[0] = '\0';
		BLOB_COPY_S(pstmt,20, event_hdr.eventid, 12);
		event_hdr.count = sqlite3_column_bytes(pstmt, 22) / sizeof(struct tms_retotdr_event_val);

		// 填充 struct tms_retotdr_event_val
		out.pevent_val = (struct tms_retotdr_event_val*)sqlite3_column_blob(pstmt,22);

		// 填充 struct tms_retotdr_chain
		chain.inf[0] = '\0';
		BLOB_COPY_S(pstmt,23, chain.inf, 20);
		chain.range  = sqlite3_column_double(pstmt, 24);
		chain.loss   = sqlite3_column_double(pstmt, 25);
		chain.att    = sqlite3_column_double(pstmt, 26);

		// 填充 struct tms_cfg_otdr_ref_val
		ref_data.leve0 = sqlite3_column_int(pstmt,27);
		ref_data.leve1 = sqlite3_column_int(pstmt,28);
		ref_data.leve2 = sqlite3_column_int(pstmt,29);

		if (-1 == pcallback(&out, ptr)) {
			break;
		}
	}
		
	sqlite3_finalize(pstmt);
	sqlite3_close(db);

	return 0;
}

/**
 * @brief	搜索 tb_otdr_his_data 表某些行
 * @param	pcondition 搜索条件\n
 * @param	pmask 搜索条件掩码，当掩码不为0时，对应pcondition字段有效\n
		有效范围 osw_frame、osw_slot、osw_type、osw_port、otdr_frame、otdr_slot、otdr_type、otdr_port
 * @param	pcallback 搜索到的行回调函数，有三个参数\n
 		out 搜索到的行指针，指向一个 tdb_otdr_his_data_t 结构体\n
 		ptr 调用者的参数指针\n
  		pcallback返回0是如果还有行则继续调用pcallback，其他值则退出，返回值为pcallback的值
 * @param	ptr 用于传递给pcallback函数，调用者自定义\n
 * @retval	0 正常退出
 * @retval	<0 异常
 * @retval	>0 pcallback返回值
 */
int tmsdb_Select_otdr_his_data(
		tdb_otdr_his_data_t *pcondition, 
		tdb_otdr_his_data_t *pmask,
		int (*pcallback)(tdb_otdr_his_data_t *cbptr, void *ptr),
		void *ptr)
{
	sqlite3 *db;
	int rc;

	char sql[2048];
	char sqlid[64] = "\0";
	char sqlval1[64] = "\0";
	char sqlval2[64] = "\0";
	char sqlval3[64] = "\0";
	char sqlval4[64] = "\0";
	char sqlval5[64] = "\0";
	char sqlval6[64] = "\0";
	char sqlval7[64] = "\0";
	char sqlval8[64] = "\0";

	sqlite3_stmt *pstmt;
	tdb_otdr_his_data_t out;
	

	struct tms_retotdr_test_hdr   test_hdr;
	struct tms_retotdr_test_param test_param;

	struct tms_retotdr_data_hdr   data_hdr;
	// struct tms_retotdr_data_val   *pdata_val = NULL;

	struct tms_retotdr_event_hdr  event_hdr;
	// struct tms_retotdr_event_val  *pevent_val = NULL;

	struct tms_retotdr_chain      chain;

	if (pcallback == NULL) {
		return -1;
	}
	rc = sqlite3_open(DB_PATH, &db);
	if ( rc ){
		fprintf(stdin, "Can't open database:");// %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	
	if (pcondition == NULL) {
		snprintf(sql, 1024, "select id,\n\
osw_frame,osw_slot,osw_type,osw_port,date,otdr_frame,otdr_slot,otdr_type,otdr_port,\n\
rang,wl,pw,time,gi,end_threshold,none_reflect_threshold,sample,param_reserve1,param_reserve2,\n\
dpid,sample_count,\n\
sample_data,\n\
eventid,event_count,\n\
event_data,\n\
inf,range,loss,att\n\
 from tb_otdr_his_data;");
	}

	else if (pcondition != NULL && pmask != NULL)
	{
		if (pmask->id) {
			snprintf(sqlid, 64, "and osw_frame=%d", pcondition->id);
		}
		// 构造SQL语句
		if (pmask->ptest_hdr->osw_frame) {
			snprintf(sqlval1, 64, "and osw_frame=%d", pcondition->ptest_hdr->osw_frame);
		}
		if (pmask->ptest_hdr->osw_slot) {
			snprintf(sqlval2, 64, "and osw_slot=%d", pcondition->ptest_hdr->osw_slot);
		}
		if (pmask->ptest_hdr->osw_type) {
			snprintf(sqlval3, 64, "and osw_type=%d", pcondition->ptest_hdr->osw_type);
		}
		if (pmask->ptest_hdr->osw_port) {
			snprintf(sqlval4, 64, "and osw_port=%d", pcondition->ptest_hdr->osw_port);
		}
		if (pmask->ptest_hdr->otdr_frame) {
			snprintf(sqlval1, 64, "and otdr_frame=%d", pcondition->ptest_hdr->otdr_frame);
		}
		if (pmask->ptest_hdr->otdr_slot) {
			snprintf(sqlval2, 64, "and otdr_slot=%d", pcondition->ptest_hdr->otdr_slot);
		}
		if (pmask->ptest_hdr->otdr_type) {
			snprintf(sqlval3, 64, "and otdr_type=%d", pcondition->ptest_hdr->otdr_type);
		}
		if (pmask->ptest_hdr->otdr_port) {
			snprintf(sqlval4, 64, "and otdr_port=%d", pcondition->ptest_hdr->otdr_port);
		}
		snprintf(sql, 1024, "select id,\n\
osw_frame,osw_slot,osw_type,osw_port,date,otdr_frame,otdr_slot,otdr_type,otdr_port,\n\
rang,wl,pw,time,gi,end_threshold,none_reflect_threshold,sample,param_reserve1,param_reserve2,\n\
dpid,sample_count,\n\
sample_data,\n\
eventid,event_count,\n\
event_data,\n\
inf,range,loss,att\n\
 from  tb_otdr_his_data where(1 %s %s %s %s %s %s %s %s %s);",
			sqlid, sqlval1, sqlval2, sqlval3, sqlval4, sqlval5, sqlval6, sqlval7, sqlval8 );
	}
	sqlite3_prepare(db, sql, -1, &pstmt, NULL);

	
	
	out.ptest_hdr 	= &test_hdr;
	out.ptest_param = &test_param;
	out.pdata_hdr 	= &data_hdr;
	out.pdata_val 	= NULL;
	out.pevent_hdr 	= &event_hdr;
	out.pevent_val 	= NULL;
	out.pchain 		= &chain;
	

	while(sqlite3_step(pstmt) == SQLITE_ROW) {
		out.id = sqlite3_column_int(pstmt, 0);
		// 填充 struct tms_retotdr_test_hdr
		test_hdr.osw_frame = sqlite3_column_int(pstmt, 1);
		test_hdr.osw_slot  = sqlite3_column_int(pstmt, 2);
		test_hdr.osw_type  = sqlite3_column_int(pstmt, 3);
		test_hdr.osw_port  = sqlite3_column_int(pstmt, 4);
		test_hdr.time[0] = '\0';
		BLOB_COPY_S(pstmt,5, test_hdr.time, 20);
		test_hdr.otdr_frame = sqlite3_column_int(pstmt, 6);
		test_hdr.otdr_slot = sqlite3_column_int(pstmt, 7);
		test_hdr.otdr_type = sqlite3_column_int(pstmt, 8);
		test_hdr.otdr_port = sqlite3_column_int(pstmt, 9);
		
		// 填充 struct tms_retotdr_test_param
		test_param.rang     = sqlite3_column_int(pstmt, 10);
		test_param.wl       = sqlite3_column_int(pstmt, 11);
		test_param.pw       = sqlite3_column_int(pstmt, 12);
		test_param.time     = sqlite3_column_int(pstmt, 13);
		test_param.gi                     = sqlite3_column_double(pstmt, 14);
		test_param.end_threshold          = sqlite3_column_double(pstmt, 15);
		test_param.none_reflect_threshold = sqlite3_column_double(pstmt, 16);
		test_param.sample   = sqlite3_column_int(pstmt, 17);
		test_param.reserve0 = sqlite3_column_int(pstmt, 18);
		test_param.reserve1 = sqlite3_column_int(pstmt, 19);

		// 填充 struct tms_retotdr_data_hdr
		data_hdr.dpid[0] = '\0';
		BLOB_COPY_S(pstmt,20, data_hdr.dpid, 12);
		data_hdr.count = sqlite3_column_int(pstmt, 21);
		data_hdr.count = sqlite3_column_bytes(pstmt, 22) / sizeof(struct tms_retotdr_data_val);
		// 填充 struct tms_retotdr_data_val
		out.pdata_val = (struct tms_retotdr_data_val*)sqlite3_column_blob(pstmt,22);
		
		// 填充 struct tms_retotdr_event_hdr
		event_hdr.eventid[0] = '\0';
		BLOB_COPY_S(pstmt,23, event_hdr.eventid, 12);
		event_hdr.count = sqlite3_column_int(pstmt, 24);
		event_hdr.count = sqlite3_column_bytes(pstmt, 25) / sizeof(struct tms_retotdr_event_val);

		// 填充 struct tms_retotdr_event_val
		out.pevent_val = (struct tms_retotdr_event_val*)sqlite3_column_blob(pstmt,25);

		// 填充 struct tms_retotdr_chain
		chain.inf[0] = '\0';
		BLOB_COPY_S(pstmt,26, chain.inf, 20);
		chain.range  = sqlite3_column_double(pstmt, 27);
		chain.loss   = sqlite3_column_double(pstmt, 28);
		chain.att    = sqlite3_column_double(pstmt, 29);

		if (-1 == pcallback(&out, ptr)) {
			break;
		}
	}
		
	sqlite3_finalize(pstmt);
	sqlite3_close(db);


	return 0;
}


#ifdef __cplusplus
}
#endif