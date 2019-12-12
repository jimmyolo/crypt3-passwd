#define NAPI_VERSION 3

#include <node_api.h>
// #include <assert.h>

#ifdef _XOPEN_SOURCE
#include <unistd.h>
#endif

#ifdef _GNU_SOURCE
#include <crypt.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

napi_value crypt3 (napi_env env, napi_callback_info info) {
	size_t argc = 3;
	napi_value argv[4];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

	if (argc < 3) {
		napi_throw_type_error(env, NULL, "Wrong number of arguments.");
		return NULL;
	}

	char password[64], salt[32];
	int32_t algo;
	napi_get_value_string_utf8(env, argv[0], password, 64, NULL);
	napi_get_value_string_utf8(env, argv[1], salt, 32, NULL);
	napi_get_value_int32(env, argv[2], &algo);

	char unixSalt[64];
	sprintf(unixSalt, "$%d$%s", algo, salt);

	napi_value hashedPassword;
	char *hashed = crypt(password, unixSalt);
	napi_create_string_utf8(env, hashed, NAPI_AUTO_LENGTH, &hashedPassword);

	return hashedPassword;
}

napi_value init_all (napi_env env, napi_value exports) {
	napi_value fn;
	napi_create_function(env, NULL, 0, crypt3, NULL, &fn);
	napi_set_named_property(env, exports, "encrypt", fn);
	return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all)
