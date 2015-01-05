#
# Makefile Template for "Application Program"
#
# Description:
#	Automatic load file config list "config_xxx_file_list.mk"
#	which will be complie,when use this Makefile Template,
#	you only edit the config_xxx_file_list.mk
#
#
# Author: Menglong Wu
# E-mail:DreagonWoo@163.com
#
#
# V0.1		2014-08-23
#		2 demo makefile AppMakefile.mk and LibMakefile.mk
#

#################################################################
# Demo directory struction
# | Makefile
# 	-main.c
# |-dir1
# 	-f1_1.c
# 	-f1_2.c
# |-dir1
# 	|dir11
#		-f11_1.c
# |-dir2
# 	-f2_1.c
# between "----" and "----" copy to your config_xxx_file_list.mk and edit.
#---------------------------------------------------------------
# _NAME_		= any name 
# DIR_$(_NAME_) 	= directory name
# SRCS_$(_NAME_) 	= file list in the directory
# SRCS =  $(foreach TMP, \
# 	$(SRCS_$(_NAME_)), \
# 	$(DIR_$(_NAME_))/$(TMP))
#---------------------------------------------------------------




#################################################################
# CROSS_COMPILE		- While the cross tool link
# ARCH				- Target platform
#ARCH=x86
DEV=MCU
ifeq ("$(ARCH)", "")
	ARCH=x86
endif

ifeq ("$(ARCH)", "arm920t")
	CROSS_COMPILE	=/opt/EmbedSky/crosstools_3.4.5_softfloat/gcc-3.4.5-glibc-2.3.6/arm-linux/bin/arm-linux-
endif

ifeq ("$(ARCH)", "armv7")
	CROSS_COMPILE	=/opt/iTop-4412/4.3.2/bin/arm-linux-
endif

ifeq ("$(ARCH)", "i586")
	CROSS_COMPILE	=i586-mingw32msvc-
endif

ifeq ("$(DEV)", "")
	DEV=MCU
endif

ifeq ("$(DEV)", "M")
	CFLAGS =-D_MANAGE
endif


#################################################################
# select which file be complie,it edit in config_app_file.mk
# Import all files,it edit in config_xxx_file_list.mk
include config_app_file.mk
include config_app_file_list.mk

export TOPDIR = $(shell pwd)






#################################################################
OBJS = 	$(patsubst %.S,%.o,\
		$(patsubst %.cpp,%.o,\
		$(patsubst %.c,%.o,$(SRCS))))

TOOL_OBJS = 	$(patsubst %.S,%.o,\
		$(patsubst %.cpp,%.o,\
		$(patsubst %.c,%.o,$(TOOL_SRCS))))

#################################################################
# Output files name and directory
NAME_ELF	= download.elf
NAME_DIS	= download.dis
NAME_BIN	= download.bin
OUTPUT_DIR	= release
#################################################################




#################################################################
# INCLUDE_DIR	- Where will be search *.h file
# LFLAGS		- Linking option
# LIB_DIR		- Where will be search *.so/*.a file
#-Wl,-rpath=./:./lib/

#when app.elf run will select *.so/a from $(PATH) -> ./ -> ./lib/
INCLUDE_DIR	= -I./include -I./osnet -I./shell -I./ -I./src -I/usr/include/readline
LFLAGS		= -lreadline -lpthread -lhistory -lncurses 
LIB_DIR 	= 
ifeq ("$(ARCH)", "x86")
	INCLUDE_DIR	+= 
	LFLAGS		+= -ltermcap  
	# -lefence 
	LIB_DIR 	+= -L/usr/local/install/lib
	CFLAGS		+= -DTARGET_X86
endif

ifeq ("$(ARCH)", "armv7")
	INCLUDE_DIR	+= -I/usr/4412/install/include
	LFLAGS		+= 
	LIB_DIR 	+= -L/usr/4412/install/lib
	CFLAGS		+= -DTARGET_ARMV7
endif


ifeq ("$(ARCH)", "arm920t")
	INCLUDE_DIR	+= -I/usr/arm920t/install/include
	LFLAGS		+= 
	LIB_DIR 	+= -L/usr/arm920t/install/lib
	CFLAGS		+= -DTARGET_ARM920T
endif


ifeq ("$(ARCH)", "i586")
	INCLUDE_DIR	+= -I/usr/win32/install/include
	LFLAGS		+= 
	LIB_DIR 	+= -L/usr/win32/install/lib
	CFLAGS		+= 
endif
#################################################################



ifeq ("$(OUTPUT_DIR)", "")
	OUTPUT_DIR=debug
endif

ifeq ("$(CROSS_COMPILE)", "")
else
endif


GCC_G++ = gcc
CC 	= $(CROSS_COMPILE)$(GCC_G++)
LD 	= $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy

#################################################################
# CFLAGS		- Compile general option
# CC_FLAGS		- Compile only for *.c file option
# CS_FLAGS		- Compile only for *.S file option
CFLAGS		+= -g  	 -Wall -static -rdynamic -D_UNUSE_QT_  -fshort-wchar 
ifeq ("$(GCC_G++)","gcc") # 只有gcc编译器才使用该选项，g++无此选项
	CC_FLAGS    = -std=gnu99
else
	CC_FLAGS    = 
endif
CS_FLAGS    = 


CC_FLAGS   += $(CFLAGS)
CS_FLAGS   += $(CFLAGS)

FF=sdf
MAKE := make -r -R -s
all:$(ARCH)
#################################################################
# which ARCH

tool:$(OUTPUT_DIR) 
	# make -C ./tools
	make -s -C ./osnet
	# echo $(TOPDIR)
	# @$(CC) -o ./tools/randmac.elf ./tools/randmac.c $(LIB_DIR) $(LFLAGS)
	# @$(CC) -o ./tools/usbcheck.elf ./tools/usbcheck.c $(LIB_DIR) $(LFLAGS)
	@$(CC) -o ./tools/deamon.elf -g  ./tools/daemon.c $(CC_FLAGS) -I/usr/include/i386-linux-gnu
	@$(CC) -o ./tools/deamon3.elf -g  -O3 ./tools/daemon.c $(CC_FLAGS) -I/usr/include/i386-linux-gnu
	objdump -S ./tools/deamon.elf > out.dis
	objdump -S ./tools/deamon3.elf > out3.dis
	# @$(CC) -o ./tools/2win.elf ./tools/2win.c  -I/usr/include/i386-linux-gnu -L/usr/lib/i386-linux-gnu/ -lcurses
	@# @$(CC) -o ./src/sq.elf ./src/sqlite_app.c $(LIB_DIR) $(LFLAGS) $(INCLUDE_DIR)
	@# $(MAKE) -C ./tools 
	@# @$(CC) -o ./tools/randmac.elf $(TOOL_OBJS) $(LIB_DIR) $(LFLAGS)
x86:$(OUTPUT_DIR) $(OBJS) 
	@echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@$(CC) -o $(OUTPUT_DIR)/$(NAME_ELF) $(OBJS) $(LIB_DIR) $(LFLAGS)

	@# @echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@# @$(LD) -Tx86.lds $(OBJS) -o $(OUTPUT_DIR)/$(NAME_ELF) $(LFLAGS) $(LIB_DIR)  
	
	@# @-echo create $(OUTPUT_DIR)/$(NAME_BIN)
	@# @-$(OBJCOPY) -O binary -S $(OUTPUT_DIR)/$(NAME_ELF) $(OUTPUT_DIR)/$(NAME_BIN)


	@echo create $(OUTPUT_DIR)/$(NAME_DIS)
	@$(OBJDUMP) -S $(OUTPUT_DIR)/$(NAME_ELF) > $(OUTPUT_DIR)/$(NAME_DIS)

armv7:$(OUTPUT_DIR) $(OBJS)
	@echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@$(CC) -o $(OUTPUT_DIR)/$(NAME_ELF) $(OBJS) $(LIB_DIR) $(LFLAGS)

	@# @echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@# @$(LD) -Tx86.lds $(OBJS) -o $(OUTPUT_DIR)/$(NAME_ELF) $(LFLAGS) $(LIB_DIR)  
	
	@# @-echo create $(OUTPUT_DIR)/$(NAME_BIN)
	@# @-$(OBJCOPY) -O binary -S $(OUTPUT_DIR)/$(NAME_ELF) $(OUTPUT_DIR)/$(NAME_BIN)


	@echo create $(OUTPUT_DIR)/$(NAME_DIS)
	@$(OBJDUMP) -S $(OUTPUT_DIR)/$(NAME_ELF) > $(OUTPUT_DIR)/$(NAME_DIS)

i586:$(OUTPUT_DIR) $(OBJS)
	@echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@$(CC) -o $(OUTPUT_DIR)/$(NAME_ELF) $(OBJS) $(LIB_DIR) $(LFLAGS)

	@# @echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@# @$(LD) -Tx86.lds $(OBJS) -o $(OUTPUT_DIR)/$(NAME_ELF) $(LFLAGS) $(LIB_DIR)  
	
	@# @-echo create $(OUTPUT_DIR)/$(NAME_BIN)
	@# @-$(OBJCOPY) -O binary -S $(OUTPUT_DIR)/$(NAME_ELF) $(OUTPUT_DIR)/$(NAME_BIN)


	@echo create $(OUTPUT_DIR)/$(NAME_DIS)
	@$(OBJDUMP) -S $(OUTPUT_DIR)/$(NAME_ELF) > $(OUTPUT_DIR)/$(NAME_DIS)


arm920t:$(OUTPUT_DIR) $(OBJS)
	@echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@$(CC) -o $(OUTPUT_DIR)/$(NAME_ELF) $(OBJS) $(LIB_DIR) $(LFLAGS)

	@# @echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@# @$(LD) -Tx86.lds $(OBJS) -o $(OUTPUT_DIR)/$(NAME_ELF) $(LFLAGS) $(LIB_DIR)  
	
	@# @-echo create $(OUTPUT_DIR)/$(NAME_BIN)
	@# @-$(OBJCOPY) -O binary -S $(OUTPUT_DIR)/$(NAME_ELF) $(OUTPUT_DIR)/$(NAME_BIN)


	@echo create $(OUTPUT_DIR)/$(NAME_DIS)
	@$(OBJDUMP) -S $(OUTPUT_DIR)/$(NAME_ELF) > $(OUTPUT_DIR)/$(NAME_DIS)

arm9220t:$(OUTPUT_DIR) $(OBJS) 

	@echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@$(LD) -Tboot.lds $(OBJS) -o $(OUTPUT_DIR)/$(NAME_ELF) $(LFLAGS) $(LIB_DIR)  
	
	@echo create $(OUTPUT_DIR)/$(NAME_BIN)
	@$(OBJCOPY) -O binary -S $(OUTPUT_DIR)/$(NAME_ELF) $(OUTPUT_DIR)/$(NAME_BIN)


	@echo create $(OUTPUT_DIR)/$(NAME_DIS)
	@$(OBJDUMP) -S $(OUTPUT_DIR)/$(NAME_ELF) > $(OUTPUT_DIR)/$(NAME_DIS)
#################################################################


%.o:%.c
	@echo compile $^
	@$(CC) -o $@ -c $^ $(CC_FLAGS) $(INCLUDE_DIR) 
%.o:%.cpp
	@echo compile $^
	@$(CC) -o $@ -c $^ $(CC_FLAGS) $(INCLUDE_DIR) 
%.o:%.S
	@echo compile $^
	@$(CC) -o $@ -c $^ $(CS_FLAGS) $(INCLUDE_DIR)

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)
clean:
	@-rm 	$(OBJS) \
		$(TOOL_OBJS) \
		$(OUTPUT_DIR)/$(NAME_DIS) \
		$(OUTPUT_DIR)/$(NAME_ELF) \
		core
		# $(OUTPUT_DIR)/$(NAME_BIN)
rmoutput:$(OUTPUT_DIR)
	@-rm -rf $(OUTPUT_DIR)
rmdb:
	@-rm /etc/tmsxx.db
sqlite3:
	sqlite3 /etc/tmsxx.db
run:
	./$(OUTPUT_DIR)/$(NAME_ELF)
copy:
	cp ./$(OUTPUT_DIR)/$(NAME_ELF) /usr/armdebug/tms4412.elf
gdb:
	gdb ./$(OUTPUT_DIR)/$(NAME_ELF)
gdb-core:
	gdb ./$(OUTPUT_DIR)/$(NAME_ELF) core
