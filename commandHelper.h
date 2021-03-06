#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <assert.h>
#include <map>
#include <unistd.h>
#include <future>
#include <thread>
#include "template.h"
//TODO: aaaaaaaaaaaaaa
//автоматическое клонирование: https://user:password@gitlab.rusklimat.ru/user/project.git

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
const path B2B_REGRESS = "b2b_tests_pw/regress_b2b/tests";
const path B2B_SMOKE = "b2b_tests_pw/smoke_b2b/tests";
const string NPX = "npx playwright test ";
string command; //что мы запустим при следующем вызове system()

path chosen; //текущая рабочая папка
std::map <string, Template> templs;
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

string liveExec(const char* cmd){ //запускает команду, возвращает результат. Также пишет его в консоль
    std::array<char, 128> buffer;
    string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) { //не получилось открыть командную строку
        throw std::runtime_error("_popen() failed!");
    }
    string s;
    std::regex remove{R"((\x9B|\x1B\[)[0-?]*[ -\/]*[@-~])"}; //регулярка на все escape-последовательности (последствия считывания из терминала)
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) { //получаем вывод
        s = buffer.data();
        s = std::regex_replace(s, remove, ""); //убираем их
        result += s;
        std::cout << s; //лайв-вывод результатов
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

void killAll(const string& arg){ //убивает все процессы с подстрокой arg
    string s = exec(string("pgrep -f \"" + arg + "\"").c_str());
    string tmp;
    for (int i = 0; i < (int)s.size(); ++i){
        int j = i;
        while (j < (int)s.size() && isdigit(s[j])){
            tmp.push_back(s[j]);
            j++;
        }
        i = j;
        if (!tmp.empty()){
            std::cout << exec(string("kill " + tmp).c_str()) << "\n";
            tmp.clear();
        }
    }
}

void stop(){ //достаем катану
    std::cout << "stopping\n";
    killAll("npm exec"); //режущий слева
    killAll("test tests/"); //рубящий сверху
    killAll("chromium"); //режущий справа
}

string runTest(const string& test){ //принимает путь, запускает тест, который там лежит.
    string s = NPX;
    s += test;
    addCommand(s);
    std::cout << command << "\n";
    std::cout << "npx output: \n";
    string r = liveExec(command.c_str());
    sleep(3);
    removeLastCommand();
    return r;
}

void mkdir(const string &name){ //создает директорию с именем name в текущей рабочей папке. Не переходит туда.
    string c = "mkdir ";
    c += name;
    addCommand(c);
    system(command.c_str());
    removeLastCommand();
}

void removeTemplate(const string& name){ //удаляет шаблон по названию
    if (templs.find(name) == templs.end()){ //удалять нечего
        throw std::runtime_error("template with this name does not exist");
    }
    addCommand(string("rm -rf " + name));
    system(command.c_str());
    templs[name].removeInformation();
    templs.erase(name);
}

void openLastReport(){
    addCommand("cd ..");
    addCommand("npx playwright show-report");
    system(command.c_str());
    removeLastCommand();
    removeLastCommand();
}

void findStoredTemplates(){
    std::ifstream in(INFORMATION);
    if (!in){
        throw std::runtime_error("templates.txt not found. Something has gone horribly wrong");
    }
    string curLine, curName, curBaseDir;
    std::vector <string> curTests;
    while(getline(in, curLine)){
        if (curLine[0] == 'n'){ //n for "name"
            curName = curLine.substr(curLine.find(LINE_SEPARATOR) + 1);
        } else if (curLine[0] == 'i'){ //i for "includedTests" aka все тесты, бывшие в шаблоне
            int pos = curLine.find(LINE_SEPARATOR) + 1;
            string tmp;
            for (; pos < (int)curLine.size(); ++pos){
                if (curLine[pos] == TEST_SEPARATOR){ //сепаратор. Названия тестов разделяются ;
                    curTests.push_back(tmp);
                    tmp.clear();
                } else {
                    tmp.push_back(curLine[pos]);
                }
            }
        } else if (curLine[0] == 'b'){ //b for "baseDir"; это последняя строка в описании шаблона. Теперь его можно создавать
            curBaseDir = curLine.substr(curLine.find(LINE_SEPARATOR) + 1);
            templs[curName] = Template(curName, curTests, curBaseDir); //этот контструктор не вызывает создание информации
            curTests.clear();
        }
    }
}

void deleteFile(const string& name){ //удаляет файл с именем name из текущей рабочей папки
    addCommand("rm " + name);
    system(command.c_str());
    removeLastCommand();
}

string sshKeygen(){
    system("ssh-keygen");
    string res = exec("cd /home/vscode/.ssh && cat id_rsa.pub");
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
