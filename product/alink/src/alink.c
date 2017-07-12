#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>


#include "common.h"
#include "timer.h"
#include "log.h"

#include "alink.h"
#include "alink_export_zigbee.h"
#include "alink_export_gateway.h"
#include "platform.h"
#include "product.h"
#include "zigbee.h"

static stALinkEnv_t env = {
	.loglevel = ALINK_LL_TRACE,
};


void _alink_init();
void _alink_end();
//////////////////////////////////////////////////////////////////////////

int		alink_init(void *_th, void *_fet, int _loglevel, int _sandbox) {
	env.th = _th;
	env.fet = _fet;
	env.loglevel = _loglevel;
	env.sandbox = _sandbox;
	
	timer_init(&env.step_timer, alink_handler_run);

	lockqueue_init(&env.msgq);

 	//file_event_reg(env.fet, env.pfd[0], alink_pipe_in, NULL, NULL);

	_alink_init();

	return 0;
}
int		alink_push_msg(int eid, void *param, int len) {
	if (eid == eid) {
		stEvent_t *e = MALLOC(sizeof(stEvent_t));
		if (e == NULL) {
			 return -1;
		}
		e->eid = eid;
		e->param = param;
		lockqueue_push(&env.msgq, e);
		alink_step();
	}
	return 0;
}

int		alink_step() {
	timer_cancel(env.th, &env.step_timer);
	timer_set(env.th, &env.step_timer, 10);
	return 0;
}
void	alink_handler_run(struct timer *timer) {
	stEvent_t *e = NULL;
	if (lockqueue_pop(&env.msgq, (void **)&e) && e != NULL) {
		alink_handler_event(e);
		FREE(e);
		alink_step();
	}
}

int		alink_handler_event(stEvent_t *event) {
	log_info("[%d] alink module now not support event handler, only free the event!!!", __LINE__);
	return 0;
}

//void alink_pipe_in(void *arg, int fd) {
//}

/////////////////////////////  Alink Util ////////////////////////////////////////////////////

static void alink_util_zigbee_view_cmd_argument(cmd_argument_t *ca, int disoff) {
	if (ca == NULL) return;
	log_info("%*carg_name:%s,data_type:%02X, mask_code:%02X", disoff, ' ', 
					ca->arg_name, ca->data_type, ca->mask_code);
	if (ca->child_argument != NULL) {
		alink_util_zigbee_view_cmd_argument(ca->child_argument, disoff+1);
	}
	if (ca->next_arg != NULL) {
		alink_util_zigbee_view_cmd_argument(ca->next_arg, disoff);
	}
}
static void alink_util_zigbee_view_attr_element(attr_element_t *ae, int disoff) {
	if (ae == NULL) return;
	log_info("%*celem_name:%s,data_type:%02X, mask_code:%02X", disoff, ' ', 
					ae->element_name, ae->data_type, ae->mask_code);
	if (ae->child_element != NULL) {
		alink_util_zigbee_view_attr_element(ae->child_element, disoff+1);
	}
	if (ae->next_element != NULL) {
		alink_util_zigbee_view_attr_element(ae->next_element, disoff);
	}
}


static void alink_util_zigbee_view_cmd_profile(cmd_profile_t *cp) {
	if (cp == NULL) return;
	log_info("cmd_name:%s, cluster_id:%04X, cmd_id:%02X, dir:%s",
						cp->cmd_name, cp->cluster_id, cp->cmd_id, cp->direction ? "svr->cli" : "cli->svr");
	if (cp->args != NULL) {
		alink_util_zigbee_view_cmd_argument(cp->args, 1);
	}
}

static void alink_util_zigbee_view_attr_profile(attr_profile_t *ap) {
	if (ap == NULL) return;
	log_info("attr_name:%s, data_type:%02X, cluster_id:%04X, attribute_id:%04X",
						ap->attr_name, ap->data_type, ap->cluster_id, ap->attribute_id);
	if (ap->attr_element != NULL) {
		alink_util_zigbee_view_attr_element(ap->attr_element, 1);
	}
}



/////////////////////////////  Service ///////////////////////////////////////////////////////
typedef struct stGwService {
	const char *name;
	ALINK_SERVICE_EXECUTE_CB func;
}stGwService_t;

int FactoryReset_service(char *json_in, char *json_out_buf, unsigned int buf_sz) {
	log_info("[%d] ", __LINE__);

	/* Alink-embedded网关SDK使用说明v0.1.pdf -> 2.2.1 */
	if (alink_factory_reset() != 0) {
		return -1;
	}

	platform_factory_reset();

	return 0;
}

int AddDeviceWiteList_service(char *json_in, char *json_out_buf, unsigned int buf_sz) {
	log_info("[%d] %s", __LINE__, json_in);

	int ret = zigbee_add_device_wite_list(char *s_dev_list);

	json_out_buf[0] = 0;

	return ret;
}
int DeleteDeviceWiteList_service(char *json_in, char *json_out_buf, unsigned int buf_sz) {
	log_info("[%d] %s", __LINE__, json_in);


	int ret = zigbee_delete_device_wite_list(char *s_dev_list);

	json_out_buf[0] = 0;

	return ret;
}

static stGwService_t gs[] = {
	{"FactoryReset", 					FactoryReset_service},
	{"AddDeviceWiteList", 		AddDeviceWiteList_service},
	{"DeleteDeviceWiteList",  DeleteDeviceWiteList_service},
};

void register_gateway_service() {
	int i = 0;
	for (i = 0; i < sizeof(gs)/sizeof(gs[0]); i++) {
		alink_register_service(gs[i].name, gs[i].func);
	}
}

/////////////////////////////  Attr //////////////////////////////////////////////////////////
typedef struct stGwAttr {
	const char *name;
	ALINK_ATTR_GET_CB get;
	ALINK_ATTR_SET_CB set;
}stGwAttr_t;

int NetworkInformation_attr_get(char *buf, unsigned int buf_sz) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_network_information_get(buf, buf_sz);

	return ret;
}
int NetworkInformation_attr_set(char *json_in) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_network_information_set(json_in);

	return ret;
}

int SceneMode_attr_get(char *buf, unsigned int buf_sz) {
	log_info("[%d] ", __LINE__);
	
	int ret = zigbee_scene_mode_get(buf, buf_ze);

	return ret;
}
int SceneMode_attr_set(char *json_in) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_scene_mode_set(json_in);

	return ret;
}

int SoundAlarmEnable_attr_get(char *buf, unsigned int buf_sz) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_scene_alarm_enable_get(buf, buf_sz);

	return ret;
}
int SoundAlarmEnable_attr_set(char *json_in) {
	log_info("[%d] ", __LINE__);
	
	int ret = zigbee_scene_alarm_enable_set(json_in);
	
	return ret;
}

int SoundAlarm_attr_get(char *buf, unsigned int buf_sz) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_sound_alarm_get(buf, buf_sz);

	return ret;
}
int SoundAlarm_attr_set(char *json_in) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_sound_alarm_set(json_in);

	return ret;
}

int LightAlarmEnable_attr_get(char *buf, unsigned int buf_sz) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_light_alarm_enable_get(buf, buf_sz);

	return ret;
}
int LightAlarmEnable_attr_set(char *json_in) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_light_alarm_enable_set(json_in);

	return ret;
}

int LightAlarm_attr_get(char *buf, unsigned int buf_sz) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_light_alarm_get(buf, buf_sz);
	
	return ret;
}
int LightAlarm_attr_set(char *json_in) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_light_alarm_set(json_in);

	return ret;
}


int DeviceWhiteList_attr_get(char *buf, unsigned int buf_sz) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_device_white_list_get(buf, buf_sz);

	return ret;
}
int DeviceWhiteList_attr_set(char *json_in) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_device_white_list_set(json_in);

	return ret;
}

int DeviceWhiteListEnable_attr_get(char *buf, unsigned int buf_sz) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_device_white_list_enable_get(buf, buf_sz);

	return ret;
}
int DeviceWhiteListEnable_attr_set(char *json_in) {
	log_info("[%d] ", __LINE__);

	int ret = zigbee_device_white_list_enable_set(json_in);

	return ret;
}

static stGwAttr_t ga[] = {
	{"NetworkInformation", 		NetworkInformation_attr_get, 		NetworkInformation_attr_set},
	{"SceneMode", 						SceneMode_attr_get, 						SceneMode_attr_set},
	{"SoundAlarmEnable", 			SoundAlarmEnable_attr_get, 			SoundAlarmEnable_attr_set},
	{"SoundAlarm", 						SoundAlarm_attr_get, 						SoundAlarm_attr_set},
	{"LightAlarmEnable",			LightAlarmEnable_attr_get,			LightAlarmEnable_attr_set},
	{"LightAlarm",						LightAlarm_attr_get,						LightAlarm_attr_set},
	{"DeviceWhiteList ",			DeviceWhiteList_attr_get,				DeviceWhiteList_attr_set},
	{"DeviceWhiteListEnable",	DeviceWhiteListEnable_attr_get,	DeviceWhiteListEnable_attr_set},
};

void register_gateway_attribute() {
	int i = 0;
	for (i = 0; i < sizeof(ga)/sizeof(ga[0]); i++) {
		alink_register_attribute(ga[i].name, ga[i].get, ga[i].set);
	}
}

/////////////////////////////  Callback /////////////////////////////////////////////////////
typedef struct stCallback {
	unsigned char type;
	void *func;
}stCallback_t;


int callback_zigbee_get_attr(unsigned char ieee_addr[IEEE_ADDR_BYTES], unsigned char endpoint_id, const char *attr_name) {
	log_info("[%d] extaddr:%08X%08X, ep:%02X, attr_name:%s", __LINE__, 
					*(int*)ieee_addr, *(((int*)ieee_addr)+1), endpoint_id, attr_name);
	return 0;
}
int callback_zigbee_set_attr(unsigned char ieee_addr[IEEE_ADDR_BYTES], unsigned char endpoint_id, const char *attr_name, const char *attr_value) {
	log_info("[%d] extaddr:%08X%08X, ep:%02X, attr_name:%s, attr_value:%s", __LINE__, 
					*(int*)ieee_addr, *(((int*)ieee_addr)+1), endpoint_id, attr_name, attr_value);
	return 0;
}
int callback_zigbee_execute_cmd(unsigned char ieee_addr[IEEE_ADDR_BYTES],unsigned char endpoint_id, const char *cmd_name,const char *cmd_args) {
	log_info("[%d] extaddr:%08X%08X, ep:%02X, cmd_name:%s, cmd_args:%s", __LINE__, 
					*(int*)ieee_addr, *(((int*)ieee_addr)+1), endpoint_id, cmd_name, cmd_args);
	return 0;
}
int callback_zigbee_update_attr_profile(attr_profile_t *profile[]) {
	log_info("[%d] ", __LINE__);

	int i = 0;
	while (profile[i] != NULL) {
		alink_util_zigbee_view_attr_profile(profile[i++]);
	}
	return 0;
}
int callback_zigbee_update_cmd_profile(cmd_profile_t *profile[]) {
	log_info("[%d] ", __LINE__);

	int i = 0;
	while (profile[i] != NULL) {
		alink_util_zigbee_view_cmd_profile(profile[i++]);
	}
	return 0;
}
int callback_zigbee_remove_device(unsigned char ieee_addr[IEEE_ADDR_BYTES]) {
	log_info("[%d] extaddr:%08X%08X", __LINE__, *(int*)ieee_addr, *(((int*)ieee_addr)+1));
	return 0;
}
int callback_zigbee_permit_join(uint8_t duration) {
	log_info("[%d] duration:%02X", __LINE__, duration);
	return 0;
}

static stCallback_t cb[] = {
	{ALINK_ZIGBEE_GET_DEVICE_STATUS, 		callback_zigbee_get_attr},
	{ALINK_ZIGBEE_SET_DEVICE_STATUS,		callback_zigbee_set_attr},
	{ALINK_ZIGBEE_EXECUTE_DEVICE_CMD, 	callback_zigbee_execute_cmd},
	{ALINK_ZIGBEE_UPDATE_ATTR_PROFILE, 	callback_zigbee_update_attr_profile},
	{ALINK_ZIGBEE_UPDATE_CMD_PROFILE,		callback_zigbee_update_cmd_profile},
	{ALINK_ZIGBEE_REMOVE_DEVICE,				callback_zigbee_remove_device},
	{ALINK_ZIGBEE_PERMIT_JOIN,					callback_zigbee_permit_join},
};

void zigbee_stack_init() {
	int i = 0;
	for (i = 0; i < sizeof(cb)/sizeof(cb[0]); i++) {
		alink_register_callback(cb[i].type, cb[i].func);
	}
}

///////////////////////////// ALink Init //////////////////////////////////////////////////////////
void _alink_init() {
	//设置日志输出级别
	alink_set_loglevel(env.loglevel);

	//设置沙箱模式
	if (env.sandbox == 1) {
		alink_enable_sandbox_mode();
	}


	//zigbee 网关事件回调注册
	zigbee_stack_init();

	//注册网关属性和服务
	register_gateway_service();
	register_gateway_attribute();

	alink_start();
	//等待连接成功，非必须项
	alink_wait_connect(ALINK_WAIT_FOREVER);
}

void _alink_end() {
	//退出 Alink
	alink_end();
}
