/* TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!KLKLJKJ!!!!!!!!!!!!!!
 *
 * Finish structs needed for closure and functions
 * RECHECK EVERYTHING BECAUSE I WAS ASLEEP WHILE WRITING THEM
 *
 * Do I want an evaluate function defined for Value struct?
 *  - YES I DO - means I don't have to check the actual variable's type in my parsing logic
 *
 * Fix arithmetic
 *  - since nodes contain other nodes and recursion is used the math is done right to left
 * Function calls in general
 * Function calls during arithmetic/Mult expressions
 *
 */

#include <iostream>
#include <fstream>
#include <peglib.h>
#include <assert.h>
#include <string>
#include <map>

using namespace std;
using namespace peg;

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

struct Value {
    enum Type {Int, Function};

    explicit Value(int val = 0): type(Int), value(val) {}
    //This constructor may need to change
    explicit Value(Ast& ast): type(Function), value(ast) {}

    Value& operator=(const Value& rhs) {
      type = rhs.type;
      value = rhs.value;
      return *this;
    }

    Value& operator=(const int rhs) {
        type = Int;
        value = rhs;
        return *this;
    }

    int evaluate() {
        switch (this->type) {
            case Int:
                return this->value.get<int>();

            case Function:
                //TODO DFLSKFSDKJFL
                break;
        }
    }

    Type type;
    peg::any value;
};

struct Closure {
    Closure(shared_ptr<Closure> parent = nullptr)
        : level(parent ? parent->level + 1 : 0) {}

    void linkParent(shared_ptr<Closure> outerLayer) {
        if(this->parent)
            this->parent->linkParent(outerLayer);
        else
            this->parent = outerLayer;
    }

    bool contains(string s) {
        if(binding.find(s) != binding.end())
            return true;

        return parent && parent->contains(s);
    }

    Value& get(string s) {
        if(binding.find(s) != binding.end())
            return binding.at(s);
        else if(parent)
            return parent->get(s);
        else
            throw runtime_error("Variable '" + s + "' is not defined");
    }

    void declare(string s, Value value) {
        if(binding.find(s) == binding.end())
            binding[s] = value;
        else
            throw runtime_error("Variable '" + s + "' already defined");
    }

    void assign(string& s, Value& value) {
      if (binding.find(s) != binding.end()) {
        Value& thing = binding[s];
        thing.value = value;
        return;
      }
      parent->assign(s, value);
      return;
    }

    map<string, Value> binding;
    shared_ptr<Closure> parent;
    int level;
};

int getVar(string ident, map<string, Value> &binding);
int evaluate(const Ast& ast, map<string, Value> &binding);
int evaluate_arithmeticTerms(const Ast& ast, map<string, Value> &binding);
int evaluate_mulTerms(const Ast& ast, map<string, Value> &binding);
int evaluate_varDec(const Ast& ast, map<string, Value> &binding);
int evaluate_Dec(const Ast& ast, map<string, Value> &binding);
int evaluate_assignment(const Ast& ast, map<string, Value> &binding);
int evaluate_boolExpr(const Ast& ast, map<string, Value> &binding);
int evaluate_ifExpr(const Ast& ast, map<string, Value> &binding);
int evaluate_funcCall(const Ast& ast, map<string, Value> &binding);

Ast visit_arithmetic_expression(Ast& ast);
Ast visit_mult_term(Ast& ast);
Ast visit_integer(Ast& ast);



int main(int argc, const char** argv) {
    Closure scope;
    map<string, Value> binding;
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
    if(parser.parse(smurfCode, ast)) {
        cout << ast_to_s(ast) << endl;
        ast = AstOptimizer(true).optimize(ast);
        cout << ast_to_s(ast) << endl;
        cout << evaluate(*ast, binding) << endl;
    }

    delete[] smurfCode;
}

int getVar(string ident, map<string, Value> &binding) {
    auto iter = binding.find(ident);
    if(iter != binding.end()) {
        return iter->second.evaluate();
    } else {
        throw runtime_error("Variable " + ident + "has not been defined");
    }
}

int evaluate(const Ast& ast, map<string, Value> &binding) {
    int result = 0;
    if(ast.name == "Int") {
        return stol(ast.token);
    } else if(ast.name == "Ident") {
        return getVar(ast.token, binding);
    } else if(ast.name == "Arith_expr") {
        return evaluate_arithmeticTerms(ast, binding);
    } else if(ast.name == "Mul_term") {
        return evaluate_mulTerms(ast, binding);
    } else if(ast.name == "Var_Dec") {
        return evaluate_varDec(ast, binding);
    } else if(ast.name == "Dec") {
        return evaluate_Dec(ast, binding);
    } else if(ast.name == "Assignment") {
        return evaluate_assignment(ast, binding);
    } else if(ast.name == "Bool_expr") {
        return evaluate_boolExpr(ast, binding);
    } else if(ast.name == "If_expr") {
        return evaluate_ifExpr(ast, binding);
    } else if(ast.name == "Func_call") {
        return evaluate_funcCall(ast, binding);
    } else {
        const auto& subAst = ast.nodes;
        for(unsigned int j = 0; j < subAst.size(); j++) {
            result = evaluate(*subAst[j], binding);
        }
    }
    return result;
}

int evaluate_arithmeticTerms(const Ast& ast, map<string, Value> &binding) {
    int result;
    if(ast.name == "Int") {
        return stol(ast.token);
    } else if(ast.name == "Ident") {
        return getVar(ast.token, binding);
    } else {
        const auto& subAst = ast.nodes;
        result = evaluate(*subAst[0], binding);
        for(unsigned int j = 1; j<subAst.size(); j+=2) {
            auto rightSide = evaluate(*subAst[j+1], binding);
            auto operation = subAst[j]->token;
            if(operation == "+")
                result += rightSide;
            else if(operation == "-")
                result -= rightSide;

        }
    }
    return result;
}

int evaluate_mulTerms(const Ast& ast, map<string, Value> &binding) {
    int result;
    if(ast.name == "Int") {
        return stol(ast.token);
    } else if(ast.name == "Ident") {
        return getVar(ast.token, binding);
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
        }
    }
    return result;
}

int evaluate_varDec(const Ast& ast, map<string, Value> &binding) {
    const auto varsToDeclare = ast.nodes;
    int result = 0;
    for(unsigned int j = 0; j < varsToDeclare.size(); j++) {
        result = evaluate_Dec(*varsToDeclare[j], binding);
    }
    return result;
}

int evaluate_Dec(const Ast& ast, map<string, Value> &binding) {
    auto iter = binding.find(ast.nodes[0]->token);
    if(iter == binding.end()) {
        if(ast.nodes[1]->name == "Func_def") {
            auto value = Value(*ast.nodes[1]);
            binding.insert(pair<string, Value>(ast.nodes[0]->token, value));
            return 1;
        } else {
            auto almostValue = evaluate(*ast.nodes[1], binding);
            auto value = Value(almostValue);
            binding.insert(pair<string, Value>(ast.nodes[0]->token, value));
            return almostValue;
        }

        //binding.insert(pair<string,int>(ast.nodes[0]->token, value));
    } else {
        throw runtime_error("Variable " + ast.nodes[0]->token + "already defined");
    }
}

int evaluate_assignment(const Ast& ast, map<string, Value> &binding) {
    auto iter = binding.find(ast.nodes[0]->token);
    if(iter != binding.end()) {
        if(ast.nodes[1]->name == "Func_def") {
            auto value = Value(*ast.nodes[1]);
            iter->second = value;
            return 1;
        } else {
            int value = evaluate(*ast.nodes[1], binding);
            iter->second = value;
            return value;
        }
    } else {
        throw runtime_error("Variable " + ast.nodes[0]->token + " is not defined");
    }
}

int evaluate_boolExpr(const Ast& ast, map<string, Value> &binding) {
    //'==' / '!=' / '>=' / '>' / '<=' / '<'
    int eval = 0;
    const auto fullExpr = ast.nodes;
    int leftSide = evaluate_arithmeticTerms(*fullExpr[0], binding);
    int rightSide = evaluate_arithmeticTerms(*fullExpr[2], binding);
    auto operation = fullExpr[1]->token;
    if(operation == "==") {
        eval = int(leftSide == rightSide);
    } else if(operation == "!=") {
        eval = int(leftSide != rightSide);
    } else if(operation == ">=") {
        eval = int(leftSide >= rightSide);
    } else if(operation == ">") {
        eval = int(leftSide > rightSide);
    } else if(operation == "<=") {
        eval = int(leftSide <= rightSide);
    } else {
        //it is guaranteed that the operator is '<' if it gets here, since the operator definition is a terminal
        eval = int(leftSide < rightSide);
    }
    return eval;
}

int evaluate_ifExpr(const Ast& ast, map<string, Value> &binding) {
    const auto nodes = ast.nodes;
    int pretendBool = evaluate(*nodes[0], binding);

    if(pretendBool != 0)
        return evaluate(*nodes[1], binding);

    if(nodes.size() == 3)
        return evaluate(*nodes[2], binding);

    return pretendBool;
}

int evaluate_funcCall(const Ast& ast, map<string, Value> &binding) {
    const auto func = ast.nodes;
    int toOutput = 0;
    if(func[0]->name == "Print_call") {
        if(func[1]->name == "Call_args") {
                cout << "Print: ";
            for(unsigned int j = 0; j < func[1]->nodes.size(); j++) {
                toOutput = evaluate(*func[1]->nodes[j], binding);
                cout << toOutput;
                if(j < func[1]->nodes.size()-1)
                    cout << "|";
            }
            cout << endl;
            return toOutput;
        } else {
            toOutput = evaluate(*func[1], binding);
            cout << "Print: " << toOutput << endl;
            return toOutput;
        }
    }
}
