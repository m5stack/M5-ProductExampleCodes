#ifndef __HOMEKIT_QUERY_PARAMS__
#define __HOMEKIT_QUERY_PARAMS__
#include "port_x.h"
#ifdef ARDUINO8266_SERVER_CPP
#ifdef __cplusplus
extern "C" {
#endif
#endif

	typedef struct _query_param {
		char *name;
		char *value;

		struct _query_param *next;
	} query_param_t;

	query_param_t *query_params_parse(const char *s);
	query_param_t *query_params_find(query_param_t *params, const char *name);
	void query_params_free(query_param_t *params);

#ifdef ARDUINO8266_SERVER_CPP
#ifdef __cplusplus
} //extern C
#endif
#endif

#endif // __HOMEKIT_QUERY_PARAMS__
