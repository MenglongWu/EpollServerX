#include <stdio.h> 
#include <time.h> 
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include <string.h>

#include <sys/unistd.h> 
#include <signal.h> 
#include <sys/param.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#define warn(...) printf(__VA_ARGS__)
void writelog(char *str)
{
	time_t t; 
		FILE *fp; 
	if((fp=fopen("test.log","a")) >=0) 
		{ 
			t=time(0); 
			fprintf(fp,"Im here at %s %s/n",asctime(localtime(&t)) ,str); 
			fclose(fp); 
		} 
}
void init_daemon(void) 
{ 
	int pid; 
	int i; 
	if(pid=fork()) {
		writelog("im father fork() exit(0)\n");
		exit(0);//是父进程，结束父进程 
	}
	else if(pid< 0) {
		writelog("im father fork() fail\n");
		exit(1);//fork失败，退出 
	}
	//是第一子进程，后台继续执行 
	setsid();//第一子进程成为新的会话组长和进程组长 
	//并与控制终端分离 
	if(pid=fork()) {
		writelog("first exit()\n");
		exit(0);//是第一子进程，结束第一子进程 
	}
	else if(pid< 0) {
		writelog("first exit() fail\n");
		exit(1);//fork失败，退出 
	}
	//是第二子进程，继续 
	//第二子进程不再是会话组长 

	for(i=0;i< NOFILE;++i) {//关闭打开的文件描述符 
		// writelog("how many NOFILE\n");
		close(i); 
	}
	chdir("/tmp");//改变工作目录到/tmp 
	umask(0);//重设文件创建掩模 
	return; 
} 
// 2． test.c清单 


void init_daemon(void);//守护进程初始化函数 
int k;
void fun_unuse(long t)
{
	long i;
	i = t;
	while(i--);
}

void fun(long t)
{
	long i;
	i = t;
	while(i--);
}

void fun_vol(long t)
{
	volatile long i;
	i = t;
	while(i--);
}

void fun_vol_unuse(long t)
{
	volatile long i;
	i = t;
	while(i--);
}

void fun_k(long t)
{
	volatile long i;
	i = t;
	while(i--);
	k = t;
}

void fun_k_unuse(long t)
{
	volatile long i;
	i = t;
	while(i--);
	k = t;
}


main() 
{ 
	int c;
	scanf("%d",&c);
	// fun_vol(2);
	// fun(2);
	// fun_k(2);
	// printf("k = %d\n",k);

	warn("s444df");

	return 0;
	chdir("/tmp");//改变工作目录到/tmp 
	FILE *fp; 
	time_t t; 
	init_daemon();//初始化为Daemon 
	
	while(1)//每隔一分钟向test.log报告运行状态 
	{ 
		sleep(60);//睡眠一分钟 
		if((fp=fopen("test.log","a")) >=0) 
		{ 
			t=time(0); 
			fprintf(fp,"Im here at %s/n",asctime(localtime(&t)) ); 
			fclose(fp); 
		} 
	} 
} 
