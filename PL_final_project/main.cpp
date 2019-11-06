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
        Comment     <- '#' '.*'
        Code        <- Statement*
        Statement   <- 'let' Var_Dec / Assignment / Expr
        Var_Dec     <- Dec (',' Dec)*
        Dec         <- Ident ('=' Expr)?
        Ident       <- [a-z][a-zA-Z_0-9]*
        Var_ref     <- Ident
        If_expr     <- Expr Braces ("else" Braces)?
        Assignment  <- Ident '=' Expr
        Expr        <- 'fn' Func_def / 'if' If_expr / Bool_expr / Arith_expr
        Bool_expr   <- Arith_expr Relop Arith_expr
        Arith_expr  <- Mul_term Addop Arith_expr / Mul_term
        Mul_term    <- Primary Mulop Mul_term / Primary
        Primary     <- Int / Func_call / Var_ref / '(' Arith_expr ')'
        Int         <- '-'? [0-9]+
        Addop       <- '+' / '-'
        Mulop       <- '*' / '/'
        Relop       <- '==' / '!=' / '>=' / '>' / '<=' / '<'
        Func_call   <- Var_ref '(' Call_args ')' / 'print(' Call_args ')'
        Call_args   <- (Expr (',' Expr)*)?
        Func_def    <- Params Braces
        Params      <- '(' Ident (',' Ident)* ')' / '(' ')'
        Braces      <- '{' Code '}'
        %whitespace <- [ \t\r\n]*
    )";

    parser parser;
    auto ok = parser.load_grammar(grammar_t);
    assert(ok);
    parser.enable_ast();

    shared_ptr<Ast> ast;
    if(parser.parse("print(hello)", ast)) {
        cout << ast_to_s(ast) << endl;
        ast = AstOptimizer(true).optimize(ast);
        cout << ast_to_s(ast);
    }
}

//program = code EOF

//comment  = "#" r'.*'

//code = statement*

//statement = "let" variable_declaration
//          | assignment
//          | expr

//variable_declaration  = decl ("," decl)*

//decl = identifier ("=" expr)?

//identifier = [a-z][a-zA-Z_0-9]*

//variable_reference = identifier

//if_expression = expr brace_block ( "else" brace_block )?

//assignment  = identifier "=" expr

//expr  = "fn" function_definition
//      | "if" if_expression
//      | boolean_expression
//      | arithmetic_expression

//boolean_expression  = arithmetic_expression relop arithmetic_expression

//arithmetic_expression  = mult_term addop arithmetic_expression
//                       | mult_term

//mult_term  = primary mulop mult_term
//           | primary

//primary  = integer
//         | function_call
//         | variable_reference
//         | "(" arithmetic_expression ")"


//integer = "-"? [0-9]+

//addop   = '+' | '-'
//mulop   = '*' | '/'
//relop   = '==' | '!=' | '>=' | '>' | '<=' | '<'


//function_call  = variable_reference "(" call_arguments ")"
//               | "print" "(" call_arguments ")"

//call_arguments = (expr ("," expr)*)?

//function_definition = param_list brace_block

//param_list =  "(" identifier ("," identifier)* ")"
//           |  "(" ")"

//brace_block = "{" code  "}"
