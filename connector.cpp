#include <iostream>
#include <string>
#include <assert.h>
#include <node_api.h>
#include <vector>
#include "commandHelper.h"
using std::string, std::vector;

vector<string> getArgString(napi_env env, napi_callback_info info, size_t argc){ //возвращает строки, переданные в js-функции в качестве аргумента
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  vector<string> res;
  for (int i = 0; i < argc; ++i){
    char* choice; //напи, увы, не умеет просто так в std::string
    size_t str_size;
    napi_get_value_string_utf8(env, args[i], NULL, 0, &str_size);
    str_size++; //нуль-терминатор
    choice = (char*)calloc(str_size + 1, sizeof(char));
    size_t read;
    napi_get_value_string_utf8(env, args[i], choice, str_size, &read);
    string final_str = string(choice); 
    res.push_back(final_str);
    delete [] choice; //чтобы память не текла
  }
  return res;
}

napi_value getArrayOfString(napi_env env, std::vector<string> t){ //возвращает жсовый массив с теми же значениями, что и наш
  napi_value js_array, js_push_fn, js_array_item; //а теперь будет мясо
  napi_status status;
  status = napi_create_array(env, &js_array); //создаем жсовыый массив
  assert(status == napi_ok);
  status = napi_get_named_property(env, js_array, "push", &js_push_fn); //получаем "указатель" на функцию push 
  assert(status == napi_ok);
  for (auto &elem : t){ //поэлементно добавляем в жсовый массив наш
    status = napi_create_string_utf8(env, elem.c_str(), elem.size(), &js_array_item); //не будет работать на винде!
    assert(status == napi_ok);
    status = napi_call_function(env, js_array, js_push_fn, 1, &js_array_item, NULL); //собственно пушим элемент
    assert(status == napi_ok);
  }
  return js_array;
}

static napi_value testCases(napi_env  env, napi_callback_info info){
  string arg = getArgString(env, info, 1)[0];
  std::vector<string> t = Parser(chosen / arg).getAllTests();
  return getArrayOfString(env, t);
}

static napi_value chooseTestSet(napi_env env, napi_callback_info info){
  string arg = getArgString(env, info, 1)[0];
  std::vector<string> t = chooseTests(arg);
  return getArrayOfString(env, t);
}

static napi_value addCommand(napi_env env, napi_callback_info info){
  addCommand(getArgString(env, info, 1)[0]);
  napi_value result;
  return result;
}

static napi_value mkdir(napi_env env, napi_callback_info info){
  mkdir(getArgString(env, info, 1)[0]);
  napi_value result;
  return result;
}

static napi_value runTest(napi_env env, napi_callback_info info){
  string s = runTest(getArgString(env, info, 1)[0]);
  napi_value result = getArrayOfString(env, {s});
  return result;
}

static napi_value printTests(napi_env env, napi_callback_info info){
  printFileNames(getArgString(env, info, 1)[0]);
  napi_value result;
  return result;
}

static napi_value removeLastCommand(napi_env env, napi_callback_info info){
  removeLastCommand();
  napi_value result;
  return result;
}

static napi_value createTemplate(napi_env env, napi_callback_info info){ //создает шаблон с переданным именем
  string arg = getArgString(env, info, 1)[0];
  if (templs.find(arg) != templs.end()){ //не допускаются шаблоны с одинаковым именем
    throw std::runtime_error("template with this name already exists");
  }
  templs[arg] = Template(chosen, arg);
  napi_value result;
  return result;
}

static napi_value addExistingTest(napi_env env, napi_callback_info info){ //записывать в шаблон существующий тест
  vector<string> args = getArgString(env, info, 2);
  templs[args[0]].addExistingTest(args[1]);
  napi_value result;
  return result;
}

static napi_value removeExistingTest(napi_env env, napi_callback_info info){ //удаляет тест из шаблона с заданным именем
  vector<string> args = getArgString(env, info, 2);
  templs[args[0]].removeExistingTest(args[1]);
  napi_value result;
  return result;
}

static napi_value run(napi_env env, napi_callback_info info){ //запускает записанную команду(не переданную в аргументе, а именно записанную ранее в command)
  string s = exec(command.c_str());
  std::cout << s << "\n";
  napi_value result;
  return result;
}

static napi_value runAllTestsInTemplate(napi_env env, napi_callback_info info){ //запускает все тесты в шаблоне
  vector<string> args = getArgString(env, info, 1);
  const char* msg = "template with this name does not exist";
  const char* code = "1";
  if (templs.find(args[0]) == templs.end()){
    //napi_throw_error(env, code, msg);
    throw std::runtime_error("template with this name does not exist");
  }
  string t = templs[args[0]].runAllIncluded();
  vector<string> tmp = {t};
  napi_value result = getArrayOfString(env, tmp);
  return result;
}

static napi_value deleteTemplate(napi_env env, napi_callback_info info){ //удаляет шаблон
  string arg = getArgString(env, info, 1)[0];
  removeTemplate(arg);
  napi_value result;
  return result;
}

static napi_value openLastReport(napi_env env, napi_callback_info info){
  openLastReport();
  napi_value result;
  return result;
}

static napi_value findStoredTempls(napi_env env, napi_callback_info info){
  findStoredTemplates();
  napi_value result;
  return result;
}

static napi_value deleteFile(napi_env env, napi_callback_info info){
  deleteFile(getArgString(env, info, 1)[0]);
  napi_value result;
  return result;
}

static napi_value getTestsInTemplate(napi_env env, napi_callback_info info){
  string arg = getArgString(env, info, 1)[0];
  vector<string> res;
  for (const auto& elem : templs[arg].includedTests){
    res.push_back(elem);
  }
  return getArrayOfString(env, res);
}

static napi_value gitClone(napi_env env, napi_callback_info info){
  vector<string> arg = getArgString(env, info, 3); //(user, passwrod, link);
  string res = gitClone(arg[0], arg[1], arg[2]);
  vector<string> t = {res};
  return getArrayOfString(env, t);
}

//дефайн для удобства
#define DECLARE_NAPI_METHOD(name, func) \
  { name, 0, func, 0, 0, 0, napi_default, 0 }


static napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor desc1 = DECLARE_NAPI_METHOD("chooseTestSet", chooseTestSet);
  status = napi_define_properties(env, exports, 1, &desc1);
  napi_property_descriptor desc2 = DECLARE_NAPI_METHOD("addCommand", addCommand);
  status = napi_define_properties(env, exports, 1, &desc2);
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
  napi_property_descriptor desc8 = DECLARE_NAPI_METHOD("getTestsFromFile", testCases);
  status = napi_define_properties(env, exports, 1, &desc8);
  napi_property_descriptor desc9 = DECLARE_NAPI_METHOD("createTemplate", createTemplate);
  status = napi_define_properties(env, exports, 1, &desc9);
  napi_property_descriptor desc10 = DECLARE_NAPI_METHOD("addExistingTest", addExistingTest);
  status = napi_define_properties(env, exports, 1, &desc10);
  napi_property_descriptor desc11 = DECLARE_NAPI_METHOD("runTestsInTemplate", runAllTestsInTemplate);
  status = napi_define_properties(env, exports, 1, &desc11);
  napi_property_descriptor desc12 = DECLARE_NAPI_METHOD("deleteTemplate", deleteTemplate);
  status = napi_define_properties(env, exports, 1, &desc12);
  napi_property_descriptor desc13 = DECLARE_NAPI_METHOD("removeTest", removeExistingTest);
  status = napi_define_properties(env, exports, 1, &desc13);
  napi_property_descriptor desc14 = DECLARE_NAPI_METHOD("openLastReport", openLastReport);
  status = napi_define_properties(env, exports, 1, &desc14);
  napi_property_descriptor desc15 = DECLARE_NAPI_METHOD("setup", findStoredTempls);
  status = napi_define_properties(env, exports, 1, &desc15);
  napi_property_descriptor desc16 = DECLARE_NAPI_METHOD("deleteFile", deleteFile);
  status = napi_define_properties(env, exports, 1, &desc16);
  napi_property_descriptor desc17 = DECLARE_NAPI_METHOD("getTestsInTemplate", getTestsInTemplate);
  status = napi_define_properties(env, exports, 1, &desc17);
  napi_property_descriptor desc18 = DECLARE_NAPI_METHOD("gitClone", gitClone);
  status = napi_define_properties(env, exports, 1, &desc18);
  assert(status == napi_ok);
  return exports;
}

NAPI_MODULE(testaddon, Init)
