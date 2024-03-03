//
//  Runtime.cpp
//  Parser
//
//  Created by Robert Wiebe on 10/4/23.
//

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include "Parser.hpp"

class Runtime {
public:
    Runtime(std::vector<NodeType::Statement> &stmts)
        :statements(stmts) {}
    void execute() {
        for (const NodeType::Statement &stmt : statements) {
            
            if (std::holds_alternative<NodeType::Assignment>(stmt)) {
                NodeType::Assignment assmt = std::get<NodeType::Assignment>(stmt);
                environment[assmt.ident.name] = evalExpr(assmt.expr);
            }
            
            if (std::holds_alternative<NodeType::ReturnStmt>(stmt)) {
                std::cout << "OUTPUT: " << evalExpr(std::get<NodeType::ReturnStmt>(stmt).expr) << std::endl;
            }
        }
    }
    
    int evalExpr(NodeType::Expression expr) {
        
        if (std::holds_alternative<NodeType::Integer>(expr)) {
            return std::get<NodeType::Integer>(expr).val;
        }
        
        if (std::holds_alternative<NodeType::Identifier>(expr)) {
            std::string name =  std::get<NodeType::Identifier>(expr).name;
            if (auto itr = environment.find(name); itr != environment.end()) {
                return itr->second;
            }
            std::cerr << "Error: Identifier " << name << " is not defined." << std::endl;
            throw std::exception();
        }
        
        if (std::holds_alternative<NodeType::BinExpression>(expr)) {
            NodeType::BinExpression binExpr = std::get<NodeType::BinExpression>(expr);
            int lhs = evalExpr(*(binExpr.lhs));
            int rhs = evalExpr(*(binExpr.rhs));
            switch (binExpr.op) {
                case '+': return lhs + rhs;
                case '-': return lhs - rhs;
                case '*': return lhs * rhs;
                case '/': return lhs / rhs;
                default:
                    std::cerr << "You messed up" << std::endl;
                    throw std::exception();
            }
        }
        
        std::cerr << "You messed up, expression invalid" << std::endl;
        throw std::exception();
    }
private:
    std::unordered_map<std::string, int> environment;
    std::vector<NodeType::Statement> statements;
};
