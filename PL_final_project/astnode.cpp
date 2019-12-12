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
