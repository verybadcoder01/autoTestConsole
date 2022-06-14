#pragma once
#include <bits/stdc++.h>
#include "commandHelper.h"
namespace fs = std::filesystem;
using std::string;
using fs::path;

const string INFORMATION = "templates.txt";
const char LINE_SEPARATOR = ':', TEST_SEPARATOR = ';';

//forward declaration (это плохо?)
extern const path B2C_TESTS, B2B_SMOKE, B2B_REGRESS;
extern path chosen;
extern string command;
std::vector<string> chooseTests(const string& name);
void mkdir(const string &name);
string runTest(const string& test);
void addCommand(const string& add);
void removeLastCommand();
//end of forward declarations

void chooseTestsWrappper(const path& p){
    if (p == B2C_TESTS){
        chooseTests("b2c");
    } else if (p == B2B_REGRESS){
        chooseTests("b2b-regress");
    } else if (p == B2B_SMOKE) {
        chooseTests("b2b-smoke");
    } else {
        assert(false);
    }
}

struct Template
{
    path baseDir;
    std::vector<string> includedTests;
    string name;

    Template() = default;
    Template(const path& dir, const string& _name){
        if (!fs::exists(dir)){
            throw std::runtime_error("invalid directory");
        }
        baseDir = dir;
        name = _name;
        chooseTestsWrappper(baseDir);
        generateSelfDescribingFile();
    }

    Template(const string& enumedName, const string& _name){
        name = _name;
        chooseTests(enumedName);
        baseDir = chosen;
        generateSelfDescribingFile();
    }

    Template(const string& _name, const std::vector<string> &tests, const path& _baseDir) : name(_name), includedTests(tests), baseDir(_baseDir) {};

    void addExistingTest(const string& test){
        if (find(includedTests.begin(), includedTests.end(), test) != includedTests.end()){ //если тест уже есть - ничего не делаем
            return;
        }
        includedTests.push_back(test);
        std::ifstream in(INFORMATION);
        bool isPrevLine = false, isFound = false;
        string curLine;
        std::vector<string> prevContent, nextContent;
        while (getline(in, curLine)){ //разделяет файл на то, что должно быть до названия теста, и после него
            if (isFound){
                isPrevLine = false;
                nextContent.push_back(curLine);
            } else {
                prevContent.push_back(curLine);
            }
            if (isPrevLine){
                int tmp = curLine.find(TEST_SEPARATOR); //ищем название последнего теста
                if (tmp != string::npos){
                    isFound = true;
                }
                if (!isFound){ //в этом шаблоне еще нет тестов
                    if (curLine.find(LINE_SEPARATOR) != string::npos){
                        isFound = true;
                    } else {
                        throw std::runtime_error("templates.txt has been corrupted"); //нет строки с тестами после имени файла, что-то пошло не так
                    }
                }
            }
            if (curLine[0] == 'n'){
                string cur = curLine.substr(curLine.find(LINE_SEPARATOR) + 1); //находим имя
                if (cur == name){
                    isPrevLine = true; //названия тестов всегда идут после имени шаблона
                }
            }
        }
        in.close();
        std::ofstream out(INFORMATION); //сначала все удаляем из файла
        for (int i = 0; i < (int)prevContent.size(); ++i){
            if (i != (int)prevContent.size() - 1){
                out << prevContent[i] << "\n";
            } else {
                out << prevContent[i];
            }
        }
        out << test << TEST_SEPARATOR << "\n";
        for (const auto& i : nextContent){
            out << i << "\n";
        }
        out.close();
    }

    void removeExistingTest(const string& test){
        if (find(includedTests.begin(), includedTests.end(), test) == includedTests.end()){ //теста нет - ничего делать не нужно
            return;
        }
        includedTests.erase(find(includedTests.begin(), includedTests.end(), test));
        std::ifstream in(INFORMATION);
        string curLine;
        bool isPrevLine = false;
        std::vector<string> content; //содержимое файла
        while (getline(in, curLine)){
            if (isPrevLine){
                int pos = curLine.find(test); //мы на нужной строчке, ищем подстроку с названием теста
                if (pos == string::npos){
                    throw std::runtime_error("templates.txt has been corrupted");
                }
                string result;
                for (int i = 0; i < (int)curLine.size(); ++i){
                    if (i >= pos && i <= pos + (int)test.size()){ //пока мы идем по названию теста, не добавляем символы в строку
                        continue;
                    }
                    result.push_back(curLine[i]); //а тут добавляем
                }
                content.push_back(result);
                isPrevLine = false; //больше ничего убирать не надо, осталось пройти по файлу и сохранить все
            } else {
                if (curLine[0] == 'n'){ //имя - первая строка в описании шаблона
                    string cur = curLine.substr(curLine.find(LINE_SEPARATOR) + 1);
                    if (cur == name){
                        isPrevLine = true;
                    }
                }
                content.push_back(curLine);
            }
        }
        in.close();
        std::ofstream out(INFORMATION); //теперь в content лежит все, кроме названия нашего теста
        for (const auto& i : content){ //выводим в файл
            out << i << "\n";
        }
        out.close();
    }
    
    string runAllIncluded(){
        if (includedTests.empty()){
            return "no tests in this template";
        }
        addCommand("cd ..");
        string alltests;
        string res;
        for (const auto &str : includedTests){ //запихаем все тесты в одну строку
            alltests += "tests/";
            alltests += str;
            alltests += " ";
        }
        res = runTest(alltests); //и запустим в одну команду
        removeLastCommand();
        return res;
    }

    void generateSelfDescribingFile(){ //добавляет шаблон в файл templates.txt в виде name:<name>, includedTests:<test1; test2; ...>, baseDir:<baseDir>
        std::ofstream out(INFORMATION, std::ios_base::app);
        out << "name:" << name << "\n";
        out << "includedTests:";
        for (int i = 0; i < (int)includedTests.size(); ++i){
            out << includedTests[i] << TEST_SEPARATOR;
        }
        out << "\n";
        out << "baseDir:" << baseDir.string();
        out << "\n";
        out.close();
    }

    void removeInformation(){ //удаляет информацию о данном шаблоне из templates.txt
        std::ifstream in(INFORMATION);
        if (!in){
            throw std::runtime_error("templates.txt does not exist. Something has gone horribly wrong");
        }
        string curLine;
        bool isFound = false;
        std::vector <string> content; //содержимое файла
        while (getline(in, curLine)){
            if (isFound){
                if (curLine[0] == 'n'){ //мы дошли до описания следующего шаблона
                    isFound = false;
                    content.push_back(curLine);
                } else {
                    continue; //мы в описании нашего шаблона, пропускаем
                }
            } else {
                if (curLine[0] == 'n'){ //название - первая строка в описании
                    string cur = curLine.substr(curLine.find(LINE_SEPARATOR) + 1); //получаем название
                    if (cur == name){
                        isFound = true;
                        continue;
                    }
                }
                content.push_back(curLine);
            }
        }
        in.close();
        std::ofstream out(INFORMATION); //теперь в content лежит все, кроме нашего шаблона
        for (const auto& i : content){ //выводим все в файл
            out << i << "\n";
        }
        out.close();
    }

};
