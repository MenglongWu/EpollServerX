/**
 ******************************************************************************
 * @file	main.c
 * @brief	
 *			TMSxx 网络Demo
*/
/*--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0			Menglong Wu		2015-1-5	
 ******************************************************************************
*/

#include "stdio.h"
#include "string.h"
#include <errno.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "minishell_core.h"

#include "ossocket.h"





volatile int __wcmd_end2   __attribute__ ((section (".w_boot_end"))) = 0;

// void server()
// {
// 	int listenfd;
// 	int clientfd;
// 	short port = 6000;
// 	struct sockaddr_in listen_addr;
// 	struct sockaddr_in client_addr;
// 	socklen_t len;
// 	int ret;
// 	char rbuf[100];

// 	listenfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	


// 	listen_addr.sin_family = AF_INET;     // Address family
// 	listen_addr.sin_port = htons (port);
// 	listen_addr.sin_addr.s_addr = inet_addr("192.168.1.252");

// 	ret = bind(listenfd,(struct sockaddr*)&listen_addr, sizeof(struct sockaddr_in));
// 	printf("ret %d bind %d %s\n",ret,os_error(),strerror(errno));


// 	listen(listenfd,5);
// 	//printf("listen %s\n", strerror(errno)); 
// 	len = sizeof(struct sockaddr_in);
// 	clientfd = accept(listenfd,(struct sockaddr*)&client_addr,&len);
// 	//printf("accept %s\n", strerror(errno)); 
// 	while(1) {
// 		ret = recv(clientfd,rbuf,10,0);
// 		printf("recv return\n");
// 		if(ret != -1) 
// 		{
// 			rbuf[ret] = '\0';
// 			printf("%s\n",rbuf);
// 		}
// 		//sleep(1);

// 	}


// }

// void client()
// {
// 	int clientfd;
// 	struct sockaddr_in client_addr;



// 	clientfd = socket(AF_INET,SOCK_STREAM,0);
// 	printf("socket %s\n", strerror(errno));
// 	bzero(&client_addr, sizeof(client_addr));
// 	client_addr.sin_family 		= AF_INET;
// 	client_addr.sin_port 		= htons (6000);
// 	client_addr.sin_addr.s_addr = inet_addr("192.168.1.52");
// 	printf("client %x\n",client_addr.sin_addr.s_addr );

// 	int ret = connect(clientfd,(struct sockaddr*)&client_addr,sizeof(struct sockaddr));
// 	printf("ret = %d\n",ret);
// 	printf("%s\n", strerror(errno));  
// 	while(1) {
// 		sleep(1);
		
// 	}

// }



#include "epollserver.h"
#include "pthread.h"
#include "malloc.h"
#include "ossocket.h"

#include <readline/readline.h>
#include <readline/history.h>

void TestList(struct ep_t *ep)
{

	// struct ep_node_t *node1,*node2,*node3,*pnode;
	// struct list_head *pos,*n;
	// //ep_Interface(&ep, 2);
	// node1 = (struct ep_node_t *)malloc(sizeof(struct ep_node_t));
	// node2 = (struct ep_node_t *)malloc(sizeof(struct ep_node_t));
	// node3 = (struct ep_node_t *)malloc(sizeof(struct ep_node_t));

	// // node1->fd = 1;
	// // node2->fd = 2;
	// // node3->fd = 3;

	// ep_AddClient(ep,node1);
	// ep_AddClient(ep,node2);
	
	// ep_AddClient(ep,node3);
	// printf("node bef cnt %d\n",ep->node_cnt);
	// ep_DelClient(ep,node2);

	
	// list_for_each_safe(pos, n, &ep->node_head) {
	// 	pnode = (struct ep_node_t*)list_entry(pos,struct ep_node_t,list);
	// 	printf("node %d\n",pnode->fd);
	// }
	
	// printf("node aft cnt %d\n",ep->node_cnt);

	

}

#include "signal.h"

void sig_handler(int sig)
{
	printf("hello %d\n",sig);
}


struct ep_t ep;
extern int ep_Callback(struct ep_t *pep);
int main(int argc, char const *argv[])
{
	// 创建epoll server对象，版本号目前随意
	ep_Interface(&ep, 2);


	// 定义回调函数，各种回调函数在ep_app.c里定义
	ep_Callback(&ep);
	
	// signal(SIGINT, sig_handler);
	// 开放监听端口，需要关闭套接字调用ep_StopServer
	// 如果不做服务器可以去除ep_Listen

#ifdef TARGET_ARMV7
	if(ep_Listen(&ep,6001)) {
		return 0;
	}
#else
	if(ep_Listen(&ep,6000)) {
		return 0;
	}
#endif
	


	// 创建若干后台线程运行epoll server服务，管理所有套接字，
	// 包括新连接的建立、接收数据，函数回调
	// ep_CreateThread(&ep,0);
	ep_RunServer(&ep);
	// sleep(1);
	// ep_StopServer(&ep);


	// if(ep_Listen(&ep,6000)) {
	// 	return 0;
	// }
	// ep_RunServer(&ep);
	//ep_Listen(&ep,6000);

	// 跳转到控制台接受用户命令，QT用GUI代替控制台
	sh_enter();


	// 停止服务
	ep_StopServer(&ep);


	// 释放epoll server对象，与ep_Interface相对应
	ep_Release(&ep);

	
	return 0;
}