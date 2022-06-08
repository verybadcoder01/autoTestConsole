#pragma once
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
#include "parser.h"
#elif (defined(_WIN32) || defined(_WIN32_))
#include "winFunctions.h"
#endif


namespace fs = std::filesystem;
using std::string;
using std::wstring;
using fs::path;
//относительные пути
const path B2C_TESTS = "b2c_tests_pw/tests";
const path B2B_REGRESS = "b2b_tests_pw/regress b2b/tests";
const path B2B_SMOKE = "b2b_tests_pw/smoke_b2b/tests";
const string NPX = "npx playwright test ";
string command; //что мы запустим при следующем вызове system()

path chosen; //текущая рабочая папка

string readFileName(const path& p){ //принимает путь к файлу и возвращет только имя файла
    return p.filename();
}

std::vector<string> getFileNames(const path& library){ //возвращает имена всех файлов в папке
    std::vector<string> res;
    for (const auto& file : fs::directory_iterator(library)){
        res.push_back(readFileName(file.path().string()));
    }
    return res;
}

void printFileNames(const path& library){ //выводит имена всех файлов в папке
    for (auto &s : getFileNames(library)){
        std::cout << s << "\n";
    }
}

std::vector<string> chooseTests(const string& name){ //устанавливает место работы (папку с тестами). Записывает cd в это место
    if (name == "b2c"){
        chosen = B2C_TESTS;
    } else if (name == "b2b-smoke"){
        chosen = B2B_SMOKE;
    } else if (name == "b2b-regress"){
        chosen = B2B_REGRESS;
    } else {
        assert(false);
    }
    command = "cd " + chosen.string();
    return getFileNames(chosen);
}

string exec(const char* cmd) { //запускает команду и возвращает вывод командной строки; почти не используется
    std::array<char, 128> buffer;
    string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) { //не получилось открыть командную строку
        throw std::runtime_error("_popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) { //получаем вывод
        result += buffer.data();
    }
    return result;
}

void addCommand(const string& add){ //записывает командну на выполнение. Все команды пишутся в одну строку и разделяются &&
    if (add.empty()){
        return;
    }
    if (!command.empty()){
        command += " && ";
    }
    command += add;
}

void removeLastCommand(){ //удаляет последнюю записанную команду с && и лишними пробелами
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

void runTest(const path& test){ //принимает путь, запускает тест, который там лежит.
    string s = NPX;
    s += readFileName(test.string());
    addCommand(s);
    std::cout << "npx output: \n";
    system(command.c_str());
    removeLastCommand();
}

void mkdir(const string &name){ //создает директорию с именем name в текущей рабочей папке. Не переходит туда.
    string c = "mkdir ";
    c += name;
    addCommand(c);
    system(command.c_str());
    removeLastCommand();
}

void runAllTests(){ //запускает все тесты в текущей рабчоей папке
    for (auto &p : fs::directory_iterator(chosen)){
        runTest(p);
    }
}
