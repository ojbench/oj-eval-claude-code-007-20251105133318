/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state, bool &running);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    bool running = true;
    while (running) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                continue;
            processLine(input, program, state, running);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state, bool &running) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    if (!scanner.hasMoreTokens()) {
        return;
    }

    std::string firstToken = scanner.nextToken();

    // Check if it's a line number
    try {
        int lineNumber = std::stoi(firstToken);

        if (!scanner.hasMoreTokens()) {
            // Just a line number - remove this line
            program.removeSourceLine(lineNumber);
        } else {
            // Line number followed by statement - add or replace line
            program.addSourceLine(lineNumber, line);

            // Parse and store the statement
            std::string restOfLine = "";
            while (scanner.hasMoreTokens()) {
                restOfLine += scanner.nextToken() + " ";
            }
            if (!restOfLine.empty()) {
                restOfLine = restOfLine.substr(0, restOfLine.length() - 1);
            }

            TokenScanner stmtScanner;
            stmtScanner.ignoreWhitespace();
            stmtScanner.scanNumbers();
            stmtScanner.setInput(restOfLine);

            if (stmtScanner.hasMoreTokens()) {
                std::string stmtType = stmtScanner.nextToken();

                Statement *stmt = nullptr;

                if (stmtType == "REM") {
                    stmt = new RemStatement();
                } else if (stmtType == "LET") {
                    stmt = new LetStatement(stmtScanner);
                } else if (stmtType == "PRINT") {
                    stmt = new PrintStatement(stmtScanner);
                } else if (stmtType == "INPUT") {
                    stmt = new InputStatement(stmtScanner);
                } else if (stmtType == "END") {
                    stmt = new EndStatement();
                } else if (stmtType == "GOTO") {
                    stmt = new GotoStatement(stmtScanner);
                } else if (stmtType == "IF") {
                    stmt = new IfStatement(stmtScanner);
                } else {
                    error("Invalid statement type");
                }

                program.setParsedStatement(lineNumber, stmt);
            }
        }
        return;
    } catch (...) {
        // Not a line number, it's a direct command
    }

    // Direct commands
    scanner.setInput(line);
    std::string command = scanner.nextToken();

    if (command == "RUN") {
        RunStatement stmt(scanner);
        stmt.execute(state, program);
    } else if (command == "LIST") {
        ListStatement stmt(scanner);
        stmt.execute(state, program);
    } else if (command == "CLEAR") {
        ClearStatement stmt(scanner);
        stmt.execute(state, program);
    } else if (command == "QUIT") {
        running = false;
        return;
    } else if (command == "HELP") {
        std::cout << "BASIC interpreter commands:\n";
        std::cout << "RUN - Execute the stored program\n";
        std::cout << "LIST - Display the stored program\n";
        std::cout << "CLEAR - Clear the stored program\n";
        std::cout << "QUIT - Exit the interpreter\n";
        std::cout << "LET <var> = <exp> - Assign a variable\n";
        std::cout << "PRINT <exp> - Print an expression\n";
        std::cout << "INPUT <var> - Read input to a variable\n";
        std::cout << "<line> <stmt> - Add line to program\n";
    } else {
        // Reset scanner to parse as statement
        scanner.setInput(line);
        std::string cmd = scanner.nextToken(); // consume command again
        if (cmd == "LET") {
            LetStatement stmt(scanner);
            stmt.execute(state, program);
        } else if (cmd == "PRINT") {
            PrintStatement stmt(scanner);
            stmt.execute(state, program);
        } else if (cmd == "INPUT") {
            InputStatement stmt(scanner);
            stmt.execute(state, program);
        } else {
            error("Invalid command");
        }
    }
}

