#ifndef COMPILATIONENGINE_HPP
#define COMPILATIONENGINE_HPP

#include "tokenizer.hpp"
#include "vmwriter.hpp"

// the expectxxx methods are in case this project gets extended with error checking
// when the current token differs from the expected value(s), we can throw an error

class CompilationEngine {
private:
    int fieldCount = 0, staticCount = 0, localCount, labelCount = 0;
    std::vector<int> argCount; // using a vector as a stack to keep track of the number of arguments passed into each function call
    Tokenizer &tokenizer;
    VMWriter &writer;
    std::string className, subroutineName;
    bool isMethod, isConstructor;
    std::unordered_map<std::string, std::string> unaryOps {
        {"-", "neg"},
        {"~", "not"}
    };
    std::unordered_map<std::string, std::string> binaryOps {
        {"+", "add"},
        {"-", "sub"},
        {"*", "call Math.multiply 2"},
        {"/", "call Math.divide 2"},
        {"&", "and"},
        {"|", "or"},
        {"<", "lt"},
        {">", "gt"},
        {"=", "eq"}
    };
    std::unordered_map<std::string, std::tuple<std::string, std::string, int>> classTable, subroutineTable;
    inline void expectKeyword(const std::string &expected) {
        tokenizer.advance();
    }
    inline void expectKeyword(const std::unordered_set<std::string> &expected) {
        tokenizer.advance();
    }
    inline void expectIdentifier() {
        tokenizer.advance();
    }
    inline void expectSymbol(const std::string &expected) {
        tokenizer.advance();
    }
    inline void expectSymbol(const std::unordered_map<std::string, std::string> &expected) {
        tokenizer.advance();
    }
    inline void expectType() {
        if (tokenizer.peekType() == KEYWORD) expectKeyword(std::unordered_set<std::string>{"bool", "int", "char"});
        else expectIdentifier();
    }
    inline void compileVar(const std::string &name, bool push) {
        if (subroutineTable.find(name) != classTable.end()) {
            if (push) writer.writePush(std::get<1>(subroutineTable[name]), std::get<2>(subroutineTable[name]));
            else writer.writePop(std::get<1>(subroutineTable[name]), std::get<2>(subroutineTable[name]));
        }
        else if (classTable.find(name) != classTable.end()) {
            if (push) writer.writePush(std::get<1>(classTable[name]), std::get<2>(classTable[name]));
            else writer.writePop(std::get<1>(classTable[name]), std::get<2>(classTable[name]));
        }
        else {
            // for error checking purposes in the future
        }
    }

public:
    CompilationEngine(Tokenizer &tokenizer, VMWriter &writer) : tokenizer{tokenizer}, writer{writer} {
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