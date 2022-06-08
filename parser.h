#include <bits/stdc++.h>
#include <unistd.h>

#if (defined(_POSIX_VERSION))
#define wcout cout
#define wstring string
#define wifstream ifstream
#endif

namespace fs = std::filesystem;
using std::string;
using fs::path;
using std::wstring;
const string search = "'"; 

struct Parser //получает названия всех тестов в файле
{
    string filePathStr;
    path filePath;
    std::vector<string> toFind = {"test.", "test("}; //с чего начинается описание теста

    Parser() = default;
    explicit Parser(const string& path){
        filePath = fs::path(path);
        filePathStr = path;
    }

    std::vector<string> getAllTests(){
        std::ifstream inFile;
        inFile.open(filePathStr); //открываем файл
        string curLine;
        std::vector <string> res;
        while (getline(inFile, curLine)){ //считываем все
            for (const auto &s : toFind){
                size_t ind = curLine.find(s);
                if (ind != string::npos){ //если есть описание теста на этой строке
                    size_t startPos = curLine.find("'"); //все описания написаны в одинарных кавычках
                    if (startPos != string::npos && startPos > ind){ //второе условие нужно чтобы отсечь ссылки (типо 'https://b2b-test.rusklimat.net')
                        res.push_back(curLine.substr(startPos + 1, curLine.rfind("'") - startPos - 1));
                    }
                }
            }
        }
        return res;
    }
};
