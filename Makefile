ROOTDIR=$(shell pwd)
WORKDIR=$(ROOTDIR)/build

targets	 += alink

.PHONY: targets

all : $(targets)


srcs							:= $(ROOTDIR)/main.c

srcs							+= $(ROOTDIR)/src/ayla/log.c
srcs							+= $(ROOTDIR)/src/ayla/lookup_by_name.c
srcs							+= $(ROOTDIR)/src/ayla/timer.c
srcs							+= $(ROOTDIR)/src/ayla/time_utils.c
srcs							+= $(ROOTDIR)/src/ayla/assert.c
srcs							+= $(ROOTDIR)/src/ayla/file_event.c
srcs							+= $(ROOTDIR)/src/ayla/file_io.c
srcs							+= $(ROOTDIR)/src/ayla/hashmap.c
srcs							+= $(ROOTDIR)/src/ayla/parse_argv.c
srcs							+= $(ROOTDIR)/src/ayla/hex.c

srcs							+= $(ROOTDIR)/src/platform/system.c

srcs							+= $(ROOTDIR)/product/alink/src/zha.c
srcs							+= $(ROOTDIR)/product/alink/src/alink.c
srcs							+= $(ROOTDIR)/product/alink/src/serial.c
srcs							+= $(ROOTDIR)/product/alink/src/product.c
srcs							+= $(ROOTDIR)/product/alink/src/zigbee.c

srcs							+= $(ROOTDIR)/src/lockqueue.c
srcs							+= $(ROOTDIR)/src/mutex.c
srcs							+= $(ROOTDIR)/src/cond.c
srcs							+= $(ROOTDIR)/src/list.c

objs = $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(srcs)))


-include $(ROOTDIR)/make/arch.mk
-include $(ROOTDIR)/make/rules.mk

$(eval $(call LinkApp,alink,$(objs)))


scp:
	scp ./build/alink root@192.168.0.230:/root

