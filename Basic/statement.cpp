/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"
#include <stdexcept>


/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

// Statement implementations


void RemStatement::execute(EvalState &state, Program &program) {
}

LetStatement::LetStatement(TokenScanner &scanner) {
    if (!scanner.hasMoreTokens()) {
        error("LET requires variable and expression");
    }
    varName = scanner.nextToken();

    if (!scanner.hasMoreTokens()) {
        error("LET requires = and expression");
    }
    std::string equals = scanner.nextToken();
    if (equals != "=") {
        error("LET requires =");
    }

    if (!scanner.hasMoreTokens()) {
        error("LET requires expression");
    }

    exp = readE(scanner);
}

LetStatement::~LetStatement() {
    delete exp;
}

void LetStatement::execute(EvalState &state, Program &program) {
    int value = exp->eval(state);
    state.setValue(varName, value);
}

PrintStatement::PrintStatement(TokenScanner &scanner) {
    if (!scanner.hasMoreTokens()) {
        error("PRINT requires expression");
    }

    exp = readE(scanner);
}

PrintStatement::~PrintStatement() {
    delete exp;
}

void PrintStatement::execute(EvalState &state, Program &program) {
    int value = exp->eval(state);
    std::cout << value << std::endl;
}

InputStatement::InputStatement(TokenScanner &scanner) {
    if (!scanner.hasMoreTokens()) {
        error("INPUT requires variable name");
    }
    varName = scanner.nextToken();
}

void InputStatement::execute(EvalState &state, Program &program) {
    std::string input;
    getline(std::cin, input);

    try {
        int value = stoi(input);
        state.setValue(varName, value);
    } catch (...) {
        error("INPUT requires integer value");
    }
}


void EndStatement::execute(EvalState &state, Program &program) {
    throw std::runtime_error("END");
}

GotoStatement::GotoStatement(TokenScanner &scanner) {
    if (!scanner.hasMoreTokens()) {
        error("GOTO requires line number");
    }
    std::string lineStr = scanner.nextToken();
    try {
        lineNumber = stoi(lineStr);
    } catch (...) {
        error("GOTO requires valid line number");
    }
}

void GotoStatement::execute(EvalState &state, Program &program) {
    if (program.getParsedStatement(lineNumber) == nullptr) {
        error("GOTO line does not exist");
    }
    throw lineNumber;
}

IfStatement::IfStatement(TokenScanner &scanner) {
    // Parse first expression
    std::string exp1Str = "";
    while (scanner.hasMoreTokens()) {
        std::string token = scanner.nextToken();
        if (token == "<" || token == ">" || token == "=") {
            op = token;
            break;
        }
        exp1Str += token + " ";
    }
    if (exp1Str.empty() || op.empty()) {
        error("IF requires expression operator expression");
    }
    exp1Str = exp1Str.substr(0, exp1Str.length() - 1);

    TokenScanner exp1Scanner;
    exp1Scanner.ignoreWhitespace();
    exp1Scanner.scanNumbers();
    exp1Scanner.setInput(exp1Str);
    exp1 = readE(exp1Scanner);

    // Parse second expression
    std::string exp2Str = "";
    bool foundThen = false;
    while (scanner.hasMoreTokens()) {
        std::string token = scanner.nextToken();
        if (token == "THEN") {
            foundThen = true;
            break;
        }
        exp2Str += token + " ";
    }
    if (!foundThen || exp2Str.empty()) {
        error("IF requires THEN and line number");
    }
    exp2Str = exp2Str.substr(0, exp2Str.length() - 1);

    TokenScanner exp2Scanner;
    exp2Scanner.ignoreWhitespace();
    exp2Scanner.scanNumbers();
    exp2Scanner.setInput(exp2Str);
    exp2 = readE(exp2Scanner);

    // Parse line number
    if (!scanner.hasMoreTokens()) {
        error("IF requires line number after THEN");
    }
    std::string lineStr = scanner.nextToken();
    try {
        lineNumber = stoi(lineStr);
    } catch (...) {
        error("IF requires valid line number");
    }
}

IfStatement::~IfStatement() {
    delete exp1;
    delete exp2;
}

void IfStatement::execute(EvalState &state, Program &program) {
    int val1 = exp1->eval(state);
    int val2 = exp2->eval(state);

    bool condition = false;
    if (op == "=") {
        condition = (val1 == val2);
    } else if (op == "<") {
        condition = (val1 < val2);
    } else if (op == ">") {
        condition = (val1 > val2);
    }

    if (condition) {
        if (program.getParsedStatement(lineNumber) == nullptr) {
            error("IF THEN line does not exist");
        }
        throw lineNumber;
    }
}

RunStatement::RunStatement(TokenScanner &scanner) {
}

void RunStatement::execute(EvalState &state, Program &program) {
    state.Clear();
    int currentLine = program.getFirstLineNumber();

    while (currentLine != -1) {
        Statement *stmt = program.getParsedStatement(currentLine);
        if (stmt) {
            try {
                stmt->execute(state, program);
            } catch (int gotoLine) {
                currentLine = gotoLine;
                continue;
            } catch (...) {
                break;
            }
        }
        currentLine = program.getNextLineNumber(currentLine);
    }
}

ListStatement::ListStatement(TokenScanner &scanner) {
}

void ListStatement::execute(EvalState &state, Program &program) {
    program.listAll();
}

ClearStatement::ClearStatement(TokenScanner &scanner) {
}

void ClearStatement::execute(EvalState &state, Program &program) {
    program.clear();
}

QuitStatement::QuitStatement(TokenScanner &scanner) {
}

void QuitStatement::execute(EvalState &state, Program &program) {
    // This shouldn't be called in the current implementation
    // QUIT is handled directly in processLine
}
