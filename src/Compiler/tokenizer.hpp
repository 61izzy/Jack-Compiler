#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

class Tokenizer {
private:
    int index = -1; // we call advance before most compilexxx methods, so we don't want to accidentally skip the first token
    std::ifstream &input;
    std::vector<std::string> tokens;
    std::vector<TokenType> types;
    std::unordered_map<std::string, int> keywordTable { // inconvenient to do enum containing NULL
        {"class", 0},
        {"method", 1},
        {"function", 2},
        {"constructor", 3},
        {"int", 4},
        {"boolean", 5},
        {"char", 6},
        {"void", 7},
        {"var", 8},
        {"static", 9},
        {"field", 10},
        {"let", 11},
        {"do", 12},
        {"if", 13},
        {"else", 14},
        {"while", 15},
        {"return", 16},
        {"true", 17},
        {"false", 18},
        {"null", 19},
        {"this", 20}
    };
    std::unordered_set<std::string> symbols {
        "{",
        "}",
        "(",
        ")",
        "[",
        "]",
        ".",
        ",",
        ";",
        "+",
        "-",
        "*",
        "/",
        "&",
        "|",
        "<",
        ">",
        "=",
        "~"
    };
    void tokenizeFile();

public:
    Tokenizer(std::ifstream &input) : input{input} {
        tokenizeFile();
    }
    inline bool hasMoreTokens() {
        return index + 1 < tokens.size();
    }
    inline std::string advance() {
        return tokens[++index];
    }
    inline std::string peek() { // get the value of next token without advancing
        return tokens[index + 1];
    }
    inline TokenType tokenType() {
        return types[index];
    }
    inline TokenType peekType() { // get the type of next token without advancing
        return types[index + 1];
    }
    inline std::string keyWord() {
        return tokens[index];
    }
    inline std::string symbol() {
        return tokens[index];
    }
    inline std::string identifier() {
        return tokens[index];
    }
    inline int intVal() {
        return stoi(tokens[index]);
    }
    inline std::string stringVal() {
        return tokens[index];
    }
};

#endif