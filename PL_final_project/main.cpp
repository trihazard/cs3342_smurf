#include <iostream>
#include <peglib.h>
#include <assert.h>
#include <string>
#include <map>

using namespace std;
using namespace peg;

int evaluate(const Ast& ast, map<string, int> &binding);
int evaluate_arithmeticTerms(const Ast& ast, map<string, int> &binding);
int evaluate_mulTerms(const Ast& ast, map<string, int> &binding);
int evaluate_varDec(const Ast& ast, map<string, int> &binding);
int evaluate_assignment(const Ast& ast, map<string, int> &binding);

Ast visit_arithmetic_expression(Ast& ast);
Ast visit_mult_term(Ast& ast);
Ast visit_integer(Ast& ast);


struct OperationNode {
    OperationNode(Ast& l, Ast& v, Ast& r)
        : left(l), verb(v), right(r) {}

    Ast& left;
    Ast& verb;
    Ast& right;
};

struct IntegerNode {
    IntegerNode(int val)
        : value(val) {}

    int value;
};

int main(/*int argc, const char** argv*/)
{
    map<string, int> binding;
    auto grammar_t = R"(
        # Grammar for Smurf
        Program     <- Code
        Comment     <- '#' .*
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
        Arith_expr  <- Mul_term Addop Arith_expr / Mul_term
        Mul_term    <- Primary Mulop Mul_term / Primary
        Primary     <- Int / Func_call / Var_ref / '(' Arith_expr ')'
        Int         <- < '-'? [0-9]+ >
        Addop       <- '+' / '-'
        Mulop       <- '*' / '/'
        Relop       <- '==' / '!=' / '>=' / '>' / '<=' / '<'
        Func_call   <- 'print' '(' Call_args ')' / Var_ref '(' Call_args ')'
        Call_args   <- (Expr (',' Expr)*)?
        Func_def    <- Params Braces
        Params      <- '(' Ident (',' Ident)* ')' / '(' ')'
        Braces      <- '{' Code '}'
        %whitespace <- [ \t\r\n]*
    )";

    parser parser;
    if(!parser.load_grammar(grammar_t)) {
        throw logic_error("Invalid PEG grammar");
    }

    parser.log = [](size_t line, size_t col, const string& msg) {
        cerr << line << ":" << col << ": " << msg << "\n";
    };
    parser.enable_ast();
    parser.enable_packrat_parsing();

    shared_ptr<Ast> ast;
    if(parser.parse("let a = 1 \nb = 3 \na = 8", ast)) {
        ast = AstOptimizer(true).optimize(ast);
        cout << ast_to_s(ast) << endl;
        cout << evaluate(*ast, binding) << endl;
    }
}

int evaluate(const Ast& ast, map<string, int> &binding) {
    int result = 0;
    //TODO: The if statements need to account for any and all "leaf nodes" of the AST (I know it's a vector just bear with me)
    if(ast.name == "Int") {
        return stol(ast.token);
    } else if(ast.name == "Arith_expr") {
        return evaluate_arithmeticTerms(ast, binding);
    } else if(ast.name == "Mul_term") {
        return evaluate_mulTerms(ast, binding);
    } else if(ast.name == "Dec") {
        return evaluate_varDec(ast, binding);
    } else if(ast.name == "Assignment") {
        return evaluate_assignment(ast, binding);
    } else {
        const auto& subAst = ast.nodes;
        for(unsigned int j = 0; j < subAst.size(); j++) {
            result = evaluate(*subAst[j], binding);
        }
    }
    return result;
}

int evaluate_arithmeticTerms(const Ast& ast, map<string, int> &binding) {
    int result;
    if(ast.name == "Int") {
        return stol(ast.token);
    } else {
        const auto& subAst = ast.nodes;
        result = evaluate(*subAst[0], binding);
        for(unsigned int j = 1; j<subAst.size(); j+=2) {
            auto rightSide = evaluate(*subAst[j+1], binding);
            auto operation = subAst[j]->token;
            if(operation == "*")
                result *= rightSide;
            else if(operation == "/")
                result /= rightSide;
            else if(operation == "+")
                result += rightSide;
            else if(operation == "-")
                result -= rightSide;

        }
    }
    return result;
}

int evaluate_mulTerms(const Ast& ast, map<string, int> &binding) {
    int result;
    if(ast.name == "Int") {
        return stol(ast.token);
    } else {
        const auto& subAst = ast.nodes;
        result = evaluate(*subAst[0], binding);
        for(unsigned int j = 1; j<subAst.size(); j+=2) {
            auto rightSide = evaluate(*subAst[j+1], binding);
            auto operation = subAst[j]->token;
            if(operation == "*")
                result *= rightSide;
            else if(operation == "/")
                result /= rightSide;
            else if(operation == "+")
                result += rightSide;
            else if(operation == "-")
                result -= rightSide;

        }
    }
    return result;
}

int evaluate_varDec(const Ast& ast, map<string, int> &binding) {
    auto iter = binding.find(ast.nodes[0]->token);
    if(iter == binding.end()) {
        binding.insert(pair<string,int>(ast.nodes[0]->token, stol(ast.nodes[1]->token)));
    } else {
        throw runtime_error("Variable " + ast.nodes[0]->token + "already defined");
    }
}

int evaluate_assignment(const Ast& ast, map<string, int> &binding) {
    auto iter = binding.find(ast.nodes[0]->token);
    if(iter != binding.end()) {
        int newValue = evaluate(*ast.nodes[1], binding);
        return iter->second = newValue;
    } else {
        throw runtime_error("Variable " + ast.nodes[0]->token + " is not defined");
    }
}
