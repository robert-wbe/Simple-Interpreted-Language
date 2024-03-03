//
//  Tokenizer.cpp
//  Parser
//
//  Created by Robert Wiebe on 10/4/23.
//

#pragma once

#include <iostream>
#include <string>
#include <vector>

class LexerException : public std::exception {
private:
    std::string desc;
public:
    LexerException(std::string description)
        :desc(description) {}
    const char* what () {
        return desc.data();
    }
};

namespace TokenType {
    struct LET {};
    struct RETURN {};
    struct BINOP { char chr; };
    struct ASSIGN {};
    struct INT_LIT { int val; };
    struct NAME { std::string val; };
    struct OPEN_PAREN {};
    struct CLOSE_PAREN {};
    struct SEMCL {};
}

using Token = std::variant<
    TokenType::LET,
    TokenType::RETURN,
    TokenType::BINOP,
    TokenType::ASSIGN,
    TokenType::INT_LIT,
    TokenType::NAME,
    TokenType::OPEN_PAREN,
    TokenType::CLOSE_PAREN,
    TokenType::SEMCL
>;

class Tokenizer {
public:
    Tokenizer(const std::string&& inputSource)
        :source(std::move(inputSource)) {}
    
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (!eof()) {
            const char next = peek();
            
            if (next == ' ' || next == '\n') {
                consume();
                continue;
            }
            
            if (next == '=') {
                tokens.push_back(TokenType::ASSIGN());
                consume();
                continue;
            }
            
            if (next == ';') {
                tokens.push_back(TokenType::SEMCL());
                consume();
                continue;
            }
            
            if (next == '(') {
                tokens.push_back(TokenType::OPEN_PAREN());
                consume();
                continue;
            }
            
            if (next == ')') {
                tokens.push_back(TokenType::CLOSE_PAREN());
                consume();
                continue;
            }
            
            if (isdigit(next)) {
                tokens.push_back(TokenType::INT_LIT{parseIntLit()});
                continue;
            }
            
            if (next == '-') {
                consume();
                if (tokens.size() &&
                   (std::holds_alternative<TokenType::NAME>
                   (tokens.back()) ||
                    std::holds_alternative<TokenType::INT_LIT>
                    (tokens.back()))) {
                    tokens.push_back(TokenType::BINOP{next});
                }
                else if (isdigit(peek())){
                    tokens.push_back(TokenType::INT_LIT{-parseIntLit()});
                }
                continue;
            }
            
            if (next == '+' || next == '*' || next == '/') {
                tokens.push_back(TokenType::BINOP{consume()});
                continue;
            }
            
            if (isalpha(next)) {
                std::string word;
                word.push_back(consume());
                while (!eof() && (isalnum(peek()))) {
                    word.push_back(consume());
                }
                
                if (word == "return")
                    tokens.push_back(TokenType::RETURN());
                else if (word == "let")
                    tokens.push_back(TokenType::LET());
                else
                    tokens.push_back(TokenType::NAME{word});
                continue;
            }
            
            std::string symbol;
            symbol.push_back(next);
            std::cerr << "Error: Found unexpected symbol: " + symbol << std::endl;
            throw LexerException("Found unexpected symbol: " + symbol);
        }
        return tokens;
    }
    
private:
    std::string source;
    int idx = 0;
    
    int parseIntLit() {
        int numVal = consume() - '0';
        while (!eof() && isdigit(peek())) {
            numVal = 10*numVal + consume() - '0';
        }
        return numVal;
    }
    
    bool eof() {
        return idx == source.size();
    }
    
    char peek() {
        return source[idx];
    }
    
    char consume() {
        return source[idx++];
    }
};


