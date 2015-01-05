# # 最小系统代码
SRCS += shell/minishell_core.c 

SRCS += src/main.c 
SRCS +=	osnet/bipbuffer.c 
SRCS +=	osnet/epollserver.c 
SRCS +=	osnet/ossocket.c 
SRCS +=	src/ep_app.c 
SRCS +=	src/tms_app.c 
SRCS +=	src/tmsxxdb.c 	
		
		

SRCS +=shell/cmd/cmd_server.c \
		shell/cmd/cmd_tr485.c 
		

SRCS +=protocol/glink.c \
		protocol/tmsxx.c \
		shell/cmd/cmd_tmsxx.c \
		protocol/MD5.cpp

TOOL_SRCS += tools/randmac.c

LIB_EPOLLSERVER = osnet/bipbuffer.o osnet/epollserver.o osnet/ossocket.o


OUT_LIBS = $(LIB_EPOLLSERVER)

# SRCS +=protocol/tr485.c \
# 		protocol/tr485_hw.c 

# 最小系统代码
# SRCS +=  \
# 		osnet/bipbuffer.c \
# 		osnet/TMSepollserver.c \
# 		osnet/ossocket.c \
		# src/ep_app.c
