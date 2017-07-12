#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "log.h"
#include "jansson.h"
#include "json_parser.h"

#include "zha.h"
#include "zigbee.h"


static stZigbeeCache_t zc = {
	.panid 		= 0x1122,

	.extaddr 	= {
		0x01, 0x00, 0x1f, 0x23, 0x1b, 0x23, 0x42, 0x23,
	},

	.channel 	= 0x11,

	.netkey		= {
		0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 
		0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
	},

	.scene_mode = 0,

	.sound_alarm_enable = 0,
	.sound_alarm = 0,

	.light_alarm_enable = 0,
	.light_alarm = 0,

	.white_list_enable = 0,
	.white_list = NULL,
};

static char *zigbee_fmt_short2str(char *buf, char *fmt, short v) {
	sprintf(buf, fmt, v);
	return buf;
}
static char *zigbee_fmt_int2str(char *buf, char *fmt, int v) {
	sprintf(buf, fmt, v);
	return buf;
}
static char *zigbee_fmt_ba2str(char *buf, char *fmt, char *data, int size) {
	int i = 0;
	int len = 0;
	for (i = 0; i < size; i++) {
		len += sprintf(buf + len, fmt, data[i]&0xff);
	}
	return buf;
}

static short zigbee_fmt_str2short(const char *buf, char *fmt, short *v) {
	sscanf(buf, fmt, v);
	return *v;
}
static int zigbee_fmt_str2int(const char *buf, char *fmt, int *i) {
	sscanf(buf, fmt, i);
	return *i;
}
static int zigbee_fmt_str2ba(const char *buf, char *fmt, char *data, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		sscanf(buf + i * 2, fmt, &data[i]);
	}
	return 0;
}

/* attr */
int zigbee_network_information_get(char *buf_out, unsigned int buf_sz) {
	log_info("[%d]", __LINE__);

	char buf[32];
	
	json_t *jout 	= json_object();
	json_object_set_new(jout, "when", 			json_string(zigbee_fmt_int2str(buf, "%d", time(NULL))) );

	json_t *val = json_object();
	json_object_set_new(val, "PanId", 		json_string(zigbee_fmt_short2str(buf, "%d", zc.panid)) );
	json_object_set_new(val, "ExtPanId",	json_string(zigbee_fmt_ba2str(buf, "%02x", zc.extaddr, 8)) );
	json_object_set_new(val, "Channel",		json_string(zigbee_fmt_int2str(buf, "%d", zc.channel)) );
	json_object_set_new(val, "NetworkKey",json_string(zigbee_fmt_ba2str(buf, "%02x", zc.netkey, 16)) );

	json_object_set_new(jout, "value", 			val);

	char *sout = json_dumps(jout, 0);

	strcpy(buf_out, sout);
	
	json_decref(jout);
	return 0;
}
int zigbee_network_information_set(char *json_in) {
	log_info("[%d] %s", __LINE__, json_in);

	json_error_t error;
	json_t *jin = json_loads(json_in, 0, &error);
	if (jin == NULL) {
		log_warn("[%d] error json format: [%s]", __LINE__, json_in);
		return -1;
	}

	json_t *jval = json_object_get(jin, "value");

	const char *sPanId 			= json_get_string(jval, "PanId");
	const char *sExtPanId		= json_get_string(jval, "ExtPanId");
	const char *sChannel		= json_get_string(jval, "Channel");
	const char *sNetworkKey = json_get_string(jval, "NetworkKey");

	zigbee_fmt_str2short(sPanId, "%04x", &zc.panid);
	zigbee_fmt_str2ba(sExtPanId, "%02x", zc.extaddr, 8);
	zigbee_fmt_str2int(sChannel, "%d", &zc.channel);
	zigbee_fmt_str2ba(sNetworkKey, "%02x", zc.netkey, 16);

	/* set the zigbee net information here */

	json_decref(jin);
	return 0;
}


int zigbee_scene_mode_get(char *buf_out, unsigned int buf_ze) {
	log_info("[%d]", __LINE__);
	char buf[32];

	json_t *jout = json_object();
	json_object_set_new(jout, "when", 			json_string(zigbee_fmt_int2str(buf, "%d", time(NULL))) );
	
	json_t *jval = json_object();
	json_object_set_new(jval, "SceneMode",	json_string(zigbee_fmt_int2str(buf, "%d", zc.scene_mode)));
	json_object_set_new(jout, "value", 			jval);
	
	char *sout = json_dumps(jout, 0);

	strcpy(buf_out, sout);

	json_decref(jout);

	return 0;
}
int zigbee_scene_mode_set(char *json_in) {
	log_info("[%d] %s", __LINE__, json_in);

	json_error_t error;
	json_t *jin = json_loads(json_in, 0, &error);
	if (jin == NULL) {
		log_warn("[%d] error json format: [%s]", __LINE__, json_in);
		return -1;
	}

	json_t *jval = json_object_get(jin, "value");

	const char *sSceneMode = json_get_string(jval, "SceneMode");

	zigbee_fmt_str2int(sSceneMode, "%d", &zc.scene_mode);

	json_decref(jin);

	return 0;
}

int zigbee_sound_alarm_enable_get(char *buf_out, unsigned int buf_sz) {
	log_info("[%d]", __LINE__);
	char buf[32];

	json_t *jout = json_object();
	json_object_set_new(jout, "when", 			json_string(zigbee_fmt_int2str(buf, "%d", time(NULL))) );
	
	json_t *jval = json_object();
	json_object_set_new(jval, "SoundAlarmEnable",	json_string(zigbee_fmt_int2str(buf, "%d", zc.sound_alarm_enable)));
	json_object_set_new(jout, "value", 			jval);
	
	char *sout = json_dumps(jout, 0);

	strcpy(buf_out, sout);

	json_decref(jout);
	return 0;
}
int zigbee_sound_alarm_enable_set(char *json_in) {
	log_info("[%d] %s", __LINE__, json_in);

	json_error_t error;
	json_t *jin = json_loads(json_in, 0, &error);
	if (jin == NULL) {
		log_warn("[%d] error json format: [%s]", __LINE__, json_in);
		return -1;
	}

	json_t *jval = json_object_get(jin, "value");

	const char *sSoundAlarmEnable = json_get_string(jval, "SoundAlarmEnable");

	zigbee_fmt_str2int(sSoundAlarmEnable, "%d", &zc.sound_alarm_enable);

	json_decref(jin);
	return 0;
}


int zigbee_sound_alarm_get(char *buf_out, unsigned int buf_sz) {
	log_info("[%d]", __LINE__);
	char buf[32];

	json_t *jout = json_object();
	json_object_set_new(jout, "when", 			json_string(zigbee_fmt_int2str(buf, "%d", time(NULL))) );
	
	json_t *jval = json_object();
	json_object_set_new(jval, "SoundAlarm",	json_string(zigbee_fmt_int2str(buf, "%d", zc.sound_alarm)));
	json_object_set_new(jout, "value", 			jval);
	
	char *sout = json_dumps(jout, 0);

	strcpy(buf_out, sout);


	json_decref(jout);

	return 0;
}
int zigbee_sound_alarm_set(char *json_in) {
	log_info("[%d] %s", __LINE__, json_in);

	json_error_t error;
	json_t *jin = json_loads(json_in, 0, &error);
	if (jin == NULL) {
		log_warn("[%d] error json format: [%s]", __LINE__, json_in);
		return -1;
	}

	json_t *jval = json_object_get(jin, "value");

	const char *sSoundAlarm = json_get_string(jval, "SoundAlarm");

	zigbee_fmt_str2int(sSoundAlarm, "%d", &zc.sound_alarm);


	json_decref(jin);
	return 0;
}



int zigbee_light_alarm_enable_get(char *buf_out, unsigned int buf_sz) {
	log_info("[%d]", __LINE__);
	char buf[32];

	json_t *jout = json_object();
	json_object_set_new(jout, "when", 			json_string(zigbee_fmt_int2str(buf, "%d", time(NULL))) );
	
	json_t *jval = json_object();
	json_object_set_new(jval, "LightAlarmEnable",	json_string(zigbee_fmt_int2str(buf, "%d", zc.light_alarm_enable)));
	json_object_set_new(jout, "value", 			jval);
	
	char *sout = json_dumps(jout, 0);

	strcpy(buf_out, sout);

	json_decref(jout);
	return 0;
}
int zigbee_light_alarm_enable_set(char *json_in) {
	log_info("[%d] %s", __LINE__, json_in);

	json_error_t error;
	json_t *jin = json_loads(json_in, 0, &error);
	if (jin == NULL) {
		log_warn("[%d] error json format: [%s]", __LINE__, json_in);
		return -1;
	}

	json_t *jval = json_object_get(jin, "value");

	const char *sLightAlarmEnable = json_get_string(jval, "LightAlarmEnable");

	zigbee_fmt_str2int(sLightAlarmEnable, "%d", &zc.light_alarm_enable);

	json_decref(jin);
	return 0;
}



int zigbee_light_alarm_get(char *buf_out, unsigned int buf_sz) {
	log_info("[%d]", __LINE__);
	char buf[32];

	json_t *jout = json_object();
	json_object_set_new(jout, "when", 			json_string(zigbee_fmt_int2str(buf, "%d", time(NULL))) );
	
	json_t *jval = json_object();
	json_object_set_new(jval, "LightAlarm",	json_string(zigbee_fmt_int2str(buf, "%d", zc.light_alarm)));
	json_object_set_new(jout, "value", 			jval);
	
	char *sout = json_dumps(jout, 0);

	strcpy(buf_out, sout);


	json_decref(jout);
	return 0;
}
int zigbee_light_alarm_set(char *json_in) {
	log_info("[%d] %s", __LINE__, json_in);

	json_error_t error;
	json_t *jin = json_loads(json_in, 0, &error);
	if (jin == NULL) {
		log_warn("[%d] error json format: [%s]", __LINE__, json_in);
		return -1;
	}

	json_t *jval = json_object_get(jin, "value");

	const char *sLightAlarm = json_get_string(jval, "LightAlarm");

	zigbee_fmt_str2int(sLightAlarm, "%d", &zc.light_alarm);

	json_decref(jin);

	return 0;
}


int zigbee_device_white_list_enable_get(char *buf_out, unsigned int buf_sz) {
	log_info("[%d]", __LINE__);
	char buf[32];

	json_t *jout = json_object();
	json_object_set_new(jout, "when", 			json_string(zigbee_fmt_int2str(buf, "%d", time(NULL))) );
	
	json_t *jval = json_object();
	json_object_set_new(jval, "WhiteListEnable",	json_string(zigbee_fmt_int2str(buf, "%d", zc.white_list_enable)));
	json_object_set_new(jout, "value", 			jval);
	
	char *sout = json_dumps(jout, 0);

	strcpy(buf_out, sout);

	json_decref(jout);

	return 0;
}
int zigbee_device_white_list_enable_set(char *json_in) {
	log_info("[%d] %s", __LINE__, json_in);

	json_error_t error;
	json_t *jin = json_loads(json_in, 0, &error);
	if (jin == NULL) {
		log_warn("[%d] error json format: [%s]", __LINE__, json_in);
		return -1;
	}
	json_t *jval = json_object_get(jin, "value");

	const char *sWhiteListEnable = json_get_string(jval, "WhiteListEnable");

	zigbee_fmt_str2int(sWhiteListEnable, "%d", &zc.white_list_enable);

	json_decref(jin);
	return 0;
}


int zigbee_device_white_list_get(char *buf_out, unsigned int buf_sz) {
	log_info("[%d]", __LINE__);

	char buf[32];

	json_t *jout = json_object();
	json_object_set_new(jout, "when", 			json_string(zigbee_fmt_int2str(buf, "%d", time(NULL))) );
	json_object_set_new(jout, "value", 			zc.white_list);
	
	char *sout = json_dumps(jout, 0);

	strcpy(buf_out, sout);

	json_decref(jout);
	return 0;
}
int zigbee_device_white_list_set(char *json_in) {
	log_info("[%d] %s", __LINE__, json_in);

	return -1;
}


/* service */
int zigbee_add_device_wite_list(char *s_dev_list) {
	log_info("[%d] %s", __LINE__, s_dev_list);

	json_error_t error;
	json_t *jin = json_loads(s_dev_list, 0, &error);
	if (jin == NULL) {
		log_warn("[%d] error json format: [%s]", __LINE__, s_dev_list);
		return -1;
	}

	json_t *jsnSet = json_object_get(jin, "snSet");

	if (!json_is_array(jsnSet)) {
		log_warn("[%d] error json format: [%s]", __LINE__, s_dev_list);
		return -1;
	}

	#if 0
	size_t i;
	json_t *jval;
	json_array_foreach(jsnSet, i, jval) {
		//
	}
	#endif

	json_decref(jin);
	return 0;
}
int zigbee_delete_device_wite_list(char *s_dev_list) {
	log_info("[%d] %s", __LINE__, s_dev_list);

	json_error_t error;
	json_t *jin = json_loads(s_dev_list, 0, &error);
	if (jin == NULL) {
		log_warn("[%d] error json format: [%s]", __LINE__, s_dev_list);
		return -1;
	}

	json_t *jsnSet = json_object_get(jin, "snSet");

	if (!json_is_array(jsnSet)) {
		log_warn("[%d] error json format: [%s]", __LINE__, s_dev_list);
		return -1;
	}

#if 0
	size_t i;
	json_t *jval;
	json_array_foreach(jsnSet, i, jval) {
		//
	}
	#endif

	json_decref(jin);
	return 0;
}

