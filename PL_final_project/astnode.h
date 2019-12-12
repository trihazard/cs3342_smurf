#pragma once
#include <string>
#include <vector>
#include "visitor.h"

using namespace std;

class AstNode
{
public:
    virtual string to_string() {return string("node");}
    virtual int accept(Visitor *visitor) { return 99999; }
};

////////////////////
//  IntegerNode

class IntegerNode : public AstNode
{
    int value;

public:
    IntegerNode(int given);
    std::string to_string();
    int accept(Visitor *visitor);
};

////////////////
// IdentNode
class IdentNode : public AstNode {
    string name;

public:
    IdentNode(string n);
    string to_string();
    int accept(Visitor *visitor);
};

////////////////////
//  BinopNode

class BinopNode : public AstNode
{
    AstNode *left;
    std::string op;
    AstNode *right;

public:
    BinopNode(AstNode *pleft, std::string pop, AstNode *pright);
    std::string to_string();
    int accept(Visitor *visitor);
};

////////////////////
//  OpNode

class OpNode : public AstNode
{
    std::string op;

public:
    OpNode(std::string pop);
    std::string to_string();
    int accept(Visitor *visitor);
};

///////////////////
// BlockNode
class BlockNode : public AstNode {
    vector<AstNode*> nodes;
public:
    BlockNode(vector<AstNode*> nodes);
    string to_string();
    int accept(Visitor *visitor);
};

///////////////////
// rValNode
class RValNode : public AstNode {
    //some kind of expression node

public:
    RValNode(AstNode* expr);
    string to_string();
    int accept(Visitor *visitor);
};

class LValNode : public AstNode {
    string name;

public:
    LValNode(string n);
    string to_string();
    int accept(Visitor *visitor);
};

class AssignmentNode : public AstNode {
    string name;
    AstNode* val; //This should theoretically work for either integers or functions being passed into variables

public:
    AssignmentNode(string n, AstNode* valueNode);
    string to_string();
    int accept(Visitor *visitor);
};

class IfNode : public AstNode {
    AstNode* exp;
    AstNode* then;
    AstNode* els;

public:
    IfNode(AstNode* ex, AstNode* tru, AstNode* fal);
    string to_string();
    int accept(Visitor *visitor);
};
