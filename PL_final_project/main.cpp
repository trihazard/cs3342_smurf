#include <iostream>
#include <peglib.h>
#include <assert.h>

using namespace std;
using namespace peg;

int main(int argc, const char** argv)
{

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
    auto ok = parser.load_grammar(grammar_t);
    assert(ok);
    parser.log = [](size_t line, size_t col, const string& msg) {
        cerr << line << ":" << col << ": " << msg << "\n";
    };
    parser.enable_ast();
    parser.enable_packrat_parsing();

    shared_ptr<Ast> ast;
    if(parser.parse("print(3)", ast)) {
        cout << ast_to_s(ast) << endl;
        ast = AstOptimizer(true).optimize(ast);
        cout << ast_to_s(ast);
    }
}
