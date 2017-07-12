#ifndef __ZHA_H_
#define __ZHA_H_

#include "timer.h"
#include "file_event.h"
#include "lockqueue.h"


typedef enum emZhaMsg{
	ZHA_NONE 					= 0x00,
	ZHA_TIMER_CHECK 	= 0x01,
}emZhaMsg_t;


typedef struct stZhaEnv {
	struct file_event_table *fet;
	struct timer_head *th;
	struct timer step_timer;
	struct timer work_timer;

	stLockQueue_t msgq;

	char	sdev[64];
	int		sbuad;
	int		sfd;
}stZhaEnv_t;




int		zha_init(void *_th, void *_fet, const char *_dev, int _buad);
int		zha_push_msg(int eid, void *param, int len);

int		zha_step();
void	zha_work_run(struct timer *timer);
void	zha_handler_run(struct timer *timer);

void	zha_serial_in(void *arg, int fd);

int		zha_handler_event(stEvent_t *event);






#endif
