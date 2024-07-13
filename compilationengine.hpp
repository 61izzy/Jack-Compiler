#ifndef COMPILATIONENGINE_HPP
#define COMPILATIONENGINE_HPP

#include "tokenizer.hpp"

class CompilationEngine {
private:
    Tokenizer &tokenizer;
    std::ofstream &output;
    std::unordered_set<std::string> ops{
        "+",
        "-",
        "*",
        "/",
        "&",
        "|",
        "<",
        ">",
        "="
    };
    inline void compileType();
    inline void compileIdentifier();
    inline void compileKeyword();
    inline void compileSymbol();

public:
    CompilationEngine(Tokenizer &tokenizer, std::ofstream &output) : tokenizer{tokenizer}, output{output} {
        compileClass();
    }
    void compileClass();
    void compileClassVarDec();
    void compileSubroutineDec();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileLet();
    void compileIf();
    void compileWhile();
    void compileDo();
    void compileReturn();
    void compileExpression();
    void compileTerm();
    void compileExpressionList();
};

#endif