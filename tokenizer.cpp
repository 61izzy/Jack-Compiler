#include "utils.hpp"
#include "tokenizer.hpp"

void Tokenizer::tokenizeFile() {
    // preprocesses the input file and stores all of the tokens along with their respective types
    // this is much easier than tokenizing as we process the file
    std::string currLine, currToken = "";
    bool isComment = false, isInt = false, isString = false;
    while (std::getline(input, currLine)) {
        if (isComment) {
            if (currLine.find("*/") == std::string::npos) continue;
            isComment = false;
            if (currLine.find("*/") + 2 == currLine.length()) continue;
            currLine = currLine.substr(currLine.find("*/") + 2);
        }
        for (int i = 0; i < currLine.length(); i++) {
            if (std::isspace(currLine[i]) && !isString) continue;
            if (currLine[i] == '/' && i + 1 < currLine.length() && (currLine[i + 1] == '/' || currLine[i + 1] == '*')) {
                if (currLine[i + 1] == '*') {
                    if (currLine.find("*/", i + 2) != std::string::npos) {
                        i = currLine.find("*/", i + 2) + 1;
                        continue;
                    }
                    else {
                        isComment = true;
                        break;
                    }
                }
                else break;
            }
            if (currLine[i] == '"') {
                if (isString) {
                    tokens.push_back(currToken);
                    types.push_back(STRING_CONST);
                    currToken = "";
                    isString = false;
                }
                else isString = true;
                continue;
            }
            if (currToken == "" && std::isdigit(currLine[i])) isInt = true;
            currToken += currLine[i];
            if (symbols.find(currToken) != symbols.end()) {
                tokens.push_back(currToken);
                types.push_back(SYMBOL);
                currToken = "";
                continue;
            }
            if (isString) continue;
            if (i + 1 == currLine.length() || std::isspace(currLine[i + 1]) || symbols.find(std::string{currLine[i + 1]}) != symbols.end()) {
                tokens.push_back(currToken);
                if (isInt) {
                    types.push_back(INT_CONST);
                    isInt = false;
                }
                else if (keywordTable.find(currToken) != keywordTable.end()) types.push_back(KEYWORD);
                else types.push_back(IDENTIFIER);
                currToken = "";
            }
        }
    }
}