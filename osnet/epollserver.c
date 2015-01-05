/**
 ******************************************************************************
 * @file	epollserver.c
 * @brief	
 *		epoll server 服务框架程序
 
linux环境采用网络服务器框架，框架采用epoll位基础
 *
 
 
 @section Platform
	-# Linux 2.6.35-22-generic #33-Ubuntu SMP Sun Sep 19 20:34:50 UTC 2010 i686 GNU/Linux
	-# gcc-4.7.4 gcc/g++
 @section Library
	-# [libpthread.so.0]
	-# [libreadline.so.5]
		-# [libhistory.so.5]
		-# [libtinfo.so.5]
	-# [libreadline.so.5.2]
		-# [libncurses.so.5.5]
	-# [libc.so.6]

@section History
- 2015-1-4,Menglong Wu,DreagonWoo@163.com
 	- 1.完成tcp连接状态管理 
 	- 2.存在服务器主动断开TIMEOUT问题

- 2015-4-3,Menglong Wu,DreagonWoo@163.com
 	- Add 添加回调接口pFUI_OnConnect
- 2015-4-13,Menglong Wu,DreagonWoo@163.com
	- Add pFUI_OnRelease
- 2015-4-15,Menglong Wu,DreagonWoo@163.com
	- Add ep_KeepAlive
- 2015-4-17,Menglong Wu,DreagonWoo@163.com
	- Add ep_Ergodic
	- Add ep_callback:callback function 
 * @attention
 *
 * ATTENTION
 *
 * <h2><center>&copy; COPYRIGHT </center></h2> 
*/


#include "epollserver.h"


#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <malloc.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/list.h>
#include <stdlib.h>


volatile int __wcmd_end3  __attribute__ ((section (".w_boot_end"))) = 0;

#define MAX_DBG_STROUT 256
char strout[MAX_DBG_STROUT];///<为监控程序预留，有缺陷，需要同步
struct ep_t gg_ep; ///<预留
static pthread_t g_pthread;///<预留


// int ep_StopServer(struct ep_t *pep);
// struct ep_con_t *ep_AddClient(struct ep_t *pep, struct ep_con_t *pepNode);
// int ep_DelConNode(struct ep_t *pep, struct ep_con_t *node);


static int ep_AddClient(
		__in__ 	struct ep_t 	  *pep, 
		__out__	struct ep_con_t  **pepNode);
static int ep_DelConNode(
		__in__ 	struct ep_t 	  *pep, 
		__in__ 	struct ep_con_t  **ppconNode);
static int ep_AddEvent(
		__in__ 	struct ep_t 	  *pep, 
		__inout__ struct ep_con_t   *pconNode, 
		__in__ 	int                fd, 
		__in__ 	struct sockaddr_in addr);
static int ep_DelEvent(
		__in__ 	struct ep_t 	  *pep, 
		__inout__ struct ep_con_t   *pconNode);
static int ep_Accept(
		__in__ 	struct ep_t 	  *pep, 
		__out__ 	struct ep_con_t   *pconNode);



/**
 * @brief	接收数据，并对数据接收的异常做出处理
 * @retval \n	NULL
 * @remarks	
 */
int ep_Recv(
		__in__ struct ep_t     *pep, 
		__in__ struct ep_con_t *pconNode, 
		__in__ int len)
{
	int ret;
	// char rbuf[1024];

	//TODO 互斥
	// char *pdata;
	// int reserved;
	// int size;
	// struct bipbuffer *pbb;
	

	// size = EP_MAX_BUF >> 4;
	// pbb = &pconNode->bb;
	// TODO post message
	// pep->pFUI_OnRemoveClose(0,NULL);
	// if ( (pdata = bipbuffer_Reserve(pbb, size, &reserved)) ) {	
	// 	ret = recv(pconNode->sockfd, pdata , reserved, 0);
	// 	printf("ret = %d %d\n",ret,reserved);
	// 	// memset(pdata, 0x01, 32);
	// 	bipbuffer_Commit(pbb, ret);	
	// }

	ret = pep->pFUI_OnRecv(pep,pconNode);
// #ifdef TARGET_ARMV7
// 	char rbuf[1024*10];
// 	ret = recv(pconNode->sockfd, rbuf , sizeof(rbuf), 0);
// 	if (ret > 0) 
// 	{
// 		ret = send(pconNode->sockfd, rbuf , ret, 0);	
// 	}
// #endif
	

	//sleep(1);
	if (ret == -1) {
		// printf("th1 error recv %d %s\n", errno, strerror(errno));
		if (errno == 11) {
			//continue;
		}

		//ECONNRESET 对方 RST ACK
		//ETIMEDOUT  TCP自带保活性超时
		else if (errno == ECONNRESET || errno == ETIMEDOUT) {
			perror("recv errno:");
			pep->pFUI_OnRemoveClose(pep,pconNode);
			// printf("delete client\n");
//			ep_DelEvent(pep, pconNode);
			ep_DelConNode(pep, &pconNode);
		}
		return -1;
	}
	if (ret == 0) {//TCP flag -->FIN ACK
		// TODO post message
		pep->pFUI_OnRemoveClose(pep,pconNode);
	 	//ret = close(events[i].data.fd);
	 	// printf("remote close fd %d\n", pconNode->sockfd);
//	 	ep_DelEvent(pep, pconNode);
	 	ep_DelConNode(pep, &pconNode);
	 	return 0;
	}

	// if (ret > 0) {
	// 	// TODO 回调函数协议分析
	// 	// TODO post message
	// 	pdata = bipbuffer_GetContiguousBlock(pbb, &reserved);
	// 	printf("len:%d data: %s\n",reserved,pdata);
		
	// }
	

	return ret;//????
}
int ep_Send(
		__in__ struct ep_t        *pep, 
		__in__ struct epoll_event *ev, 
		__in__ int len)
{
	return 0;//????
}

/**
 * @brief	连接TCP连接
 * @param	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @param	pconNode 返回struct ep_con_t *结构指针
 * @param	addr 连接IP地址
 * @param	port 连接端口
 * @retval	0 成功
 * @retval	其他 调用strerrno(errno)查阅错误码含义
 * @remarks	
 */
 int ep_Connect(
		__in__ 	struct ep_t 			*pep, 
		__out__ 	struct ep_con_t 		*pnode, 
		__in__ 	const char 				*strAddr, 
		__in__ 	const unsigned short 	 port)
{
	// 探测连接
	int fd;						  
	struct sockaddr_in remoteAddr;// 远端连接端口
	struct sockaddr_in locateAddr;// 绑定本地端口
	socklen_t len = sizeof(struct sockaddr_in);

	fd = socket(AF_INET, SOCK_STREAM, 0);

	// 连接后获取外部地址
	remoteAddr.sin_family      = AF_INET;
	remoteAddr.sin_addr.s_addr = inet_addr(strAddr);
	remoteAddr.sin_port        = htons(port);	
	if (connect(fd, (struct sockaddr*)&remoteAddr, sizeof(struct sockaddr_in)) != 0) {
		perror("ep_Connect");
		
		// TODO post message
		close(fd);
		return -1;
	}
	// TODO post message
	perror("Connect");

	
	//如果连接成功就申请内存加入链表，并将节点地址返回调用者
	struct ep_con_t *pconNode = NULL;

	int ret = ep_AddClient(pep, &pconNode);
	if(ret == -1) {
		// printf("error\n");
		// while(1) {
		// 	sleep(1);
		// }
	}
	
	getsockname(fd, (struct sockaddr*)&locateAddr, &len);
	// printf("pconNode %x rem_addr %x\n",(unsigned int)pconNode, pconNode->rem_addr);
	// printf("pconNode %x\n",(unsigned int)pconNode);
	pconNode->rem_addr = remoteAddr;
	pconNode->loc_addr = locateAddr;

	// todo post message
	ep_AddEvent(pep, pconNode, fd, locateAddr);
	pep->pFUI_OnConnect(pep,pconNode);
	if(pep->node_cnt >= 800) {
		// TODO: post message limit
		ep_DelConNode(pep, &pconNode);
		pconNode = NULL;
		//*pnode = *pconNode;
		// printf("max file\n");
		return 0;
	}
	if (pnode) {
		*pnode = *pconNode;
	}
	return 0;

	
}

/**
 * @brief	关闭连接
 * @param	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @param	pfreenode 调用者要保证pnode是pep的一个节点，否则导致内存错误
 * @retval	fd 连接设备描述符
 * @remarks	如果pfreenode不为NULL则释放，否则遍历pep所有节点寻找节点sockfd与fd
 相同的节点释放掉
 * @see	
 */

int ep_Close(
		__in__ 	struct ep_t 	*pep, 
		__out__ 	struct ep_con_t *pfreenode, 
		__out__ 	int 		     fd)
{
	// TODO 遍历，找出fd的pnode
	struct ep_con_t *pnode;
	struct list_head *pos, *n;
	int find = 0;

	if (pfreenode) {
		goto _Free;
	}


	list_for_each_safe(pos, n, &pep->node_head) 
	{		
		pnode = (struct ep_con_t*)list_entry(pos, struct ep_con_t, list);
		if (pnode->sockfd == fd ) {
			find = 1;
			break;
		}
		// todo 删除节点
		//printf("aft %x %x\n",(int)pep->listen,(int)pnode);
	}

	if(find) {
_Free:;

		// TODO post message
		pep->pFUI_Close(pep, pnode);
		// printf("dbg fd %d\n",pnode->sockfd);
		ep_DelEvent(pep, pnode);
		ep_DelConNode(pep, &pnode);
		// close(fd);
		return 0;
	}
	return -1;
}



/**
 * @brief	开启TCP保活机制
 * @param	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @param	pfreenode 调用者要保证pnode是pep的一个节点，否则导致内存错误
 * @retval	fd 连接设备描述符
 * @remarks	如果pfreenode不为NULL则释放，否则遍历pep所有节点寻找节点sockfd与fd
 相同的节点释放掉
 * @see	
 */

int ep_KeepAlive(
		__in__ 	struct ep_t 	*pep, 
		__in__	struct ep_con_t *pconNode,
		__in__	int idle,
		__in__	int interval,
		__in__	int count,
		__in__	int enable)
{  
    int val = enable;
    int fd = pconNode->sockfd;

    //开启keepalive机制  
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) == -1)  
    {  
        perror("setsockopt SO_KEEPALIVE: ");  
        return -1;  
    }
  	
  	if (enable == 0) {
  		return 0;
  	}
#ifdef __linux__  
    /* Default settings are more or less garbage, with the keepalive time 
     * set to 7200 by default on Linux. Modify settings to make the feature 
     * actually useful. */  
  
    /* Send first probe after interval. */  
    val = idle;  
    if (setsockopt(fd, IPPROTO_TCP, 4/*TCP_KEEPIDLE*/, &val, sizeof(val)) < 0) {  
        perror( "setsockopt TCP_KEEPIDLE: \n");  
        return -1;  
    }  
  
    /* Send next probes after the specified interval. Note that we set the 
     * delay as interval / 3, as we send three probes before detecting 
     * an error (see the next setsockopt call). */  
    val = interval;
    if (val == 0) val = 1;  
    if (setsockopt(fd, IPPROTO_TCP, 5/*TCP_KEEPINTVL*/, &val, sizeof(val)) < 0) {  
        perror( "setsockopt TCP_KEEPINTVL: \n");  
        return -1;  
    }  
  
    /* Consider the socket in error state after three we send three ACK 
     * probes without getting a reply. */  
    val = count;  
    if (setsockopt(fd, IPPROTO_TCP, 6/*TCP_KEEPCNT*/, &val, sizeof(val)) < 0) {  
        perror( "setsockopt TCP_KEEPCNT: %s\n");  
        return -1;  
    }  
#endif  
  
    return 0;  
}  

/**
 * @brief	连接
 * @param	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @param	pconNode
 * @retval	NULL
 * @remarks	
 */
static int ep_Accept(
		__in__ 	struct ep_t 	*pep, 
		__out__ 	struct ep_con_t *pconNode)
{
	// 先完成accept接收确认，防止服务器accept缓存挤满
	socklen_t 		 len = sizeof(struct sockaddr_in);
	int fd;
	struct sockaddr_in remoteAddr;

	fd = accept(pconNode->sockfd, (struct sockaddr*)&remoteAddr, &len);

	if (-1 == fd) {
		// printf("port %d\n",htons(remoteAddr.sin_port));

		// perror("Accept");
		// ep_DelEvent(pep, pconNode);
		// TODO post message
		pep->pFUI_OnError(pep,NULL);
		return -1;
	}
	
	
	perror("Accept");
	ep_Debug("%s:%d\n",inet_ntoa(remoteAddr.sin_addr), htons(remoteAddr.sin_port));

	// 添加到链表，如果失败返回错误码，否则设置套接字为非阻塞
	struct ep_con_t *pnewNode;
	struct sockaddr_in locateAddr;

	ep_AddClient(pep, &pnewNode);
	ep_AddEvent(pep, pnewNode, fd, remoteAddr);
	getsockname(fd, (struct sockaddr*)&locateAddr, &len);

	pnewNode->loc_addr = locateAddr;
	pnewNode->rem_addr = remoteAddr;

	pep->pFUI_OnAccept(pep,pnewNode);

	if(pep->node_cnt >= 800) {
		// TODO post message
		ep_DelConNode(pep, &pnewNode);
		ep_Debug("max file\n");
	}
	// TODO post message
	

	// *pconNode = *pnewNode;
	return 0;
}


void dbg_Server(const char *str)
{
	printf("%s", str);
}

static int ep_Setnonblocking(int fd)
{
  int op;
 
  op=fcntl(fd, F_GETFL);
  if (fcntl(fd, F_SETFL, op|O_NONBLOCK) < 0) {
  	perror("ep_Setnonblocking fctl error\n");
  	
  	return errno;
  }
 
  return op;
}

#include <netinet/tcp.h>
// int epollfd;
// int clientfd;
// int tmpevfd;
void *ep_WorkThread(void *arg)
{
	ep_Debug("\npid %d work thread id 0x%8.8x\n", getpid(), (unsigned int)pthread_self());	
	printf("\npid %d work thread id 0x%8.8x\n", getpid(), (unsigned int)pthread_self());
	struct ep_t *pep = (struct ep_t*)arg;
	struct epoll_event events[10];
	int nfds;
	struct ep_con_t *pnode;

	// 首先提取fd，防止线程不同步，pep->listen内存已经被释放却被二次访问
	int listenfd = pep->listen->sockfd;
	while (1) {

		nfds = epoll_wait(pep->epfd, events, 10, -1);
		// while(0 != only) {
		// 	sleep(0);
		// }
		// only = 1;
		if (nfds == -1) {
			printf("!!!!epoll_wait err %d %s\n", errno, strerror(errno));
			// only = 0;
			continue;
			//exit(0);
		}

		// ep_Debug("\nth1 have %d\n", nfds);
		//sleep(1);
		for (int i = 0;i < nfds;++i) {
			pnode = (struct ep_con_t*)events[i].data.ptr;

			if (pnode->sockfd != listenfd) {
				ep_Recv(pep, pnode, 100);
			}
			else if (pnode->sockfd == listenfd) {
				ep_Accept(pep, pnode);
				continue;
			}
		}// end for
		// sleep(0);
		// only = 0;
	}// end while(1)
	return 0;
}
	
/**
 * @brief	启动服务器，并开启监听，监听套接字属于非阻塞，
 * @param	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @param	port 如果输入为0则监听端口EP_DEF_PORT\n
 * @retval	0 成功
 * @retval	其他 调用strerrno(errno)查阅错误码含义
 * @remarks	ep_Listen将监听端口选项设置成复用模式SO_REUSEADDR，若之前某服务器端口之前连接
 过客户端，服务器没有完成4次TCP握手退出导致服务器端口状态为TIME_WAIT，持续时间默认60S，此时
 调用bind绑定端口返回EADDRINUSE，绑定失败，设置SO_REUSEADDR选项允许处于TIME_WAIT状态下的端口
 被另一套接字绑定
 * @see	ep_CreateThread
 */
int ep_Listen(
		__in__ struct ep_t 	   *pep, 
		__in__ unsigned short 	port)
{
	int fd;
	struct sockaddr_in locateAddr;
	int 				ret;
	int 				opt = 1;

	// 如果之前创建过端口则释放它，创建成功后socket设置成复用模式
#ifdef _UNUSE_QT_
	ep_StopServer(pep);
#endif
	fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof( opt ));
	// 绑定端口
	if (port == 0) {
		port = EP_DEF_PORT;
	}	
	locateAddr.sin_family 	   = AF_INET;  
	locateAddr.sin_port 	   = htons (port);
	locateAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(fd, (struct sockaddr*)&locateAddr, sizeof(struct sockaddr_in));
	if (ret) {
		// TODO：执行shell脚本，确认端口全处于TIMEWAIT状态则绑定成功，否则失败
		if (errno == EADDRINUSE && 1/*do shell*/) {
			perror("!!!warning bind:");
			// TODO 返回值
			// pep->pFUI_OnError(0,NULL);
		}
		else {
			perror("!!!err bind:");
			// TODO 返回值
			// pep->pFUI_OnError(0,NULL);
			return errno;
		}
	}
	ret = listen(fd, 10);
	if (ret == 0) {
		printf("Run server listening at: %s:%d\n", 
						inet_ntoa(locateAddr.sin_addr), 
						htons(locateAddr.sin_port));
	}
	else {
		perror("!!!err listen:");
		return errno;
	}



	struct ep_con_t *pconNode = NULL;

	ret = ep_AddClient(pep, &pconNode);
	perror("ep_AddClient");
	ret = ep_AddEvent(pep, pconNode, fd, locateAddr);
	perror("ep_AddEvent");
	pconNode->loc_addr = locateAddr;
	pep->listen = pconNode;//*listen
	// pep->listen = &pconNode;//**listen

	pep->cs = EP_SERVER;
	perror("ep_listen");


	return 0;
}



/**
 * @brief	创建epoll工作线程
 * @param	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @param	num 0默认创建cpu数2倍线程，否则创建num个线程\n
 * @retval	0 成功
 * @retval	其他 调用strerrno(errno)查阅错误码含义
 * @remarks	后期可能去掉该函数，用ep_Run代替。建议调用ep_CreateThread后再调用ep_Listen，否则可能端口开启监听后未创建工作线程
 导致客户端响应不及时
 */
int ep_CreateThread(
		__in__ struct ep_t 	*pep, 
		__in__ int 	num)
{
	assert(pep);
	// dbg_Server("ep_CreateThread()\n");

	//创建若干线程
	if (num <= 0) {
		num = 4;
	}

	for (int i = 0;i < num;++i) {
		// TODO:创建内容
	}
	
	// struct ep_thread_arg_t *ptarg;
	// ptarg   = (struct ep_thread_arg_t*)malloc(sizeof(struct ep_thread_arg_t));
	// ptarg->pep = pep;
	
	
	pthread_create(&g_pthread, NULL, ep_WorkThread, (void*)pep);
	// pthread_create(&pthread, NULL, ep_WorkThread, (void*)pep);
	// pthread_create(&pthread, NULL, ep_WorkThread, (void*)pep);
	ep_Debug("pthread id 0x%8.8x\n", (unsigned int)g_pthread);
	// pthread_cancel(g_pthread);
	return 0;
}


/**
 * @brief	关闭所有线程
 * @param	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @retval	0 总是返回0
 * @remarks	后期可能去掉该函数，用ep_Stop代替
 * @see	
 */

int ep_CloseThread(
		__in__ struct ep_t 	*pep)
{
	// TODO 释放步骤
	return 0;
}

/**
 * @brief	实例化epoll server对象
 * @param	pep struct ep_t对象指针
 * @param	version epoll server类版本号，目前忽略改值，请填0
 * @retval	null
 * @remarks	释放epoll server对象调用ep_Release
 * @see	ep_Release
 */

int ep_Interface(
		__inout__	struct ep_t *pep, 
		__opt__		int     	 version)
{
	// 保证pep指针有效，将其内存赋值为0
	assert(pep);
	// memset(pep, 0, sizeof(struct ep_t));
	bzero(pep, sizeof(struct ep_t));

	// 所有链表初始化
	INIT_LIST_HEAD(&pep->msg_head);
	INIT_LIST_HEAD(&pep->thread_head);
	INIT_LIST_HEAD(&pep->node_head);

	// 互斥量初始化
	MUTEX_INIT(&pep->node_mutex);
	MUTEX_INIT(&pep->msg_mutex);

	// 创建epoll文件
	pep->epfd = epoll_create(1);

	// TODO:回调函数初始化
	//pep->pFep_CreateThread = ep_CreateThread;
	return 0;
}


/**
 * @brief	释放epoll server对象所有资源
 * @param	pep struct ep_t对象指针
 * @retval	null
 * @remarks	释放epoll server对象调用ep_Release
 * @see	ep_Interface
 */
int ep_Release(
	__inout__	struct ep_t *pep)
{
	// 释放所有连接
	ep_StopServer(pep);
	// TODO 结束各线程

	// TODO 正确释放各个链表
	if (pep->pFUI_OnRelease) {
		pep->pFUI_OnRelease(pep);
	}
	close(pep->epfd);
	return 0;
}


/**
 * @brief	将epoll设备文件里添加socket事件，该socket自动设置成非阻塞
 * @param	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @param	pconNode 
 * @param	fd socket设备号
 * @param	addr socket关联的struct sockaddr_in结构地址信息
 * @retval	0 成功
 * @retval	其他 调用strerrno(errno)查阅错误码含义
 * @remarks	pconNode不能为NULL指针，用户层永远不会调用该函数
 * @see	ep_DelEvent
 */
static int ep_AddEvent(
		__in__ 	  struct ep_t 		*pep, 
		__inout__ struct ep_con_t 	*pconNode, 
		__in__ 	  int 				 fd, 
		__in__ 	  struct sockaddr_in addr)
{
	pconNode->sockfd     = fd;
	//pconNode->loc_addr 	 = addr;
	pconNode->this_addr  = (unsigned int)pconNode;
	// Step 1.2todo：节点ptr指针初始化

	// Step 3:socket设置成非阻塞，加入epoll事件中
	ep_Setnonblocking(pconNode->sockfd);
	pconNode->event.data.ptr = pconNode;
	pconNode->event.events  = EPOLLIN;
	if (epoll_ctl(pep->epfd, EPOLL_CTL_ADD, pconNode->sockfd, &pconNode->event) == -1) 
	{
		perror("epoll_ctl: serverfd");
		ep_DelConNode(pep, &pconNode);
		return -1;
	}
	return 0;
}

/**
 * @brief	将epoll从设备文件里添加socket事件，该socket自动设置成非阻塞
 * @param	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @param	pconNode 释放的struct ep_con_t对象
 * @retval	0 成功
 * @remarks	用户层永远不会调用该函数
 * @see	ep_AddEvent
 */
static int ep_DelEvent(
	__in__    struct ep_t *pep, 
	__inout__ struct ep_con_t *pconNode)
{
	//Step 1:将socket从epoll事件中删除
	if (pconNode->event.events == 0) {
		return 0;
	}
	if (epoll_ctl(pep->epfd, EPOLL_CTL_DEL, pconNode->sockfd, &pconNode->event) == -1) {
		perror("err ep_DelConNode:epoll_ctl");
	}
	else {
		// printf("del success\n");
	}
	// pconNode->event.data.ptr = NULL;

	return 0;
}

/**
 * @brief	将客户端描述符从struct ep_t node_list里删除
 * @param[in]	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @param[out]	ppconNode 返回指向struct ep_con_t大小的内存
 * @retval	0 成功
 * @retval	非0 struct ep_con_t节点指针
 * @remarks	ppconNode必须是有效指针，否则ep_AddClient会弹出异常。如果ppconNode指针
 有效，但是申请内存错误将调用exit(1)终结程序
 * @see	ep_DelConNode
 */
int ep_AddClient(
		__in__ 	struct ep_t 	 *pep, 
		__out__	struct ep_con_t **ppconNode)
{
	// Step 1.1:申请内存加入到pep->node_head链表
	assert(pep    != NULL);
	assert(ppconNode != NULL);
	if(ppconNode == NULL) {
		return -1;
	}
	
	*ppconNode = (struct ep_con_t*)malloc(sizeof(struct ep_con_t));
	if (*ppconNode == 0) {
		perror("ep_AddClient malloc");
		exit(1);
		return -1;
	}
	// memset(*ppconNode, 0, sizeof(struct ep_con_t));
	bzero(*ppconNode, sizeof(struct ep_con_t));
	
	// Step 2:临界区：插入链表
	MUTEX_LOCK(&pep->node_mutex);
	list_add_tail(&(*ppconNode)->list, &pep->node_head);
	pep->node_cnt++;	
	MUTEX_UNLOCK(&pep->node_mutex);

	// struct bipbuffer *pbb;
	// pbb = &(*ppconNode)->bb;
	// bipbuffer_Init(pbb);
	// bipbuffer_AllocateBuffer(pbb,EP_MAX_BUF*2);
	return 0;
}


/**
 * @brief	将客户端描述符从struct ep_t node_list里删除
 * @param[in]	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @param[out]	ppconNode 待释放的struct ep_con_t内存
 * @retval	0 成功
 * @remarks	ppconNode必须是有效指针，否则ep_AddClient会弹出异常。ep_DelConNode
 同时会关闭struct ep_con_t.sockfd设备，所以在调用ep_DelConNode之后不要再访问sockfd
 * @see	ep_AddClient
 */
static int ep_DelConNode(
		__in__ struct ep_t 	    *pep, 
		__in__ struct ep_con_t **ppconNode)
{
	assert(pep    != NULL);
	assert(ppconNode != NULL);

	// debug
	// debug("delete fd %d\n",(*ppconNode)->sockfd);
	//Step 2:临界区，释放节点
	MUTEX_LOCK(&pep->node_mutex);
	// list_del(&(*ppconNode)->list);
	list_del_init(&(*ppconNode)->list);
	pep->node_cnt--;
	MUTEX_UNLOCK(&pep->node_mutex);

	
	//Step 4:在这里关闭套接字，待验证，这之后是不是真的友好关闭了
	if (close((*ppconNode)->sockfd) == 0) {
		(*ppconNode)->sockfd = 0;
	}	
	//Step 5:释放节点资源
	free(*ppconNode);
	*ppconNode = NULL;
	// printf("*ppconNode = NULL;\n");
	return 0;
}

/**
 * @brief	运行服务器
 * @param [in]	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @retval \n	NULL
 * @remarks	暂时内部无代码
 */

int ep_RunServer(
		__inout__ struct ep_t *pep)
{
	int fd;
	struct ep_con_t *pconNode = NULL;
	// int ret;
	

	// struct sockaddr_in unuseAdrr;
	
	// 创建无用的socket设备文件，不做任何事情，只有2个目的
	// 1.防止ep_WorkThread的pep->listen内存
	// 2.防止ep_StopServer关闭无效设备文件
	if (pep->listen == NULL) {
		printf("client\n");
		// 无实际用途
		fd = socket(AF_INET, SOCK_STREAM, 0);
		fd = fd;

		pconNode = (struct ep_con_t*)malloc(sizeof(struct ep_con_t));
		pconNode->event.events = 0;

		pep->listen = pconNode;
		pep->cs = EP_CLIENT;
	}
	ep_CreateThread(pep,0);
	return 0;
}


/**
 * @brief	暂停服务器
 * @param	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @retval \n	NULL
 * @remarks	
 */
int ep_StopServer(
	__inout__ struct ep_t *pep)
{

	// Step 1:删除各个客户端
	struct ep_con_t *pnode;
	struct list_head *pos, *n;


	list_for_each_safe(pos, n, &pep->node_head) 
	{	
		pnode = (struct ep_con_t*)list_entry(pos, struct ep_con_t, list);
		
		// UNTODO post message

		ep_DelEvent(pep, pnode);
		// ep_DelConNode(pep, &pnode);
		
		// todo 删除节点
		//printf("aft %x %x\n",(int)pep->listen,(int)pnode);
	}
	// EPOLL 缺陷
	// sleep(0);sleep(0);sleep(0);sleep(0);sleep(0);sleep(0);sleep(0);sleep(0);sleep(0);
	// nfds = 1;
	// while(nfds) {
	// 	nfds = epoll_wait(pep->epfd, events, 10, 1);
	// 	if(nfds	> 0) {
	// 		printf("oh noooooooooo\n");
	// 		exit(1);
	// 		sleep(0);
	// 	}
	// }
	struct sockaddr_in locateAddr,remoteAddr;
	socklen_t len;
	list_for_each_safe(pos, n, &pep->node_head) 
	{		
		pnode = (struct ep_con_t*)list_entry(pos, struct ep_con_t, list);
		len = sizeof(struct sockaddr_in);
		getsockname(pnode->sockfd, (struct sockaddr*)&locateAddr, &len);
		len = sizeof(struct sockaddr_in);
		getpeername(pnode->sockfd, (struct sockaddr*)&remoteAddr, &len);
		// printf("fd %d %s:%d\n",pnode->sockfd);
		printf("%8d%16s:%-8d",
			pnode->sockfd,
			inet_ntoa(locateAddr.sin_addr),
			htons(locateAddr.sin_port));
		printf("%16s:%-8d\n",
			inet_ntoa(remoteAddr.sin_addr),
			htons(remoteAddr.sin_port));

		ep_DelConNode(pep, &pnode);	
		
	}
	// Step 2:
	// 不做服务器
	// printf ("pep->listen %8.8x\n",pep->listen);

	if (pep->cs == EP_CLIENT && pep->listen) {
		free(pep->listen);
	}
	pep->listen = NULL;

	// Step 3:关闭线程
	// pthread_cancel(g_pthread);
	// only = 0;	
	return 0;
}

/**
 * @brief	跳过第一个ep_con_t节点，遍历其余所有连接节点，第一个节点是监听端口
 * @param	pep struct ep_t 结构，必须先调用ep_Interface初始化\n
 * @param	cb 回调函数，当cb返回-1时结束遍历
 * @retval \n	NULL
 * @remarks	建议应用程序访问节点时使用该方法，该方法有“锁”机制，否则应用层访问
 			遍历节点耗时过长，如果恰好某连接节点断开连接，释放所有内存，应用程序
 			将访问非法内存而出现“段错误”崩溃\n
 			第一个监听节点如果调用send方法将导致崩溃，所以目前不返回
 */
int ep_Ergodic(
	__inout__ struct ep_t *pep,
	__in__    ep_callback cb,
	__in__    void *ptr)
{

	// Step 1:删除各个客户端
	struct ep_con_t *pnode;
	struct list_head *pos, *n;

	MUTEX_LOCK(&pep->node_mutex);	
	
	list_for_each_safe(pos, n, &pep->node_head) 
	{	
		pnode = (struct ep_con_t*)list_entry(pos, struct ep_con_t, list);
		if ( pnode->sockfd != pep->listen->sockfd && cb(pnode,ptr) == -1) {
			break;
		}
	}
	MUTEX_UNLOCK(&pep->node_mutex);
	return 0;
}