#include <stdio.h>
#include <string.h>

#include "product.h"

#include "common.h"
#include "log.h"

//TODO: update these product info
/*#define product_model		    "ALINKTEST_LIVING_LIGHT_SMARTLED"
#define product_key             "ljB6vqoLzmP8fGkE6pon"
#define product_secret		    "YJJZjytOCXDhtQqip4EjWbhR95zTgI92RVjzjyZF"
#define product_debug_key       "dpZZEpm9eBfqzK7yVeLq"
#define product_debug_secret    "THnfRRsU5vu6g6m9X6uFyAjUWflgZ0iyGjdEneKm"
*/

#define product_model		    			"ALINKTEST_SECURITY_GATEWAY_QUANWU_001"
#define product_key             	"V2hpRG0k7Pbr1bmxDCat"
#define product_secret		    		"O71DlsrrTkImG0NxowxaA5oFFjyxTj1n8FwWzOJv"
#define product_debug_key       	"dpZZEpm9eBfqzK7yVeLq"
#define product_debug_secret    	"THnfRRsU5vu6g6m9X6uFyAjUWflgZ0iyGjdEneKm"
#define PRODUCT_ASR_APP_KEY     	"box2015product01"

char *product_get_name(char name_str[PRODUCT_NAME_LEN]) {
	log_info("[%d]", __LINE__);
	return strncpy(name_str, "alink_product", PRODUCT_NAME_LEN);
}

char *product_get_version(char ver_str[PRODUCT_VERSION_LEN]) {
	log_info("[%d]", __LINE__);
	return strncpy(ver_str, "1.0", PRODUCT_VERSION_LEN);
}

char *product_get_model(char model_str[PRODUCT_MODEL_LEN]) {
	log_info("[%d]", __LINE__);
	return strncpy(model_str, product_model, PRODUCT_MODEL_LEN);
}

char *product_get_key(char key_str[PRODUCT_KEY_LEN]) {
	log_info("[%d]", __LINE__);
	return strncpy(key_str, product_key, PRODUCT_KEY_LEN);
}

char *product_get_secret(char secret_str[PRODUCT_SECRET_LEN]) {
	log_info("[%d]", __LINE__);
	return strncpy(secret_str, product_secret, PRODUCT_SECRET_LEN);
}

char *product_get_debug_key(char key_str[PRODUCT_KEY_LEN]) {
	log_info("[%d]", __LINE__);
	return strncpy(key_str, product_debug_key, PRODUCT_KEY_LEN);
}

char *product_get_debug_secret(char secret_str[PRODUCT_SECRET_LEN]) {
	log_info("[%d]", __LINE__);
	return strncpy(secret_str, product_debug_secret, PRODUCT_SECRET_LEN);
}

char *product_get_sn(char sn_str[PRODUCT_SN_LEN]) {
	log_info("[%d]", __LINE__);
	return strncpy(sn_str, "6923450656860", PRODUCT_SN_LEN);
}

