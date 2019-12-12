#define NAPI_VERSION 4

#include <node_api.h>
#include <assert.h>

#ifdef _XOPEN_SOURCE
#include <unistd.h>
#endif

#ifdef _GNU_SOURCE
#include <crypt.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    napi_async_work work;
    napi_threadsafe_function tsfn;
    char password[64];
    char salt[32];
    int32_t algo;
    char hashed[128];
} AddonData;

static void ExecuteWork(napi_env env, void* data) {
    AddonData* addon_data = (AddonData*)data;
    assert(napi_acquire_threadsafe_function(addon_data->tsfn) == napi_ok);

    char unixSalt[64];
    sprintf(unixSalt, "$%d$%s", addon_data->algo, addon_data->salt);
    sprintf(addon_data->hashed, "%s", crypt(addon_data->password, unixSalt));

    assert(napi_call_threadsafe_function(addon_data->tsfn, addon_data, napi_tsfn_blocking) == napi_ok);
    assert(napi_release_threadsafe_function(addon_data->tsfn, napi_tsfn_release) == napi_ok);
}

static void WorkComplete(napi_env env, napi_status status, void* data) {
    AddonData* addon_data = (AddonData*)data;
    assert(napi_release_threadsafe_function(addon_data->tsfn, napi_tsfn_release) == napi_ok);
    assert(napi_delete_async_work(env, addon_data->work) == napi_ok);

    // Set both values to NULL so JavaScript can order a new run of the thread.
    addon_data->work = NULL;
    addon_data->tsfn = NULL;
}

// This function is responsible for converting data coming in from the worker
// thread to napi_value items that can be passed into JavaScript, and for
// calling the JavaScript function.
static void CallJs(napi_env env, napi_value js_cb, void* context, void* data) {
    AddonData* addon_data = (AddonData*)data;
    // env and js_cb may both be NULL if Node.js is in its cleanup phase, and
    // items are left over from earlier thread-safe calls from the worker thread.
    // When env is NULL, we simply skip over the call into Javascript and free the
    // items.
    if (env != NULL) {
        napi_value undefined, hashedPassword;
        assert(napi_create_string_utf8(env, addon_data->hashed, NAPI_AUTO_LENGTH, &hashedPassword) == napi_ok);

        // Retrieve the JavaScript `undefined` value so we can use it as the `this`
        // value of the JavaScript function call.
        assert(napi_get_undefined(env, &undefined) == napi_ok);

        // Call the JavaScript function and pass it the prime that the secondary
        // thread found.
        assert(napi_call_function(env,
                                  undefined,
                                  js_cb,
                                  1,
                                  &hashedPassword,
                                  NULL) == napi_ok);
    }
}

static napi_value crypt3 (napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value argv[4];

    napi_value js_cb, work_name;
    AddonData *addon_data;
    assert(napi_get_cb_info(env,
                            info,
                            &argc,
                            argv,
                            NULL,
                            (void **)(&addon_data)) == napi_ok);
    if (argc < 3) {
        napi_throw_type_error(env, NULL, "Wrong number of arguments.");
        return NULL;
    }
    js_cb = argv[3];

    // Ensure that no work is currently in progress.
    assert(addon_data->work == NULL && "Only one work item must exist at a time");

    // Create a string to describe this asynchronous operation.
    assert(napi_create_string_utf8(env,
                                   "N-API Thread-safe Call from Async Work Item",
                                   NAPI_AUTO_LENGTH,
                                   &work_name) == napi_ok);

    // Convert the callback retrieved from JavaScript into a thread-safe function
    // which we can call from a worker thread.
    assert(napi_create_threadsafe_function(env,
                                           js_cb,
                                           NULL,
                                           work_name,
                                           0,
                                           1,
                                           NULL,
                                           NULL,
                                           NULL,
                                           CallJs,
                                           &(addon_data->tsfn)) == napi_ok);

    napi_get_value_int32(env, argv[2], &(addon_data->algo));
    napi_get_value_string_utf8(env, argv[0], addon_data->password, 64, NULL);
    napi_get_value_string_utf8(env, argv[1], addon_data->salt, 32, NULL);

    printf("%s", addon_data->password);

    // Create an async work item, passing in the addon data, which will give the
    // worker thread access to the above-created thread-safe function.
    assert(napi_create_async_work(env,
                                  NULL,
                                  work_name,
                                  ExecuteWork,
                                  WorkComplete,
                                  addon_data,
                                  &(addon_data->work)) == napi_ok);

    // Queue the work item for execution.
    assert(napi_queue_async_work(env, addon_data->work) == napi_ok);

    return NULL;
}

// Free the per-addon-instance data.
static void addon_getting_unloaded(napi_env env, void* data, void* hint) {
    AddonData *addon_data = (AddonData *)data;
    assert(addon_data->work == NULL &&
           "No work item in progress at module unload");
    free(addon_data);
}

napi_value init_all (napi_env env, napi_value exports) {
    // Define addon-level data associated with this instance of the addon.
    AddonData *addon_data = (AddonData *)malloc(sizeof(*addon_data));
    addon_data->work = NULL;

    // Define the properties that will be set on exports.
    napi_property_descriptor start_work = {
        "encrypt",
        NULL,
        crypt3,
        NULL,
        NULL,
        NULL,
        napi_default,
        addon_data
    };

    // Decorate exports with the above-defined properties.
    assert(napi_define_properties(env, exports, 1, &start_work) == napi_ok);

    // Associate the addon data with the exports object, to make sure that when
    // the addon gets unloaded our data gets freed.
    assert(napi_wrap(env,
                    exports,
                    addon_data,
                    addon_getting_unloaded,
                    NULL,
                    NULL) == napi_ok);

    // Return the decorated exports object.
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all)
