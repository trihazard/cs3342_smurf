#include "astnode.h"

////////////////////
//  IntegerNode

IntegerNode::IntegerNode(int given)
{
  value = given;
}

std::string IntegerNode::to_string()
{
  return std::to_string(value);
}

int IntegerNode::accept(Visitor *visitor)
{
  return visitor->evaluate_integer(this, value);
}

///////////////
// IdentNode
IdentNode::IdentNode(string n) {
    name = n;
}

string IdentNode::to_string() {
    return name;
}

int IdentNode::accept(Visitor *visitor) {
    //I think this is where I'd retrieve the value of a variable from my binding yes?
}
////////////////////
//  BinopNode

BinopNode::BinopNode(AstNode *pleft, std::string pop, AstNode *pright)
{
  left = pleft;
  op = pop;
  right = pright;
}

int BinopNode::accept(Visitor *visitor)
{
  return visitor->evaluate_binop(this, left, op, right);
}

std::string BinopNode::to_string()
{
  return "(" + left->to_string() + op + right->to_string() + ")";
}

////////////////////
//  OpNode

OpNode::OpNode(std::string pop)
{
  op = pop;
}

int OpNode::accept(Visitor *visitor) {
  return 0;  // never called.
}

std::string OpNode::to_string()
{
  return op;
}

///////////////////
// RValNode
//RValNode::RValNode(AstNode* node) {

//}

//string RValNode::to_string() {
//    return exp->to_string();
//}

//int RValNode::accept(Visitor* visitor, ) {
//    visitor->evaluate_varRVal(this, expr);
//}

///////////////////
// LValNode
LValNode::LValNode(string n) {
    name = n;
}

string LValNode::to_string() {
    return name;
}

int LValNode::accept(Visitor *visitor) {
    return visitor->evaluate_varLVal(this, name);
}


//////////////////
// AssignmentNode
AssignmentNode::AssignmentNode(string n, AstNode* valueNode) {
    name = n;
    val = valueNode;
}

string AssignmentNode::to_string() {
    return (name + val->to_string());
}

int AssignmentNode::accept(Visitor *visitor) {
    return visitor->evaluate_assignment(this, name, val);
}


/////////////////
// IfNode
IfNode::IfNode(AstNode* ex, AstNode* tru, AstNode* fal) {
    exp = ex;
    then = tru;
    els = fal;
}

string IfNode::to_string() {
    string toReturn = ("If " + exp->to_string() + "\n\t" + then->to_string());
    if(els != nullptr)
        toReturn += ("\nelse \n\t" + els->to_string());
    return toReturn;
}

int IfNode::accept(Visitor *visitor) {
    return visitor->evaluate_ifExpr(this, exp, then, els);
}
