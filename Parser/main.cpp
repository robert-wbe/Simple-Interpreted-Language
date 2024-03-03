//
//  main.cpp
//  Parser
//
//  Created by Robert Wiebe on 10/4/23.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "Runtime.hpp"

int main(int argc, const char * argv[]) {
    std::ifstream sourceFile;
    std::stringstream sourceBuffer;
    std::string sourceCode;
    
    sourceFile.open("source.txt");
    sourceBuffer << sourceFile.rdbuf();
    sourceCode = sourceBuffer.str();
    
    Tokenizer tokenizer(std::move(sourceCode));
    std::vector<Token> tokens = tokenizer.tokenize();
    
    Parser parser(tokens);
    std::vector<NodeType::Statement> statements = parser.parse();
    
    Runtime runtime(statements);
    runtime.execute();
}
