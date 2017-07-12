#ifndef __ALINK_H_
#define __ALINK_H_

#include "timer.h"
#include "file_event.h"
#include "lockqueue.h"


typedef enum emALinkMsg{
	ALINK_NONE 					= 0x00,
	ALINK_TIMER_CHECK 	= 0x01,
}emALinkMsg_t;


typedef struct stALinkEnv {
	struct file_event_table *fet;
	struct timer_head *th;
	struct timer step_timer;

	stLockQueue_t msgq;

	int		loglevel;
	int		sandbox;
}stALinkEnv_t;


int		alink_init(void *_th, void *_fet, int _loglevel, int _sandbox);
int		alink_push_msg(int eid, void *param, int len);

int		alink_step();
void	alink_handler_run(struct timer *timer);

void alink_pipe_in(void *arg, int fd);

int		alink_handler_event(stEvent_t *event);



void *alink_thread(void *arg);


#endif
