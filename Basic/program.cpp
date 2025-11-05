/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"



Program::Program() = default;

Program::~Program() {
    clear();
}

void Program::clear() {
    for (auto& pair : parsedStatements) {
        delete pair.second;
    }
    sourceLines.clear();
    parsedStatements.clear();
    lineNumbers.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    sourceLines[lineNumber] = line;
    lineNumbers.insert(lineNumber);

    auto it = parsedStatements.find(lineNumber);
    if (it != parsedStatements.end()) {
        delete it->second;
        parsedStatements.erase(it);
    }
}

void Program::removeSourceLine(int lineNumber) {
    sourceLines.erase(lineNumber);
    lineNumbers.erase(lineNumber);

    auto it = parsedStatements.find(lineNumber);
    if (it != parsedStatements.end()) {
        delete it->second;
        parsedStatements.erase(it);
    }
}

std::string Program::getSourceLine(int lineNumber) {
    auto it = sourceLines.find(lineNumber);
    if (it != sourceLines.end()) {
        return it->second;
    }
    return "";
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    auto it = parsedStatements.find(lineNumber);
    if (it != parsedStatements.end()) {
        delete it->second;
    }
    parsedStatements[lineNumber] = stmt;
}

//void Program::removeSourceLine(int lineNumber) {

Statement *Program::getParsedStatement(int lineNumber) {
   auto it = parsedStatements.find(lineNumber);
   if (it != parsedStatements.end()) {
       return it->second;
   }
   return nullptr;
}

int Program::getFirstLineNumber() {
    if (lineNumbers.empty()) {
        return -1;
    }
    return *lineNumbers.begin();
}

int Program::getNextLineNumber(int lineNumber) {
    auto it = lineNumbers.find(lineNumber);
    if (it != lineNumbers.end() && ++it != lineNumbers.end()) {
        return *it;
    }
    return -1;
}

void Program::listAll() {
    for (int lineNumber : lineNumbers) {
        std::cout << sourceLines[lineNumber] << std::endl;
    }
}

//more func to add
//todo


