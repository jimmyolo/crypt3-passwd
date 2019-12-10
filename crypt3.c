#include <node_api.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

napi_value crypt3 (napi_env env, napi_callback_info info) {
	size_t argc = 3;
	napi_value argv[3];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

	if (argc < 3) {
		napi_throw_type_error(env, NULL, "Wrong number of arguments");
		return NULL;
	}

	char password[128];
	char salt[32];
	char unixSalt[64];
	int32_t algo;
	napi_get_value_int32(env, argv[2], &algo);
	napi_get_value_string_utf8(env, argv[0], password, 128, NULL);
	napi_get_value_string_utf8(env, argv[1], salt, 32, NULL);
	sprintf(unixSalt, "$%d$%s", algo, salt);

	napi_value hashedPassword;
	char *hashed = crypt(password, unixSalt);
	napi_create_string_utf8(env, hashed, strlen(hashed), &hashedPassword);
	return hashedPassword;
}

#define DECLARE_NAPI_METHOD(name, func)                                        \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value init_all (napi_env env, napi_value exports) {
	// napi_property_descriptor desc = DECLARE_NAPI_METHOD("encrypt", crypt3);
	// napi_define_properties(env, exports, 1, &desc);
	napi_value fn;
	napi_create_function(env, NULL, 0, crypt3, NULL, &fn);
	napi_set_named_property(env, exports, "encrypt", fn);
	return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all)
