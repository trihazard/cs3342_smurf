/* TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * I'm not sure where the binding should be placed in this architecture
 * Add comments to stuff to make it more legible
 *
 * Comments left on master-closures branch detail why I couldn't get closures to work
 */

#include <iostream>
#include <fstream>
#include <peglib.h>
#include <assert.h>
#include <string>
#include <map>

#include "visitor.h"
#include "astnode.h"
#include "interpreter.h"

using namespace std;
using namespace peg;


auto grammar_t = R"(
    # Grammar for Smurf
    Program     <- Code
    Comment     <- '#' (!End .)* &End
    End         <-  EndOfLine / EndOfFile
    EndOfLine   <-  '\r\n' / '\n' / '\r'
    EndOfFile   <-  !.
    Code        <- Statement*
    Statement   <- 'let' Var_Dec / Assignment / Expr
    Var_Dec     <- Dec (',' Dec)*
    Dec         <- Ident ('=' Expr)?
    Ident       <- < [a-z][a-zA-Z_0-9]* >
    Var_ref     <- Ident
    If_expr     <- Expr Braces ("else" Braces)?
    Assignment  <- Ident '=' Expr
    Expr        <- 'fn' Func_def / 'if' If_expr / Bool_expr / Arith_expr
    Bool_expr   <- Arith_expr Relop Arith_expr
    Arith_expr  <- Mul_term (Addop Mul_term)* / Mul_term
    Mul_term    <- Primary (Mulop Primary)* / Primary
    Primary     <- Int / Func_call / Var_ref / '(' Arith_expr ')'
    Int         <- < '-'? [0-9]+ >
    Addop       <- '+' / '-'
    Mulop       <- '*' / '/'
    Relop       <- '==' / '!=' / '>=' / '>' / '<=' / '<'
    Func_call   <- Print_call '(' Call_args ')' / Var_ref '(' Call_args ')'
    Print_call  <- 'print'
    Call_args   <- (Expr (',' Expr)*)?
    Func_def    <- Params Braces
    Params      <- '(' Ident (',' Ident)* ')' / '(' ')'
    Braces      <- '{' Code '}'
    %whitespace <- ([ \t\r\n] / Comment)*
)";

class Visitor;

////////////////////
//  ParseTreeNode

class ParseTreeNode
{
    AstNode *content;

public:
    ParseTreeNode(){};
    ParseTreeNode(AstNode *content_node)
    {
        content = content_node;
    }

    AstNode *get() const
    {
        return content;
    }

    string to_string()
    {
        return content->to_string();
    }
};

AstNode *bin_op(const SemanticValues &sv)
{
    AstNode *left = sv[0].get<ParseTreeNode>().get();

    for (auto i = 1u; i < sv.size(); i += 2)
    {
        AstNode *right = sv[i + 1].get<ParseTreeNode>().get();
        string op = sv[i].get<ParseTreeNode>().get()->to_string();
        left = new BinopNode(left, op, right);
    }
    return left;
};

AstNode *assignment(const SemanticValues &sv) {
    string varName = sv[0].get<string>();
    AstNode *right = sv[1].get<ParseTreeNode>().get();
    return new AssignmentNode(varName, right);
}

AstNode *if_expr(const SemanticValues &sv) {
    AstNode *expr = sv[0].get<ParseTreeNode>().get();
    AstNode *then = sv[1].get<ParseTreeNode>().get();
    AstNode *el;
    if(sv.size() == 3)
        el = sv[2].get<ParseTreeNode>().get();
    else
        el = nullptr;
    return new IfNode(expr, then, el);
}

void setup_ast_generation(parser &parser)
{
    parser["Statement"] = [](const SemanticValues &sv) {
        cout << "statement: " << sv.str() << endl;
        AstNode *n = bin_op(sv);
        return ParseTreeNode(n);
    };

    parser["Addop"] = [](const SemanticValues &sv) {
        return ParseTreeNode(new OpNode(sv.str()));
    };

    parser["Mulop"] = [](const SemanticValues &sv) {
        return ParseTreeNode(new OpNode(sv.str()));
    };

    parser["Relop"] = [](const SemanticValues &sv) {
        return ParseTreeNode(new OpNode(sv.str()));
    };

    parser["Mul_term"] = [](const SemanticValues &sv) {
        AstNode *n = bin_op(sv);
        return ParseTreeNode(n);
    };

    parser["Arith_expr"] = [](const SemanticValues &sv) {
        AstNode *n = bin_op(sv);
        return ParseTreeNode(n);
    };

    parser["Bool_expr"] = [](const SemanticValues &sv) {
        AstNode *n = bin_op(sv);
        return ParseTreeNode(n);
    };

    parser["Dec"] = [](const SemanticValues &sv) {
        AstNode *n = assignment(sv);
        return ParseTreeNode(n);
    };

    parser["Assignment"] = [](const SemanticValues &sv) {
        AstNode *n = assignment(sv);
        return ParseTreeNode(n);
    };

    parser["If_expr"] = [](const SemanticValues &sv) {
        AstNode *n = if_expr(sv);
        return ParseTreeNode(n);
    };

    parser["Int"] = [](const SemanticValues &sv) {
        return ParseTreeNode(new IntegerNode(atoi(sv.c_str())));
    };

    parser["Ident"] = [](const SemanticValues &sv) {
        return ParseTreeNode(new IdentNode(sv.str()));
    };

    parser["Var_ref"] = [](const SemanticValues &sv) {
        return ParseTreeNode(new IdentNode(sv.str()));
    };
}

//int getVar(string ident, Closure &scope);
//int evaluate(const Ast& ast, Closure &scope);
//int evaluate_arithmeticTerms(const Ast& ast, Closure &scope);
//int evaluate_mulTerms(const Ast& ast, Closure &scope);
//int evaluate_varDec(const Ast& ast, Closure &scope);
//int evaluate_Dec(const Ast& ast, Closure &scope);
//int evaluate_assignment(const Ast& ast, Closure &scope);
//int evaluate_boolExpr(const Ast& ast, Closure &scope);
//int evaluate_ifExpr(const Ast& ast, Closure &scope);
//int evaluate_funcCall(const Ast& ast, Closure &scope);

int main(int argc, const char** argv) {
//    Closure scope;
//    map<string, Value> binding;

    ifstream smurfFile;
    if(argc > 2)
        throw logic_error("Invalid number of inputs passed to program");

    smurfFile.open(argv[1]);
    char* smurfCode;
    if (smurfFile) {
        smurfFile.seekg(0, smurfFile.end);
        int length = smurfFile.tellg();
        smurfFile.seekg(0, smurfFile.beg);
        smurfCode = new char[length+1];
        smurfFile.read(smurfCode, length);
        smurfCode[length] = '\0';
        smurfFile.close();
    } else {
        throw logic_error("Invalid file name");
    }

    parser parser(grammar_t);
    setup_ast_generation(parser);

    ParseTreeNode val = ParseTreeNode();
    if(parser.parse(smurfCode, val)) {
        cout << val.to_string() << " = " << val.get()->accept(new Interpreter()) << endl;
    }

//    parser.enable_ast();
//    parser.enable_packrat_parsing();

//    cout << "at AST" << endl;
//    shared_ptr<Ast> ast;
//    if(parser.parse(smurfCode, ast)) {
//        cout << "Started parsing" << endl;
//        cout << ast_to_s(ast) << endl;
//        ast = AstOptimizer(true).optimize(ast);
//        cout << ast_to_s(ast) << endl;
//        evaluate(*ast, scope);
//    }

    delete[] smurfCode;
}

//int getVar(string ident, Closure &scope) {
//    auto has = scope.contains(ident);
//    if(has) {
////        return scope.get(ident).evaluate();
//        return scope.get(ident).value.get<int>();
//    } else {
//        throw runtime_error("Variable " + ident + "has not been defined");
//    }
//}

//int evaluate(const Ast& ast, Closure &scope) {
//    int result = 0;
//    if(ast.name == "Int") {
//        return stol(ast.token);
//    } else if(ast.name == "Ident") {
//        return getVar(ast.token, scope);
//    } else if(ast.name == "Arith_expr") {
//        return evaluate_arithmeticTerms(ast, scope);
//    } else if(ast.name == "Mul_term") {
//        return evaluate_mulTerms(ast, scope);
//    } else if(ast.name == "Var_Dec") {
//        return evaluate_varDec(ast, scope);
//    } else if(ast.name == "Dec") {
//        return evaluate_Dec(ast, scope);
//    } else if(ast.name == "Assignment") {
//        return evaluate_assignment(ast, scope);
//    } else if(ast.name == "Bool_expr") {
//        return evaluate_boolExpr(ast, scope);
//    } else if(ast.name == "If_expr") {
//        return evaluate_ifExpr(ast, scope);
//    } else if(ast.name == "Func_call") {
//        return evaluate_funcCall(ast, scope);
//    } else {
//        const auto& subAst = ast.nodes;
//        for(unsigned int j = 0; j < subAst.size(); j++) {
//            result = evaluate(*subAst[j], scope);
//        }
//    }
//    return result;
//}

//int evaluate_arithmeticTerms(const Ast& ast, Closure &scope) {
//    int result;
//    if(ast.name == "Int") {
//        return stol(ast.token);
//    } else if(ast.name == "Ident") {
//        return getVar(ast.token, scope);
//    } else {
//        const auto& subAst = ast.nodes;
//        result = evaluate(*subAst[0], scope);
//        for(unsigned int j = 1; j<subAst.size(); j+=2) {
//            auto rightSide = evaluate(*subAst[j+1], scope);
//            auto operation = subAst[j]->token;
//            if(operation == "+")
//                result += rightSide;
//            else if(operation == "-")
//                result -= rightSide;

//        }
//    }
//    return result;
//}

//int evaluate_mulTerms(const Ast& ast, Closure &scope) {
//    int result;
//    if(ast.name == "Int") {
//        return stol(ast.token);
//    } else if(ast.name == "Ident") {
//        return getVar(ast.token, scope);
//    } else {
//        const auto& subAst = ast.nodes;
//        result = evaluate(*subAst[0], scope);
//        for(unsigned int j = 1; j<subAst.size(); j+=2) {
//            auto rightSide = evaluate(*subAst[j+1], scope);
//            auto operation = subAst[j]->token;
//            if(operation == "*")
//                result *= rightSide;
//            else if(operation == "/")
//                result /= rightSide;
//        }
//    }
//    return result;
//}

//int evaluate_varDec(const Ast& ast, Closure &scope) {
//    const auto varsToDeclare = ast.nodes;
//    int result = 0;
//    for(unsigned int j = 0; j < varsToDeclare.size(); j++) {
//        result = evaluate_Dec(*varsToDeclare[j], scope);
//    }
//    return result;
//}

//int evaluate_Dec(const Ast& ast, Closure &scope) {
//    //auto iter = binding.find(ast.nodes[0]->token);
////    auto has = scope.contains(ast.nodes[0]->token); Test cases have redefined variables so I got rid of this check
////    if(!has) {
//        if(ast.nodes[1]->name == "Func_def") {
//            auto value = Value(*ast.nodes[1]);
//            //binding.insert(pair<string, Value>(ast.nodes[0]->token, value));
//            scope.declare(ast.nodes[0]->token, value);
//            return 1;
//        } else {
//            auto almostValue = evaluate(*ast.nodes[1], scope);
//            auto value = Value(almostValue);
//            //binding.insert(pair<string, Value>(ast.nodes[0]->token, value));
//            scope.declare(ast.nodes[0]->token, value);
//            return almostValue;
//        }

//        //binding.insert(pair<string,int>(ast.nodes[0]->token, value));
////    } else {
////        throw runtime_error("Variable " + ast.nodes[0]->token + "already defined");
////    }
//}

//int evaluate_assignment(const Ast& ast, Closure &scope) {
//    //auto iter = binding.find(ast.nodes[0]->token);
//    auto has = scope.contains(ast.nodes[0]->token);
//    if(has) {
//        if(ast.nodes[1]->name == "Func_def") {
//            auto value = Value(*ast.nodes[1]);
//            //iter->second = value;
//            scope.assign(ast.nodes[0]->token, value);
//            return 1;
//        } else {
//            auto value = Value(evaluate(*ast.nodes[1], scope));
//            //iter->second = value;
//            scope.assign(ast.nodes[0]->token, value);
////            return value.evaluate();
//            return value.value.get<int>();
//        }
//    } else {
//        throw runtime_error("Variable " + ast.nodes[0]->token + " is not defined");
//    }
//}

//int evaluate_boolExpr(const Ast& ast, Closure &scope) {
//    //'==' / '!=' / '>=' / '>' / '<=' / '<'
//    int eval = 0;
//    const auto fullExpr = ast.nodes;
//    int leftSide = evaluate_arithmeticTerms(*fullExpr[0], scope);
//    int rightSide = evaluate_arithmeticTerms(*fullExpr[2], scope);
//    auto operation = fullExpr[1]->token;
//    if(operation == "==") {
//        eval = int(leftSide == rightSide);
//    } else if(operation == "!=") {
//        eval = int(leftSide != rightSide);
//    } else if(operation == ">=") {
//        eval = int(leftSide >= rightSide);
//    } else if(operation == ">") {
//        eval = int(leftSide > rightSide);
//    } else if(operation == "<=") {
//        eval = int(leftSide <= rightSide);
//    } else {
//        //it is guaranteed that the operator is '<' if it gets here, since the operator definition is a terminal
//        eval = int(leftSide < rightSide);
//    }
//    return eval;
//}

//int evaluate_ifExpr(const Ast& ast, Closure &scope) {
//    const auto nodes = ast.nodes;
//    int pretendBool = evaluate(*nodes[0], scope);

//    if(pretendBool != 0)
//        return evaluate(*nodes[1], scope);

//    if(nodes.size() == 3)
//        return evaluate(*nodes[2], scope);

//    return pretendBool;
//}

//int evaluate_funcCall(const Ast& ast, Closure &scope) {
//    const auto funcCall = ast.nodes;
//    int toOutput = 0;
//    if(funcCall[0]->name == "Print_call") {
//        cout << "Print: ";
//        if(funcCall[1]->name == "Call_args") {
//            for(unsigned int j = 0; j < funcCall[1]->nodes.size(); j++) {
//                toOutput = evaluate(*funcCall[1]->nodes[j], scope);
//                cout << toOutput;
//                if(j < funcCall[1]->nodes.size()-1)
//                    cout << "|";
//            }
//            cout << endl;
//            return toOutput;
//        } else {
//            toOutput = evaluate(*funcCall[1], scope);
//            cout << toOutput << endl;
//            return toOutput;
//        }
//    } else {
//        auto functionNode = scope.get(funcCall[0]->token).value.get<Ast>();
//            Closure innerClosure;
//            innerClosure.linkParent(make_shared<Closure>(scope));
//            int param;
//            if(funcCall[1]->name != "Call_args") {
//                param = evaluate(*funcCall[1], innerClosure);
//                auto val = Value(param);
//                innerClosure.declare(functionNode.nodes[0]->token, val);
//            } else {
//                for(unsigned int j = 0; j < funcCall[1]->nodes.size(); j++) {
//                    param = evaluate(*funcCall[1]->nodes[j], innerClosure);
//                    auto val = Value(param);
//                    innerClosure.declare(functionNode.nodes[0]->nodes[j]->token, val);
//                }
//            }
//            int functionResult = evaluate(*functionNode.nodes[1], innerClosure);
//            return functionResult;
//    }
//}
