#pragma once
#include <map>
#include <string>
#include <functional>
#include "visitor.h"

using namespace std;

class  Interpreter : public Visitor
{
    map<string, function<int(int, int)>> eval_op = {
        {"+", [](int l, int r) { return l + r; }},
        {"-", [](int l, int r) { return l - r; }},
        {"*", [](int l, int r) { return l * r; }},
        {"/", [](int l, int r) { return l / r; }},
        {"==", [](int l, int r) {return l == r;}},
        {"!=", [](int l, int r) {return l != r;}},
        {"<=", [](int l, int r) {return l <= r;}},
        {">=", [](int l, int r) {return l >= r;}},
        {"<", [](int l, int r) {return l < r;}},
        {">", [](int l, int r) {return l > r;}}
    };

public:
    int evaluate_integer(AstNode *node, int value);
    int evaluate_binop(AstNode *node, AstNode *left, std::string op, AstNode *right);
    int evaluate_varLVal(AstNode *node, string name);
    int evaluate_varRVal(AstNode *node, AstNode* expr);
    int evaluate_funcCall(AstNode *node);
    int evaluate_printing(AstNode *node);
    int evaluate_braces(AstNode *node, vector<AstNode *> nodes);
    int evaluate_varDec(AstNode *node, AstNode *left, string op, AstNode *right);
    int evaluate_assignment(AstNode *node, string name, AstNode* value);
    int evaluate_ifExpr(AstNode *node, AstNode *boolExpr, AstNode *then, AstNode *els);
};
