/*
Responsible for:
- ignoring whitespace
- advancing input one token at a time
- getting value and type of current token


API:
- constructor(input file): opens jack file
- hasMoreTokens() -> bool: returns if there are more tokens
- advance(): returns next token and makes it current token. should only be called if hasMoreTokens() is true
- tokenType() -> tokenType: returns type of current token as a constant (KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST)

- each of the methods below should only be called if the current token is the corresponding type 
- keyWord() -> keyword: returns one of (CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN, CHAR, VOID, VAR, STATIC, FIELD, LET, DO, IF, ELSE, WHILE, 
    RETURN, TRUE, FALSE, NULL, THIS)
    - prob create lookup table for this
- symbol() -> char: return current token as char
- identifier() -> string: return current token as string
- intVal() -> int: return current token as int (use stoi)
- stringVal() -> string: return current token without the double quotes (though that should be taken care of when parsing/advancing)


- the play is probably not to use str.find(), but rather to go through each line one char at a time
- keep two vectors (or queues, but probably vectors): one for each token and one for each token type
*/

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

class Tokenizer {
private:
    int index = -1;
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
    inline TokenType peekType() {
        return types[index + 1];
    }
    // inline int keyWord() {
    //     return keywordTable[tokens[index]];
    // }
    inline std::string keyWord() {
        return tokens[index];
    }
    inline std::string symbol() {
        if (tokens[index] == "<") return "&lt;";
        if (tokens[index] == ">") return "&gt;";
        if (tokens[index] == "&") return "&amp;";
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