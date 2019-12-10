#pragma once

#include <string>
#include "visitor.h"

using namespace std;

class AstNode
{
public:
    virtual string to_string() {return "node";}
    virtual int accept(Visitor *visitor) {return 9999;}
};

class IntegerNode : public AstNode {
private:
    int value;
public:
    IntegerNode(int val);
    string to_string();
    int accept(Visitor *visitor);
};

class BinopNode : public AstNode {
private:
    AstNode *left;
    string op;
    AstNode *right;
public:
    BinopNode(AstNode *pleft, string pop, AstNode *pright);
    string to_string();
    int accept(Visitor *visitor);
};

class OpNode : public AstNode {
private:
    string op;
public:
    OpNode(string pop);
    string to_string();
    int accept(Visitor *visitor);
};
