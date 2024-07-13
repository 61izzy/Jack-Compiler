#include "utils.hpp"
#include "compilationengine.hpp"

// pretty sure theres no need to check if hasNextToken before peeking since we're assuming that the code is correct anyways
// this means that none of the compiles should end there

// all of the methods below should be pretty self-explanatory
// Jack grammar reference: nand2tetris lecture 10 pg 41

void CompilationEngine::compileClass() {
    output << "<class>\n";
    compileKeyword();
    compileIdentifier();
    compileSymbol();
    while (tokenizer.peek() == "static" || tokenizer.peek() == "field") compileClassVarDec();
    while (tokenizer.peekType() == KEYWORD) compileSubroutineDec();
    compileSymbol();
    output << "</class>\n";
}
void CompilationEngine::compileClassVarDec() {
    output << "<classVarDec>\n";
    compileKeyword();
    compileType();
    compileIdentifier();
    while (tokenizer.peek() == ",") {
        compileSymbol();
        compileIdentifier();
    }
    compileSymbol();
    output << "</classVarDec>\n";
}
void CompilationEngine::compileSubroutineDec() {
    output << "<subroutineDec>\n";
    compileKeyword();
    compileType();
    compileIdentifier();
    compileSymbol();
    compileParameterList();
    compileSymbol();
    compileSubroutineBody();
    output << "</subroutineDec>\n";
}
void CompilationEngine::compileParameterList() {
    output << "<parameterList>\n";
    if (tokenizer.peekType() != SYMBOL) {
        compileType();
        compileIdentifier();
    }
    while (tokenizer.peek() == ",") {
        compileSymbol();
        compileType();
        compileIdentifier();
    }
    output << "</parameterList>\n";
}
void CompilationEngine::compileSubroutineBody() {
    output << "<subroutineBody>\n";
    compileSymbol();
    while (tokenizer.peek() == "var") compileVarDec();
    compileStatements();
    compileSymbol();
    output << "</subroutineBody>\n";
}
void CompilationEngine::compileVarDec() {
    output << "<varDec>\n";
    compileKeyword();
    compileType();
    compileIdentifier();
    while (tokenizer.peek() == ",") {
        compileSymbol();
        compileIdentifier();
    }
    compileSymbol();
    output << "</varDec>\n";
}
void CompilationEngine::compileStatements() {
    output << "<statements>\n";
    while (tokenizer.peekType() == KEYWORD) {
        if (tokenizer.peek() == "let") compileLet();
        else if (tokenizer.peek() == "if") compileIf();
        else if (tokenizer.peek() == "while") compileWhile();
        else if (tokenizer.peek() == "do") compileDo();
        else if (tokenizer.peek() == "return") compileReturn();
        else return;
    }
    output << "</statements>\n";
}
void CompilationEngine::compileLet() {
    output << "<letStatement>\n";
    compileKeyword();
    compileIdentifier();
    while (tokenizer.peek() == "[") { // honestly not sure if Jack supports multidimensional arrays, but just in case
        compileSymbol();
        compileExpression();
        compileSymbol();
    }
    compileSymbol();
    compileExpression();
    compileSymbol();
    output << "</letStatement>\n";
}
void CompilationEngine::compileIf() {
    output << "<ifStatement>\n";
    compileKeyword();
    compileSymbol();
    compileExpression();
    compileSymbol();
    compileSymbol();
    compileStatements();
    compileSymbol();
    if (tokenizer.peek() == "else") {
        compileKeyword();
        compileSymbol();
        compileStatements();
        compileSymbol();
    }
    output << "</ifStatement>\n";
}
void CompilationEngine::compileWhile() {
    output << "<whileStatement>\n";
    compileKeyword();
    compileSymbol();
    compileExpression();
    compileSymbol();
    compileSymbol();
    compileStatements();
    compileSymbol();
    output << "</whileStatement>\n";
}
void CompilationEngine::compileDo() {
    output << "<doStatement>\n";
    compileKeyword();
    compileTerm();
    compileSymbol();
    output << "</doStatement>\n";
}
void CompilationEngine::compileReturn() {
    output << "<returnStatement>\n";
    compileKeyword();
    if (tokenizer.peek() != ";") compileExpression();
    compileSymbol();
    output << "</returnStatement>\n";
}
void CompilationEngine::compileExpression() {
    output << "<expression>\n";
    compileTerm();
    if (ops.find(tokenizer.peek()) != ops.end()) {
        compileSymbol();
        compileTerm();
    }
    output << "</expression>\n";
}
void CompilationEngine::compileTerm() {
    bool isDoStatement = tokenizer.stringVal() == "do";
    if (!isDoStatement) output << "<term>\n"; // printing <term></term> after a do statement fails comparison when testing
    tokenizer.advance();
    switch (tokenizer.tokenType()) {
        case IDENTIFIER:
            if (tokenizer.peek() == ".") {
                output << "<identifier>" << tokenizer.identifier() << "</identifier>\n";
                compileSymbol();
                tokenizer.advance();
            }
            output << "<identifier>" << tokenizer.identifier() << "</identifier>\n";
            if (tokenizer.peek() == "(" || tokenizer.peek() == "[") { // in case Jack supports multidimensional arrays
                compileSymbol();
                if (tokenizer.symbol() == "(") compileExpressionList();
                else compileExpression();
                compileSymbol();
            }
            break;
        case SYMBOL:
            output << "<symbol>" << tokenizer.symbol() << "</symbol>\n";
            if (tokenizer.symbol() == "(" || tokenizer.symbol() == "[") {
                compileExpression();
                compileSymbol();
            }
            else compileTerm();
            break;
        case INT_CONST:
            output << "<integerConstant>" << tokenizer.intVal() << "</integerConstant>\n";
            break;
        case STRING_CONST:
            output << "<stringConstant>" << tokenizer.stringVal() << "</stringConstant>\n";
            break;
        case KEYWORD:
            output << "<keyword>" << tokenizer.keyWord() << "</keyword>\n";
            break;
    }
    if (!isDoStatement) output << "</term>\n";
}
void CompilationEngine::compileExpressionList() {
    output << "<expressionList>\n";
    while (tokenizer.peek() != ")") {
        if (tokenizer.peek() == ",") compileSymbol();
        compileExpression();
    }
    output << "</expressionList>\n";
}
inline void CompilationEngine::compileType() {
    if (tokenizer.peekType() == KEYWORD) compileKeyword();
    else compileIdentifier();
}
inline void CompilationEngine::compileIdentifier() {
    tokenizer.advance();
    output << "<identifier>" << tokenizer.identifier() << "</identifier>\n";
}
inline void CompilationEngine::compileKeyword() {
    tokenizer.advance();
    output << "<keyword>" << tokenizer.keyWord() << "</keyword>\n";
}
inline void CompilationEngine::compileSymbol() {
    tokenizer.advance();
    output << "<symbol>" << tokenizer.symbol() << "</symbol>\n";
}