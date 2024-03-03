//
//  Parser.cpp
//  Parser
//
//  Created by Robert Wiebe on 10/4/23.
//

#pragma once

#include <vector>
#include <string>
#include <variant>
#include "Tokenizer.hpp"

class ParseException : public std::exception {
private:
    std::string desc;
public:
    ParseException(std::string description)
        :desc(description) {}
    const char* what () {
        return desc.data();
    }
};

int operatorPrec(char opChr) {
    switch (opChr) {
        case '+': return 1;
        case '-': return 1;
        case '*': return 2;
        case '/': return 2;
        default: return 0;
    }
}

namespace NodeType {
    struct Integer {int val;};
    struct Identifier {std::string name;};
    struct BinExpression;
    using Expression = std::variant<Integer, Identifier, BinExpression>;
    struct BinExpression {Expression* lhs; char op; Expression* rhs;};
    
    struct Assignment {Identifier ident; Expression expr;};
    struct ReturnStmt {Expression expr;};
    using Statement = std::variant<Assignment, ReturnStmt>;
}

class Parser {
public:
    Parser(std::vector<Token> tkns): tokens(tkns) {}
    
    std::vector<NodeType::Statement> parse() {
        std::vector<NodeType::Statement> statements;
        while (!eof()){
            if (try_consume<TokenType::LET>()) {
                if (auto name = try_consume<TokenType::NAME>()) {
                    if (try_consume<TokenType::ASSIGN>()) {
                        if (auto expr = parseExpression()) {
                            if (try_consume<TokenType::SEMCL>()) {
                                statements.push_back(NodeType::Assignment{NodeType::Identifier{name->val}, *(*expr)});
                                continue;
                            }
                            std::cerr << "Error: Expected semicolon after assignment" << std::endl;
                            throw ParseException("");
                        }
                        std::cerr << "Error: Expected expression in assignment" << std::endl;
                        throw ParseException("");
                    }
                    std::cerr << "Error: Expected '=' in assignment" << std::endl;
                    throw ParseException("");
                }
                std::cerr << "Error: Expected identifier in assignment" << std::endl;
                throw ParseException("");
            }
            
            if (try_consume<TokenType::RETURN>()) {
                if (auto exp = parseExpression()) {
                    if (try_consume<TokenType::SEMCL>()) {
                        statements.push_back(NodeType::ReturnStmt{*(*exp)});
                        continue;
                    }
                    std::cerr << "Error: Expected semicolon after return statement" << std::endl;
                    throw ParseException("");
                }
                std::cerr << "Error: Expected expression in return statement" << std::endl;
                throw ParseException("");
            }
            
            std::cerr << "Error: Unexpected token" << std::endl;
            throw ParseException("");
        }
        return statements;
    }
    
private:
    std::optional<NodeType::Expression*> parseExpression(int minPrec = 0) {
        
        NodeType::Expression* lhs;
        if (try_consume<TokenType::OPEN_PAREN>()) {
            auto optExp = parseExpression(0);
            if (!optExp.has_value()) {
                std::cerr << "Error while parsing expression" << std::endl;
                throw ParseException("");
            }
            NodeType::Expression* nestedExpr = optExp.value();
            if (try_consume<TokenType::CLOSE_PAREN>()) {
                lhs = nestedExpr;
            }
            else {
                std::cerr << "Error. Expected ')' in expression." << std::endl;
                throw ParseException("");
            }
        }
        else if (auto name = try_consume<TokenType::NAME>())
            lhs = new NodeType::Expression(NodeType::Identifier{name->val});
        else if (auto intLit = try_consume<TokenType::INT_LIT>())
            lhs = new NodeType::Expression(NodeType::Integer{intLit->val});
        else return {};
        
        while (true) {
            if (eof() || !std::holds_alternative<TokenType::BINOP>(peek()))
                return lhs;
            char opChr = std::get<TokenType::BINOP>(peek()).chr;
            if (operatorPrec(opChr) < minPrec)
                return lhs;
            consume();
            auto optExpr = parseExpression(minPrec+1);
            if (!optExpr.has_value()) {
                std::cerr << "Error while parsing expression" << std::endl;
                throw ParseException("");
            }
            NodeType::Expression* rhs = optExpr.value();
            lhs = new NodeType::Expression(NodeType::BinExpression{lhs, opChr, rhs});
        }
        

    }
    
    bool eof() {
        return idx == tokens.size();
    }
    
    Token peek() {
        return tokens[idx];
    }
    
    Token consume() {
        return tokens[idx++];
    }
    
    template<typename TkType>
    std::optional<TkType> try_consume() {
        if (std::holds_alternative<TkType>(peek())) {
            return std::get<TkType>(consume());
        }
        return {};
    }
    
    std::vector<Token> tokens;
    int idx = 0;
};
