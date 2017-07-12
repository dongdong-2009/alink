#ifndef __ZIGBEE_H_
#define __ZIGBEE_H_

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "jansson.h"

typedef struct stZigbeeCache {
	short 	panid;
	char 		extaddr[8];
	int			channel;
	char		netkey[16]; 	//128 bit key, 16 bytes

	int			scene_mode;

	int			sound_alarm_enable;
	int			sound_alarm;

	int			light_alarm_enable;
	int			light_alarm;

	int 		white_list_enable;
	json_t	*white_list;
}stZigbeeCache_t;


/* service */
int zigbee_add_device_wite_list(char *s_dev_list);
int zigbee_delete_device_wite_list(char *s_dev_list);





/* attr */
int zigbee_network_information_get(char *buf, unsigned int buf_sz);
int zigbee_network_information_set(char *json_in);
int zigbee_scene_mode_get(char *buf, unsigned int buf_ze);
int zigbee_scene_mode_set(char *json_in);
int zigbee_sound_alarm_enable_get(char *buf, unsigned int buf_sz);
int zigbee_sound_alarm_enable_set(char *json_in);
int zigbee_sound_alarm_get(char *buf, unsigned int buf_sz);
int zigbee_sound_alarm_set(char *json_in);
int zigbee_light_alarm_enable_get(char *buf, unsigned int buf_sz);
int zigbee_light_alarm_enable_set(char *json_in);
int zigbee_light_alarm_get(char *buf, unsigned int buf_sz);
int zigbee_light_alarm_set(char *json_in);
int zigbee_device_white_list_get(char *buf, unsigned int buf_sz);
int zigbee_device_white_list_set(char *json_in);
int zigbee_device_white_list_enable_get(char *buf, unsigned int buf_sz);
int zigbee_device_white_list_enable_set(char *json_in);



#endif
