/* TODO/Problems: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!KLKLJKJ!!!!!!!!!!!!!!
 *
 * Add comments to stuff to make it more legible
 *
 *
 * Closures don't work; when I run add_n(2) and it evaluates the inner function
 * my current scope is getting deleted somehow, don't know how to fix
 *
 */

#include <iostream>
#include <fstream>
#include <peglib.h>
#include <assert.h>
#include <string>
#include <map>
#include <vector>

using namespace std;
using namespace peg;


struct Value;
struct Closure;
struct Function;

struct FunctionObject {
    FunctionObject(Ast& function, Closure* localC) : localClosure(localC), value(function){}

    Closure* localClosure;
    Ast value;
};

struct Value {
    enum Type {Int, Function};

    explicit Value(int val = 0): type(Int), value(val) {}
    //This constructor may need to change
    explicit Value(Ast& currAst, Closure* localC): type(Function), value(FunctionObject(currAst, localC)) {}

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

    Value& operator+=(const Value& rhs) {
        if(this->type != rhs.type)
            throw logic_error("You tried to add different variable types");
        switch (this->type) {
            case Int:
            this->value = this->value.get<int>() + rhs.value.get<int>();
            break;

            case Function:
            throw logic_error("You tried to add function declarations");
        }
        return *this;
    }

    Value& operator-=(const Value& rhs) {
        if(this->type != rhs.type)
            throw logic_error("You tried to subtract different variable types");
        switch (this->type) {
            case Int:
            this->value = this->value.get<int>() - rhs.value.get<int>();
            break;

            case Function:
            throw logic_error("You tried to subtract function declarations");
        }
        return *this;
    }

    Value& operator*=(const Value& rhs) {
        if(this->type != rhs.type)
            throw logic_error("You tried to multiply different variable types");
        switch (this->type) {
            case Int:
            this->value = this->value.get<int>() * rhs.value.get<int>();
            break;

            case Function:
            throw logic_error("You tried to multiply function declarations");
        }
        return *this;
    }

    Value& operator/=(const Value& rhs) {
        if(this->type != rhs.type)
            throw logic_error("You tried to divide different variable types");
        switch (this->type) {
            case Int:
            this->value = this->value.get<int>() / rhs.value.get<int>();
            break;

            case Function:
            throw logic_error("You tried to divide function declarations");
        }
        return *this;
    }

    bool operator==(const Value& rhs) {
        if(this->type != rhs.type)
            throw logic_error("You tried to compare different variable types");
        switch (this->type) {
            case Int:
            return (this->value.get<int>() == rhs.value.get<int>());

            case Function:
            throw logic_error("You tried to compare function declarations");
        }
    }

    bool operator!=(const Value& rhs) {
        if(this->type != rhs.type)
            throw logic_error("You tried to compare different variable types");
        switch (this->type) {
            case Int:
            return (this->value.get<int>() != rhs.value.get<int>());

            case Function:
            throw logic_error("You tried to compare function declarations");
        }
    }

    bool operator<=(const Value& rhs) {
        if(this->type != rhs.type)
            throw logic_error("You tried to compare different variable types");
        switch (this->type) {
            case Int:
            return (this->value.get<int>() <= rhs.value.get<int>());

            case Function:
            throw logic_error("You tried to compare function declarations");
        }
    }

    bool operator>=(const Value& rhs) {
        if(this->type != rhs.type)
            throw logic_error("You tried to compare different variable types");
        switch (this->type) {
            case Int:
            return (this->value.get<int>() >= rhs.value.get<int>());

            case Function:
            throw logic_error("You tried to compare function declarations");
        }
    }

    bool operator<(const Value& rhs) {
        if(this->type != rhs.type)
            throw logic_error("You tried to compare different variable types");
        switch (this->type) {
            case Int:
            return (this->value.get<int>() < rhs.value.get<int>());

            case Function:
            throw logic_error("You tried to compare function declarations");
        }
    }

    bool operator>(const Value& rhs) {
        if(this->type != rhs.type)
            throw logic_error("You tried to compare different variable types");
        switch (this->type) {
            case Int:
            return (this->value.get<int>() > rhs.value.get<int>());

            case Function:
            throw logic_error("You tried to compare function declarations");
        }
    }

    friend ostream& operator<<(ostream &output, const Value rhs) {
        switch (rhs.type) {
            case Int:
            output << rhs.value.get<int>();
            return output;

            case Function:
            output << "This is a function of type: " << rhs.value.get<FunctionObject>().value.name;
            return output;
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
//        if(binding.find(s) == binding.end()) Test cases use the "Let" keyword multiple times for the same variable so I took out this error handling
            binding[s] = value;
//        else
//            throw runtime_error("Variable '" + s + "' already defined");
    }

    void assign(string s, Value value) {
      if (binding.find(s) != binding.end()) {
        Value& thing = binding[s];
        thing.value = value.value.get<int>();
        return;
      }
      parent->assign(s, value);
      return;
    }

    map<string, Value> binding;
    shared_ptr<Closure> parent;
    int level;
};

Value getVar(string ident, Closure &scope);
Value evaluate(Ast& ast, vector<Closure> &closureCollection, Closure* currScope);
Value evaluate_arithmeticTerms(Ast& ast, vector<Closure> &closureCollection, Closure* currScope);
Value evaluate_mulTerms(Ast& ast, vector<Closure> &closureCollection, Closure* currScope);
Value evaluate_varDec(Ast& ast, vector<Closure> &closureCollection, Closure* currScope);
Value evaluate_Dec(Ast& ast, vector<Closure> &closureCollection, Closure* currScope);
Value evaluate_assignment(Ast& ast, vector<Closure> &closureCollection, Closure* currScope);
Value evaluate_boolExpr(Ast& ast, vector<Closure> &closureCollection, Closure* currScope);
Value evaluate_ifExpr(Ast& ast, vector<Closure> &closureCollection, Closure* currScope);
Value evaluate_funcCall(Ast& ast, vector<Closure> &closureCollection, Closure* currScope);
Value evaluate_funcDef(Ast& ast, vector<Closure> &closureCollection, Closure* currScope);

int main(int argc, const char** argv) {
    vector<Closure> closureCollection;
    Closure baseScope;
    closureCollection.push_back(baseScope);
    Closure* currClosure = &closureCollection[0];
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
//        cout << ast_to_s(ast) << endl;
        ast = AstOptimizer(true).optimize(ast);
        cout << ast_to_s(ast) << endl;
        cout << evaluate(*ast, closureCollection, currClosure) << endl;
    }

    delete[] smurfCode;
}

Value getVar(string ident, Closure &scope) {
    auto has = scope.contains(ident);
    if(has) {
        return Value(scope.get(ident).value.get<int>());
    } else {
        throw runtime_error("Variable " + ident + "has not been defined");
    }
}

Value evaluate(Ast& ast, vector<Closure> &closureCollection, Closure* currScope) {
    Value result;
    if(ast.name == "Int") {
        result = stol(ast.token);
        return Value(result);
    } else if(ast.name == "Ident") {
        return getVar(ast.token, *currScope);
    } else if(ast.name == "Arith_expr") {
        return evaluate_arithmeticTerms(ast, closureCollection, currScope);
    } else if(ast.name == "Mul_term") {
        return evaluate_mulTerms(ast, closureCollection, currScope);
    } else if(ast.name == "Var_Dec") {
        return evaluate_varDec(ast, closureCollection, currScope);
    } else if(ast.name == "Dec") {
        return evaluate_Dec(ast, closureCollection, currScope);
    } else if(ast.name == "Assignment") {
        return evaluate_assignment(ast, closureCollection, currScope);
    } else if(ast.name == "Bool_expr") {
        return evaluate_boolExpr(ast, closureCollection, currScope);
    } else if(ast.name == "If_expr") {
        return evaluate_ifExpr(ast, closureCollection, currScope);
    } else if(ast.name == "Func_call") {
        return evaluate_funcCall(ast, closureCollection, currScope);
    } else if(ast.name == "Func_def") {
        return evaluate_funcDef(ast, closureCollection, currScope);
    } else {
        const auto& subAst = ast.nodes;
        for(unsigned int j = 0; j < subAst.size(); j++) {
            result = evaluate(*subAst[j], closureCollection, currScope);
//            cout << "Finished " << j << endl;
        }
    }
    return result;
}

Value evaluate_arithmeticTerms(Ast& ast, vector<Closure> &closureCollection, Closure* currScope) {
    Value result;
    if(ast.name == "Int") {
        return Value(stol(ast.token));
    } else if(ast.name == "Ident") {
        return getVar(ast.token, *currScope);
    } else {
        const auto& subAst = ast.nodes;
        result = evaluate(*subAst[0], closureCollection, currScope);
        for(unsigned int j = 1; j<subAst.size(); j+=2) {
            auto rightSide = evaluate(*subAst[j+1], closureCollection, currScope);
            auto operation = subAst[j]->token;
            if(operation == "+")
                result += rightSide;
            else if(operation == "-")
                result -= rightSide;

        }
    }
    return result;
}

Value evaluate_mulTerms(Ast& ast, vector<Closure> &closureCollection, Closure* currScope) {
    Value result;
    if(ast.name == "Int") {
        return Value(stol(ast.token));
    } else if(ast.name == "Ident") {
        return getVar(ast.token, *currScope);
    } else {
        const auto& subAst = ast.nodes;
        result = evaluate(*subAst[0], closureCollection, currScope);
        for(unsigned int j = 1; j<subAst.size(); j+=2) {
            auto rightSide = evaluate(*subAst[j+1], closureCollection, currScope);
            auto operation = subAst[j]->token;
            if(operation == "*")
                result *= rightSide;
            else if(operation == "/")
                result /= rightSide;
        }
    }
    return result;
}

Value evaluate_varDec(Ast& ast, vector<Closure> &closureCollection, Closure* currScope) {
    const auto varsToDeclare = ast.nodes;
    Value result;
    for(unsigned int j = 0; j < varsToDeclare.size(); j++) {
        result = evaluate_Dec(*varsToDeclare[j], closureCollection, currScope);
    }
    return result;
}

Value evaluate_Dec(Ast& ast, vector<Closure> &closureCollection, Closure* currScope) {
        if(ast.nodes[1]->name == "Func_def") {
            auto value = Value(*ast.nodes[1], currScope);
            //binding.insert(pair<string, Value>(ast.nodes[0]->token, value));
            currScope->declare(ast.nodes[0]->token, value);
            return value;
        } else {
            auto value = evaluate(*ast.nodes[1], closureCollection, currScope);
            //auto value = Value(almostValue);
            //binding.insert(pair<string, Value>(ast.nodes[0]->token, value));
            currScope->declare(ast.nodes[0]->token, value);
            return value;
        }
}

Value evaluate_assignment(Ast& ast, vector<Closure> &closureCollection, Closure* currScope) {
    //auto iter = binding.find(ast.nodes[0]->token);
    auto has = currScope->contains(ast.nodes[0]->token);
    if(has) {
        if(ast.nodes[1]->name == "Func_def") {
            auto value = Value(*ast.nodes[1], currScope);
            //iter->second = value;
            currScope->assign(ast.nodes[0]->token, value);
            return value;
        } else {
            auto value = evaluate(*ast.nodes[1], closureCollection, currScope);
            //iter->second = value;
            currScope->assign(ast.nodes[0]->token, value);
//            return value.evaluate();
            return value;
        }
    } else {
        throw runtime_error("Variable " + ast.nodes[0]->token + " is not defined");
    }
}

Value evaluate_boolExpr(Ast& ast, vector<Closure> &closureCollection, Closure* currScope) {
    //'==' / '!=' / '>=' / '>' / '<=' / '<'
    int eval = 0;
    const auto fullExpr = ast.nodes;
    Value leftSide = evaluate_arithmeticTerms(*fullExpr[0], closureCollection, currScope);
    Value rightSide = evaluate_arithmeticTerms(*fullExpr[2], closureCollection, currScope);
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
    return Value(eval);
}

Value evaluate_ifExpr(Ast& ast, vector<Closure> &closureCollection, Closure* currScope) {
    const auto nodes = ast.nodes;
    Value pretendBool = evaluate(*nodes[0], closureCollection, currScope);

    if(pretendBool != Value())
        return evaluate(*nodes[1], closureCollection, currScope);

    if(nodes.size() == 3)
        return evaluate(*nodes[2], closureCollection, currScope);

    return pretendBool;
}

Value evaluate_funcCall(Ast& ast, vector<Closure> &closureCollection, Closure* currScope) {
    const auto funcCall = ast.nodes;
    Value toOutput;
    if(funcCall[0]->name == "Print_call") {
        cout << "Print: ";
        if(funcCall[1]->name == "Call_args") {
            for(unsigned int j = 0; j < funcCall[1]->nodes.size(); j++) {
                toOutput = evaluate(*funcCall[1]->nodes[j], closureCollection, currScope);
                cout << toOutput;
                if(j < funcCall[1]->nodes.size()-1)
                    cout << "|";
            }
            cout << endl;
            return toOutput;
        } else {
            toOutput = evaluate(*funcCall[1], closureCollection, currScope);
            cout << toOutput << endl;
            return toOutput;
        }
    } else {
        auto f = currScope->get(funcCall[0]->token).value.get<FunctionObject>();
        auto functionNode = f.value;
        auto funcScope = f.localClosure;
        Closure innerClosure;
        innerClosure.linkParent(make_shared<Closure>(*funcScope));
        Value param;
        if(funcCall[1]->name != "Call_args") {
//             try {
//                 param = evaluate(*funcCall[1], closureCollection, &innerClosure);
//             } catch (logic_error) {
                   param = evaluate(*funcCall[1], closureCollection, currScope);
//             }
               innerClosure.declare(functionNode.nodes[0]->token, param);
        } else {
            for(unsigned int j = 0; j < funcCall[1]->nodes.size(); j++) {
                param = evaluate(*funcCall[1]->nodes[j], closureCollection, &innerClosure);
                innerClosure.declare(functionNode.nodes[0]->nodes[j]->token, param);
            }
        }
        Value functionResult = evaluate(*functionNode.nodes[1], closureCollection, &innerClosure);
        return functionResult;
    }
}

Value evaluate_funcDef(Ast& ast, vector<Closure> &closureCollection, Closure* currScope) {
    closureCollection.push_back(*currScope);
    return Value(ast, &closureCollection[closureCollection.size()-1]);
}
