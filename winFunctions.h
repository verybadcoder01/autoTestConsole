#include <bits/stdc++.h>

namespace fs = std::filesystem;
using std::string;
using fs::path;
using std::wstring;

struct winParser
{
    string filePathStr;
    path filePath;
    std::vector<wstring> toFind = {L"test.", L"test("}; //с чего начинается описание теста

    winParser() = default;
    explicit winParser(const string& path){
        filePath = fs::path(path);
        filePathStr = path;
    }

    std::vector<wstring> getAllTests(){
        std::wifstream inFile;
        inFile.open(filePathStr); //открываем файл
        wstring curLine;
        std::vector <wstring> res;
        while (getline(inFile, curLine)){ //считываем все
            for (const auto &s : toFind){
                size_t ind = curLine.find(s);
                if (ind != string::npos){ //если есть описание теста на этой строке
                    size_t startPos = curLine.find(L"'"); //все описания написаны в одинарных кавычках
                    if (startPos != string::npos && startPos > ind){ //второе условие нужно чтобы отсечь ссылки (типо 'https://b2b-test.rusklimat.net')
                        res.push_back(curLine.substr(startPos + 1, curLine.rfind(L"'") - startPos - 1));
                    }
                }
            }
        }
        return res;
    }
};


void resetStreamOrientation(){ //делает поток вывода нейтральным для cout и wcout
    int mode = fwide(stdout, 0);
    if (mode == 0){
        return;
    }
    freopen(NULL, "w", stdout);
    mode = fwide(stdout, 0);
}
