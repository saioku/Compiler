#include "parse.h"


map<string, bool> defVar;
map<string, Token> SymTable;

namespace Parser {
    bool pushed_back = false;
    LexItem	pushed_token;

    static LexItem GetNextToken(istream& in, int& line) {
        if( pushed_back ) {
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line);
    }

    static void PushBackToken(LexItem & t) {
        if( pushed_back ) {
            abort();
        }
        pushed_back = true;
        pushed_token = t;
    }

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
    ++error_count;
    cout << line << ": " << msg << endl;
}

bool Prog(istream& in, int& line)
{
    LexItem tok;

    //    if (tok.GetToken() == BEGIN) {
//        sl = StmtList(in, line);
//        if( !sl  )
//            ParseError(line, "No statements in program");
//        if( error_count > 0 )
//            return false;
//    }


    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() != PROGRAM) //If there is no program at all
    {
        ParseError(line, "Missing PROGRAM."); //The error that we wish to print out.
        return false;

        ///	return true;
    }

    tok = Parser::GetNextToken(in, line);
//    if (tok.GetToken() == END)
//        return true;
//    else if(tok.GetToken() == ERR){
//        ParseError(line, "Unrecognized Input Pattern");
//        cout << "(" << tok.GetLexeme() << ")" << endl;
//        return false;
    if (tok != IDENT) //If there is no name to the program (after detecting the existence of program)
    {
        ParseError(line, "Missing Program Name.");
        return false;
    }

    tok = Parser::GetNextToken(in, line);

    if (!DeclBlock(in, line))
    {
        ParseError(line,"Incorrect Declaration Section.");
        return false;
    }

    if(tok != SEMICOL)
    {
        ParseError(line, "Missing semicolon in Statement");
        return false;
    }

    if (!ProgBody(in, line)) //If the program body itself was wrong.
    {
        ParseError(line,"Incorrect Program Body.");
        return false;
    }

    return true;

}

bool DeclBlock(istream &in, int &line)
{
    LexItem tok = Parser::GetNextToken(in, line);//Using the scope operator to specify
    //This is using "inline LexItem getNextToken(istream& in, int& lineNum)"


    if (tok == VAR) //We're going to handle all the variable declarations in this section.
    {

        tok = Parser::GetNextToken(in, line);

//        while (tok != BEGIN)
//        {
//            p::tokenPush(tok);
//            if (DeclStmt(in, line))
//            {
//                tok = p::GetNextToken(in, line);
//
//                if (tok == SEMICOL) {tok = p::GetNextToken(in, line); continue;}
//                else {ParseErr(line, "Missing semicolon in Statement"); return false;}
//            }
//            else
//            {ParseErr(line, "Syntactic error in Declaration Block."); return false;}
//        }
        while (tok != BEGIN)
        {

            Parser::PushBackToken(tok);
            if (DeclStmt(in, line))
            {
                tok = Parser::GetNextToken(in, line);
                if (tok == SEMICOL)
                {
                    tok = Parser::GetNextToken(in, line);
                    continue;
                }
                else
                {
                    ParseError(line, "Missing semicolon in Statement");
                    return false;
                }
            }
            else
            {
                ParseError(line, "Syntactic error in Declaration Block.");
                return false;
            }
        }

        Parser::PushBackToken(tok);
        return true;
    }

    ParseError(line, "Non-recognizable Declaration Block.");
    return false;
}


bool DeclStmt(istream &in, int &line)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok == IDENT) //The identifier token is a large umbrella
    {
        defVar.insert({tok.GetLexeme(), true}); //we insert this lexeme into the map so that we can use it.

        tok = Parser::GetNextToken(in, line);
        while(tok != COLON) //This defines the scope that we're looking at.
        {
            if(tok == COMMA)
            {
                tok = Parser::GetNextToken(in, line);
                if (tok == IDENT ) //rmr that this is a big category.
                {
                    if (defVar.count(tok.GetLexeme()) != 1)
                    {
                        defVar.insert({tok.GetLexeme(), true});
                        tok = Parser::GetNextToken(in, line);
                    }
                    else
                    {
                        ParseError(line, "Variable Redefinition");
                        return false;
                    }
                    //    bool ex = Var(in,line);
//    if (!ex){
//        ParseError(line, "Missing variable");
//        return false;
//    }
                }
                else
                {
                    ParseError(line,"Using Undefined Variable");
                    return false;
                }
            }
            else { break; }
        }
        if(tok == COLON)
        {
            tok = Parser::GetNextToken(in, line);
            if (tok.GetLexeme()=="INTEGER"||tok.GetLexeme()=="REAL"||tok.GetLexeme()=="STRING") { return true; }
            else
            {
                ParseError(line,"Incorrect Declaration Type.");
                return false;
            }
        }
    }
    return false;
}
bool ProgBody(istream& in, int& line)
{
    //We don't really need status here.
    LexItem tok;

    tok = Parser::GetNextToken(in, line);
    if(tok == BEGIN)
    {
        tok = Parser::GetNextToken(in, line); //Don't forget to get next token

        while(tok != END)
        {

            Parser::PushBackToken(tok);


            if(Stmt(in,line))
            {
                tok = Parser::GetNextToken(in,line);

                if(tok== SEMICOL)
                {
                    tok  = Parser::GetNextToken(in,line);
                    continue;
                }
                    //                if (tok != SCOMA){
//                    ParseError(line, "Missing semicolon");
//                    return false;
//                }
                else
                {
                    line -=1; //won't work without this statement
                    ParseError(line,"Missing semicolon in statement.");
                    return false;
                }


            }
            else
            {
                ParseError(line,"Syntactic error in Program Body.");
                return false;
            }


        }
        return true;
//    tok = Parser::GetNextToken(in,line);
//    if (tok == ERR){
//        ParseError(line, "Unrecognized input pattern (" + tok.GetLexeme() + ")");
//        return false;
//    }

    }

    else
    {
        ParseError(line, "Incorrect Program Body.");
        return false;
    }

}

//Stmt is either a WriteLnStmt, ForepeatStmt, IfStmt, or AssigStmt
//Stmt = AssigStmt | IfStmt | WriteStmt | ForStmt
bool Stmt(istream& in, int& line)
{
    bool status;
    //cout << "in ContrlStmt" << endl;
    LexItem t = Parser::GetNextToken(in, line);

    switch( t.GetToken() ) {

        case WRITELN:
            status = WriteLnStmt(in, line);
            //cout << "After WriteStmet status: " << (status? true:false) <<endl;
            break;

        case IF:
            status = IfStmt(in, line);
            break;

        case IDENT:
            Parser::PushBackToken(t);
            status = AssignStmt(in, line);
            break;

        case FOR:
            status = ForStmt(in, line);
            break;


        default:
            Parser::PushBackToken(t);
            return false;
    }

    return status;
}//End of Stmt


//WriteStmt:= wi, ExpreList
bool WriteLnStmt(istream& in, int& line) {
    LexItem t;
    //cout << "in WriteStmt" << endl;

    t = Parser::GetNextToken(in, line);
    if( t != LPAREN ) {

        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    bool ex = ExprList(in, line);

    if( !ex ) {
        ParseError(line, "Missing expression after WriteLn");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != RPAREN ) {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }
    //Evaluate: print out the list of expressions values

    return ex;
}
bool IfStmt(istream& in, int& line)
{

    //This is a similar implementation to the starter code
    LexItem t;

    t = Parser::GetNextToken(in, line);
    //    if (Parser::GetNextToken(in,line) != LPAREN){
//        ParseError(line, "Missing left parentheses");
//        return false;
//    }
    if( t != LPAREN ) //If the first token that is returned is not a left parathesis (meaning its missing)
    {
        ParseError(line, "Missing ( after expression");
        return false;
    }

    bool status = LogicExpr(in,line); //change the status!
    if(!status)
    {
        ParseError(line,"Missing if statement Logic Expression");
        return false;
    }


    t = Parser::GetNextToken(in, line);//get the next token before checking the right parenthesis

    if( t != RPAREN )
    {

        ParseError(line, "Missing  ) after expression");
        return false;
    }
    //    if (Parser::GetNextToken(in,line) != RPAREN){
//        ParseError(line, "Missing right parentheses");
//        return false;
//    }

    t = Parser::GetNextToken(in, line);
    if( t != THEN )
    {

        ParseError(line, "If-Stmt Syntax Error");
        return false;
    }
    //    ex = Stmt(in,line);
//    if (!ex){
//        ParseError(line, "Missing statement after then");
//        return false;
//    }
    if(!Stmt(in,line))
    {
        ParseError(line,"Missing Statement for If-Stmt Then-Part");
        return false;
    }


    t = Parser::GetNextToken(in, line);
    if(t == ELSE)
    {
        if(Stmt(in,line))
        {
            return true;
        }

        ParseError(line,"Missing Statement for If-Stmt Then-Part");
        return false;
    }
    else
    {
        Parser::PushBackToken(t);
    }
    return true;
}

bool ForStmt(istream& in, int& line)
{

    if(!Var(in,line))
    {
        ParseError(line, "Missing Var");
        return false;
    }

    LexItem t= Parser::GetNextToken(in, line);
    //we don't need a statement to handle  if the variable is equal to ICONST we only need to print out the appropriate error statements.

    if(t != ASSOP)
    {
        ParseError(line, "Missing ASSign Parenthesis");
        return false;
    }

    t= Parser::GetNextToken(in, line);
    if(t != ICONST)
    {
        ParseError(line, "Missing Initialzation Value in For Statment");
        return false;

    }

    t= Parser::GetNextToken(in, line);
    if(t != TO && t != DOWNTO)
    {
        ParseError(line, "For Statment Syntax Error");
        return false;

    }

    //    if(tok.GetToken() == ERR){
//        ParseError(line, "Unrecognized Input Pattern");
//        cout << "(" << tok.GetLexeme() << ")" << endl;
//        return false;
//    }
//    while ( tok == MULT || tok == DIV  )
//    {
//        Value nextVal;
//        LexItem opTok = tok;
//        t1 = Factor(in, line, nextVal);
//        cout << "status of factor2: " << t1<< endl;
//        if( !t1 ) {
//            ParseError(line, "Missing expression after operator");
//            return false;
//        }

    t= Parser::GetNextToken(in, line);
    if(t != ICONST)
    {
        ParseError(line, "Missing Termination Value in For Statement.");
        return false;

    }

    t= Parser::GetNextToken(in, line);
    if (t != DO)
    {
        ParseError(line, "Missing DO");
        return false;

    }
    if(!Stmt(in,line))
    {
        ParseError(line, "Missing Stmt");
        return false;
    }

    return true;
}

bool AssignStmt(istream& in, int& line)
{
    if(!Var(in,line))
    {
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);

    //    if (varstatus){
//        if ((t=Parser::GetNextToken(in, line)) == EQ){
//            status = Expr(in, line, newval);
//            if(!status) {
//                ParseError(line, "Missing Expression in Assignment Statment");
//                return status;
//            }
//
    if(t != ASSOP)
    {
        ParseError(line,"Missing Assignment Operator");
        return false;
    }

    if(!Expr(in,line))
    {
        line -=1;
        ParseError(line, "Missing Expression in Assignment Statment");
        return false;
    }
    return true;
}
//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
    bool status = false;
    //cout << "in ExprList and before calling Expr" << endl;
    status = Expr(in, line);
    if(!status){
        ParseError(line, "Missing Expression");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == COMMA) {
        //cout << "before calling ExprList" << endl;
        status = ExprList(in, line);
        //cout << "after calling ExprList" << endl;
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
}

bool Expr(istream& in, int& line)
{
    bool status;
    status = Term(in, line);

    if(!status)
    {
        ParseError(line, "Missing Term");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    //    tok = Parser::GetNextToken(in, line);
//    if(tok.GetToken() == ERR){
//        ParseError(line, "Unrecognized Input Pattern");
//        cout << "(" << tok.GetLexeme() << ")" << endl;
//        return false;
//    }
    if(tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        return false;
    }
    //Now we check for the plus and minus operators

    while( tok == PLUS )
    {
        status = Term(in, line);
        if(!status)
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR)
        {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }

    }
    //    while ( tok == PLUS || tok == MINUS )
//    {
//        Value nextVal;
//        LexItem opTok = tok;
//        t1 = Term(in, line, nextVal);
//        if( !t1 )
//        {
//            ParseError(line, "Missing expression after operator");
//            return false;
//        }
//
    while( tok == MINUS)
    {
        status = Term(in, line);
        if(!status)
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR)
        {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }

    }
    Parser::PushBackToken(tok);

    return true;
}

bool Term(istream& in, int& line)
{
    bool status;
    status = SFactor(in, line);

    if(!status)
    {
        ParseError(line, "Missing SFactor");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        return false;
    }

    // This is when we check the operands for multiplication and division.

    while( tok == MULT)
    {
        status = Term(in, line);
        if(!status)
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR)
        {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }

    }

   //    while ( tok == MULT || tok == DIV  )
//    {
//        Value nextVal;
//        LexItem opTok = tok;
//        t1 = Factor(in, line, nextVal);
//        cout << "status of factor2: " << t1<< endl;
//        if( !t1 ) {
//            ParseError(line, "Missing expression after operator");
//            return false;
//        }
//
//        tok	= Parser::GetNextToken(in, line);
//        if(tok.GetToken() == ERR){
//            ParseError(line, "Unrecognized Input Pattern");
//            cout << "(" << tok.GetLexeme() << ")" << endl;
//            return false;
//        }
//        //Evaluate: evaluate the expression for multiplication or division
//        if (opTok == MULT)
//            retVal = retVal * nextVal;
//        else if (opTok == DIV)
//            retVal = retVal / nextVal;
//    }

    while( tok == DIV)
    {
        status = Term(in, line);
        if(!status)
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR)
        {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }

    }
    Parser::PushBackToken(tok);

    return true;
}

bool SFactor(istream& in, int& line)
{
    bool status;
    int sign= 0;
    LexItem t = Parser::GetNextToken(in, line);

//    LexItem t = Parser::GetNextToken(in, line);
//    LexItem tok = p::GetNextToken(in, line);
//    int status = 0;
//    bool flag;
//
//    if(tok == MINUS)
//    {status = -1;}
//    else if(tok == PLUS)
//    {status = 1;}
//        //if it's not a plus or an minus (EXP)
//    else
//    {p::tokenPush(tok); status = 1;}
//    flag = Factor(in,line,status);
//
//    return flag;
//
    if(t == PLUS)
    {
        sign = 1;
    }
    else if(t==MINUS)
    {
        sign = -1;
    }
        //if it's not a plus or an minus (EXP)

    else
    {
        Parser::PushBackToken(t);
        sign = 1;
    }
    status = Factor(in,line,sign);

    return status;
}

bool LogicExpr(istream& in, int& line)
{
    bool status = false;     //This basically checks for the logic of the expression.
    status = Expr(in, line);  //remember to assign flag to false first

    if(!status)
    {
        ParseError(line, "Missing Expression");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    if (tok != EQUAL && tok != GTHAN && tok != LTHAN)
    {
        ParseError(line, "Missing sign");
        return false;
    }
    status = Expr(in, line);
    if(!status)
    {
        ParseError(line, "Missing expression after relational operator");
        return false;
    }

    return true;

}


bool Var(istream &in, int &line)
{
    LexItem tok = Parser::GetNextToken(in, line);
//    if (tok == IDENT){
//        identstr = tok.GetLexeme();
//        if (!(defVar.find(identstr)->second))
//            defVar[identstr] = true;
//        return true;
//    }
    if(tok == IDENT)
    {
        if(defVar.count(tok.GetLexeme()) == 1)
        {
            return true;
        }
        else
        {
            ParseError(line,"Undeclared Variable");
            return false;
        }
    }
    else
    {
        ParseError(line,"Using Undefined Variable");
        return false;
    }
}

bool Factor(istream& in, int& line, int sign) {

    LexItem tok = Parser::GetNextToken(in, line);

    //    if( tok == IDENT ) {
//        //check if the var is defined
//        int val;
//        string lexeme = tok.GetLexeme();
//        if (!(defVar.find(lexeme)->second))
//        {
//            ParseError(line, "Undefined Variable");
//            return false;
//        }
//        retVal = symbolTable.find(lexeme)->second;
//        return true;

    if( tok == IDENT ) {

        string lexeme = tok.GetLexeme();
        if (!(defVar.find(lexeme)->second))
        {
            ParseError(line, "Undefined Variable");
            return false;
        }
        return true;
    }
    else if( tok == RCONST ) {

        return true;
    }
    else if( tok == ICONST ) {

        return true;
    }
    else if( tok == SCONST ) {

        return true;
    }
    else if( tok == LPAREN ) {
        bool ex = Expr(in, line);
        if( !ex ) {
            ParseError(line, "Missing Left Parantheses");
            return false;
        }
        if( Parser::GetNextToken(in, line) == RPAREN )
            return ex;

        ParseError(line, "Missing ) after expression");
        return false;
    }

        //    else if( tok == LPAREN ) {
//        bool ex = Expr(in, line);
//        if( !ex ) {
//            ParseError(line, "Missing expression after (");
//            return false;
//        }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }

    ParseError(line, "Unrecognized input");
    return 0;
}
