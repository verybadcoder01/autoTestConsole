#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <assert.h>
#include <map>
#include <unistd.h>

#if (defined(_POSIX_VERSION))
#define _popen popen
#define _pclose pclose
#endif


namespace fs = std::filesystem;
using std::string;
using std::wstring;
using fs::path;

const path B2C_TESTS = "b2c_tests_pw/tests";
const path B2B_REGRESS = "b2b_tests_pw/regress b2b/tests";
const path B2B_SMOKE = "b2b_tests_pw/smoke_b2b/tests";
const string NPX = "npx playwright test ";
string command; //что мы запустим при следующем вызове system()

path chosen;

string readFileName(const path& p){
    return p.filename();
}

std::vector<string> getFileNames(const path& library){
    std::vector<string> res;
    for (const auto& file : fs::directory_iterator(library)){
        res.push_back(readFileName(file.path().string()));
    }
    return res;
}

void printFileNames(const path& library){
    for (auto &s : getFileNames(library)){
        std::cout << s << "\n";
    }
}

void chooseTests(const string& name){
    if (name == "b2c"){
        chosen = B2C_TESTS;
    } else if (name == "b2b-smoke"){
        chosen = B2B_SMOKE;
    } else if (name == "b2b-regress"){
        chosen = B2B_REGRESS;
    } else {
        assert(false);
    }
    printFileNames(chosen);
    command = "cd " + chosen.string();
}

string exec(const char* cmd) {
    std::array<char, 128> buffer;
    string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) { //мы не смогли открыть командную строку
        throw std::runtime_error("_popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) { //получаем вывод
        result += buffer.data();
    }
    return result;
}

void addCommand(const string& add){
    if (add.empty()){
        return;
    }
    if (!command.empty()){
        command += " && ";
    }
    command += add;
}

void removeLastCommand(){
    if (command.find('&') == string::npos){
        command.clear();
        return;
    }
    while (command.back() != '&'){
        command.pop_back();
    }
    command.pop_back(); //стереть первый &
    command.pop_back(); //стереть второй &
    command.pop_back(); //стереть пробел
}

void runTest(const path& test){
    string s = NPX;
    s += readFileName(test.string());
    addCommand(s);
    std::cout << "npx output: \n";
    system(s.c_str());
    removeLastCommand();
}

void mkdir(const string &name){
    string c = "mkdir ";
    c += name;
    addCommand(c);
    // string s = exec(command.c_str());
    // std::cout << s << "\n";
    system(command.c_str());
    removeLastCommand();
}

void runAllTests(){
    for (auto &p : fs::directory_iterator(chosen)){
        runTest(p);
    }
}
