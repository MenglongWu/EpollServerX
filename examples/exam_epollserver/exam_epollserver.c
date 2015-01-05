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





#include "epollserver.h"


#include "signal.h"

struct ep_t ep;
struct ep_con_t client;


void sig_handler(int sig)
{
	printf("hello %d\n",sig);
}


int ShowInfoServer()
{
	struct list_head *pos,*n;
	struct ep_con_t *pnode;
	int index = 1;


	printf("%-4s%8s%16s%24s\n","Index","FD","locate","Remote");
	list_for_each_safe(pos, n, &ep.node_head) {
		pnode = list_entry(pos,struct ep_con_t,list);
		printf("%-4d%8d%16s:%-8d",
		 	index++,
			pnode->sockfd,
			inet_ntoa(pnode->loc_addr.sin_addr),
			htons(pnode->loc_addr.sin_port));
		printf("%16s:%-8d\n",
			inet_ntoa(pnode->rem_addr.sin_addr),
			htons(pnode->rem_addr.sin_port));
	}
	return 0;
}



// 因为是演示，所以为了保证各个线程完成所有任务，所以加入sleep延时
// 正式代码里没有加延时的必要

extern int ep_Callback(struct ep_t *pep);

int InitServer()
{
	printf("\n**********************************************\n");
	printf("Run server\n");
	// 创建epoll server对象，版本号目前随意
	ep_Interface(&ep, 2);


	// 定义回调函数，各种回调函数在ep_app.c里定义
	ep_Callback(&ep);
	
	// signal(SIGINT, sig_handler);
	// 开放监听端口，需要关闭套接字调用ep_StopServer
	// 如果不做服务器可以去除ep_Listen


	if(ep_Listen(&ep,6000)) {
		return 0;
	}



	// 创建若干后台线程运行epoll server服务，管理所有套接字，
	// 包括新连接的建立、接收数据，函数回调
	// ep_CreateThread(&ep,0);
	ep_RunServer(&ep);

	// 显示有多少个socket设备文件，此时只有一个设备号为4
	ShowInfoServer();
	sleep(0);
	return 0;
}
int ConnectSelf()
{
	printf("\n**********************************************\n");
	printf("Client connect\n");
	// 连接服务器
	
	char addr[] = "127.0.0.1";
	unsigned short port = 6000;

	// 自己连接自己
	if (0 == ep_Connect(&ep,&client, addr, port)) {
		printf("fd:%d ip:%s port:%d\n", 
			client.sockfd,
			inet_ntoa(client.loc_addr.sin_addr),
			htons(client.loc_addr.sin_port));	
	}
	else {
		return 0;
	}
	// 稍作等待
	// 显示有多少个socket设备文件，此时多了设备号5和6，如果不延时只能看到设备5
	sleep(3);
	ShowInfoServer();
	return 0;
}

int SendData()
{
	printf("\n**********************************************\n");
	printf("Send data\n");
	char data[] = "Hello KuGou\n";
	int ret;

	ret = send(client.sockfd, data, sizeof(data), 0);
	printf("send data %s",data);
	sleep(3);
	return 0;
}

int CloseServer()
{
	printf("\n**********************************************\n");
	printf("Stop server\n");
	// 停止服务
	// ep_StopServer未做同步，会导致其他线程访问已释放的内存，稍作延时，保证其他线程无任务
	// 后期会将BUG补上
	sleep(3);
	ep_StopServer(&ep);
	// sleep(5);

	// 释放epoll server对象，与ep_Interface相对应
	// ep_Release未做同步，会导致其他线程访问已释放的内存，稍作延时，保证其他线程无任务
	// 后期会将BUG补上
	sleep(3);
	ep_Release(&ep);
	return 0;
}
int main(int argc, char const *argv[])
{	
	InitServer();
	ConnectSelf();
	SendData();
	CloseServer();
	return 0;
}