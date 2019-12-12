#include <string>
#include <functional>
#include "astnode.h"
#include "interpreter.h"

using namespace std;

int Interpreter::evaluate_integer(AstNode *node, int value)
{
  return value;
}

int Interpreter::evaluate_binop(AstNode *node, AstNode *left, std::string op, AstNode *right)
{
  int lval = left->accept(this);
  int rval = right->accept(this);
  return eval_op[op](lval, rval);
}


//errors saying cout is undefined
//int Interpreter::evaluate_printing(AstNode *node) {
//    cout << "Print";
//    cout << node->accept(this);
//}
