#include <bits/stdc++.h>

namespace fs = std::filesystem;
using std::string;
using fs::path;
using std::wstring;
const string search = "'"; 

struct Parser //получает названия всех тестов в файле
{
    string filePathStr;
    path filePath;

    Parser() = default;
    explicit Parser(const string& path){
        filePath = fs::path(path);
        filePathStr = path;
    }

    std::vector<wstring> getAllTests(){
        std::wifstream inFile;
        inFile.open(filePathStr);
        wstring curLine;
        std::vector <wstring> res;
        std::vector <wstring> toFind = {L"test.", L"test("};
        while (getline(inFile, curLine)){
            //std::wcout << L"string is " << curLine << L"\n";
            for (auto &s : toFind){
                size_t ind = curLine.find(s);
                //std::wcout << ind << L"\n";
                if (ind != string::npos){
                    size_t startPos = curLine.find(L"'");
                    //std::wcout << "Position of " << "'" << " " << startPos << L"\n";
                    if (startPos != string::npos && startPos > ind){
                        res.push_back(curLine.substr(startPos + 1, curLine.rfind(L"'") - startPos - 1));
                    }
                }
            }
        }
        return res;
    }
};
