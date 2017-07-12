#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>

#include "common.h"
#include "log.h"

#include "zha.h"
#include "serial.h"


static  stZhaEnv_t env = {
	.sdev = "/dev/ttyS1", 
	.sbuad = 115200,
	.sfd = -1,
};

//////////////////////////////////////////////////////////////////////////
int		zha_init(void *_th, void *_fet, const char *_dev, int _buad) {
	env.th = _th;
	env.fet = _fet;
	
	timer_init(&env.work_timer, zha_work_run);
	timer_init(&env.step_timer, zha_handler_run);

	lockqueue_init(&env.msgq);


	/* serial */
	strcpy(env.sdev, _dev);
	env.sbuad = _buad;
	int ret =  serial_open(env.sdev, env.sbuad);
	if (ret < 0) {
		log_err("[%d] open serial %s(%d) error: %d", __LINE__, env.sdev, env.sbuad, ret);
		return -1;
	}
	env.sfd = ret;
  file_event_reg(env.fet, env.sfd, zha_serial_in, NULL, NULL);

	return 0;
}

int		zha_step() {
	timer_cancel(env.th, &env.step_timer);
	timer_set(env.th, &env.step_timer, 10);
	return 0;
}

int		zha_push_msg(int eid, void *param, int len) {
	if (eid == eid) {
		stEvent_t *e = MALLOC(sizeof(stEvent_t));
		if (e == NULL) {
			 return -1;
		}
		e->eid = eid;
		e->param = param;
		lockqueue_push(&env.msgq, e);
		zha_step();
	}
	return 0;
}

void	zha_work_run(struct timer *timer) {
	//zha_push_msg(IE_TIMER_CHECK, NULL, 0);
	timer_set(env.th, &env.work_timer, 30 * 1000);
}
void	zha_handler_run(struct timer *timer) {
	stEvent_t *e = NULL;
	if (lockqueue_pop(&env.msgq, (void **)&e) && e != NULL) {
		zha_handler_event(e);
		FREE(e);
		zha_step();
	}
}


int zha_handler_event(stEvent_t *event) {
	log_info("[%d] zha module now not support event handler, only free the event!!!", __LINE__);
	return 0;
}


void	zha_serial_in(void *arg, int fd) {
	char buf[1024];
	int ret = serial_read(env.sfd, buf, sizeof(buf), 80);
	if (ret <= 0) {
		log_warn("[%d] can't recv any bytes from serial : %d", __LINE__, ret);
		return;
	}

	buf[ret] = 0;
	log_debug_hex("serialbuf:", buf, ret);
	return;
}


