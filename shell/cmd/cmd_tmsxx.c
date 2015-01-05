/**
 ******************************************************************************
 * @file	cmd_tmsxx.c
 * @brief	TMSxx项目命令行调试及应用


*/

#ifdef __cplusplus
extern "C" {
#endif

#include "minishell_core.h"
#include "epollserver.h"
#include "tms_app.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "signal.h"
#include <ctype.h>


#include "./protocol/tmsxx.h"

#include <stdlib.h>
#include <locale.h>
#include "src/tmsxxdb.h"

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

extern struct ep_t ep;
extern int g_en_connect_cu;


static int sg_frameid = 0;
static int sg_slotid = 0;
static int sg_sockfdid = 0;
static int sg_portid = 0;
static int sg_sudo = 0;
static struct tms_cfg_olp_ref_val sg_olpref[3];
static struct tms_cfg_olp_ref_val sg_olpref_def;
static struct tms_cfg_opm_ref_val sg_opmref[8];
static struct tms_cfg_opm_ref_val sg_opmref_def;

// 内部数据结构
struct _dispinf
{
	struct trace_cache tc;
	int index;
};



int32_t UpdateOTDR(char *fname, char *ip, int16_t port)
{
	int32_t flen;

	FILE *fp = NULL;
	fp = fopen(fname, "rb");
	if (fp == NULL) {
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	flen = ftell(fp);
	printf("flen %d\n", flen);

	fseek(fp, 0, SEEK_SET);
	
	char *pmem = (char*)malloc(flen);
	char *ptmem;

	fread(pmem, 1, flen, fp);

	ptmem = pmem;
	for (int i = 0; i < flen; i++) {
		printf("%c", *ptmem++);
	}

	fclose(fp);
	// TODO 发送
	free(pmem);
	return 0;
}
int cmd_tms(int argc, char **argv)
{
	char sbuf[1024];
	if (argc == 1){
		printf("Usage:\n");
		printf("\ttr485 <inf>\n");
		printf("\ttr485 <data> <user data>\n");
		printf("\ttr485 <data> -f <file path>\n");
		printf("\ttr485 <token> <addr>\n");
		printf("\ttr485 <name> <local name>\n");
		printf("\ttr485 <name> <name>\n");
	}
	if (argc >= 2) {
		int fd;
		fd = atoi(argv[1]);
		struct tms_sms_duty duty;
		struct tms_sms_clerk clerk[2];
		struct tms_sms_alarm alarm1[3], alarm2[2];



		duty.time = 0xa;
		duty.clerk_count = 2;
		duty.clerk     = &clerk[0];


		clerk[0].alarm = &alarm1[0];
		strcpy((char*)clerk[0].phone, "12345");
		clerk[1].alarm = &alarm2[0];
		strcpy((char*)clerk[1].phone, "67891");
		clerk[0].alarm_count = 3;
		clerk[1].alarm_count = 2;


		alarm1[0].type = 0x10;
		alarm1[1].type = 0x20;
		alarm1[2].type = 0x30;
		alarm1[0].level = 0x1a;
		alarm1[1].level = 0x2a;
		alarm1[2].level = 0x3a;

		
		
		alarm2[0].type = 0x11;
		alarm2[1].type = 0x21;
		alarm2[0].level = 0xca;
		alarm2[1].level = 0xcb;

		char sn[128] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		// tms_RetSerialNumber(fd, sn);
		// tms_SetSendSMSAuthorization_old(fd, &duty);
		
	}
	return 0;
}

W_BOOT_CMD(eptms, cmd_tms, "cmd epoll server send");


void cmd_InitTmsxxEnv()
{
	sg_olpref_def.ref_power = -150;
	sg_olpref_def.leve0 = 100;
	sg_olpref_def.leve1 = 80;
	sg_olpref_def.leve2 = 50;
	for (int i = 0; i < 3; i++) {
		sg_olpref[i].isminitor = 1;
		sg_olpref[i].wave = 1550;
		sg_olpref[i].port = i;
		sg_olpref[i].isminitor = 1;
		sg_olpref[i].wave = 1550;
		sg_olpref[i].ref_power = -150;
		sg_olpref[i].leve0 = 100;
		sg_olpref[i].leve1 = 80;
		sg_olpref[i].leve2 = 50;
	}


	sg_opmref_def.ref_power = -150;
	sg_opmref_def.leve0 = 100;
	sg_opmref_def.leve1 = 80;
	sg_opmref_def.leve2 = 50;
	for (int i = 0; i < 8; i++) {
		sg_opmref[i].isminitor = 1;
		sg_opmref[i].wave = 1550;
		sg_opmref[i].port = i;
		sg_opmref[i].isminitor = 1;
		sg_opmref[i].wave = 1550;
		sg_opmref[i].ref_power = -150;
		sg_opmref[i].leve0 = 100;
		sg_opmref[i].leve1 = 80;
		sg_opmref[i].leve2 = 50;
	}
}
int cf_howline(char *file, int row, int *start, int *end)
{
	int rlen;
	int trow;
	char strout[1024] = "12311l1k2jijaoiweor";
	FILE *fp;
	int startIndex,endIndex,total;
	int len;

	startIndex = 0;
	endIndex = 0;
	if (row < 1) {
		return -1;
	}

	fp = fopen(file, "r");
	if (fp == NULL) {
		return -1;
	}
	


	trow = row;
	startIndex = 0;
	if (trow == 1) {
		goto _Next;
	}
	while(1) {
		rlen = fread(strout, 1, 10, fp);
		if (rlen > 0) {
			for (int i = 0; i < rlen; i++) {
				if (strout[i] == '\n') {
					trow--;
					if (trow == 1) {
						startIndex += i + 1;
						goto _Next;	
					}
					
				}

			}
		}
		else {
			break;
		}
		startIndex += 10;
	}
_Next:;
	fseek(fp, startIndex, SEEK_SET);

	endIndex = startIndex;
	while(1) {
		rlen = fread(strout, 1, 10, fp);
		if (rlen > 0) {
			for (int i = 0; i < rlen; i++) {
				if (strout[i] == '\n') {
					endIndex += i;
					goto _JumpReadLine;	
				}
			}
		}
		else {
			break;
		}
		endIndex += 10;
	}

_JumpReadLine:;
	fseek(fp, startIndex, SEEK_SET);

	total = endIndex - startIndex;
	if (total == 0) {
		return -1;
	}
	while(total > 0) {
		if (total > 1024) {
			len = 1024;
		}
		else {
			len = total;
		}
		rlen = fread(strout, 1, len, fp);
		if (rlen >= 0) {
			total  -= rlen;
			strout[rlen+1] = '\0';
			printf("%4.4d:%s\n", row, strout);
		}
		else {
			break;
		}
	}
	fclose(fp);

	*start = startIndex;
	*end   = endIndex;
	return 0;

}

int cmd_listfile(int argc, char **argv)
{
	char *pfpath;
	int row,trow;
	FILE *fp;
	char strout[1024] = "12311l1k2jijaoiweor";
	size_t rlen;



	if (argc == 2) {
		for (int i = 10; i < 30; i++) {
			int start = 0, end = 0;
			pfpath = argv[1];
			row = i+1;
			cf_howline(pfpath, row, &start, &end);


		}
		return 0;


		pfpath = argv[1];
		fp = fopen(pfpath, "r");
		if (fp == NULL) {
			return -1;
		}
		rlen = 1;
		while(1) {
			rlen = fread(strout, 1, 3, fp);

			if (rlen > 0) {
				strout[rlen] = '\0';
				printf("%s", strout);
			}
			else {
				break;
			}
		}
		putchar('\n');
		fclose(fp);
	}
	else if (argc == 3) {
		int start, end;
		pfpath = argv[1];
		row = atoi(argv[2]);
		cf_howline(pfpath, row, &start, &end);
		printf("start %d end %d\n", start, end);
		return 0;
	}


}
W_BOOT_CMD(lf, cmd_listfile, "cmd epoll server send");
#include "readline/readline.h"
#include "readline/history.h"
extern char *rl_display_prompt ;
int cmd_sql(int argc, char **argv)
{
	char sql[1024];
	char *ptprompt;
	char *pstr;
	// 不带strcat溢出检查
	
	// strcpy(sql, "sqlite3 /etc/tmsxx.db \"");
	// for (int i = 1; i < argc; i++) {
	// 	strcat(sql, argv[i]);
	// 	strcat(sql, " ");
	// }
	// strcat(sql, "\"");
	pstr = readline("SQL:>");
	if (pstr != NULL) {
		snprintf(sql, 1024, "sqlite3 /etc/tmsxx.db \"%s\"",pstr);
		system(sql);
		if (*pstr != '\0') {
			add_history(pstr);
		}
		free(pstr);
	}
	return 0;
}
W_BOOT_CMD(sql, cmd_sql, "call sqlite3 console");

static int _cb_Select_commom(tdb_common_t *output, void *ptr, int len)
{
	printf("id %d val1 %d val2 %d val3 %d val4 %d val5 %d \n\
		val6 %d val7 %d val8 %d val9 %d val10 %d val11 %d val12 %d pdata %s\n",
		output->id,output->val1,output->val2,output->val3,output->val4,output->val5,
		output->val6,output->val7,output->val8,output->val9,output->val10,output->val11,output->val12,
		output->pdata);
	return 0;

}

static int _cb_Select_otdr_rollcall(tdb_otdr_rollcall_t *output, void *ptr)
{
	// 打印部分信息
	tms_Print_tms_getotdr_test_hdr( output->ptest_hdr);
	tms_Print_tms_retotdr_test_param((struct tms_retotdr_test_param*)output->ptest_param);
	return 0;
}

static int _cb_Select_otdr_ref(tdb_otdr_ref_t *output, void *ptr)
{
	// 打印部分信息
	printf("addr %x\n", (int)output->pevent_val);
	tms_Print_tms_retotdr_event(output->pevent_hdr, output->pevent_val);
	tms_Print_tms_retotdr_chain(output->pchain);
	tms_Print_tms_otdr_ref_hdr(output->pref_hdr);
	tms_Print_tms_retotdr_test_param(output->ptest_param);
	return 0;
}

static int _cb_Select_otdr_his_data(tdb_otdr_his_data_t *output, void *ptr)
{
	// 打印部分信息
	tms_Print_tms_retotdr_event(output->pevent_hdr, output->pevent_val);
	tms_Print_tms_retotdr_chain(output->pchain);
	tms_Print_tms_retotdr_test_hdr(output->ptest_hdr);
	tms_Print_tms_retotdr_test_param(output->ptest_param);
	return 0;

}
int cmd_select(int argc, char **argv)
{
	int row;
	if (argc >= 3 && memcmp(argv[1], "tb_common", strlen(argv[1])) == 0) {
		tdb_common_t input, mask;

		bzero(&mask,  sizeof(tdb_common_t));
		input.val1 = atoi(argv[2]);	
		mask.val1 = 1;
		mask.val2 = 0;
		mask.val3 = 0;
		mask.val4 = 0;
		tmsdb_Select_common(&input, &mask, _cb_Select_commom, 0 );
	}
	else if (argc == 3 && memcmp(argv[1], "tb_sn", strlen(argv[1])) == 0) {
		tdb_sn_t input, mask;
		tdb_sn_t *ppout;

		bzero(&mask,  sizeof(tdb_common_t));
		strcpy((char*)&input.sn[0], argv[2]);
		
		mask.sn[0] = 1;
		row = tmsdb_Select_sn(&input, &mask, &ppout);
		printf("row = %d\n",row);

		for (int r = 0; r < row; r++) {
			printf("%s\n",ppout[r].sn);
		}
		// 注意，用完后必须释放
		free(ppout);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_sms", strlen(argv[1])) == 0) {
		tdb_sms_t *ppout = NULL;
		tdb_sms_t input, mask;

		bzero(&mask,  sizeof(tdb_sms_t));	
		// strcpy((char*)&input.phone[0], argv[2]);
		input.time = atoi(argv[2]);
		mask.time = 1;
		// mask.phone[0] = 1;

		row = tmsdb_Select_sms(&input, &mask, &ppout);
		printf("row = %d\n",row);

		for (int r = 0; r < row; r++) {
			printf("%d\t",ppout[r].id);
			printf("%d\t",ppout[r].time);
			printf("%s\t",ppout[r].phone);
			printf("%d\t",ppout[r].type);
			printf("%d\n",ppout[r].level);
		}
		// 注意，用完后必须释放
		free(ppout);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_composition", strlen(argv[1])) == 0) {
		tdb_composition_t *ppout = NULL;
		tdb_composition_t input, mask;

		bzero(&mask,  sizeof(tdb_composition_t));	
		input.frame = atoi(argv[2]);

		mask.frame = 1;

		row = tmsdb_Select_composition(&input, &mask, &ppout);
		
		printf("row = %d\n",row);

		for (int r = 0; r < row; r++) {
			printf("%d\t",ppout[r].id);
			printf("%d\t",ppout[r].frame);
			printf("%d\t",ppout[r].slot);
			printf("%d\t",ppout[r].type);
			printf("%d\n",ppout[r].port);
		}
		// 注意，用完后必须释放
		free(ppout);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_dev_map", strlen(argv[1])) == 0) {
		tdb_dev_map_t *ppout = NULL;
		tdb_dev_map_t input, mask;

		bzero(&mask, sizeof(tdb_dev_map_t));
		input.frame = atoi(argv[2]);
		mask.frame = 1;
		row = tmsdb_Select_dev_map(&input, &mask, &ppout);
		
		printf("row = %d\n",row);

		for (int r = 0; r < row; r++) {
			printf("%d\t",ppout[r].id);
			printf("%d\t",ppout[r].frame);
			printf("%d\t",ppout[r].slot);
			printf("%d\t",ppout[r].type);
			printf("%d\t",ppout[r].port);
			printf("%s\t",ppout[r].dev_name );
			printf("%s\t",ppout[r].cable_name );
			printf("%s\t",ppout[r].start_name );
			printf("%s\n",ppout[r].end_name );
		}
		// 注意，用完后必须释放
		free(ppout);
	}
	else if (argc>=  3 && memcmp(argv[1], "tb_any_unit_osw", strlen(argv[1])) == 0) {
		tdb_any_unit_osw_t *ppout = NULL;
		tdb_any_unit_osw_t input, mask;

		bzero(&mask,  sizeof(tdb_common_t));
		input.any_frame = atoi(argv[2]);	
		mask.any_frame = 1;
		row = tmsdb_Select_any_unit_osw(&input, &mask, &ppout);
		
		printf("row = %d\n",row);

		for (int r = 0; r < row; r++) {
			printf("%d\t",ppout[r].id);
			printf("%d\t",ppout[r].any_frame);
			printf("%d\t",ppout[r].any_slot);
			printf("%d\t",ppout[r].any_type);
			printf("%d\n",ppout[r].any_port);
		}
		// 注意，用完后必须释放
		free(ppout);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_osw_cyc", strlen(argv[1])) == 0) {
		tdb_osw_cyc_t *ppout = NULL;
		tdb_osw_cyc_t input, mask;

		bzero(&mask, sizeof(tdb_osw_cyc_t));
		input.frame = atoi(argv[2]);
		mask.frame = 1;
		row = tmsdb_Select_osw_cyc(&input, &mask, &ppout);

		printf("row = %d\n",row);
		for (int r = 0; r < row; r++) {
			printf("%d\t",ppout[r].id);
			printf("%d\t",ppout[r].frame);
			printf("%d\t",ppout[r].slot);
			printf("%d\t",ppout[r].type);
			printf("%d\t",ppout[r].port);
			printf("%d\t",ppout[r].iscyc);
			printf("%d\n",ppout[r].interval);

		}
		// 注意，用完后必须释放
		free(ppout);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_otdr_rollcall", strlen(argv[1])) == 0) {
		tdb_otdr_rollcall_t input, mask;
		struct tms_getotdr_test_hdr itest_hdr,mtest_hdr;
		
		bzero(&mask,      sizeof(tdb_otdr_rollcall_t));
		bzero(&mtest_hdr, sizeof(struct tms_getotdr_test_hdr));
		input.ptest_hdr = &itest_hdr;
		mask.ptest_hdr  = &mtest_hdr;
		
		itest_hdr.frame = atoi(argv[2]);
		mtest_hdr.frame = 1;

		tmsdb_Select_otdr_rollcall(&input, &mask, _cb_Select_otdr_rollcall, NULL);
		// tmsdb_Select_otdr_rollcall(NULL, NULL, _cb_Select_otdr_rollcall, NULL);
	}

	else if (argc >=  3 && memcmp(argv[1], "tb_otdr_ref", strlen(argv[1])) == 0) {
		tdb_otdr_ref_t input, mask;
		struct tms_otdr_ref_hdr itest_hdr,mtest_hdr;
		
		bzero(&mask,      sizeof(tdb_otdr_ref_t));
		bzero(&mtest_hdr, sizeof(struct tms_otdr_ref_hdr));
		input.pref_hdr = &itest_hdr;
		mask.pref_hdr  = &mtest_hdr;
		
		mask.id = 0;
		itest_hdr.osw_frame = atoi(argv[2]);
		mtest_hdr.osw_frame = 1;

		tmsdb_Select_otdr_ref(&input, &mask, _cb_Select_otdr_ref, NULL);
	}
	else if (argc >=  3 && memcmp(argv[1], "tb_otdr_his_data", strlen(argv[1])) == 0) {
		tdb_otdr_his_data_t input, mask;
		struct tms_retotdr_test_hdr itest_hdr,mtest_hdr;
		
		bzero(&mask,      sizeof(tdb_otdr_his_data_t));
		bzero(&mtest_hdr, sizeof(struct tms_retotdr_test_hdr));
		input.ptest_hdr = &itest_hdr;
		mask.ptest_hdr = &mtest_hdr;
		
		mask.id = 0;
		itest_hdr.osw_frame = atoi(argv[2]);
		mtest_hdr.osw_frame = 1;

		tmsdb_Select_otdr_his_data(&input, &mask, _cb_Select_otdr_his_data, NULL);
	}
	else {
		printf("Usage: input err\n");
	}
	// if(1) {
	// 	tdb_composition_t con,mask;
	// 	con.frame = 1;con.slot = 2;
	// 	mask.frame = 1;mask.slot = 1;
	// 	mask.port = 0;mask.type = 0;
	// 	tmsdb_Delete_composition(0,0);

	// 	tmsdb_Delete_composition(&con, &mask);
	// }
}
W_BOOT_CMD(select, cmd_select, "select tmsxxdb");


int cmd_delete(int argc, char **argv)
{
	// tmsdb_Delete_otdr_ref(NULL,NULL);

	if (argc == 6 && memcmp(argv[1], "tb_common", strlen(argv[1])) == 0) {
		tdb_common_t input, mask;

		bzero(&mask,  sizeof(tdb_common_t));
		input.val1 = atoi(argv[2]);	
		input.val2 = atoi(argv[3]);	
		input.val3 = atoi(argv[4]);	
		input.val4 = atoi(argv[5]);	
		mask.val1 = 1;
		mask.val2 = 1;
		mask.val3 = 1;
		mask.val4 = 1;
		tmsdb_Delete_common(&input, &mask);
	}
	if (argc == 2 && memcmp(argv[1], "tb_sn", strlen(argv[1])) == 0) {
		tdb_sn_t input, mask;

		bzero(&mask,  sizeof(tdb_common_t));
		
		tmsdb_Delete_sn(&input, &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_sms", strlen(argv[1])) == 0) {
		tdb_sms_t input, mask;

		bzero(&input,  sizeof(tdb_sms_t));
		bzero(&mask,  sizeof(tdb_sms_t));
		input.time = atoi(argv[2]);	
		strcpy((char*)&input.phone[0], argv[3]);
		// input.type = atoi(argv[3]);	

		mask.time =1;
		// mask.type =1;
		mask.phone[0] = 1;
		
		tmsdb_Delete_sms(&input, &mask);
	}

	else if (argc >= 4 && memcmp(argv[1], "tb_composition", strlen(argv[1])) == 0) {
		tdb_composition_t input[1], mask;

		bzero(&mask,  sizeof(tdb_composition_t));
		input[0].frame = atoi(argv[2]);	
		input[0].slot = atoi(argv[3]);	

		mask.frame =1;
		mask.slot = 1;
		mask.type = 1;
		mask.port = 1;

		tmsdb_Delete_composition(&input[0], &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_dev_map", strlen(argv[1])) == 0) {
		tdb_dev_map_t input[1], mask;

		bzero(&mask,  sizeof(tdb_dev_map_t));
		input[0].frame = atoi(argv[2]);	
		input[0].slot = atoi(argv[3]);	
		input[0].type = 1;	
		input[0].port = 2;

		mask.frame =1;
		mask.slot = 1;
		mask.type = 1;
		mask.port = 1;

		tmsdb_Delete_dev_map(&input[0], &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_any_unit_osw", strlen(argv[1])) == 0) {
		tdb_any_unit_osw_t input[1], mask;

		bzero(&mask, sizeof(tdb_any_unit_osw_t));
		input[0].any_frame = atoi(argv[2]);	
		input[0].any_slot  = atoi(argv[3]);	

		mask.any_frame =1;
		mask.any_slot = 1;
		mask.any_type = 0;
		mask.any_port = 0;

		tmsdb_Delete_any_unit_osw(&input[0], &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_osw_cyc", strlen(argv[1])) == 0) {
		tdb_osw_cyc_t input, mask;

		bzero(&mask, sizeof(tdb_osw_cyc_t));
		input.frame = atoi(argv[2]);	
		input.slot  = atoi(argv[3]);	

		mask.frame =1;
		mask.slot = 1;

		tmsdb_Delete_osw_cyc(&input, &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_otdr_his_data", strlen(argv[1])) == 0) {
		tdb_otdr_his_data_t input, mask;
		struct tms_retotdr_test_hdr itest_hdr,mtest_hdr;

		bzero(&mask,      sizeof(tdb_otdr_his_data_t));
		bzero(&mtest_hdr, sizeof(struct tms_retotdr_test_hdr));
		input.ptest_hdr = &itest_hdr;
		mask.ptest_hdr  = &mtest_hdr;
		
		mask.id = 0;
		itest_hdr.osw_frame = atoi(argv[2]);
		itest_hdr.osw_slot = atoi(argv[3]);
		mtest_hdr.osw_frame = 1;
		mtest_hdr.osw_slot = 1;
		
		tmsdb_Delete_otdr_his_data(&input, &mask);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_otdr_rollcall", strlen(argv[1])) == 0) {
		tdb_otdr_rollcall_t input, mask;
		struct tms_getotdr_test_hdr itest_hdr,mtest_hdr;

		bzero(&mask,      sizeof(tdb_otdr_rollcall_t));	
		bzero(&mtest_hdr, sizeof(struct tms_getotdr_test_hdr));
		input.ptest_hdr = &itest_hdr;
		mask.ptest_hdr  = &mtest_hdr;
		
		itest_hdr.frame = atoi(argv[2]);
		itest_hdr.slot = atoi(argv[3]);
		mtest_hdr.frame = 1;
		mtest_hdr.slot = 1;
		tmsdb_Delete_otdr_rollcall(&input, &mask);
	}

	else if (argc >= 4 && memcmp(argv[1], "tb_otdr_ref", strlen(argv[1])) == 0) {
		tdb_otdr_ref_t input, mask;
		struct tms_otdr_ref_hdr itest_hdr,mtest_hdr;

		bzero(&mask,      sizeof(tdb_otdr_ref_t));	
		bzero(&mtest_hdr, sizeof(struct tms_otdr_ref_hdr));
		input.pref_hdr = &itest_hdr;
		mask.pref_hdr  = &mtest_hdr;

		itest_hdr.osw_frame = atoi(argv[2]);
		itest_hdr.osw_slot = atoi(argv[3]);
		mask.id = 0;
		mtest_hdr.osw_frame = 1;
		mtest_hdr.osw_slot = 1;
		tmsdb_Delete_otdr_ref(&input, &mask);
	}
	else {
		printf("Usage: input err\n");
	}
}
W_BOOT_CMD(delete, cmd_delete, "delete tmsxxdb");

int cmd_insert(int argc, char **argv)
{
	if (argc == 7 && memcmp(argv[1], "tb_common", strlen(argv[1])) == 0) {
		tdb_common_t input, mask;

		bzero(&mask,  sizeof(tdb_common_t));
		input.val1 = atoi(argv[2]);	
		input.val2 = atoi(argv[3]);	
		input.val3 = atoi(argv[4]);	
		input.val4 = atoi(argv[5]);	
		input.pdata = argv[6];
		input.lenpdata = strlen((char*)input.pdata);
		mask.val1 = 1;
		mask.val2 = 1;
		mask.val3 = 1;
		mask.val4 = 1;
		tmsdb_Insert_common(&input, &mask, 1);
	}
	else if (argc == 3 && memcmp(argv[1], "tb_sn", strlen(argv[1])) == 0) {
		tdb_sn_t input, mask;

		bzero(&mask,  sizeof(tdb_common_t));
		strcpy((char*)&input.sn[0], argv[2]);
		
		mask.sn[0] = 1;
		tmsdb_Insert_sn(&input, &mask, 1);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_sms", strlen(argv[1])) == 0) {
		tdb_sms_t input, mask;
		bzero(&input, sizeof(tdb_sms_t));
		bzero(&mask, sizeof(tdb_sms_t));
		input.time = atoi(argv[2]);	
		// input.slot = atoi(argv[3]);	
		strcpy((char*)&input.phone[0], argv[3]);
		// input.type = atoi(argv[3]);	
		input.level = 2;
		
		mask.time =1;
		mask.phone[0] =1;
		tmsdb_Insert_sms(&input, &mask, 1);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_composition", strlen(argv[1])) == 0) {
		tdb_composition_t input[2], mask;

		input[0].frame = atoi(argv[2]);	
		input[0].slot = atoi(argv[3]);	
		input[0].type = 0;
		input[0].port = 0;
		input[1].frame = input[0].frame;
		input[1].slot  = input[0].slot ;

		// printf("%d %d %d %d\n",input[1].frame,);
		mask.frame =1;
		mask.slot = 1;
		mask.type = 0;
		mask.port = 0;
		tmsdb_Insert_composition(&input[0], &mask, 1);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_dev_map", strlen(argv[1])) == 0) {
		tdb_dev_map_t input[2], mask;

		input[0].frame = atoi(argv[2]);	
		input[0].slot = atoi(argv[3]);	
		input[0].type = 11;	
		input[0].port = 8;

		snprintf((char*)&input[0].dev_name, 64, "dev_name");
		snprintf((char*)&input[0].cable_name, 64, "cable_name");
		snprintf((char*)&input[0].start_name, 64, "start_name");
		snprintf((char*)&input[0].end_name, 64, "end_name");
		

		input[1].frame = input[0].frame;
		input[1].slot  = input[0].slot ;
		input[1].type  = input[0].type *10;
		input[1].port  = input[0].port *10;
		
		snprintf((char*)&input[1].dev_name, 64, "dev_name");
		snprintf((char*)&input[1].cable_name, 64, "cable_name");
		snprintf((char*)&input[1].start_name, 64, "start_name");
		snprintf((char*)&input[1].end_name, 64, "end_name");

		mask.frame =1;
		mask.slot = 1;
		mask.type = 1;
		mask.port = 1;
		tmsdb_Insert_dev_map(&input[0], &mask, 1);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_any_unit_osw", strlen(argv[1])) == 0) {
		tdb_any_unit_osw_t input[2], mask;

		input[0].any_frame = atoi(argv[2]);	
		input[0].any_slot = atoi(argv[3]);	
		input[0].any_type = 2;	
		input[0].any_port = 3;

		mask.any_frame = 1;
		mask.any_slot = 1;
		mask.any_type = 1;
		mask.any_port = 1;
		tmsdb_Insert_any_unit_osw(&input[0], &mask, 1);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_osw_cyc", strlen(argv[1])) == 0) {
		tdb_osw_cyc_t input, mask;

		bzero(&mask, sizeof(tdb_osw_cyc_t));
		input.frame = atoi(argv[2]);	
		input.slot = atoi(argv[3]);	
		input.type = 1;	
		input.port = 2;

		mask.frame = 1;
		mask.slot = 1;
		mask.type = 1;
		mask.port = 1;
		tmsdb_Insert_osw_cyc(&input, &mask, 1);
	}
	else if (argc >= 4 && memcmp(argv[1], "tb_otdr_rollcall", strlen(argv[1])) == 0) {
		tdb_otdr_rollcall_t input;
		struct tms_getotdr_test_hdr       ref_hdr; 		
		struct tms_getotdr_test_param    test_param; 

		

		
		// 得到所有指针
		input.ptest_hdr 		= &ref_hdr;
		input.ptest_param 	= &test_param;
		
		

		ref_hdr.frame = atoi(argv[2]);
		ref_hdr.slot  = atoi(argv[3]);
		ref_hdr.type  = 1;
		ref_hdr.port  = 2;

		test_param.rang = 31111;
		test_param.wl = 1550;			
		test_param.pw =1280		;		
		test_param.time = 15;				
		test_param.gi = 1.2345;				
		test_param.end_threshold = 9.0;
		test_param.none_reflect_threshold = 8.8;
		test_param.reserve0 = 0;
		test_param.reserve1 = 0;	
		test_param.reserve1 = 0;	


		tmsdb_Insert_otdr_rollcall(&input,NULL,1);
	}
	else if (argc >= 6 && memcmp(argv[1], "tb_otdr_ref", strlen(argv[1])) == 0) {
		tdb_otdr_ref_t input;
		struct tms_otdr_ref_hdr       ref_hdr; 		
		struct tms_retotdr_test_param test_param; 

		struct tms_retotdr_data_hdr   data_hdr; 
		struct tms_retotdr_data_val   data_val[10];//;= {'o', 't', 'd', 'r', 'd', 'a', 't', 'a', '-', '-'};

		struct tms_retotdr_event_hdr  event_hdr; 
		struct tms_retotdr_event_val event_val[4]= {
			{123, 444, 3.1, 3.0, 3.9, 0.0}, 
			{1000, 1, 100000.1, 100000.2, 100000.3, 100000.4}, 
			{2, 2, 2.0, 2.2, 2.3, 2.4}, 
			{3, 3, 3.0, 3.2, 3.3, 3.4}};

		struct tms_retotdr_chain      chain; 
		struct tms_cfg_otdr_ref_val   ref_data;

		
		// 得到所有指针
		input.pref_hdr 		= &ref_hdr;
		input.ptest_param 	= &test_param;
		input.pdata_hdr 	= &data_hdr;
		input.pdata_val 	= &data_val[0];
		input.pevent_hdr 	= &event_hdr;
		input.pevent_val 	= &event_val[0];
		input.pchain 		= &chain;
		input.pref_data 	= &ref_data;

		memcpy(data_val,"OTDR DATA",10);
		printf(" %c %c %c %c\n", input.pdata_val[0].data,input.pdata_val[1].data,input.pdata_val[2].data,input.pdata_val[3].data);
		ref_hdr.osw_frame = atoi(argv[2]);
		ref_hdr.osw_slot  = atoi(argv[3]);
		ref_hdr.osw_type  = 1;
		ref_hdr.osw_port  = 2;
		strcpy((char*)ref_hdr.strid,"strid");

		test_param.rang 					= 31111;
		test_param.wl 						= 1550;			
		test_param.pw 						=1280;
		test_param.time 					= 15;				
		test_param.gi 						= 1.2345;				
		test_param.end_threshold 			= 9.0;
		test_param.none_reflect_threshold 	= 8.8;
		test_param.sample 					= 20000000;
		test_param.reserve0 				= 0;
		test_param.reserve1 				= 0;	

		memcpy(data_hdr.dpid , "OTDRData", sizeof("OTDRData"));
		data_hdr.count = 10;


		memcpy(event_hdr.eventid, "KeyEvents", sizeof("KeyEvents"));
		event_hdr.count = 4;
		

		strcpy((char*)chain.inf, "OTDRTestResultInfo");
		chain.range = 1200;
		chain.loss = 10;
		chain.att = 1.23;
		

		ref_data.leve0 = 789;
		ref_data.leve1 = 456;
		ref_data.leve2 = 123;

		
		
		tmsdb_Insert_otdr_ref(&input,NULL,1);
	}
	else if (argc >= 6 && memcmp(argv[1], "tb_otdr_his_data", strlen(argv[1])) == 0) {
		tdb_otdr_his_data_t input;
		struct tms_retotdr_test_hdr   test_hdr; 		
		struct tms_retotdr_test_param test_param; 

		struct tms_retotdr_data_hdr   data_hdr; 
		struct tms_retotdr_data_val   data_val[10];//;= {'o', 't', 'd', 'r', 'd', 'a', 't', 'a', '-', '-'};

		struct tms_retotdr_event_hdr  event_hdr; 
		struct tms_retotdr_event_val event_val[4]= {
			{0, 0, 3.1, 3.0, 3.9, 0.0}, 
			{1000, 1, 100000.1, 100000.2, 100000.3, 100000.4}, 
			{2, 2, 2.0, 2.2, 2.3, 2.4}, 
			{3, 3, 3.0, 3.2, 3.3, 3.4}};

		struct tms_retotdr_chain      chain; 
		struct tms_cfg_otdr_ref_val   ref_data;

		
		// 得到所有指针
		input.ptest_hdr 	= &test_hdr;
		input.ptest_param 	= &test_param;
		input.pdata_hdr 	= &data_hdr;
		input.pdata_val 	= &data_val[0];
		input.pevent_hdr 	= &event_hdr;
		input.pevent_val 	= &event_val[0];
		input.pchain 		= &chain;

		memcpy(data_val,"OTDR DATA",10);
		printf(" %c %c %c %c\n", input.pdata_val[0].data,input.pdata_val[1].data,input.pdata_val[2].data,input.pdata_val[3].data);
		test_hdr.osw_frame = atoi(argv[2]);
		test_hdr.osw_slot  = atoi(argv[3]);
		test_hdr.osw_type  = 1;
		test_hdr.osw_port  = 2;
		memcpy(&test_hdr.time[0], "time",5);

		test_param.rang = 31111;
		test_param.wl = 1550;			
		test_param.pw =1280		;		
		test_param.time = 15;				
		test_param.gi = 1.2345;				
		test_param.end_threshold = 9.0;
		test_param.none_reflect_threshold = 8.8;
		test_param.sample = 20000000;
		test_param.reserve0 = 0;
		test_param.reserve1 = 0;	

		memcpy(data_hdr.dpid , "abcd", 5);
		data_hdr.count = 10;

		memcpy(event_hdr.eventid, "[Name]", sizeof("[Name]"));
		event_hdr.count = 4;
		
		chain.range = 1200;
		chain.loss = 10;
		chain.att = 1.23;


		ref_data.leve0 = 123;
		ref_data.leve1 = 456;
		ref_data.leve2 = 789;

		tmsdb_Insert_otdr_his_data(&input,NULL,1);
	}
	
	else {
		printf("Usage: input err\n");
	}
}
W_BOOT_CMD(insert, cmd_insert, "insert tmsxxdb");


void sh_analyse (char *fmt, long len);
int cmd_tmsall(int argc, char **argv)
{
	int32_t frame;
	int32_t slot;
	int32_t type;


	// printf("%s \n%s \n%s \n%s\n", dev_name, cable_name, start_name, end_name);
	// return 0;
	char sbuf[1024];
	int fd = 0;
	int allcmd = 0;
	int port = 0;

	// tms_RetOTDRTest(0, 0, 0, 0, 3, 0, 0, 0, 0);
	// return 0;
	fd = sg_sockfdid;
	// tms_Tick(5);

	
	if (argc == 2 && strcmp(argv[1], "rsn") == 0) {
		uint8_t sn[128] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '\0'};
		tms_RetSerialNumber(fd, NULL, &sn);
	}
	else if (argc == 5 && strcmp(argv[1], "sip") == 0) {
		uint8_t ip[16] = "192.168.2.2";
		uint8_t mask[16] = "255.255..0.0";
		uint8_t gw[16] = "192.168.2.1";

		snprintf((char*)ip, 16, "%s", argv[2]);
		snprintf((char*)mask, 16, "%s", argv[3]);
		snprintf((char*)gw, 16, "%s", argv[4]);
		tms_SetIPAddress(fd, NULL, ip, mask, gw);
	}
	else if(argc == 2 && strcmp(argv[1], "gcom") == 0) {
		tms_GetDeviceComposition(fd, NULL);//, sg_frameid, sg_slotid);

	}

	else if(argc == 2 && strcmp(argv[1], "gdev") == 0) {
		tms_MCU_GetDeviceType(fd, NULL);//, sg_frameid, sg_slotid);
	}
	else if(argc == 2 && strcmp(argv[1], "gsn") == 0) {
		tms_GetSerialNumber(fd, NULL);
	}
	else if(argc == 2 && strcmp(argv[1], "cfgsmsa") == 0) {
		struct tms_cfg_sms_val val[6];
		for (int i = 0; i < 6; i++) {
			val[i].time = i;
			memcpy(&val[i].phone[0], "123456789012345", 16);
			val[i].type = i;
			val[i].level = i & 0x07;
		}
		tms_CfgSMSAuthorization(fd, NULL, 6, val);
	}
	else if(argc == 2 && strcmp(argv[1], "clsmsa") == 0) {
		tms_ClearSMSAuthorization(fd, NULL);
	}
	// OSW
	else if(argc == 3 && strcmp(argv[1], "cfgoswcyc") == 0) {
		struct tms_cfg_mcu_osw_cycle_val val[3];
		val[0].port = atoi(argv[2]);
		val[0].iscyc    = 1;
		val[0].interval = 15;
		tms_CfgMCUOSWCycle(fd, NULL, sg_frameid, sg_slotid, 1, val);
	}
	else if(argc == 3 && strcmp(argv[1], "closwcyc") == 0) {
		struct tms_cfg_mcu_osw_cycle_val val[3];
		val[0].port = atoi(argv[2]);
		val[0].iscyc    = 0;
		val[0].interval = 15;
		tms_CfgMCUOSWCycle(fd, NULL, sg_frameid, sg_slotid, 1, val);	
	}
	else if(argc == 3 && strcmp(argv[1], "oswsw") == 0) {
		port = atoi(argv[2]);
		if ( (unsigned int)port < 7) {
			tms_MCU_OSWSwitch(fd, NULL, sg_frameid, sg_slotid, port);	
		}
		else {
			printf("Error over range\n");
		}
	}
	else if(argc == 3 && strcmp(argv[1], "oswp") == 0) {
		uint8_t dev_name[64];
		uint8_t cable_name[64];
		uint8_t start_name[64];
		uint8_t end_name[64];
		
		port = atoi(argv[2]) & 0x07;
		snprintf((char*)dev_name, 64, "dev_name port %d", port);
		snprintf((char*)cable_name, 64, "cable_name port %d", port);
		snprintf((char*)start_name, 64, "start_name port %d", port);
		snprintf((char*)end_name, 64, "end_name port %d", port);
		
		tms_CfgMCUOSWPort(fd, NULL, sg_frameid, sg_slotid, port, &dev_name, &cable_name, &start_name, &end_name);
	}
	else if (argc == 2 && strcmp(argv[1], "adjt") == 0) {
		int8_t adjtime[20]="abcdefg";
		tms_AdjustTime(fd, NULL, &adjtime);
	}
	else if (argc == 3 && strcmp(argv[1], "trace") == 0) {
		tms_Trace(NULL, argv[2], strlen(argv[2]), 3);

	}
	else if (argc >= 3 && strcmp(argv[1], "cmd") == 0) {
		char strout[256];
		strout[0] = '\0';

		for (int i = 2; i < argc; i++) {
			strcat(strout, argv[i]);
			strcat(strout, " ");
		}

		strout[strlen(strout)-1] = '\0';
		// printf("len %d send cmd %s\n", strlen(strout), strout);
		
		tms_Command(sg_sockfdid, NULL, strout, strlen(strout)+1);

	}
	else if(argc == 3 && strcmp(argv[1], "oswclear") == 0) {
		struct tms_cfg_mcu_any_port_clear_val pval[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
		int count = 0;
		sscanf(argv[2], "%d/%d/%d/%d/%d/%d/%d/%d", 
				&pval[0].any_port, &pval[1].any_port, &pval[2].any_port, &pval[3].any_port, 
				&pval[4].any_port, &pval[5].any_port, &pval[6].any_port, &pval[7].any_port);
		
		for (int i = 0; i < 8; i++) {
			if (pval[i].any_port != -1) {
				count++;
			}
			else {
				break;
			}
		}
		// tms_CfgMCUOPMPortClear(fd, NULL, sg_frameid, sg_slotid, count, pval);
		tms_CfgMCUOSWPortClear(fd, NULL, sg_frameid, sg_slotid, count, pval);

	}
	
	else if(argc == 3 && strcmp(argv[1], "encu") == 0) {
		g_en_connect_cu = atoi(argv[2]);
	}


	else if(argc == 3 && strcmp(argv[1], "smstxt") == 0) {
		char phone[16] = "18777399591";
		//QT里面这一行编译不过的
		wchar_t wcstr[20] = L"字符测试123abc";
		// uint16_t wcstr[20];// = L"字符测试123abc";
		

		tms_SendSMS(fd, NULL, &phone, 20, wcstr);
	}


	
	else if(argc == 2 && strcmp(argv[1], "rdevc") == 0) {
		struct tms_dev_composition_val list[8];
		for (int i = 0; i < 8; i++) {
			list[i].frame = i;
			list[i].slot = i;
			list[i].type = 2;
			list[i].port = 3;
		}
		tms_RetDeviceComposition(fd, NULL, 2, list);
		// 
	}
	else if(argc == 3 && strcmp(argv[1], "update") == 0) {
		UpdateOTDR("update.txt", "", 12345);
	}
	else if(argc == 2 && (
		strcmp(argv[1], "getotdr") == 0 ||
		strcmp(argv[1], "getotdrc") == 0)  ){

		struct tms_getotdr_test_param test_param;
		test_param.rang =  30000;
		test_param.wl	= 1550;
		test_param.pw   = 640;
		test_param.time = 15;
		test_param.gi   = 1.4685;
		test_param.end_threshold = 9.0;
		test_param.none_reflect_threshold = 2.0;
		test_param.reserve0 = 0;
		test_param.reserve1 = 0;
		test_param.reserve2 = 0;
		
		if (strcmp(argv[1], "getotdr") == 0) {
			tms_GetOTDRTest(fd, NULL, 0, 8,  0, &test_param);
		}
		else if(strcmp(argv[1], "getotdrc") == 0) {
			tms_GetOTDRTestCycle(fd, NULL, 0, 8,  0, &test_param);	
		}
	}
	else if( argc == 2 && (  
			strcmp(argv[1], "retotdr") == 0 || 
			strcmp(argv[1], "refotdr") == 0 || 
			strcmp(argv[1], "retotdrc") == 0 )	) {

		struct tms_otdr_ref_hdr     ref_hdr;
		struct tms_retotdr_test_hdr test_hdr;
		struct tms_retotdr_test_param test_param;
		struct tms_retotdr_data_hdr data_hdr = {0}; 
		struct tms_retotdr_data_val data_val[10]= {'o', 't', 'd', 'r', 'd', 'a', 't', 'a', '-', '-'};

		struct tms_retotdr_event_hdr event_hdr = {0};
		struct tms_retotdr_event_val event_val[4]= {
			{0, 0, 3.1, 3.0, 3.9, 0.0}, 
			{1000, 1, 100000.1, 100000.2, 100000.3, 100000.4}, 
			{2, 2, 2.0, 2.2, 2.3, 2.4}, 
			{3, 3, 3.0, 3.2, 3.3, 3.4}};
		struct tms_retotdr_chain chain;
		struct tms_cfg_otdr_ref_val ref_data;

		ref_hdr.osw_frame = 15;
		ref_hdr.osw_slot  = 0;
		ref_hdr.osw_type = DEV_OSW;
		ref_hdr.osw_port = 1;
		ref_hdr.osw_slot  = 0;
		ref_hdr.osw_port = 0;



		test_hdr.osw_frame = 15;
		test_hdr.osw_slot  = 0;
		test_hdr.osw_type = DEV_OSW;
		test_hdr.osw_port = 1;
		memcpy(&test_hdr.time, "[TIME]", 7);
		test_hdr.otdr_frame = 1;
		test_hdr.otdr_slot = 2;
		test_hdr.otdr_type = DEV_OTDR;
		test_hdr.otdr_port = 3;




		test_param.rang = 31111;
		test_param.wl = 1550;			
		test_param.pw =1280		;		
		test_param.time = 15;				
		test_param.gi = 1.2345;				
		test_param.end_threshold = 9.0;
		test_param.none_reflect_threshold = 8.8;
		test_param.sample = 20000000;
		test_param.reserve0 = 0;
		test_param.reserve1 = 0;	

		strcpy((char*)data_hdr.dpid , "OTDRData");
		// printf("data.dpid %s\n", data_hdr.dpid);
		data_hdr.count = 10;
		strcpy((char*)event_hdr.eventid, "KeyEvents");
		// printf("event_hdr.eventid %s\n", event_hdr.eventid);
		event_hdr.count = 4;
		strcpy((char*)chain.inf, "OTDRTestResultInfo");
		chain.range = 1200;
		chain.loss = 10;
		chain.att = 1.23;


		ref_data.leve0 = 123;
		ref_data.leve1 = 456;
		ref_data.leve2 = 789;

		if (strcmp(argv[1], "retotdr") == 0) {
			// tms_RetOTDRTest(fd, NULL, 
			// 	&test_hdr, &test_param, 
			// 	&data_hdr, data_val, 
			// 	&event_hdr, event_val, 
			// 	&chain);
			tms_RetOTDRCycle(fd, NULL, 
				&test_hdr, &test_param, 
				&data_hdr, data_val, 
				&event_hdr, event_val, 
				&chain);
		}
		else if (strcmp(argv[1], "retotdrc") == 0) {
			tms_RetOTDRTestCycle(fd, NULL, 
				&test_hdr, &test_param, 
				&data_hdr, data_val, 
				&event_hdr, event_val, 
				&chain);
		}
		else {
			tms_CfgOTDRRef(fd, NULL, 
				&ref_hdr, &test_param, 
				&data_hdr, data_val, 
				&event_hdr, event_val, 
				&chain, 
				&ref_data);	
		}
		
	}
	else if(argc == 100000000 && strcmp(argv[1], "todo.............") == 0) {
		// 
	}
	else if(argc == 100000000 && strcmp(argv[1], "todo.............") == 0) {
		// 
	}
	else if(argc == 100000000 && strcmp(argv[1], "todo.............") == 0) {
		// 
	}
	else if(argc == 100000000 && strcmp(argv[1], "todo.............") == 0) {
		// 
	}
	else if(argc == 2 && strcmp(argv[1], "tick") == 0) {
		tms_Tick(fd, NULL);
	}
	else if(allcmd && strcmp(argv[1], "rdev") == 0) {
		tms_MCU_RetDeviceType(fd, NULL, sg_frameid, sg_slotid, 0xc3, 0xd4);
	}
	else if(allcmd && strcmp(argv[1], "opma") == 0) {
		tms_MCU_GetOLPAlarm(fd, NULL, sg_frameid, sg_slotid);
	}


	else if(argc == 2 && strcmp(argv[1], "olpa") == 0) {
		tms_MCU_GetOLPAlarm(fd, NULL, sg_frameid, sg_slotid);
	}
	else if(argc == 3 && strcmp(argv[1], "olpstart") == 0) {
		if ('m' == (argv[2][0] | 0x20)) {
			tms_MCU_OLPStartOTDRTest(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_ACTION_MASTER);
		}
		else if ('s' == (argv[2][0] | 0x20)) {
			tms_MCU_OLPStartOTDRTest(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_ACTION_SLAVE);
		}
		else {
			printf("Error over range\n");
			printf("\tM\tmaster line start\n");
			printf("\tS\tslave line start\n");
		}
	}
	else if(argc == 3 && strcmp(argv[1], "olpfinish") == 0) {
		if ('m' == (argv[2][0] | 0x20)) {
			tms_MCU_OLPFinishOTDRTest(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_ACTION_MASTER);
		}
		else if ('s' == (argv[2][0] | 0x20)) {
			tms_MCU_OLPFinishOTDRTest(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_ACTION_SLAVE);
		}
		else {
			printf("Error over range\n");
			printf("\tM\tmaster line finish\n");
			printf("\tS\tslave line finish\n");
		}
	}
	else if (argc == 3 && strcmp(argv[1], "cfgolp") == 0){
		int32_t port;
		uint8_t dev_name[64] =  "dev    name";
		uint8_t cable_name[64]= "cable  name";
		uint8_t start_name[64]= "start  name";
		uint8_t end_name[64]=   "end    name" ;

		port = atoi(argv[2]);
		strcat((char*)dev_name, argv[2]);
		strcat((char*)cable_name, argv[2]);
		strcat((char*)start_name, argv[2]);
		strcat((char*)end_name, argv[2]);
		
		tms_CfgMCUOLPPort(fd, NULL, sg_frameid, sg_slotid, port , &dev_name, &cable_name, &start_name, &end_name);
	}
	else if (argc == 3 && strcmp(argv[1], "clolp") == 0){
		struct tms_cfg_mcu_any_port_clear_val list[2];
		list[0].any_port = 0;
		list[1].any_port = atoi(argv[2]);
		printf("%d %d\n", list[0].any_port, list[1].any_port	);

		tms_CfgMCUOLPPortClear(fd, NULL, sg_frameid, sg_slotid, 2, list);
	}
	else if (argc == 3 && strcmp(argv[1], "cfgolpu") == 0){
		struct tms_cfg_mcu_u_any_osw_val list[2];
		list[0].any_port  = 3;
		list[0].osw_frame = 11;
		list[0].osw_slot  = 2;
		// list[0].osw_type = 0;
		list[0].osw_port  = 3;

		list[1].any_port  = atoi(argv[2]);
		list[1].osw_frame = 11;
		list[1].osw_slot  = 3;
		// list[1].osw_type = 0;
		list[1].osw_port  = 4;

		tms_CfgMCUUniteOLPOSW(fd, NULL, sg_frameid, sg_slotid, 2, list);
	}
	else if (argc == 3 && strcmp(argv[1], "clolpu") == 0){
		struct tms_cfg_mcu_any_port_clear_val list[2];
		list[0].any_port = 3;
		list[1].any_port = atoi(argv[2]);
		printf("%d %d\n", list[0].any_port, list[1].any_port	);

		tms_CfgMCUUniteOLPOSWClear(fd, NULL, sg_frameid, sg_slotid, 2, list);
	}
	else if (argc == 3 && strcmp(argv[1], "cfgopmu") == 0){
		struct tms_cfg_mcu_u_any_osw_val list[2];
		list[0].any_port  = 0;
		list[0].osw_frame = 12;
		list[0].osw_slot  = 2;
		// list[0].osw_type = 0;
		list[0].osw_port  = 3;

		list[1].any_port  = atoi(argv[2]);
		list[1].osw_frame = 12;
		list[1].osw_slot  = 3;
		// list[1].osw_type = 0;
		list[1].osw_port  = 4;

		tms_CfgMCUUniteOPMOSW(fd, NULL, sg_frameid, sg_slotid, 2, list);
	}
	else if (argc == 3 && strcmp(argv[1], "clopmu") == 0){
		struct tms_cfg_mcu_any_port_clear_val list[2];
		list[0].any_port = 0;
		list[1].any_port = atoi(argv[2]);
		printf("%d %d\n", list[0].any_port, list[1].any_port	);

		tms_CfgMCUUniteOPMOSWClear(fd, NULL, sg_frameid, sg_slotid, 1, list);
	}
	else if(argc == 3 && strcmp(argv[1], "olpref") == 0 &&
		memcmp(argv[2], "go", strlen(argv[2])) == 0) {

		tms_CfgOLPRefLevel(fd, NULL, sg_frameid, sg_slotid, 3, sg_olpref);
	}
	else if(argc == 3 && strcmp(argv[1], "olpref") == 0 && 
		memcmp(argv[2], "list", strlen(argv[2])) == 0) {
		
		tms_Print_tms_cfg_olp_ref_val(sg_olpref, 3);
	}
	else if(argc >= 3 && strcmp(argv[1], "olpref") == 0) {
		
		int index;
		index = atoi(argv[2]);
		if (index >= 3) {
			printf("out of rang <0~2>\n");
			return -1;
		}
		sg_olpref[index].port = atoi(argv[2]);
		if (sg_olpref[index].port > 2) {
			printf("port <0~2>\n");
			return -1;
		}
		// 功率设置/缺省值
		if (argc >= 4) {
			sg_olpref[index].ref_power = atoi(argv[3]);
		}
		else {
			sg_olpref[index].ref_power = sg_olpref_def.ref_power;
		}

		// 告警门限设置/缺省值
		if (argc == 7) {
			sg_olpref[index].leve0 = atoi(argv[4]);
			sg_olpref[index].leve1 = atoi(argv[5]);
			sg_olpref[index].leve2 = atoi(argv[6]);	
		}
		else {
			sg_olpref[index].leve0 = sg_olpref_def.leve0;
			sg_olpref[index].leve1 = sg_olpref_def.leve1;
			sg_olpref[index].leve2 = sg_olpref_def.leve2;
		}
		sg_olpref[index].isminitor = 1;
		sg_olpref[index].wave      = 1550;

		sg_olpref_def = sg_olpref[index];
		
		// tms_CfgOLPRefLevel(fd, NULL, sg_frameid, sg_slotid, 1, &olpref);
	}
	else if(argc == 2 && strcmp(argv[1], "olpop") == 0) {
		tms_GetOLPOP(fd, NULL, sg_frameid, sg_slotid);
	}

	else if(argc == 2 && strcmp(argv[1], "opmop") == 0) {
		tms_GetOPMOP(fd, NULL, sg_frameid, sg_slotid);
	}
	
	else if(argc == 2 && strcmp(argv[1], "cfgolpm") == 0) {
		tms_CfgOLPMode(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_MODE_UNBACK, 1, 0);
	}

	else if(argc == 3 && strcmp(argv[1], "olpsw") == 0) {
		port = atoi(argv[2]);
		if ( (unsigned int)port < 7) {
			tms_MCU_OLPSwitch(fd, NULL, sg_frameid, sg_slotid, port);	
		}
		else {
			printf("Error over range\n");
		}
	}
	else if(argc == 2 && strcmp(argv[1], "olprep") == 0) {
		tms_ReportOLPAction(fd, NULL, sg_frameid, sg_slotid, OLP_SWITCH_ACTION_PERSION, OLP_SWITCH_ACTION_MASTER);
	}
	




	else if(argc == 2 && strcmp(argv[1], "gver") == 0) {
		struct tms_devbase devbase;
		if (tms_GetDevBaseByLocation(sg_frameid, sg_slotid, &devbase) == 0) {
			printf("f%d/s%d unexit\n", sg_frameid, sg_slotid);
		}
		tms_GetVersion(fd, NULL, sg_frameid, sg_slotid, devbase.type);
	}
	else if(allcmd && strcmp(argv[1], "rver") == 0) {
		tms_RetVersion(fd, NULL, sg_frameid, sg_slotid, DEV_OPM, (uint8_t*)("1.1.1.1.1"));
	}
	else if(0 && (allcmd && strcmp(argv[1], "update") == 0)) {
		char data[40] = {0};
		char fname[256] = "abcdefg";
		tms_Update(fd, NULL, 0x01, 0x02, DEV_OPM, (uint8_t(*)[256])&fname, 40, (uint8_t*)data);
	}
	else if(allcmd && strcmp(argv[1], "ack") == 0) {
		tms_Ack(fd, NULL, RET_SUCCESS, 0x1);
	}
	// else if(argc == 2 && strcmp(argv[1], "opmref") == 0) {
	// 	struct tms_cfg_opm_ref_val opmlist[8];
	// 	for (int i = 0; i < 8; i++) {
	// 		opmlist[i].port = i;
	// 		opmlist[i].isminitor = 0x01;
	// 		opmlist[i].wave = 1550;
	// 		opmlist[i].ref_power = 400;
	// 		opmlist[i].leve0 = 0xa1;
	// 		opmlist[i].leve1 = 0xb2;
	// 		opmlist[i].leve2 = 0xc3;
	// 	}
	// 	tms_CfgOPMRefLevel(fd, NULL, sg_frameid, sg_slotid, 8, opmlist);
	// }
	else if(argc == 3 && strcmp(argv[1], "opmref") == 0 &&
		memcmp(argv[2], "go", strlen(argv[2])) == 0) {

		tms_CfgOPMRefLevel(fd, NULL, sg_frameid, sg_slotid, 8, sg_opmref);
	}
	else if(argc == 3 && strcmp(argv[1], "opmref") == 0 && 
		memcmp(argv[2], "list", strlen(argv[2])) == 0) {
		tms_Print_tms_cfg_opm_ref_val(&sg_opmref[0], 8);
		// printf("    port  ism  wave  r_power   lv0    lv1  lv2\n");
		// for (int i = 0; i < 8; i++) {
		// 	printf("%6.1d%5d  %6d%8d %6d%6d%6d\n",
		// 		sg_opmref[i].port,
		// 		sg_opmref[i].isminitor,
		// 		sg_opmref[i].wave,
		// 		sg_opmref[i].ref_power,
		// 		sg_opmref[i].leve0,
		// 		sg_opmref[i].leve1,
		// 		sg_opmref[i].leve2);
		// }
	}
	else if(argc >= 3 && strcmp(argv[1], "opmref") == 0) {
		
		int index;
		index = atoi(argv[2]);
		if (index >= 8) {
			printf("out of rang <0~7>\n");
			return -1;
		}
		sg_opmref[index].port = atoi(argv[2]);
		
		// 功率设置/缺省值
		if (argc >= 4) {
			sg_opmref[index].ref_power = atoi(argv[3]);
		}
		else {
			sg_opmref[index].ref_power = sg_opmref_def.ref_power;
		}

		// 告警门限设置/缺省值
		if (argc == 7) {
			sg_opmref[index].leve0 = atoi(argv[4]);
			sg_opmref[index].leve1 = atoi(argv[5]);
			sg_opmref[index].leve2 = atoi(argv[6]);	
		}
		else {
			sg_opmref[index].leve0 = sg_opmref_def.leve0;
			sg_opmref[index].leve1 = sg_opmref_def.leve1;
			sg_opmref[index].leve2 = sg_opmref_def.leve2;
		}
		sg_opmref[index].isminitor = 1;
		sg_opmref[index].wave      = 1550;

		sg_opmref_def = sg_opmref[index];
		
		// tms_CfgOLPRefLevel(fd, NULL, sg_frameid, sg_slotid, 1, &olpref);
	}



	

	
	else if(allcmd && strcmp(argv[1], "aopm") == 0) {
		struct tms_alarm_opm_val alarmopmlist[8];

		for (int i = 0; i < 8; i++) {
			alarmopmlist[i].port = i;
			alarmopmlist[i].levelx = 1;
			alarmopmlist[i].power = 200;
			memcpy(alarmopmlist[i].time, (uint8_t*)"12345", sizeof("12345"));
		}
		// tms_AlarmOPM(fd, sg_frameid, sg_slotid, 1, 8, alarmopmlist);
		tms_AlarmOPM(fd, NULL, sg_frameid, sg_slotid, 8, alarmopmlist);
	}
	else if(allcmd && strcmp(argv[1], "aopmc") == 0) {
		struct tms_alarm_opm_val alarmopmlist[8];
		for (int i = 0; i < 8; i++) {
			alarmopmlist[i].port = i;
			alarmopmlist[i].levelx = 1;
			alarmopmlist[i].power = 200;
			memcpy(alarmopmlist[i].time, (uint8_t*)"bbbb", sizeof("bbbb"));
		}
		// tms_AlarmOPMChange(fd, sg_frameid, sg_slotid, 1, 6, 8, alarmopmlist);
		tms_AlarmOPMChange(fd, NULL, sg_frameid, sg_slotid, 6, 8, alarmopmlist);
	}
	else if(allcmd && strcmp(argv[1], "gopm") == 0) {
		tms_GetOPMOP(fd, NULL, sg_frameid, sg_slotid);
	}
	else if(argc == 2 && strcmp(argv[1], "ropm") == 0) {
		struct tms_any_op_val opmvallist[2];
		opmvallist[0].port = 1;
		opmvallist[0].power = 200;
		opmvallist[1].port = 2;
		opmvallist[1].power = 400;
		tms_RetOPMOP(fd, NULL, sg_frameid, sg_slotid, 2, opmvallist);
	}
	else if(argc == 2 && strcmp(argv[1], "rolp") == 0) {
		struct tms_any_op_val opmvallist[2];
		opmvallist[0].port = 1;
		opmvallist[0].power = 200;
		opmvallist[1].port = 2;
		opmvallist[1].power = 400;
		tms_RetOLPOP(fd, NULL, sg_frameid, sg_slotid, 2, opmvallist);
	}
	else {
_Usage:;
		printf("Usage:\n");
		printf("\tcmd\n");
		printf("\t\toswsw <port>\n");
		printf("\t\tgdev\n");
		printf("\t\trdev\n");
		printf("\t\topmp\n");
		printf("\t\tolpp\n");
		printf("\t\tgver\n");
		printf("\t\trver\n");
		printf("\t\tack\n");
		printf("\t\topmref\n");
		printf("\t\taopm\n");
		printf("\t\tgopm\n");
		printf("\t\tropm\n");
	}
	return 0;
	

	// tms_MCU_GetDeviceType(fd, 0xa1, 0xb2);
	// tms_MCU_RetDeviceType(fd, 0xa1, 0xb2, 0xc3, 0xd4);

	// tms_MCU_GetOPMRayPower(fd, 0xa1, 0xb2);
	// tms_MCU_GetOLPRayPower(fd, 0xa1, 0xb2);


	
	return 0;
}
W_BOOT_CMD(tms, cmd_tmsall, "cmd epoll server send");

int cmd_remotecmd(int argc, char **argv)
{
	for (int i = 0;i < argc;i++) {
		printf("%s ",argv[i]);
	}
	if (argc >= 2) {//} && strcmp(argv[1], "") == 0) {
		char strout[256];
		strout[0] = '\0';

		for (int i = 1; i < argc; i++) {
			strcat(strout, argv[i]);
			strcat(strout, " ");
		}

		strout[strlen(strout)-1] = '\0';
		printf("len %d send cmd %s\n", strlen(strout), strout);
		
		tms_Command(sg_sockfdid, NULL, strout, strlen(strout)+1);
	}
}
W_BOOT_CMD(r, cmd_remotecmd, "cmd epoll server send");
int cmd_im(int argc, char **argv)
{
	if (argc == 2 && memcmp(argv[1], "trace", strlen(argv[1])) == 0) {
		char strout[] = "im add trace";
		tms_Command(sg_sockfdid, NULL, strout, sizeof(strout));
	}
	else if (argc == 3 && memcmp(argv[1], "add", strlen(argv[1])) == 0) {	
		int fd = tms_GetTempFd();
		tms_AddManage( fd, MT_TRACE);
	}


	else if (argc == 2 && memcmp(argv[1], "level", strlen(argv[1])) == 0) {
		char strout[] = "im del trace";
		tms_Command(sg_sockfdid, NULL, strout, sizeof(strout));
	}
	else if (argc == 3 && memcmp(argv[1], "del", strlen(argv[1])) == 0) {	
		int fd = tms_GetTempFd();
		tms_DelManage( fd);
	}

	
}
W_BOOT_CMD(im, cmd_insert, "which dev");

int list(struct ep_con_t *pconNode, void *ptr)
{
	int fd = pconNode->sockfd;
	if (fd != 4) {
		printf("scan fd %d \n", fd);
		tms_MCU_GetDeviceType(fd, NULL);//, 0, 6);
	}
	return 0;
}
int cmd_tmsscan(int argc, char **argv)
{
	int fd = 0;
	if (argc == 2) {
		if (memcmp(argv[1], "all", strlen(argv[1])) == 0) {
			printf("scan all\n");
			ep_Ergodic(&ep, list, NULL);
		}
		else if (fd = atoi(argv[1]), fd != 0 && fd != 4) {
			printf("scan fd :%d\n", fd);
			sg_sockfdid = fd;
			tms_MCU_GetDeviceType(fd, NULL);//, 0, 6);
		}
	} 
	else if(argc == 3) {
		if (memcmp(argv[1], "fd", strlen(argv[1])) == 0) {
			struct tms_devbase dev;

			fd = atoi(argv[2]);			
			if (tms_GetDevBaseByFd(fd, &dev) == 0) {
				printf("frame %2.2d slot %2.2d type %d port %d\n", dev.frame, dev.slot, dev.type, dev.port);
			}
			else {
				printf("un find\n");
			}

		}
	}
	else if(argc == 4) {
		if (memcmp(argv[1], "fs", strlen(argv[1])) == 0) {
			struct tms_devbase dev;
			int frame, slot;

			frame = atoi(argv[2]);
			slot  = atoi(argv[3]);
			tms_GetDevBaseByLocation(frame, slot, &dev);
			printf("frame %2.2d slot %2.2d type %d port %d\n", dev.frame, dev.slot, dev.type, dev.port);
			
		}
	}
	else {
_Usage:;
		printf("Usage:\n");
		printf("  scan <fdid>\n");
		printf("  cmd\n");
		printf("  fd<1~N> \tSpecify scan socket fdid\n");
		printf("  all     \tSpecify scan all connected socket fdid\n");
	}
	return 0;
}
W_BOOT_CMD(scan, cmd_tmsscan, "cmd epoll server send");

void sh_analyse (char *fmt, long len);
int cmd_intface(int argc, char **argv)
{
	int frame = 0, slot = 0, sockfd = 0;
	struct tms_devbase oneframe[12];

	if (argc == 2) {
		char c = 0;
		char unuse = 0;
		char unuse1 = 0;// 必须初始化，某些编译器可能不对其复值，
						// "int 1/2"作为输入sscanf返回的unuse1是随机数
		

		sscanf(argv[1], "%d/%d%c", &frame, &slot, &unuse1);
		sscanf(argv[1], "%d%c%c", &frame, &c, &unuse);
		if (c == '/' && 
			(unuse >= '0' && unuse <= '9') &&
			unuse1 == 0) {

			if ((unsigned int)frame > 15 || (unsigned int)slot > 11) {
				printf("Error over range\n");
				return 0;
			}
			tms_GetFrame(frame, &oneframe);
			
			//todo frame slot合法性检测
			if (oneframe[slot].fd != 0) 
			{
				sg_sockfdid = oneframe[slot].fd;
				sg_frameid = frame;
				sg_slotid = slot;
				char path[36];
				struct sockaddr_in remoteAddr;
				socklen_t 		 len;
				len = sizeof(struct sockaddr_in);
				getpeername(sg_sockfdid, (struct sockaddr*)&remoteAddr , &len);

				// snprintf(path, 36, "f%d/s%d", sg_frameid, sg_slotid);
				snprintf(path, 36, "%s:f%d/s%d", inet_ntoa(remoteAddr.sin_addr), sg_frameid, sg_slotid);
				sh_editpath(path);
			}
			else {
				sg_frameid = frame;
				sg_slotid = slot;
				printf("Unknow interface %d/%d waiting\n", frame, slot);
			}
			
		}
	}
	else if (argc == 3 && memcmp(argv[1], "sockfd", strlen(argv[1])) == 0) {
			sockfd = atoi(argv[2]);
			sg_sockfdid = sockfd;
			printf("Change sockfd %d\n", sg_sockfdid);

			
			struct tms_devbase dev;
			char path[36];//192.168.100.xx:fxx/sxx>
			struct sockaddr_in remoteAddr;
			socklen_t 		 len;
			len = sizeof(struct sockaddr_in);
			getpeername(sockfd, (struct sockaddr*)&remoteAddr , &len);
			if (tms_GetDevBaseByFd(sg_sockfdid, &dev) == 0) {
				sg_frameid = dev.frame;
				sg_slotid  = dev.slot;
				snprintf(path, 36, "%s:f%d/s%d", inet_ntoa(remoteAddr.sin_addr), sg_frameid, sg_slotid);
				printf("path:%s\n", path);
			}
			else {
				snprintf(path, 36, "%s:unknow", inet_ntoa(remoteAddr.sin_addr));
			}
			
			sh_editpath(path);
		}
	else {
_Usage:;
		printf("Usage:\n");
		printf("  interface frame <1~N> slot <1~N>\n");
	}
	return 0;
}
W_BOOT_CMD(interface, cmd_intface, "cmd epoll server send");



int cmd_sudo(int argc, char **argv)
{
	if(0 == strcmp(argv[0], "sudo") ) {
		sg_sudo = 1;
		// todo:命令嵌套
		sg_sudo = 0;
	}
	else if(0 == strcmp(argv[0], "sudo") ) {
		sg_sudo = 0;
		
	}
	else {
_Usage:;
		printf("Usage:\n");
		printf("  sudo <cmd>   supper\n");	
		printf("  unsudo <cmd> unsupper\n");	
	}
	return 0;
}
W_BOOT_CMD(sudo, cmd_sudo, "sudo");
W_BOOT_CMD(unsudo, cmd_sudo, "unsudo");



struct frame
{

};

// 用字符串打印机框信息
int DispFrame(struct tms_devbase *pframe, uint32_t flag, struct trace_cache *ptc)
{
	static char devName[][8] = {
		{"      "}, 
		{"OPW   "}, //1
		{"MCU   "}, //2
		{"OPM   "}, //3
		{"OSW   "}, 
		{"OTDR  "}, 
		{"OLS   "}, 
		{"OLP   "}, 
		{"SMS   "}, 
	};
	int ret;

	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
			"-------------------------------------------------\n");
	ptc->offset += ret;

	for (int k = 0; k < 12; k++) {
		ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, 
				"|%2.2d ", k);
		ptc->offset += ret;		
	}

	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, 
			"|\n-------------------------------------------------\n");
	ptc->offset += ret;
	for (int i = 0; i < 5; i++) {
		for (int k = 0; k < 12; k++) {
			// 防止溢出
			if ((uint32_t)pframe[k].type < sizeof(devName) / sizeof(char[8])) {
				ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, 
						"|%2.2c ", devName[pframe[k].type][i]);	
				ptc->offset += ret;
			}
		}
		ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, "|\n");
		ptc->offset += ret;
	}
	for (int k = 0; k < 12; k++) {
		ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,
				"|%2.2d ", pframe[k].port);
		ptc->offset += ret;
	}

	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, 
		"|\n-------------------------------------------------\n");
	ptc->offset += ret;

	if (flag & 0x01) {
		ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, "Socket Fd:\n");
		ptc->offset += ret;
		for (int i = 0; i < 12; i++) {
			if (pframe[i].fd == 0) {
				ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, "%2.2d:no    ", i);
				ptc->offset += ret;
			}
			else {
				ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, "%2.2d:%-5.0d ", i, pframe[i].fd);
				ptc->offset += ret;
			}
			if (i == 5) {
				ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, "\n");
				ptc->offset += ret;
			}
		}
		ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset, "\n");	
		ptc->offset += ret;
	}
}

int Dispinf(struct tms_devbase *pframe, uint32_t flag, struct trace_cache *ptc)
{
	// struct ep_con_t *pnode = pconNode;
	// struct _dispinf *p = (struct _dispinf*)ptr;
	
	// int ret;
	struct sockaddr_in locateAddr,remoteAddr;
	socklen_t 		 len;
	int ret;
	int fd;

	if (ptc->offset > ptc->limit) {
		printf("%s", ptc->strout);
		tms_Trace(NULL, ptc->strout, ptc->offset, 1);
		ptc->offset = 0;
	}
	for (int i = 0; i < 12; i++) {
		if (pframe[i].fd != 0) {
			fd = pframe[i].fd;

			len = sizeof(struct sockaddr_in);
			getsockname(fd, (struct sockaddr*)&locateAddr, &len);
			len = sizeof(struct sockaddr_in);
			getpeername(fd, (struct sockaddr*)&remoteAddr, &len);


			ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,"%-4d%8d%16s:%-8d",
				i,
				fd,
				inet_ntoa(locateAddr.sin_addr),
				htons(locateAddr.sin_port));
			ptc->offset += ret;

			ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,"%16s:%-8d\n",
					inet_ntoa(remoteAddr.sin_addr),
					htons(remoteAddr.sin_port));
			ptc->offset += ret;
		}
	}
	ret = snprintf(ptc->strout + ptc->offset, ptc->empty - ptc->offset,"\n");
	ptc->offset += ret;
	
}


#define FLAG_DISP_FRAME 1
#define FLAG_DISP_CON 2
#define FLAG_DISP_ALL 0xffff

int cmd_Disp(int argc, char **argv)
{
	int havedev;
	int frametotal = 0, slottotal = 0;
	int slot = 0;
	char strout[1024];
	struct tms_devbase oneframe[12];
	struct _dispinf cbval;
	struct trace_cache tc;
	int ret;

	int flag = 0;


	// 打印设备连接状态，有两种显示方式、frame、connect
	if (argc == 2) {
		if(memcmp(argv[1], "frame", strlen(argv[1])) == 0) {
			flag = FLAG_DISP_FRAME;
		}
		else if(memcmp(argv[1], "connect", strlen(argv[1])) == 0) {
			flag = FLAG_DISP_CON;
		}
		else if(memcmp(argv[1], "all", strlen(argv[1])) == 0) {
			flag = FLAG_DISP_ALL;
		}
	} 


	if (argc == 2 && (flag & (FLAG_DISP_FRAME + FLAG_DISP_CON) ) ) {

		// 遍历16个机框各槽位
		for (int i = 0; i < 16; i++) {
			havedev = 0;
			slot = 0;
			tms_GetFrame(i, &oneframe);
			for (int k = 0; k < 12; k++) {
				if (oneframe[k].fd != 0) {
					havedev = 1;
					slottotal++;
					slot++;
				}
			}

			// 只有该机框有设备才打印
			if (havedev == 1) {
				frametotal++;
				tc.strout = strout;
				tc.limit = 500;
				tc.empty = 1024;
				tc.offset = 0;
				ret = snprintf(tc.strout + tc.offset, tc.empty - tc.offset, 
					"\nFrame:%2.2d Slot count:%2.2d\n", i, slot);
				tc.offset += ret;


				if (flag & FLAG_DISP_FRAME) {
					DispFrame(oneframe , -1, &tc);
					printf("%s",tc.strout);
					tms_Trace(NULL, tc.strout, tc.offset + 1, 1);	
				}
				
				if (flag & FLAG_DISP_CON) {
					tc.strout = strout;
					tc.limit = 800;
					tc.empty = 1024;
					tc.offset = 0;
					ret = snprintf(tc.strout + tc.offset, tc.empty - tc.offset, "%s%-4s%8s%16s%24s\n",
						"Connect:\n",
						"Slot ","FD","locate","Remote");
					tc.offset += ret;

					Dispinf(oneframe , -1, &tc);
					printf("%s",tc.strout);
					tms_Trace(NULL, tc.strout, tc.offset + 1, 1);
				}
				
			}
		}

		// 打印总共有多少机框和槽位
		tc.strout = strout;
		tc.limit = 500;
		tc.empty = 1024;
		tc.offset = 0;
		ret = snprintf(tc.strout + tc.offset, tc.empty - tc.offset, 
				"                    Total Frame:%2.2d Total Slot:%2.2d\n", frametotal, slottotal);
		tc.offset += ret;
		printf("%s", tc.strout);
		tms_Trace(NULL, tc.strout, tc.offset + 1, 1);
	}


	// 显示当前向那个设备执行shell结果，只有trace端才显示
	else if(argc == 2 && memcmp(argv[1], "cur", strlen(argv[1])) == 0) {
		tc.strout = strout;
		tc.limit = 500;
		tc.empty = 1024;
		tc.offset = 0;
		ret = snprintf(tc.strout + tc.offset, tc.empty - tc.offset, 
				"fd %d frame %d slot %d\n", sg_sockfdid, sg_frameid, sg_slotid);
		tc.offset += ret;
		printf("%s", strout);
		tms_Trace(NULL, tc.strout, tc.offset + 1, 1);	
	}

	else {
_Usage:;
		printf("Usage:\n");
		printf("  disp cur\n");
		printf("  disp frame\n");
		printf("  disp connect\n");
		printf("  disp all\n");
	}
	
	return 0;

}
W_BOOT_CMD(disp, cmd_Disp, "Display more information");



#ifdef __cplusplus
}
#endif