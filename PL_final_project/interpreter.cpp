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


//I don't think this is correct
//int Interpreter::evaluate_printing(AstNode *node) {
//    cout << "Print";
//    cout << node->accept(this);
//}

//Not sure where to put the binding so it's accessible across everything
//int Interpreter::evaluate_assignment(AstNode *node, string name, AstNode *value) {
//    int trueVal = value->accept(this);
//    //need a binding here to actually put the stuff in
//}

int Interpreter::evaluate_ifExpr(AstNode *node, AstNode *boolExpr, AstNode *then, AstNode *els) {
    int pretendBool = boolExpr->accept(this);
    int result = 0;
    if(pretendBool)
        result = then->accept(this);
    else if(els != nullptr)
        result = els->accept(this);

    return result;
}
