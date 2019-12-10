#pragma once
#include <string>

using namespace std;

class AstNode;

class Visitor {
public:
    virtual int evaluate_integer(AstNode *node, int value) {return 0;}
    virtual int evaluate_binop(AstNode *node, AstNode *left, string op, AstNode *right) {return 0;}
};
