/*
- gets input from tokenizer
- a bunch of compilexxx routines, one for almost every non-terminal rule xxx in the grammar
- each routine is responsible for handling all tokens that make up xxx, advancing the tokenizer right after these tokens and outputing the parsing of xxx
- use provided table to lookup grammar


API:
- constructor(tokenizer (acting as input), output file): creates new compilationengine object and calls compileClass
- compileClass(): compiles a complete class
- compileClassVarDec(): compiles a static variable declaration or field declaration
- compileSubroutineDec(): compiles a complete method, function, or constructor
- compileParameterList(): compiles a compiler list (possibly empty)
- compileSubroutineBody(): compiles a subroutine's body
- compileVarDec(): compiles a var dec
- compileStatements(): compiles a seq of statements without handling curly braces

statments (all pretty self explanatory):
- compileLet()
- compileIf()
- compileWhile()
- compileDo()
    - will call compileTerm() ig?
- compileReturn()

expressions:
- compileExpression(): self explanatory
- compileTerm():
    - if curr token is identifier, must distinguish between variable, array entry, or subroutine call by looking ahead one token, else should not be advanced
        - prob add tokenizer.next/getNext() or smth
    - also handles subroutine calls
- compileExpressionList(): compiles a list of comma separated expressions (possibly empty)
*/

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