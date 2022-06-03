#include <iostream>
#include <string>
#include <assert.h>
#include <node_api.h>
#include "commandHelper.h"
using std::string;

//здесь сплошная копипаста
//refactoring really needed

static napi_value chooseTestSet(napi_env env, napi_callback_info info){
  size_t argc = 1;
  napi_value args[1];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  char* choice;
  size_t str_size;
  napi_get_value_string_utf8(env, args[0], NULL, 0, &str_size);
  str_size++; //нуль-терминатор
  choice = (char*)calloc(str_size + 1, sizeof(char));
  size_t read;
  napi_get_value_string_utf8(env, args[0], choice, str_size, &read);
  string final_str = string(choice);
  chooseTests(final_str);
  napi_value result;
  delete [] choice;
  return result;
}

static napi_value addCommand(napi_env env, napi_callback_info info){
  size_t argc = 1;
  napi_value args[1];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  char* add;
  size_t str_size;
  napi_get_value_string_utf8(env, args[0], NULL, 0, &str_size);
  str_size++; //нуль-терминатор
  add = (char*)calloc(str_size + 1, sizeof(char));
  size_t read;
  napi_get_value_string_utf8(env, args[0], add, str_size, &read);
  string final_str = string(add);
  addCommand(final_str);
  napi_value result;
  delete [] add;
  return result;
}

static napi_value mkdir(napi_env env, napi_callback_info info){
  size_t argc = 1;
  napi_value args[1];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  char* dirName;
  size_t str_size;
  napi_get_value_string_utf8(env, args[0], NULL, 0, &str_size);
  str_size++; //нуль-терминатор
  dirName = (char*)calloc(str_size + 1, sizeof(char));
  size_t read;
  napi_get_value_string_utf8(env, args[0], dirName, str_size, &read);
  string final_str = string(dirName);
  mkdir(final_str);
  napi_value result;
  delete [] dirName;
  return result;
}

static napi_value runTest(napi_env env, napi_callback_info info){
  size_t argc = 1;
  napi_value args[1];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  char* testName;
  size_t str_size;
  napi_get_value_string_utf8(env, args[0], NULL, 0, &str_size);
  str_size++;
  testName = (char*)calloc(str_size + 1, sizeof(char));
  size_t read;
  napi_get_value_string_utf8(env, args[0], testName, str_size, &read);
  string res = string(testName);
  runTest(res);
  napi_value result;
  delete [] testName;
  return result;
}

static napi_value printTests(napi_env env, napi_callback_info info){
  size_t argc = 1;
  napi_value args[1];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  char* pthToLib;
  size_t str_size;
  napi_get_value_string_utf8(env, args[0], NULL, 0, &str_size);
  str_size++;
  pthToLib = (char*)calloc(str_size + 1, sizeof(char));
  size_t read;
  napi_get_value_string_utf8(env, args[0], pthToLib, str_size, &read);
  string res = string(pthToLib);
  getFileNames(res);
  napi_value result;
  delete [] pthToLib;
  return result;
}

static napi_value removeLastCommand(napi_env env, napi_callback_info info){
  removeLastCommand();
  napi_value result;
  return result;
}

static napi_value run(napi_env env, napi_callback_info info){
  string s = exec(command.c_str());
  std::cout << s << "\n";
  napi_value result;
  return result;
}

#define DECLARE_NAPI_METHOD(name, func) \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

//а вот это, увы, отрефакторить никак нельзя
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