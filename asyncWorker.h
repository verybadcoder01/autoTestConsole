#include <bits/stdc++.h>
#include <node_api.h>
#include "commandHelper.h"

const std::string TEMPLATE = "name.txt", RESULT = "result.txt";

struct promData
{
    int asyncStatus;
    napi_deferred deferred;
    napi_async_work work;
};

void read(std::string& name){
    std::ifstream in(TEMPLATE);
    in >> name;
    in.close();
}

void executeInTempl(napi_env env, void* data){
    promData *tmp = (promData*)data;
    std::string name;
    read(name);
    templs[name].runAllIncluded(RESULT);
    tmp->asyncStatus = 0; //success!
}

void execute(napi_env env, void* data){
    promData *tmp = (promData *)data;
    std::ifstream in(TEMPLATE);
    std::string name;
    read(name);
    std::string res = runTest(name);
    std::ofstream out(RESULT);
    out << res;
    out.close();
    tmp->asyncStatus = 0;
}

void finishPromise(napi_env env, napi_status status, void* data){
    promData *prom = (promData*)data;
    napi_value val;
    std::string result, tmp;
    std::ifstream in(RESULT);
    while (getline(in, tmp)){
        result += tmp + "\n";
    }
    napi_create_string_utf8(env, result.c_str(), result.size(), &val);
    if (prom->asyncStatus == 0){
        status = napi_resolve_deferred(env, prom->deferred, val);
    } else {
        napi_value undefined;
        status = napi_get_undefined(env, &undefined);
        status = napi_reject_deferred(env, prom->deferred, undefined);
    }
    if (status != napi_ok){
        napi_throw_error(env, NULL, "unable to create promise result");
    }
    napi_delete_async_work(env, prom->work);
    free(prom);
}