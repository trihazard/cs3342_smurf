#pragma once
#include <string>
#include <vector>
using namespace std;

class AstNode;

class Visitor {
public:
    virtual int evaluate_integer(AstNode* node, int value) {return 0;}
    virtual int evaluate_varLVal(AstNode* node, string name){return 0;}
    virtual int evaluate_varRVal(AstNode* node, AstNode* expr){return 0;}
    virtual int evaluate_binop(AstNode* node, AstNode* left, string op, AstNode* right) {return 0;}
    virtual int evaluate_varDec(AstNode* node, AstNode* left, string op, AstNode* right){return 0;}
    virtual int evaluate_assignment(AstNode* node, string name, AstNode* value){return 0;}
    virtual int evaluate_ifExpr(AstNode* node, AstNode* boolExpr, vector<AstNode*> thenelse){return 0;}
    virtual int evaluate_funcCall(AstNode* node){return 0;}
    virtual int evaluate_printing(AstNode* node){return 0;}
    virtual int evaluate_braces(AstNode* node, vector<AstNode*> nodes){return 0;}
};
