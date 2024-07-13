#include "utils.hpp"
#include "tokenizer.hpp"

void Tokenizer::tokenizeFile() {
    // preprocesses the input file and stores all of the tokens along with their respective types
    // this is much easier than tokenizing as we process the file
    std::string currLine, currToken = "";
    bool isComment = false, isInt = false, isString = false;

    while (std::getline(input, currLine)) {
        // if a multiline comment continues on current line
        if (isComment) {
            if (currLine.find("*/") == std::string::npos) continue; // skip current line if fully contained within comment
            isComment = false;
            if (currLine.find("*/") + 2 == currLine.length()) continue; // else start parsing the rest of the line
            currLine = currLine.substr(currLine.find("*/") + 2);
        }
        for (int i = 0; i < currLine.length(); i++) {
            // skip whitespaces in code, but don't skip them in string constants
            if (std::isspace(currLine[i]) && !isString) continue;

            // checking if it's the start of a comment
            if (currLine[i] == '/' && i + 1 < currLine.length() && (currLine[i + 1] == '/' || currLine[i + 1] == '*')) {
                // start of multiline comment
                if (currLine[i + 1] == '*') {
                    // if it ends on the same line, we can still parse the rest of the line; else we skip
                    if (currLine.find("*/", i + 2) != std::string::npos) {
                        i = currLine.find("*/", i + 2) + 1;
                        continue;
                    }
                    else {
                        isComment = true;
                        break;
                    }
                }
                // start of single line comment, can skip the rest of the line
                else break;
            }
            if (currLine[i] == '"') {
                if (isString) { // end of a string constant, reset
                    tokens.push_back(currToken);
                    types.push_back(STRING_CONST);
                    currToken = "";
                    isString = false;
                }
                else isString = true;
                continue;
            }
            // identifiers and keywords can't start with an integer, so it must be an integer constant
            if (currToken == "" && std::isdigit(currLine[i])) isInt = true;
            currToken += currLine[i];
            if (symbols.find(currToken) != symbols.end()) { // found symbol, reset
                tokens.push_back(currToken);
                types.push_back(SYMBOL);
                currToken = "";
                continue;
            }
            if (isString) continue; // for strings, don't stop until closing quotation mark is found
            // end of current token, add token and corresponding type and reset
            if (i + 1 == currLine.length() || !isalnum(currLine[i + 1]) && currLine[i + 1] != '_') {
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