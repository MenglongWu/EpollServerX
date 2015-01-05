#ifndef _MINI_SHELL_H_
#define _MINI_SHELL_H_

// int do_setenv(char **argv,int argc);
// int do_saveenv(char **argv,int argc);
// int do_help(int argc,char **argv);
// int do_null(int argc,char **argv);
// int do_hostname(int argc,char **argv);

// extern int do_sector(int argc,char **argv);
// extern int do_erase(int argc,char **argv);
// extern int do_writepage(int argc,char **argv);
// extern int do_readpage(int argc,char **argv);
// extern int do_nandscan(int argc,char **argv);

// extern int do_format(int argc,char **argv);
// extern int do_ls(int argc,char **argv);
// extern int do_mkdir(int argc,char **argv);
// extern int do_rm(int argc,char **argv);
// extern int do_wfile(int argc,char **argv);
// extern int do_rfile(int argc,char **argv);

// extern int do_copy_sdram2nand(int argc,char **argv);
// extern int do_file2nand(int argc,char **argv);
// extern int do_mount(int argc,char **argv);
#include "minishell_core.h"

int cmd_InfoServer(int argc,char **argv);
int cmd_Connect(int argc,char **argv);
int cmd_Close(int argc,char **argv);


int cmd_listfile(int argc, char **argv);
int cmd_tmsall(int argc,char **argv);
int cmd_tmsscan(int argc,char **argv);
int cmd_intface(int argc,char **argv);
int cmd_sudo(int argc,char **argv);
int cmd_Disp(int argc,char **argv);
int cmd_sql(int argc, char **argv);
int cmd_select(int argc, char **argv);
int cmd_delete(int argc, char **argv);
int cmd_insert(int argc, char **argv);
int cmd_im(int argc, char **argv);
int cmd_remotecmd(int argc, char **argv);
#define INIT_CMD \
		{(char*)"inf",cmd_InfoServer,(char*)"shell help"}, \
		{(char*)"connect",cmd_Connect,(char*)"shell help"}, \
		{(char*)"close",cmd_Close,(char*)"shell help"}, \
		\
		{(char*)"sql",cmd_sql,(char*)"call sqlite3 console"}, \
		{(char*)"select",cmd_select,(char*)"select tmsxxdb"}, \
		{(char*)"delete",cmd_delete,(char*)"delete tmsxxdb"}, \
		{(char*)"insert",cmd_insert,(char*)"insert tmsxxdb"}, \
		{(char*)"im",cmd_im,(char*)"shell help"}, \
		{(char*)"r",cmd_remotecmd,(char*)"shell help"}, \
		{(char*)"tms",cmd_tmsall,(char*)"shell help"}, \
		{(char*)"lf",cmd_listfile,(char*)"shell help"}, \
		{(char*)"scan",cmd_tmsscan,(char*)"shell help"}, \
		{(char*)"interface",cmd_intface,(char*)"shell help"}, \
		{(char*)"sudo",cmd_sudo,(char*)"shell help"}, \
		{(char*)"display",cmd_Disp,(char*)"shell help"} 


		
// {"inf",NULL,},
#endif

