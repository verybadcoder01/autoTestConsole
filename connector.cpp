#include <iostream>
#include <string>
#include <assert.h>
#include <node_api.h>
#include "commandHelper.h"
using std::string;

string getArgString(napi_env env, napi_callback_info info){ //возвращает строку, переданную в js-функции в качестве аргумента. Да, это надо, тк напрямую получить данную строку нельзя
  size_t argc = 1;
  napi_value args[1];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  char* choice; //напи, увы, не умеет просто так в std::string
  size_t str_size;
  napi_get_value_string_utf8(env, args[0], NULL, 0, &str_size);
  str_size++; //нуль-терминатор
  choice = (char*)calloc(str_size + 1, sizeof(char));
  size_t read;
  napi_get_value_string_utf8(env, args[0], choice, str_size, &read);
  string final_str = string(choice); 
  delete [] choice; //чтобы память не текла
  return final_str;
}

static napi_value chooseTestSet(napi_env env, napi_callback_info info){
  chooseTests(getArgString(env, info));
  napi_value result;
  return result;
}

static napi_value addCommand(napi_env env, napi_callback_info info){
  addCommand(getArgString(env, info));
  napi_value result;
  return result;
}

static napi_value mkdir(napi_env env, napi_callback_info info){
  mkdir(getArgString(env, info));
  napi_value result;
  return result;
}

static napi_value runTest(napi_env env, napi_callback_info info){
  runTest(getArgString(env, info));
  napi_value result;
  return result;
}

static napi_value printTests(napi_env env, napi_callback_info info){
  printFileNames(getArgString(env, info));
  napi_value result;
  return result;
}

static napi_value removeLastCommand(napi_env env, napi_callback_info info){
  removeLastCommand();
  napi_value result;
  return result;
}

static napi_value run(napi_env env, napi_callback_info info){ //запускает записанную команду(не переданную в аргументе, а именно записанную ранее в command)
  string s = exec(command.c_str());
  std::cout << s << "\n";
  napi_value result;
  return result;
}
//дефайн для удобства
#define DECLARE_NAPI_METHOD(name, func) \
  { name, 0, func, 0, 0, 0, napi_default, 0 }


static napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor desc1 = DECLARE_NAPI_METHOD("chooseTestSet", chooseTestSet);
  status = napi_define_properties(env, exports, 1, &desc1);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("addCommand", addCommand);
  status = napi_define_properties(env, exports, 1, &desc2);\
  napi_property_descriptor desc3 = DECLARE_NAPI_METHOD("mkdir", mkdir);
  status = napi_define_properties(env, exports, 1, &desc3);
  napi_property_descriptor desc4 = DECLARE_NAPI_METHOD("runTest", runTest);
  status = napi_define_properties(env, exports, 1, &desc4);
  napi_property_descriptor desc5 = DECLARE_NAPI_METHOD("printTests", printTests);
  status = napi_define_properties(env, exports, 1, &desc5);
  napi_property_descriptor desc6 = DECLARE_NAPI_METHOD("removeLastCommand", removeLastCommand);
  status = napi_define_properties(env, exports, 1, &desc6);
  napi_property_descriptor desc7 = DECLARE_NAPI_METHOD("run", run);
  status = napi_define_properties(env, exports, 1, &desc7);
  assert(status == napi_ok);
  return exports;
}

NAPI_MODULE(testaddon, Init)
