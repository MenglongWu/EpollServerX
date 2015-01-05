/**
 ******************************************************************************
 * @file	
 * @brief	
 *		TODO:introduce 
 *
 *--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0			Menglong Wu		20xx-xx-xx		1.xxxxx
 *****************************************************************************
*/
#ifndef _MINI_SHELL_CORE_H_
#define _MINI_SHELL_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif



#if !defined(CMD_ARRAY) && !defined(CMD_SECTION) && !defined(CMD_LDS)
	#define CMD_ARRAY
 	// #define CMD_SECTION
#endif
#if defined(CMD_ARRAY) && (defined(CMD_SECTION) || defined(CMD_LDS))
	#error "select CMD_ARRAY,CMD_SECTION,CMD_LDS only one"
#elif (defined(CMD_ARRAY) || defined(CMD_SECTION)) && defined(CMD_LDS)
	#error "select CMD_ARRAY,CMD_SECTION,CMD_LDS only one"
#elif defined(CMD_SECTION) && (defined(CMD_ARRAY) || defined(CMD_LDS))
	#error "select CMD_ARRAY,CMD_SECTION,CMD_LDS only one"
#endif

struct cmd_prompt
{
	char *name;
	char *help;
	struct cmd_prompt	*next;
};

struct cmd_table
{
	char *name;
	// int (*fun)(char **argv,int argc);
	int (*fun)(int argc,char **argv);
	char *help;
};
#define SHALL_PATH_LEN 96
struct env{
	char host[10];
	char path[SHALL_PATH_LEN];
};

extern struct env g_envLocal;
extern void sh_init();
extern int sh_enter();
extern void sh_editpath(char *path);




extern volatile int __wcmd_start;
extern volatile int __wcmd_end;

//W_BOOT_CMD
#ifdef CMD_SECTION
	#define W_BOOT_CMD(name,cmd,help) \
	volatile struct cmd_table   __w_boot_cmd_##name __attribute__ ((unused,section (".w_boot_cmd"))) = {(char*)#name,cmd,(char*)help}
#else
	#define W_BOOT_CMD(name,cmd,help) 
#endif




// #if (defined(CMD_ARRAY) || defined(CMD_SECTION)) && defined(CMD_LDS)
// 	#error "select CMD_ARRAY,CMD_SECTION,CMD_LDS only one"
// #endif
// #if defined(CMD_SECTION) && (defined(CMD_ARRAY) || defined(CMD_LDS))
// 	#error "select CMD_ARRAY,CMD_SECTION,CMD_LDS only one"
// #endif

//MINISHELL_START\MINISHELL_END
#ifdef CMD_ARRAY
	#define MINISHELL_START(p) (p = &cmd_tbl_list[0])
	#define MINISHELL_END(p) (p = &cmd_tbl_list[0] + sizeof(cmd_tbl_list)/sizeof(struct cmd_table) - 1)
#endif

#ifdef CMD_SECTION
	#define MINISHELL_START(p) (p = (struct cmd_table*)((char*)&__wcmd_start+sizeof(int)))
	#define MINISHELL_END(p) (p = (struct cmd_table*)((char*)&__wcmd_end  -sizeof(int)))
#endif 

#ifdef CMD_LDS //define __w_boot_cmd_start\__w_boot_cmd_end in *.lds file
	#define MINISHELL_START(p) (&__w_boot_cmd_start)
	#define MINISHELL_END(p)   (&__w_boot_cmd_end)
#endif


#ifdef __cplusplus
}
#endif

#endif

