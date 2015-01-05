/**
 ******************************************************************************
 * @file	minishell_core.c
 * @brief	MiniShell 核心代码，提供命令行操作，不提供正则表达式

舍弃最初版本里用SHELL_REG_CMD定义数组cmd_tbl_list命令的定义，改成同uboot一样
用W_BOOT_CMD(name,fun,help)三个参数
 *
 @section History
|     Version    |     Author        |      Date      |    Content   
| :------------: | :---------------: | :------------: | :------------
|     V1.0       |    Menglong Wu    |   2014-11-11   | 1.基础接口do_help、sh_init、sh_analyse、sh_enter
|     V1.1.0     |    Menglong Wu    |   2014-12-08   | 1.采用uboot的定义命令方式
|                |                   |                | 2.添加命令排序
 
 @section Platform
	-# Linux 2.6.35-22-generic #33-Ubuntu SMP Sun Sep 19 20:34:50 UTC 2010 i686 GNU/Linux
	-# gcc-4.7.4 gcc/g++
 @section Library
	-# [libreadline.so.5.2]
		-# [libhistory.so.5]
		-# [libncurses.so.5.5]

	-# [libpthread.so.0]
	-# [libc.so.6]
- 2015-4-3,Menglong Wu,DreagonWoo@163.com
 	- Add sh_editpath
 	- Add
 * @attention
 *
 * ATTENTION
 *
 * <h2><center>&copy; COPYRIGHT </center></h2> 
*/
#ifdef __cplusplus
extern "C" {
#endif
 

#include "minishell_core.h"
// #include "board.h"
// #include "linux/string.h"
#include "shell.h"
#include "stdio.h"
#include "malloc.h"
#include <readline/readline.h>
#include <readline/history.h>



int do_help(int argc,char **argv);
int do_null(int argc,char **argv);


#ifdef CMD_SECTION
volatile int __wcmd_start __attribute__ ((section (".w_boot_start"))) = 0;

W_BOOT_CMD(help,do_help,"shell help");
volatile int __wcmd_end   __attribute__ ((section (".w_boot_end"))) = 0;
#endif

//*****************************************************************************
//命令列表

#ifdef CMD_ARRAY
int do_help(int argc,char **argv);

int cmd_Server(int argc,char **argv);
int cmd_Connect(int argc,char **argv);
int cmd_InfoServer(int argc,char **argv);
static struct cmd_table cmd_tbl_list[] = 
{
	INIT_CMD,
	{(char*)"help",do_help,(char*)"shell help"},
	{(char*)"quit",do_null,(char*)"quit shell"},
	// {"server",cmd_Server,"dsdsf"},
	// {"inf",cmd_InfoServer,"dsdsf"},
	// {"connect",cmd_Connect,"dsdsf"},
	
	// SHELL_REG_CMD(0,0,0),
	{0,0,0},
};
#endif

struct env g_envLocal;


//*****************************************************************************
//默认命令
#if(1) //SHELL_REG_CMD宏才用到该函数
int do_null(int argc,char **argv)
{
	return 0;
}
#endif
/**
 * @brief	Mini Shell自带命令，输出命令帮助
 */


W_BOOT_CMD(null,do_null,"shell help");

int do_help(int argc,char **argv)
{


	struct cmd_table *pstart;//= (struct cmd_table*)((char*)&__wcmd_start+sizeof(int));
	struct cmd_table *pend;// = (struct cmd_table*)((char*)&__wcmd_end-sizeof(int));


	// MINISHELL_START(pstart);
	// MINISHELL_END(pend);

	// printf("%8.8x %8.8x\n",(int)pstart,(int)pend);

	// while(pstart < pend) {
	// 	printf("%8.8x~%8.8x %s %s\n",(unsigned int)pstart,(unsigned int)pend,pstart->name,pstart->help);
	// 	pstart++;
	// }


	MINISHELL_START(pstart);
	MINISHELL_END(pend);

	
	// pstart = &__w_boot_cmd_start;
	while((pstart < pend)) {
		printf("  %-12s\t",pstart->name);
		printf("%s\r\n",pstart->help);
		pstart++;
	}
	return 0;
	
}


/**
 * @brief	Mini Shell自带命令，修改命令提示符名
 */
int do_hostname(int argc,char **argv)
{
	if(argc != 2) {
	 	puts("Usage:\n");
	 	puts("\thostname <name>\n");
	}
	else {
		int len;
		len = strlen(argv[1]);
		if (len > 10) {
			len = 10;
		}
		memcpy(g_envLocal.host, argv[1], len);
		g_envLocal.host[9] = '\0';
	}
	return 0;	
}



//*****************************************************************************
//核心代码
#if(0)
void sh_init()
{
	struct cmd_table *pstart;
	struct cmd_table *pend;

	pstart = &__w_boot_cmd_start;
	pend   = &__w_boot_cmd_end;

	
}
#endif

/**
 * @brief	按照字母顺序排序编译后生成的Mini Shell命令
 */

void sh_sort()
{
	struct cmd_table *pstart;
	struct cmd_table *pend;
	struct cmd_table tmp,*pmin,*pfind;
	
	// pstart = &__w_boot_cmd_start;
	// pend   = &__w_boot_cmd_end;
	MINISHELL_START(pstart);
	MINISHELL_END(pend);
	

	
	// for(pstart;pstart < pend;pstart++) {
	while(pstart < pend) {
		pmin = pstart;

		for(pfind = pstart;  pfind < pend;pfind++) {
			if(strcmp(pmin->name,pfind->name) > 0) {
				pmin = pfind;
			}
		}
		printf("%s\n",pstart->name);
		tmp     = *pmin;
		*pmin    = *pstart;
		*pstart = tmp;
	}	
}



/**
 * @brief	Mini Shell分析命令，找到命令则执行
 * @param	fmt 若干个"  ,\t\n"分隔的字符串
 * @param	len fmt字符串长度
 * @remark	fmt中所有字符串总数不得超过30个
 */


void sh_analyse (char *fmt,long len)
{
	//char (*cmd)[10];
	char *cmd[256],*token = NULL;	
	unsigned int count = 0;
	char seps[]   = " ,\t\n";

	struct cmd_table *pstart;
	struct cmd_table *pend;
	int find = 0;

	//step 1:提取第一个单词，并在命令列表里寻找是否存在命令
	*(fmt+len) = '\0';
	token = strtok(fmt,seps);
	
	if(token != NULL) {
		cmd[count] = token;

		MINISHELL_START(pstart);
		MINISHELL_END(pend);


		
		// pstart = &__w_boot_cmd_null;
		while((pstart < pend)) {
			
			//if(0 == strcmp(cmd[0],pstart->name)) {
			if(0 == memcmp(cmd[0],pstart->name,strlen(cmd[0]))) {
				find = 1;
				break;
			}
			pstart++;
		}

		//step 2:提取参数
		if(find == 1) 
		{
			while(token != NULL) {
				cmd[count] = token;
				count++;
				token = strtok(NULL,seps);	
			}
			pstart->fun(count,(char**)cmd);
		}
		else {
			printf("%s: command not found\n",token);
		}
	}
}

void sh_editpath(char *path)
{
	int len;
	len = strlen(path);
	if (len > SHALL_PATH_LEN) {
		len = SHALL_PATH_LEN;
	}
	memcpy(g_envLocal.path, path, len);
	g_envLocal.path[SHALL_PATH_LEN-1] = '\0';
}


/**
 * @brief	Mini Shell入口函数，完成一切初始化，
 * @remark	单行命令长度不得超过256字节
 */

int sh_enter()
{
	char shell_prompt[256];
	char *input = (char*)NULL;
	// sh_sort();
	// sh_init();
	memcpy(g_envLocal.host,"MiniShell\0",10);
	g_envLocal.path[0] = '\0';
	
	// add_history(g_envLocal.host);
	// add_history("aaaaddf");
	while(1) 
	{
		
		// printf("%s>",g_envLocal.host);
		//gets_s(input,256);
		// input = fgets(shell_prompt,80,stdin);
		if(input) {
			free(input);
			input = NULL;
		}
		
		snprintf(shell_prompt, sizeof(shell_prompt),"%s:%s>",g_envLocal.host,g_envLocal.path);
		input= readline(shell_prompt);
		if(!input) {
			return -1;
		}
		if (*input != '\0') {
			add_history(input);
			// printf("add\n");

			// printf("bef analy\n");
			// sh_analyse(input,256);
			sh_analyse(input,strlen(input));
			// printf("aft analy\n");
			
			
			
			if(0 == strcmp(input,"quit")) {
				printf("\r\n");
				break;
			}	
		}
	}

	return 0;
}



W_BOOT_CMD(hostname,do_hostname,"set hostname");

#ifdef __cplusplus
}
#endif