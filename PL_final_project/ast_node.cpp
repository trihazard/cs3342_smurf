#include "ast_node.h"

using namespace std;

IntegerNode::IntegerNode(int val) {
    value = val;
}

string IntegerNode::to_string() {
    return std::to_string(value);
}

int IntegerNode::accept(Visitor *visitor) {
    return visitor->evaluate_integer(this, value);
}

BinopNode::BinopNode(AstNode *pleft, string pop, AstNode *pright) {
    left = pleft;
    op = pop;
    right = pright;
}

string BinopNode::to_string() {
    return "(" + left->to_string() + op + right->to_string() + ")";
}

int BinopNode::accept(Visitor *visitor) {
    return visitor->evaluate_binop(this, left, op, right);
}

OpNode::OpNode(string pop) {
    op = pop;
}

string OpNode::to_string() {
    return op;
}

int OpNode::accept(Visitor *visitor) {
    return 0; //never should be called
}

