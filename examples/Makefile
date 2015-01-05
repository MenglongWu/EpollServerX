
CFLAGS = -std=gnu99 -D_DEBUG -g

INCLUDE_DIR	= -I../include -I../osnet -I../shell -I../protocol

# 链接目录必须注意，x86的库安装在/usr/local/install/lib
# 4412的链接目录安装在/usr/4412/install/lib，需要做相应修改
LIB_DIR 	+= -L/usr/local/install/lib
#LIB_DIR 	+= -L/usr/4412/install/lib


all:bipbuffer tr485 epoll
bipbuffer:
	@# 测试bipbuffer
	gcc -o exam_bipbuffer.elf $(CFLAGS) $(INCLUDE_DIR) \
	 			exam_bipbuffer.c \
	 			../osnet/bipbuffer.c 
tr485:
	@# 测试tr485
	gcc -o exam_tr485.elf $(CFLAGS) $(INCLUDE_DIR) \
				exam_tr485.c \
				../protocol/tr485.c \
				../protocol/tr485_hw.c \
				../osnet/bipbuffer.c 

epoll:
	@# 测试epollserver
	gcc -o exam_epollserver.elf $(CFLAGS) $(INCLUDE_DIR) $(LIB_DIR) -lpthread  \
			exam_epollserver/ep_examapp.c \
			exam_epollserver/exam_epollserver.c \
			../osnet/epollserver.c \
			../osnet/ossocket.c
clean:
	rm *.elf
	rm *.o
	