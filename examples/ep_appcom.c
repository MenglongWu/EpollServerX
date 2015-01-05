#include "epollserver.h"
#include <stdio.h>
#include "bipbuffer.h"
#include "malloc.h"
#include <unistd.h>
#include "./protocol/glink.h"
#include "./protocol/tmsxx.h"

#include "minishell_core.h"
#include "stdlib.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include <time.h>
#include "tms_app.h"
#include "ep_app.h"


#ifdef __cplusplus
extern "C" {
#endif

struct tms_callback tcb;
struct epapp_callback epapp_cb;
// #define BIPBUFFER_LEN 2048
#define BIPBUFFER_LEN 30000
void ep_unuse(int fd)
{
	// printf("\nep_unuse()\n\tfd = %d\n",fd);
}


int InitSocketStruct(struct tmsxx_app **pss, int fd)
{

	*pss = (struct tmsxx_app*)malloc( sizeof(struct tmsxx_app));
	if (*pss == 0) {
		perror("malloc struct tmsxx_app");
		// exit(1);
		return -1;
	}
	memset(*pss, 0, sizeof(struct tmsxx_app));

	// ((struct tmsxx_app*)pnode->ptr)->context.fd = pnode->sockfd;
	(**pss).context.fd = fd;
	(**pss).context.ptcb = &tcb;


	bipbuffer_Init(&(*pss)->bb);
	//bipbuffer_AllocateBuffer(&(*pss)->bb,1024*2);
	bipbuffer_AllocateBuffer(&(*pss)->bb,BIPBUFFER_LEN);
	(*pss)->morebyte = 400;
	
}

int FreeSocketStruct(struct tmsxx_app **pss)
{
	if(*pss == 0) {
		return 0;
	}
	
	bipbuffer_FreeBuffer(&(*pss)->bb);
	free(*pss);	
	
	*pss = NULL;	
}


#define PINF_FLAG_ACCEPT   (0)
#define PINF_FLAG_CONNECT  1
#define PINF_FLAG_REMOVE   2
#define PINF_FLAG_CLOSE    4

// void PrintConnectRemoveInf(int fd, int flag)
void PrintConnectRemoveInf(struct ep_con_t *pconNode, int flag)
{
	struct sockaddr_in locateAddr,remoteAddr;
	socklen_t 		 len;
	time_t now;
	struct tm *timenow;
	// char strtemp[255];  


	// len = sizeof(struct sockaddr_in);
	// getsockname(fd, (struct sockaddr*)&locateAddr, &len);
	// len = sizeof(struct sockaddr_in);
	// getpeername(fd, (struct sockaddr*)&remoteAddr, &len);


	time(&now);
	timenow = localtime(&now);   


	// printf("%-4d%8d%16s:%-8d",
	printf("%8d%16s:%-8d",
		pconNode->sockfd,
		inet_ntoa(pconNode->loc_addr.sin_addr),
		htons(pconNode->loc_addr.sin_port));


	if (flag == PINF_FLAG_ACCEPT) {
		printf("<----");
	}
	else if (flag == PINF_FLAG_CONNECT) {
		printf("---->");
	}
	else if (flag == PINF_FLAG_REMOVE) {
		printf("---xx");
	}
	else {//PINF_FLAG_CLOSE
		printf("xx---");
	}
	// printf("%16s:%-8d",
	// 	inet_ntoa(remoteAddr.sin_addr),
	// 	htons(remoteAddr.sin_port));
	printf("%16s:%-8d\n",
		inet_ntoa(pconNode->rem_addr.sin_addr),
		htons(pconNode->rem_addr.sin_port));

	printf("%d-%d-%d %d:%d:%d\n", timenow->tm_year+1900,timenow->tm_mon+1,timenow->tm_mday,
		timenow->tm_hour,timenow->tm_min,timenow->tm_sec); 
}
int epFUI_OnAccept(struct ep_t *pep, struct ep_con_t *pnode)
{
	printf("----OnAccept----\n");
	ep_KeepAlive(pep,pnode,3,30,4,1);
	// ep_KeepAlive(pep,pnode,3,15,3,1);
	InitSocketStruct((struct tmsxx_app**)&pnode->ptr, pnode->sockfd);
	// ((struct tmsxx_app*)pnode->ptr)->fd = pnode->sockfd;
	// ((struct tmsxx_app*)pnode->ptr)->context.fd = pnode->sockfd;
	
	PrintConnectRemoveInf(pnode, PINF_FLAG_ACCEPT);
	printf("%8d%16s:%-8d",
		pnode->sockfd,
		inet_ntoa(pnode->loc_addr.sin_addr),
		htons(pnode->loc_addr.sin_port));
	printf("%16s:%-8d\n",
		inet_ntoa(pnode->rem_addr.sin_addr),
		htons(pnode->rem_addr.sin_port));
	
	epapp_cb.pf_Accept(pnode->sockfd);
	// sleep(3);
	// tms_GetVersion(pnode->sockfd,  0, 0,DEV_OPM);
	// printf("send over\n");
	// sleep(3);

	return 0;
	
}

int epFUI_OnConnect(struct ep_t *pep, struct ep_con_t *pnode)
{
	printf("----OnConnect----\n");
	// printf("start\n")	;
	InitSocketStruct((struct tmsxx_app**)&pnode->ptr, pnode->sockfd);
	// ((struct tmsxx_app*)pnode->ptr)->fd = pnode->sockfd;
	((struct tmsxx_app*)pnode->ptr)->context.fd = pnode->sockfd;
	// printf("connect struct ok");
	PrintConnectRemoveInf(pnode, PINF_FLAG_CONNECT);

	return 0;

	
}

void PrintfMemory(uint8_t *buf,uint32_t len);;
int epFUI_OnRecv(struct ep_t *pep, struct ep_con_t *pnode)
{
	// static int rtimes = 0;
	// printf("\r--------!!!!!! OnRecv !!!!! --------%8.8x\r",rtimes++);
	int ret;
	char rbuf[100];
	struct tmsxx_app *pss;


	

	int32_t retRecv,retFramelen,recvTotal;
	int32_t reserved;
	char *pdata;

	
	if (pnode->ptr == NULL) {
		printf("wait \n");
		sleep(1);
		return 1;
	}
	pss = (struct tmsxx_app *)pnode->ptr;
	//pss->morebyte = 100;
	if (pss->enable_lbb == 0) {
		pdata = bipbuffer_Reserve(&pss->bb,pss->morebyte,&reserved);	
	}
	else {
		printf("bipbuffer_Reserve lbb  ");
		pdata = bipbuffer_Reserve(&pss->lbb,pss->lbyte,&reserved);
		int size;
		bipbuffer_GetContiguousBlock(&pss->lbb,&size);
		printf("size %d\n",size);
	}


	// sleep(1);
	if (pdata == NULL) {
		retRecv = 1;//无用，必须大于0
	}
	// 固定环形缓存
	else if (pss->enable_lbb == 0) {
		// printf("want to recv %d\n",reserved);
		retRecv = recv(pnode->sockfd, pdata, reserved, 0);	
		// printf("this times recv %d\n",retRecv);
		bipbuffer_Commit(&pss->bb,retRecv);
	}
	// 大型环形缓存，只够存储一帧数据，填满缓存前不找合法帧，
	// 填满后无论是否找到合法帧均释放
	else {
		retRecv = recv(pnode->sockfd, pdata, reserved, 0);	
		bipbuffer_Commit(&pss->lbb,retRecv);
		pss->lbyte -= retRecv;


		if (pss->enable_lbb == 1 && pss->lbyte > 0) {
			return retRecv;
		}
		else {
			struct bipbuffer tbb;
			tbb      = pss->lbb;
			pss->lbb = pss->bb;
			pss->bb  = tbb;
		}
	}
	//bipbuffer_PrintMemory(&pss->bb);

_Again:;
	pdata = bipbuffer_GetContiguousBlock(&pss->bb, &recvTotal);
	if (bipbuffer_GetUsedSize(&pss->bb) >= 40 && recvTotal < 40) {
		int unuse;
		bipbuffer_GetUnContiguousBlock(&pss->bb, &unuse);
	}
	if (recvTotal >= 40) {
		ret = glink_FindFrame((int8_t*)pdata, &recvTotal, &retFramelen);	
	}
	else {
		ret = -6;
		retFramelen = 0;
	}
	
	// printf("ret %d retRecv %d %recvTotal %d retFramelen %d\n",
	// 	ret,retRecv,recvTotal,retFramelen);
	if (ret == 0) {
		
		bipbuffer_DecommitBlock(&pss->bb, retFramelen);
		
		// tms_Analyse(pnode->sockfd, (int8_t*)pdata, retFramelen);
		tms_Analyse(    &(((struct tmsxx_app*)(pnode->ptr))->context), 
						(int8_t*)pdata, 
						retFramelen);

		pss->morebyte = 40;
		if (pss->enable_lbb == 0) {
			goto _Again;
		}
		//if (reserved > 0) {
			
		//}
	}
	else if (ret == -2) {
		// printf("frame err Decommit %d %d\n",retFramelen,reserved);
		bipbuffer_DecommitBlock(&pss->bb, retFramelen);
		

		pss->morebyte = 40;
		//if (reserved > 0) {
			goto _Again;
		//}
	}
	else if (ret == -3) {
		// printf("recvTotal %d retFramelen %d used %d\n",recvTotal,retFramelen,bipbuffer_GetUsedSize(&pss->bb));
		if (recvTotal + retFramelen <= bipbuffer_GetUsedSize(&pss->bb)) {
			int unuse;
			bipbuffer_GetUnContiguousBlock(&pss->bb, &unuse);
		}
		pss->morebyte = 40;//retFramelen;
		bipbuffer_GetContiguousBlock(&pss->bb, &reserved);
		// printf("after copy recvTotal %d reserved %d retFramelen %d\n",
		// 	recvTotal,reserved,retFramelen);
		bipbuffer_PrintMemory(&pss->bb);
		if (reserved >=  recvTotal + retFramelen) {
			goto _Again;
		}

		if (retFramelen + recvTotal > BIPBUFFER_LEN) {
			// 初始化大块临时缓存
			printf("large bipbuffer\n");
			char *pbb_buf,*plbb_buf;
			int pbb_len,plbb_unuselen;

			pss->lbyte = recvTotal + retFramelen;
			bipbuffer_Init(&pss->lbb);
			bipbuffer_AllocateBuffer(&pss->lbb,pss->lbyte);


			pbb_buf = bipbuffer_GetContiguousBlock(&pss->bb, &pbb_len);
			//plbb_buf = bipbuffer_GetContiguousBlock(&pss->lbb, &plbb_unuselen);
			plbb_buf = bipbuffer_Reserve(&pss->lbb, pbb_len, &plbb_unuselen);
			memcpy(plbb_buf, pbb_buf, pbb_len);

			bipbuffer_Commit(&pss->lbb, pbb_len);
			bipbuffer_DecommitBlock(&pss->bb, pbb_len);
			pss->lbyte -= pbb_len;
			pss->enable_lbb = 1;
		}
	

	}
	

	if (pss->enable_lbb == 1 && pss->lbyte <= 0) {
		printf("free....\n");
		struct bipbuffer tbb;

		pss->enable_lbb = 0;
		tbb      = pss->lbb;
		pss->lbb = pss->bb;
		pss->bb  = tbb;

		bipbuffer_FreeBuffer(&pss->lbb);
		
	}
	return retRecv;


	// ret = recv(pnode->sockfd, rbuf, sizeof(rbuf), 0);
	// // printf("recv data %s",rbuf);
	// // 返回值必须是recv函数的返回值
	// return ret;
}

int epFUI_OnError(struct ep_t *pep, struct ep_con_t *pnode)
{
	printf("----OnError----\n");
	perror("which Err:");
	printf("errcode %d %s\n", errno, strerror(errno));
	// printf
	return 0;
}

int epFUI_OnRemoveClose(struct ep_t *pep, struct ep_con_t *pnode)
{
	printf("----OnRemoveClose----\n");
	// printf("close addr %8.8x\n",pnode->ptr);
	FreeSocketStruct((struct tmsxx_app**)&pnode->ptr);

	PrintConnectRemoveInf(pnode, PINF_FLAG_REMOVE);

	printf("%8d%16s:%-8d",
		pnode->sockfd,
		inet_ntoa(pnode->loc_addr.sin_addr),
		htons(pnode->loc_addr.sin_port));
	printf("%16s:%-8d\n",
		inet_ntoa(pnode->rem_addr.sin_addr),
		htons(pnode->rem_addr.sin_port));
	
	tms_RemoveDev(pnode->sockfd);


	epapp_cb.pf_RemoteClose(pnode->sockfd);
	tms_DelManage(pnode->sockfd);
	return 0;

}
int epFUI_OnClose(struct ep_t *pep, struct ep_con_t *pnode)
{
	PrintConnectRemoveInf(pnode, PINF_FLAG_CLOSE);
	tms_RemoveDev(pnode->sockfd);
	tms_DelManage(pnode->sockfd);
	return 0;
}
int epFUI_OnRelease(struct ep_t *pep)
{
	return 0;
}

void ep_Callback(struct ep_t *pep)
{
	pep->pFUI_OnAccept      = epFUI_OnAccept;
	pep->pFUI_OnConnect     = epFUI_OnConnect;
	pep->pFUI_OnRecv        = epFUI_OnRecv;
	pep->pFUI_OnError       = epFUI_OnError;
	pep->pFUI_OnRemoveClose = epFUI_OnRemoveClose;
	pep->pFUI_OnRelease     = epFUI_OnRelease;
	pep->pFUI_Close         = epFUI_OnClose;


	epapp_cb.pf_Accept      = ep_unuse;
	epapp_cb.pf_RemoteClose = ep_unuse;
}


void SetEPAppCallback(struct epapp_callback *p)
{
	if (p == NULL) {
		return;
	}
	if (p->pf_Accept) {
		epapp_cb.pf_Accept = p->pf_Accept;
	}
	if (p->pf_RemoteClose) {
		epapp_cb.pf_RemoteClose = p->pf_RemoteClose;
	}
}


int ConnectCU(struct ep_t *pep)
{
	struct ep_con_t client;

	if (0 == ep_Connect(pep,&client, "127.0.0.1", 6500) ) {    
    }
}

/*
 *本系统至少有4个线程
 * main主线程
 *      |-----epollserver网络线程
 *      |-----自动连接CU线程，该线程每10S执行一次
 *      |-----minishell控制台线程
 *      |
 *      |......主线开放的其他线程
 */

static pthread_t g_pthreadshell,g_pthreadconnect_cu;
void *ThreadConnectCU(void *arg);


void *ThreadShell(void *arg)
{
    int ret = 1;

    ret = -1;
    while(ret == -1) {
		ret = sh_enter();
		if (ret == 0) {
			break;
		}
		sleep(4);
    }
    
    

    // 主线程应该调用它，shell线程不能自杀
	// pthread_cancel(g_pthreadshell);
	// pthread_join(g_pthreadshell,0);

	pthread_cancel(g_pthreadconnect_cu);
    pthread_join(g_pthreadconnect_cu,0);

	    
    exit(0);
}







#include <sys/time.h> 
#include <stdio.h> 
#include <math.h>

struct timeval tpstart,tpend; 
float timeuse; 


void ResetConst()
{
#ifdef WIN32
#else
	gettimeofday(&tpstart,NULL); 
#endif
}
void PrintCost()
{
#ifdef WIN32
#else
	gettimeofday(&tpend,NULL); 
	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+ 
	tpend.tv_usec-tpstart.tv_usec; 
	timeuse/=10000; 
	printf("Used Time:%f\n",timeuse);
#endif
}


#include <readline/readline.h>
#include <readline/history.h>

// #include "rlprivate.h"
extern char *rl_display_prompt ;
struct cmd_prompt cmd_cr[] = 
{
	{"","port <0~2>", NULL},
	{NULL,NULL, NULL},
};
struct cmd_prompt cmd_olplist_ref[] = 
{
	{"port","port <0~2>", NULL},
	{"port1","port <0~2>", NULL},
	{"port2","port <0~2>", NULL},
	{NULL,NULL, NULL},
};


struct cmd_prompt cmd_olplist_oplevel1[] = 
{
	{"oplevel1","opm 1", NULL},	
	{"oplevel1","opm 1", NULL},	
	{"oplevel1","opm 1", NULL},	
	{"ref","reference", NULL},	
	{NULL,NULL, NULL},
};

struct cmd_prompt cmd_olplist_op[] = 
{
	{"opm1","opm 1", cmd_olplist_oplevel1},
	{"opm2","opm 2", cmd_olplist_oplevel1},
	{"op","opm 3", NULL},
	{"auto","opm 3", NULL},
	{"\7<cr>","", NULL},
	{NULL,NULL, NULL},
};

struct cmd_prompt cmd_olplist[] = 
{
	{"ref","reference OP ", cmd_olplist_ref},
	{"op","Get OLP OP", cmd_olplist_op},
	{"alarm","alarm list ", NULL},
	{NULL,NULL, NULL},
};


typedef struct {
  char *name;			/* User printable name of the function. */
  char *func;		/* Function to call to do the job. */
  char *doc;			/* Documentation for this function.  */
} COMMAND;

COMMAND commands[] = {
  { "ref", NULL, "Change to directory DIR" },
  { "sder", NULL, "Change to directory DIR" },
  { "opm", NULL, "Change to directory DIR" },
  { "ef", NULL, "Change to directory DIR" },
  { (char *)NULL, (char *)NULL, (char *)NULL }
};

char *
dupstr (
     char *s)
{
  char *r;

  r = (char*)malloc (strlen (s) + 1);
  strcpy (r, s);
  return (r);
}

char *
command_generator (
     const char *text,
          int state)
{
  static int list_index, len;
  char *name;

  char *pbuf = (char*)malloc(12);
  // snprintf(pbuf, 12,"hello");
  // printf("pbuf\n:%s\n", pbuf);
  printf("state %d\n", state);
  // return pbuf;
  /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0. */
  if (!state)
    {
      list_index = 0;
      len = strlen (text);
    }
printf("text :%s\n", text);
	int lret;
  /* Return the next name which partially matches from the command list. */
	if (len == 0) {
		printf("----too slow----\n");
		return ((char *)NULL);
	}
  while (name = commands[list_index].name)
    {
      list_index++;
      // printf("name %s text %s len %d\n", name, text, len);
      lret = strncmp (name, text, len);
      printf("lret = %d\n", lret);
      if (lret == 0) {
      	// char *ret = (dupstr(name));
      	// printf("ret = :%s\n", ret);
      	printf("--------yes-----\n");
      	return strdup(name);

      }
        
    }
    printf("------no ---------\n");
  /* If no names matched, then return NULL. */
  return ((char *)NULL);
}

char **
fileman_completion (
     const char *text,
     int start, 
     int end)
{
  char **matches;
  printf("start = %d end %d\n", start, end);
  matches = (char **)NULL;

  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory. */
  // char *pbuf = (char*)malloc(100);
  // sprintf(pbuf,"hello");
  // char **ppbuf = pbuf;
  // return ppbuf;
  // if (start == 0) 
  {
  	matches = rl_completion_matches (text, command_generator);
  }
  // printf("matches %s\n", *matches);  

  return (matches);
}

void initialize_readline ()
{
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "FileMan";

  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = fileman_completion;
}
void sh_analyse_ex (char *fmt,long len, char **cmd, int *count)
{
	// char *cmd[256],
	char *token = NULL;	
	int   index = 0;
	char  seps[] = " ,\t\n";

	//step 1:提取第一个单词，并在命令列表里寻找是否存在命令
	*(fmt+len) = '\0';
	token = strtok(fmt,seps);

	while(token != NULL) {
		cmd[index] = token;
		index++;
		token = strtok(NULL,seps);	
	}
	*count = index;

}
void sh_display_prompt(struct cmd_prompt *pprompt);

int
searchboot(
		int argc, 
		char **argv, 
		struct cmd_prompt *pprompt,
		struct cmd_prompt **plist)
{

	struct cmd_prompt *pstart;
	
	// struct cmd_prompt *boot;
	int find = 0;
	int index = 0;
	int len;

	// boot = pprompt;

	pstart = pprompt;
	for (index = 0; index < argc; index++) {
		find = 0;
		len = strlen(argv[index]);
		while ( pstart && pstart->name ) {
			if ( 0 == memcmp(argv[index], pstart->name, len) ) {
				find = 1;
				pstart = pstart->next;
				break;
			}
			pstart++;
		}

		if (find == 0) {
			break;
		}	
	}

	
	// printf("\nindex %d argc %d list %x    ", index, argc, pstart);
	// 当index与argc相等表示完全匹配，
	// 当pstart不为NULL表示有下一层提示

	if (index == argc && pstart) {
		*plist = pstart;
		if (pstart->next == NULL) {
			return 0;
		}
		else {
			return 1;	
		}
	}
	if (index == argc ) {
		*plist = pstart;
		return 2;
	}

	// 当index 小于argc且pstart不为NULL表示前段部分匹配
	// if (pstart) {
	// 	printf("start name %s\n", pstart->name);
	// 	*plist = pstart;
	// 	printf("c\n");
	// 	return 0;
	// }
	*plist = NULL;
	return -1;
}
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
void sh_display_prompt(struct cmd_prompt *pprompt)
{
	// int i = 0;
	struct cmd_prompt *ptprompt;

	ptprompt = pprompt;
	while( ptprompt->name ) {
		printf("\t%-12s\t%s\n", ptprompt->name, ptprompt->help);
		ptprompt++;
	}
}
void sh_list(char *text, int len, struct cmd_prompt *boot)
{

}
int funtest2(int a, int b);
int funtest(int a, int b)
{
	printf ("a = %d b = %d\n", a, b);
	printf("%s", rl_display_prompt);
	rl_bind_key('?',funtest2);
	return 0;
}	
#include "string.h"
int funtest2(int a, int b)
{
	char hello[12]="hello";
	char *pstr;
	// pstr = strndup(hello,2);
	// sprintf(pstr,"01234567890122222222222223");
	// printf("%s \n", pstr);

	// printf(" a %d b %d %c\n",a,b,63);
	int len    = strlen(rl_line_buffer);
#ifdef MINISHELL_USE_MALLOC
	char *pbuf = (char*)malloc(len);
#else
	char buf[1024];
	char *pbuf = buf;
#endif
	char *cmd[256];
	int count;
	memcpy(pbuf, rl_line_buffer, len);

	sh_analyse_ex(pbuf, len, cmd, &count);
	
	struct cmd_prompt *plist;
	putchar('\n');
	int ret;
	ret = searchboot(count, cmd, cmd_olplist, &plist);
	// printf("ret = %d\n", ret);
	if (ret == 0 || ret == 1) {
		// sh_display_prompt(plist);
		sh_display_prompt(plist);
	}
	else if (ret == 2){
		printf("\t<cr>\n");
	}

	// for (int i = 0; i < 30; i++) {
	// 	printf("[%x] (%c) ",i,i);
	// }
	// printf("\n%x %x",'\r','\n');
	// else if (ret == 0) {
	// else {

	// 	printf("\t<cr> Enter %x\n", plist);
	// }
	


#ifdef MINISHELL_USE_MALLOC
	free(pbuf);
#endif
	printf("%s%s", rl_prompt, rl_line_buffer);

	return 0;
}	
int ThreadRunServerAndShell(struct ep_t *pep)
{
	rl_bind_key('?',funtest2);
	initialize_readline();
	tms_Init();
	tms_Callback(&tcb);
    ep_Interface(pep, 2);           // 初始化ep接口
    ep_Callback(pep);               // 设在epollserver在本工程的回掉函数

#ifdef _MANAGE
    if(ep_Listen(pep,6000)) {     // 监听TCP 0.0.0.0:6500端口
        return 0;
    }
#else
	if(ep_Listen(pep,6500)) {     // 监听TCP 0.0.0.0:6500端口
        return 0;
    }
#endif
    
    ep_RunServer(pep);             // 运行epollserver线程
    
	
    pthread_create(&g_pthreadshell,NULL,ThreadShell,pep);
    pthread_create(&g_pthreadconnect_cu,NULL,ThreadConnectCU,pep);
    
    return 0;
}




#ifdef __cplusplus
}
#endif
