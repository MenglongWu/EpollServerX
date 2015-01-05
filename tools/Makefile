# include ../rule.mk
include $(TOPDIR)/config_app_file_list.mk


TOOL_OBJS = 	$(patsubst %.S,%.o,\
		$(patsubst %.cpp,%.o,\
		$(patsubst %.c,%.o,$(TOOL_SRCS))))

all:
	echo $(TOOL_OBJS)
	echo $(TOPDIR)
	@$(CC) -o deamon.elf daemon.c $(CC_FLAGS) -I/usr/include/i386-linux-gnu

	