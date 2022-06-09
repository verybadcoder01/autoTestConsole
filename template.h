#pragma once;
#include <bits/stdc++.h>
#include "commandHelper.h"
namespace fs = std::filesystem;
using std::string;
using fs::path;

//forward declaration (это плохо?)
extern const path B2C_TESTS, B2B_SMOKE, B2B_REGRESS;
extern path chosen;
extern string command;
std::vector<string> chooseTests(const string& name);
void mkdir(const string &name);
void runAllTests();
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
        mkdir(name);
    }

    Template(const string& enumedName, const string& _name){
        name = _name;
        chooseTests(enumedName);
        baseDir = chosen;
        mkdir(name);
    }

    void addExistingTest(const string& test){
        addCommand(string("cp " + test + " " + name));
        std::cout << command << "\n";
        system(command.c_str());
        removeLastCommand();
        includedTests.push_back(test);
    }
    
    void runAllIncluded(){
        path location = chosen;
        chosen /= name;
        addCommand(string("cd " + name));
        runAllTests();
        chosen = location;
        removeLastCommand();
    }

};
