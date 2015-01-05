#include "epollserver.h"
#include <stdio.h>


int epFUI_OnAccept(struct ep_t *pep, struct ep_con_t *pnode)
{
	printf("----OnAccept----\n");
	return 0;
}

int epFUI_OnRecv(struct ep_t *pep, struct ep_con_t *pnode)
{
	printf("----OnRecv----\n");
	int ret;
	char rbuf[100];
	ret = recv(pnode->sockfd, rbuf, sizeof(rbuf), 0);
	printf("recv data %s",rbuf);
	// 返回值必须是recv函数的返回值
	return ret;
}

int epFUI_OnError(struct ep_t *pep, struct ep_con_t *pnode)
{
	printf("----OnError----\n");
	return 0;
}

int epFUI_OnRemoveClose(struct ep_t *pep, struct ep_con_t *pnode)
{
	printf("----OnRemoveClose----\n");
	return 0;
}

void ep_Callback(struct ep_t *pep)
{
	pep->pFUI_OnAccept      = epFUI_OnAccept;
	pep->pFUI_OnRecv        = epFUI_OnRecv;
	pep->pFUI_OnError       = epFUI_OnError;
	pep->pFUI_OnRemoveClose = epFUI_OnRemoveClose;
}