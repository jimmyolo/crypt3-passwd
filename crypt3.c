#include <node_api.h>
#include <napi-macros.h>
// #include <assert.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

NAPI_METHOD(crypt3) {
	// size_t argc = 3;
	// napi_value argv[3];
	// napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	NAPI_ARGV(3)
	if (argc < 3) {
		napi_throw_type_error(env, NULL, "Wrong number of arguments.");
		return NULL;
	}

	NAPI_ARGV_UTF8(password, 64, 0)
	NAPI_ARGV_UTF8(salt, 32, 1)
	NAPI_ARGV_INT32(algo, 2)
	
	char unixSalt[64];
	sprintf(unixSalt, "$%d$%s", algo, salt);

	char *hashed = crypt(password, unixSalt);

	NAPI_RETURN_STRING(hashed)
}

NAPI_INIT () {
	napi_value fn;
	napi_create_function(env, NULL, 0, crypt3, NULL, &fn);
	napi_set_named_property(env, exports, "encrypt", fn);
	// NAPI_EXPORT_FUNCTION(crypt3)
}