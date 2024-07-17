#include "utils.hpp"
#include "compilationengine.hpp"

// pretty sure theres no need to check if hasNextToken before peeking since we're assuming that the code is correct anyways
// this means that none of the compiles should end there

// all of the methods below should be pretty self-explanatory
// Jack grammar reference: nand2tetris lecture 10 pg 41s

void CompilationEngine::compileClass() {
    expectKeyword("class");
    expectIdentifier();
    // store name of the current file, which is always the same as the class name according to jack language specifications
    className = tokenizer.identifier();
    expectSymbol("{");
    while (tokenizer.peek() == "static" || tokenizer.peek() == "field") compileClassVarDec();
    while (tokenizer.peekType() == KEYWORD) compileSubroutineDec();
    expectSymbol("}");
}

void CompilationEngine::compileClassVarDec() {
    expectKeyword(std::unordered_set<std::string>{"static", "field"});
    std::string kind = tokenizer.keyWord();
    bool isStatic = kind == "static";
    // we assign "this" to field variables for generating vm code more easily
    if (!isStatic) kind = "this";
    expectType();
    std::string type = tokenizer.stringVal();
    while (true) {
        expectIdentifier();
        // add identifier to symbol table along with its type, kind, and index
        classTable[tokenizer.identifier()] = {type, kind, isStatic ? staticCount++ : fieldCount++};
        // if no comma, we assume no more variable declarations on current line and expect a ';'
        if (tokenizer.peek() != ",") break;
        expectSymbol(",");
    }
    expectSymbol(";");
}

void CompilationEngine::compileSubroutineDec() {
    localCount = 0;
    isMethod = false, isConstructor = false;
    expectKeyword(std::unordered_set<std::string>{"constructor", "function", "method"});
    if (tokenizer.keyWord() == "method") {
        // add "this" (the current object instance) to the symbol table of the current subroutine
        // only added to the symbol table when processing methods (out of convention, not really necessity)
        subroutineTable["this"] = {className, "argument", 0};
        isMethod = true;
    }
    if (tokenizer.keyWord() == "constructor") isConstructor = true;
    expectType();
    expectIdentifier();
    // store the name of current subroutine
    subroutineName = tokenizer.identifier();
    expectSymbol("(");
    compileParameterList();
    expectSymbol(")");
    compileSubroutineBody();
    // once we finish compiling a subroutine, we no longer need its symbol table
    subroutineTable.clear();
}

void CompilationEngine::compileParameterList() {
    while (tokenizer.peekType() != SYMBOL) {
        expectType();
        std::string type = tokenizer.stringVal();
        expectIdentifier();
        // add current argument to subroutine symbol table
        subroutineTable[tokenizer.identifier()] = {type, "argument", subroutineTable.size()};
        // if no comma, we assume no more parameter declarations
        if (tokenizer.peek() != ",") break;
        expectSymbol(",");
    }
}

void CompilationEngine::compileSubroutineBody() {
    expectSymbol("{");
    while (tokenizer.peek() == "var") compileVarDec();
    writer.writeFunction(className + '.' + subroutineName, localCount);
    if (isConstructor) {
        // allocate space for a new instance of current class and assign it to this
        writer.writePush("constant", fieldCount);
        writer.writeCall("Memory.alloc", 1);
        writer.writePop("pointer", 0);
    }
    if (isMethod) {
        // assign argument 0 to this as per the jack language specification
        writer.writePush("argument", 0);
        writer.writePop("pointer", 0);
    }
    compileStatements();
    expectSymbol("}");
}

void CompilationEngine::compileVarDec() {
    expectKeyword("var");
    expectType();
    std::string type = tokenizer.stringVal();
    while (true) {
        expectIdentifier();
        // add current local variable to subroutine symbol table
        subroutineTable[tokenizer.identifier()] = {type, "local", localCount++};
        // if no comma, we assume no more variable declarations on current line and expect a ';'
        if (tokenizer.peek() != ",") break;
        expectSymbol(",");
    }
    expectSymbol(";");
}

void CompilationEngine::compileStatements() {
    while (tokenizer.peekType() == KEYWORD) {
        if (tokenizer.peek() == "let") compileLet();
        else if (tokenizer.peek() == "if") compileIf();
        else if (tokenizer.peek() == "while") compileWhile();
        else if (tokenizer.peek() == "do") compileDo();
        else if (tokenizer.peek() == "return") compileReturn();
        else return;
    }
}

void CompilationEngine::compileLet() {
    expectKeyword("let");
    expectIdentifier();
    std::string name = tokenizer.identifier();
    bool isArray = false;
    if (tokenizer.peek() == "[") {
        compileVar(name, true);
        expectSymbol("[");
        compileExpression();
        expectSymbol("]");
        writer.writeArithmetic("add");
        isArray = true;
    }
    expectSymbol("=");
    compileExpression();
    expectSymbol(";");
    if (isArray) {
        writer.writePop("temp", 0);
        writer.writePop("pointer", 1);
        writer.writePush("temp", 0);
        writer.writePop("that", 0);
    }
    else {
        compileVar(name, false);
    }
}

void CompilationEngine::compileIf() {
    int temp = labelCount; labelCount += 2;
    expectKeyword("if");
    expectSymbol("(");
    compileExpression();
    writer.writeArithmetic("not");
    writer.writeIf("L" + std::to_string(temp));
    expectSymbol(")");
    expectSymbol("{");
    compileStatements();
    expectSymbol("}");
    if (tokenizer.peek() == "else") {
        writer.writeGoto("L" + std::to_string(temp + 1));
        writer.writeLabel("L" + std::to_string(temp));
        expectKeyword("else");
        expectSymbol("{");
        compileStatements();
        expectSymbol("}");
        writer.writeLabel("L" + std::to_string(temp + 1));
    }
    // for if statements without else clause, we only need to generate 1 label
    else writer.writeLabel("L" + std::to_string(temp));
}

void CompilationEngine::compileWhile() {
    int temp = labelCount; labelCount += 2;
    writer.writeLabel("L" + std::to_string(temp));
    expectKeyword("while");
    expectSymbol("(");
    compileExpression();
    writer.writeArithmetic("not");
    writer.writeIf("L" + std::to_string(temp + 1));
    expectSymbol(")");
    expectSymbol("{");
    compileStatements();
    writer.writeGoto("L" + std::to_string(temp));
    expectSymbol("}");
    writer.writeLabel("L" + std::to_string(temp + 1));
}

void CompilationEngine::compileDo() {
    expectKeyword("do");
    compileTerm();
    expectSymbol(";");
    writer.writePop("temp", 0);
}

void CompilationEngine::compileReturn() {
    expectKeyword("return");
    // if no expression is returned, return null instead (represented by constant 0 in jack)
    if (tokenizer.peek() != ";") compileExpression();
    else writer.writePush("constant", 0);
    expectSymbol(";");
    writer.writeReturn();
}

void CompilationEngine::compileExpression() {
    compileTerm();
    // process an indefinite amount of binary operations, compiling each expression and performing the operation with the previous result
    while (binaryOps.find(tokenizer.peek()) != binaryOps.end()) {
        expectSymbol(binaryOps);
        std::string op = tokenizer.symbol();
        compileTerm();
        writer.writeArithmetic(binaryOps[op]);
    }
}

void CompilationEngine::compileTerm() {
    std::string name = tokenizer.peek(), currClass = className, segment = "pointer";
    int index = 0;
    switch (tokenizer.peekType()) {
        case IDENTIFIER:
            // if curr token is identifier, must distinguish between variable, array entry, 
            // or subroutine call by looking ahead one token
            expectIdentifier();
            if (tokenizer.peek() == ".") {
                // pretty much guaranteed to be a subroutine call
                if (subroutineTable.find(name) != subroutineTable.end()) {
                    currClass = std::get<0>(subroutineTable[name]);
                    segment = std::get<1>(subroutineTable[name]);
                    index = std::get<2>(subroutineTable[name]);
                }
                else if (classTable.find(name) != classTable.end()) {
                    currClass = std::get<0>(classTable[name]);
                    segment = std::get<1>(classTable[name]);
                    index = std::get<2>(classTable[name]);
                }
                // this means that it is a static function
                else currClass = name;
                expectSymbol(".");
                expectIdentifier();
            }
            if (tokenizer.peek() == "(") {
                // keep track of argument count of current call using a "stack"
                argCount.push_back(0);
                if (name != currClass) { // if method, add instance as argument 0
                    writer.writePush(segment, index);
                    ++argCount.back();
                }
                name = tokenizer.identifier();
                expectSymbol("(");
                compileExpressionList();
                expectSymbol(")");
                writer.writeCall(currClass + "." + name, argCount.back());
                // pop argument count once done
                argCount.pop_back();
            }
            else compileVar(name, true);
            if (tokenizer.peek() == "[") {
                // can do regular array accesses when not assigning
                expectSymbol("[");
                compileExpression();
                expectSymbol("]");
                writer.writeArithmetic("add");
                writer.writePop("pointer", 1);
                writer.writePush("that", 0);
            }
            break;
        case SYMBOL:
            // parenthesis priority
            if (tokenizer.peek() == "(") {
                expectSymbol("(");
                compileExpression();
                expectSymbol(")");
            }
            // otherwise we expect a unary operation
            else {
                expectSymbol(unaryOps);
                std::string op = tokenizer.symbol();
                compileTerm();
                writer.writeArithmetic(unaryOps[op]);
            }
            break;
        case INT_CONST:
            tokenizer.advance();
            writer.writePush("constant", tokenizer.intVal());
            break;
        case STRING_CONST:
            tokenizer.advance();
            writer.writePush("constant", tokenizer.stringVal().length());
            writer.writeCall("String.new", 1);
            for (const char &c : tokenizer.stringVal()) {
                writer.writePush("constant", static_cast<int>(c));
                writer.writeCall("String.appendChar", 2);
            }
            break;
        case KEYWORD:
            expectKeyword(std::unordered_set<std::string>{"this", "null", "true", "false"});
            if (tokenizer.keyWord() == "this") writer.writePush("pointer", 0);
            else if (tokenizer.keyWord() == "true") {
                writer.writePush("constant", 1);
                writer.writeArithmetic("neg");
            }
            else writer.writePush("constant", 0);
            break;
    }
}

void CompilationEngine::compileExpressionList() {
    while (tokenizer.peek() != ")") {
        if (tokenizer.peek() == ",") expectSymbol(",");
        compileExpression();
        ++argCount.back();
    }
}