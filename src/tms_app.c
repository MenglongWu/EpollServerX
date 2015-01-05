#include "protocol/tmsxx.h"
#include <string.h>
#include "ep_app.h"
#include "stdio.h"
#include "epollserver.h"

#ifdef __cplusplus
extern "C" {
#endif

int g_en_connect_cu = 1;
extern struct tms_callback tcb;

int unuse_copy2use(char *buf, int datalen, int msec)
{

	printf("\nunuse_copy2use()\t %d\n",datalen);
	uint32_t *pid;
	uint32_t *plen;

	if (datalen > 20) {
		datalen = 20;
	}
	for (int i = 0;i < datalen; i++) {
		printf("%2.2x ",buf[i]);
	}
	printf("\n");
	// pid = (uint32_t*)buf;
	// plen = (uint32_t*)(buf + sizeof(int32_t));
	// printf("\tcmdid[%8.8x] len[%d] datalen %d\n",*pid,*plen,datalen);
	return 0;	
}

int32_t tms_OnGetDeviceComposition(struct tms_context *pcontext, int8_t *pdata, int32_t len)
{
	printf("OnGetDeviceComposition\n");
	struct tms_dev_composition_val devcom[16*12];

	int havedev;
	int frametotal = 0,slottotal = 0;
	int slot = 0;
	struct tms_devbase oneframe[12];

	int count = 0;

	for (int i = 0; i < 16; i++) {
		tms_GetFrame(i, &oneframe);
		for (int k = 0;k < 12;k++) {
			devcom[i*12+k].frame = oneframe[k].frame;
			devcom[i*12+k].slot  = oneframe[k].slot;
			devcom[i*12+k].type  = oneframe[k].type;
			devcom[i*12+k].port  = oneframe[k].port;
			count++;
		}
	}
	printf("count = %d\n",count);
	tms_RetDeviceComposition(pcontext->fd, NULL, count,devcom);
	return 0;
}




struct tms_dev_composition_val sg_appdevcom[17][12] = {0};

extern int DispFrame(struct tms_devbase *pframe,uint32_t flag);


int32_t tmsapp_GetFrame(int32_t frame, struct tms_devbase (*pdev)[12])
{
	if ((uint32_t)frame >= 16) {
		return 0;
	}
	// memcpy(&pdev[0][0],sg_devnet[frame],MAX_SLOT * sizeof(struct tms_devbase));

	for (int i = 0;i < 12;i++) {
		pdev[0][i].frame = sg_appdevcom[frame][i].frame;
		pdev[0][i].slot = sg_appdevcom[frame][i].slot;
		pdev[0][i].type = sg_appdevcom[frame][i].type;
		pdev[0][i].port = sg_appdevcom[frame][i].port;

	}


	return 0;
}

int32_t tms_OnSetIPAddress(uint8_t (*ip)[16], uint8_t (*mask)[16], uint8_t (*gw)[16])
{
	printf("ip  :%s\nmask:%s\ngate:%s\n",ip[0],mask[0],gw[0]);
	return 0;
}
int32_t tms_OnRetSerialNumber(uint8_t (*sn)[128])
{
	printf("sn:12312313   %s\n" ,sn[0]);
	return 0;
}
int32_t tms_OnCUNoteNet(void)
{
	return 0;
}

int32_t tms_OnCUNoteManageConnect(int32_t state)
{
	return 0;
}
void sh_analyse (char *fmt,long len);
int32_t tms_OnCommand(struct tms_context *pcontext, char *strcmd, int32_t slen)
{
	// printf("aasdf\n");
	sh_analyse(strcmd, slen);
	return 0;
}


int32_t tms_OnRetDeviceComposition(struct tms_context *pcontext, int8_t *pdata, int32_t len,
			struct tms_dev_composition *pcfg_hr,
			struct tms_dev_composition_val *plist)
{
	printf("OnRetDeviceComposition\n");


	int havedev;
	int frametotal = 0,slottotal = 0;
	int slot = 0;
	struct tms_devbase oneframe[12];

	struct tms_dev_composition_val *ptlist;
	int frame;


	ptlist = plist;
	for (int i = 0;i < pcfg_hr->count; i++) {
		// 防止ptlist->frame溢出
		if ((uint32_t)ptlist->frame < 16 && (uint32_t)ptlist->slot < 12) {
			sg_appdevcom[ptlist->frame][ptlist->slot].frame = ptlist->frame ;
			sg_appdevcom[ptlist->frame][ptlist->slot].slot  = ptlist->slot  ;
			sg_appdevcom[ptlist->frame][ptlist->slot].type  = ptlist->type  ;
			sg_appdevcom[ptlist->frame][ptlist->slot].port  = ptlist->port  ;
		}
		ptlist++;
	}

	for (int i = 0; i < 16; i++) {

		havedev = 0;
		slot = 0;
		tmsapp_GetFrame(i, &oneframe);
		for (int k = 0;k < 12;k++) {
			if (oneframe[k].type != 0) {
				havedev = 1;
				slottotal++;
				slot++;
			}
		}
		if (havedev == 1) 
		{
			frametotal++;
			printf("\nFrame:%2.2d Slot count:%2.2d\n",i,slot);
			DispFrame(oneframe, 0);
		}
	}
	printf("                    Total Frame:%2.2d Total Slot:%2.2d\n",frametotal,slottotal);

	return 0;	
}

int32_t tms_OnRetOTDRTest(struct tms_context *pcontext,
			struct tms_retotdr_test_hdr   *ptest_hdr,
			struct tms_retotdr_test_param *ptest_param,
			struct tms_retotdr_data_hdr   *pdata_hdr,
			struct tms_retotdr_data_val   *pdata_val,
			struct tms_retotdr_event_hdr  *pevent_hdr,
			struct tms_retotdr_event_val  *pevent_val,
			struct tms_retotdr_chain      *pchain)
{
	tms_SaveOTDRData(	ptest_hdr,
						ptest_param,
						pdata_hdr,
						pdata_val,
						pevent_hdr,
						pevent_val,
						pchain,
						(char*)"abc.txt",0);
	
	tms_Print_tms_retotdr_event(pevent_hdr, pevent_val);
	tms_Print_tms_retotdr_chain(pchain);
	tms_Print_tms_retotdr_test_hdr(ptest_hdr);
	tms_Print_tms_retotdr_test_param(ptest_param);

	// 打印信息

	return 0;
}

int32_t tms_OnCfgOTDRRef(struct tms_context *pcontext,
			struct tms_retotdr_test_param *ptest_param,
			struct tms_retotdr_data_hdr   *pdata_hdr,
			struct tms_retotdr_data_val   *pdata_val,
			struct tms_retotdr_event_hdr  *pevent_hdr,
			struct tms_retotdr_event_val  *pevent_val,
			struct tms_retotdr_chain      *pchain,
			struct tms_cfg_otdr_ref_val   *pref_data)
{
	tms_Print_tms_retotdr_event(pevent_hdr, pevent_val);
	tms_Print_tms_retotdr_chain(pchain);
	tms_Print_tms_retotdr_test_param(ptest_param);
	tms_Print_tms_cfg_otdr_ref_val(pref_data);
	return 0;
}
void tms_Callback(struct tms_callback *ptcb)
{
	bzero(ptcb, sizeof(struct tms_callback));
	
	ptcb->pf_OnCopy2Use             =  unuse_copy2use;
	ptcb->pf_OnGetDeviceComposition = tms_OnGetDeviceComposition;
	ptcb->pf_OnRetDeviceComposition = tms_OnRetDeviceComposition;
	ptcb->pf_OnCommand              = tms_OnCommand;
	ptcb->pf_OnCUNoteNet            = tms_OnCUNoteNet;
	ptcb->pf_OnCUNoteManageConnect  = tms_OnCUNoteManageConnect;
	ptcb->pf_OnRetSerialNumber      = tms_OnRetSerialNumber;
	ptcb->pf_OnSetIPAddress         = tms_OnSetIPAddress;
	ptcb->pf_OnRetOTDRTest          = tms_OnRetOTDRTest;
	ptcb->pf_OnCfgOTDRRef           = tms_OnCfgOTDRRef;
#ifdef _MANAGE

	// 重定向tms_Analysexxx函数处理方式，如果不执行任何tms_SetDoWhat，则默认表示
	// 协议处于MCU工作方式，回调函数的dowhat设置为0表示不做任何转发，收到的数据一律
	// 传递给应用层
	// 1.作为网管和板卡来说都是传递给应用层
	// 2.作为CU和MCU就要仔细修改dowhat的处理方式
	int cmd_0xx000xxxx[100];

	bzero(cmd_0xx000xxxx, sizeof(cmd_0xx000xxxx) / sizeof(int));
	tms_SetDoWhat(0x10000000, sizeof(cmd_0xx000xxxx) / sizeof(int),cmd_0xx000xxxx);
	tms_SetDoWhat(0x60000000, sizeof(cmd_0xx000xxxx) / sizeof(int),cmd_0xx000xxxx);
	tms_SetDoWhat(0x80000000, sizeof(cmd_0xx000xxxx) / sizeof(int),cmd_0xx000xxxx);
#endif
	
}


void tms_SetCB(void *fun)
{
	if (fun) {
		tcb.pf_OnCopy2Use =  (int32_t (*)(char *, int32_t , int))fun;
	}
}


void *ThreadConnectCU(void *arg)
{
	struct ep_t *pep = (struct ep_t*)arg;
	struct ep_con_t client;

	// return 0;
#ifdef _MANAGE
	return 0;
#endif
	usleep(3000000);//延时3s，防止x86下efence奔溃
	while(1) {
		if (g_en_connect_cu == 1 && 0 == tms_ManageCount()) {
			if (0 == ep_Connect(pep,&client, "192.168.0.253", 6000) ) {
		    	printf("connect CU success :  %s:%d\n", 
							inet_ntoa(client.loc_addr.sin_addr),
							htons(client.loc_addr.sin_port));
		    	tms_AddManage(client.sockfd, 0);
		    	sleep(5);
		    }
		    else {
		    	sleep(5);
		    	continue;
		    }
	    }

	    if (g_en_connect_cu == 1 && 0 != tms_ManageCount()) {
	    	struct tmsxx_app *ptapp = (struct tmsxx_app*)client.ptr;
			
			for (int i = 0;i < 10; i++) {
				tms_Tick(client.sockfd, NULL);
				sleep(3);

				if (ptapp->context.tick == 0) {
					if (i < 6) {
						printf("continue send TICK to cu %d\n",i);
						continue;
					}
					else {
						ep_Close(pep, NULL, client.sockfd);
						break;
					}
				}
				else {
					break;
				}
			}
			ptapp->context.tick = 0;
	    }
	    sleep(5);
	}
    

}

#ifdef __cplusplus
}
#endif