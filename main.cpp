//файл для тестирования; в финальную сборку включать не стоит; тут половина вещей не работает

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <assert.h>
#include <map>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <unistd.h>

#if (defined(_POSIX_VERSION))
#define _popen popen
#define _pclose pclose
#include "parser.h"
#elif (defined(_WIN32) || defined(_WIN32_))
#include "winFunctions.h"
#endif

namespace fs = std::filesystem;
using std::string;
using std::wstring;
using fs::path;

const path B2C_TESTS = "b2c_tests_pw/tests";
const path B2B_REGRESS = "b2b_tests_pw/regress b2b/tests";
const path B2B_SMOKE = "b2b_tests_pw/smoke_b2b/tests";

string command; //что мы запустим при следующем вызове system()

path chosen;

const std::map<string, string> hardCodeOptions = {{"headed", "--headed"}, {"headless", "--headless"}, {"firefox", "-firefox"}, {"chromium", "-chromium"}, {"webkit", "-webkit"}};

string readFileName(const string& p){
    string fullName = p.substr(p.find_last_of("/\\") + 1);
    return fullName;
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

void addCommand(const string& add){
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

string exec(const char* cmd) {
    std::array<char, 128> buffer;
    string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("_popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}


void runTest(const path& test){
    string s = "npx playwright test ";
    s += readFileName(test.string());
    addCommand(s);
    std::cout << "npx output: \n";
    system(s.c_str());
    removeLastCommand();
}

void runAllTests(){
    for (auto &p : fs::directory_iterator(chosen)){
        runTest(p);
    }
}

void mkdir(const string &name){
    string c = "mkdir ";
    c += name;
    addCommand(c);
    system(command.c_str());
    removeLastCommand();
}

template <class T> void printType(const T&)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

string sshKeygen(){
    system("ssh-keygen\n\n");
    system("cd /home/vscode/.ssh");
    string res = exec("cat id_rsa.pub");
    return res;
}

string gitClone(const string& user, const string& password, const string& basicLink){
    int pos = basicLink.find('/');
    if (pos == string::npos){
        throw std::runtime_error("link not valid");
    }
    string link = basicLink.substr(0, pos + 2);
    link += user;
    link += ":";
    link += password;
    link += "@";
    link += basicLink.substr(pos + 2);
    string com = "git clone " + link;
    std::cout << com << "\n";
    string res = exec(com.c_str());
    return res;
}

static bool keepRunning = true;

void intHandler(int) {
    std::cout << "want to quit for sure?\n";
    char c;
    std::cin >> c;
    if (c == 'y'){
        keepRunning = false;
    }
}

void stop(){
    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);
}

int main(){
    // setlocale(LC_ALL, "russian");
    // chooseTests("b2c");
    // resetStreamOrientation();
    // string test = "menu.spec.ts";
    // string x = "ярусский";
    // std::cout << x << "\n";
    // std::vector<string> v = winParser((chosen / test).string()).getAllTests();
    // for (auto &elem : v){
    //     std::cout << elem << std::endl;
    // }
    int i = 0;
    while (keepRunning){
        stop();
        std::cout << "going through loop " << i << "\n";
        sleep(10);
        i++;
    }
    return 0;
}
