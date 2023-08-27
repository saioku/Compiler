
#include "parseInt.h" //different from parse.h as we have more values now
#include <queue>


queue<Value> *Qval;
bool flagx;
bool idList(istream& in, int& line);


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
    flagx = true;

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

bool DeclBlock(istream& in, int& line)
{
    bool rtn;
    bool status = false;
    LexItem tok;//Using the scope operator to specify
    //This is using "inline LexItem getNextToken(istream& in, int& lineNum)"

    tok = Parser::GetNextToken(in, line);

    if(tok == VAR) //We're going to handle all the variable declarations in this section.
    {
        status = DeclStmt(in, line);

        while(status)
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

            if(tok != SEMICOL)
            {
                line--;
                ParseError(line, "Missing semicolon in Declaration Statement.");
                rtn = false;
            }

            status = DeclStmt(in, line);
        }

        tok = Parser::GetNextToken(in, line);

        if(tok == BEGIN )
        {
            rtn = true;
            Parser::PushBackToken(tok);
        }
        else
        {
            rtn = false;
            ParseError(line, "Syntactic error in Declaration Block.");
        }
    }
    else
    {
        rtn = false;
        ParseError(line, "Non-recognizable Declaration Block.");
    }

    return rtn;
}

map<string, bool> defvar;
map<string, Token> symTable;

bool DeclStmt(istream& in, int& line)
{
    //we did not need status in PA 2 really, but implementing it here is much more helpful.
    //we cant really use a variable to keep track of the return here as it has multiple levels.
    
    bool status = idList(in, line);

    if (!status)
    {
        ParseError(line, "Incorrect variable in Declaration Statement.");
        return status;
    }

   LexItem t = Parser::GetNextToken(in, line);
 //   if(tok == COLON)
//        {
//            tok = Parser::GetNextToken(in, line);
//            if (tok.GetLexeme()=="INTEGER"||tok.GetLexeme()=="REAL"||tok.GetLexeme()=="STRING") { return true; }
//            else
//            {
//                ParseError(line,"Incorrect Declaration Type.");
//                return false;
//            }
    if(t == COLON)
    {
        t = Parser::GetNextToken(in, line);
        if(t == INTEGER || t == REAL || t == STRING)
        {
            for (auto i = defvar.begin(); i != defvar.end(); i++)
            {
                if (i->second && symTable.find(i->first) == symTable.end())
                {symTable[i->first] = t.GetToken();}
            }
            return true;
        }
            //    bool ex = Var(in,line);
//    if (!ex){
//        ParseError(line, "Missing variable");
//        return false;
//    }
        else
        {
            ParseError(line, "Incorrect Declaration Type.");
            return false;
        }
    }
    else
    {Parser::PushBackToken(t); return false;}

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


bool Stmt(istream& in, int& line)
{
    bool status;
    LexItem t = Parser::GetNextToken(in, line);

    switch( t.GetToken() )
    {
        case WRITELN:
            status = WriteLnStmt(in, line);
            break;

        case IF:
            status = IfStmt(in, line);
            break;

        case IDENT:
            Parser::PushBackToken(t);
            status = AssignStmt(in, line);
            break;

            //we dont need for statement in this case

//        case FOR:
//            status = ForStmt(in, line);
//            break;


        default: //rmr to have the default case
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

    Qval = new queue<Value>();
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
    while (!(*Qval).empty())
    {
        cout << (*Qval).front();
        Qval->pop();
    }
    //Evaluate: print out the list of expressions values

    cout<<endl; //needed a new line. 

    return ex;
}

bool IfStmt(istream& in, int& line) 
{
    Value val;
    bool  ex =false, status ;
    //This is a similar implementation to the starter code

    LexItem t;

    t = Parser::GetNextToken(in, line);
    if( t != LPAREN ) 
    {
        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    ex = LogicExpr(in, line, val);
    
    if( !ex ) 
    {
        ParseError(line, "Missing if statement Logic Expression");
        return false;
    }
//    ex = Stmt(in,line);
//    if (!ex){
//        ParseError(line, "Missing statement after then");
//        return false;
//    }

    t = Parser::GetNextToken(in, line);
    if(t != RPAREN ) 
    {
        ParseError(line, "Missing Right Parenthesis");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != THEN)
    {
        ParseError(line, "If-Stmt Syntax Error");
        return false;
    }
    
    flagx = val.GetBool();
    status = Stmt(in, line);
    
    if(!status)
    {
        ParseError(line, "Missing Statement for If-Stmt Then-Part");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    flagx = !val.GetBool();
    
//    if (Parser::GetNextToken(in,line) != RPAREN){
//        ParseError(line, "Missing right parentheses");
//        return false;
//    }

    if( t == ELSE ) 
    {
        status = Stmt(in, line);
        if(!status)
        {
            ParseError(line, "Missing Statement for If-Stmt Else-Part");
            return false;
        }
        flagx = true;
        //cout << "in IFStmt status of Stmt true" << endl;
        return true;
    }
    
    flagx = true;
    Parser::PushBackToken(t);
    return true;
    
}//End of IfStmt function

//We dont need ForStmt

map<string, Value> symTable2;

bool AssignStmt(istream& in, int& line) 
{
//    if(!Var(in,line))
//    {
//        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
//        return false;
//    }

  
    string strtok;
    Value val;

    LexItem idtok;
    LexItem t;

    bool varstatus = false; 
    bool status = false;


    varstatus = Var( in, line, idtok);

    if (varstatus)
    {
        t = Parser::GetNextToken(in, line);

        if (t == ASSOP)
        {
            status = Expr(in, line, val);
            if(!status)
            {
                ParseError(line, "Missing Expression in Assignment Statment");
                return status;
            }
            if ( val.IsErr())
            {
                ParseError(line, "Illegal Assignment Operation");
                return false;
            }

            strtok = idtok.GetLexeme();
            if (flagx) symTable2[strtok] = val;

            if (symTable[strtok] == STRING)
            {
                if (!val.IsString())
                {
                    ParseError(line, "Illegal Assignment Operation");
                    return false;
                }
            }

            else
            {
                if ( val.IsString() || val.IsBool())
                {
                    ParseError(line, "Illegal Assignment Operation");
                    return false;
                }
                if ( symTable[strtok] == INTEGER)
                {
                    if ( val.IsReal())
                    {
                        if (flagx)
                        {
                            symTable2[strtok].SetType(VINT);
                            symTable2[strtok].SetInt ( (int)val.GetReal());
                        }
                    }
                }
                else
                {
                    if (val.IsInt())
                    {
                        if (flagx)
                        {
                            symTable2[strtok].SetType(VREAL);
                            symTable2[strtok].SetReal ( val.GetInt());
                        }
                    }
                }
            }

        }
//        if(t != ASSOP)
//        {
//            ParseError(line,"Missing Assignment Operator");
//            return false;
//        }
//
//        if(!Expr(in,line))
//        {
//            line -=1;
//            ParseError(line, "Missing Expression in Assignment Statment");
//            return false;
//        }
//        return true;
        else if(t.GetToken() == ERR){
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << t.GetLexeme() << ")" << endl;
            return false;
        }
        else {
            ParseError(line, "Missing Assignment Operator");
            return false;
        }
    }
    else
    {
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }
    return status;
}

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line)
{
    bool status = false;
    Value val;

    status = Expr(in, line, val);
    if(!status)
    {
        ParseError(line, "Missing Expression");
        return false;
    }
    else
    {
        Qval->push(val);

        LexItem tok = Parser::GetNextToken(in, line);
        //if (tok == COMMA) {
//        //cout << "before calling ExprList" << endl;
//        status = ExprList(in, line);
//        //cout << "after calling ExprList" << endl;
//    }
//    else if(tok.GetToken() == ERR){
//        ParseError(line, "Unrecognized Input Pattern");
//        cout << "(" << tok.GetLexeme() << ")" << endl;
//        return false;
//    }
//    else{
//        Parser::PushBackToken(tok);
//        return true;
//    }
//    re
        switch( tok.GetToken() )
        {
            case COMMA:
                status = ExprList(in, line);
                break;

            case ERR:
                ParseError(line, "Unrecognized Input Pattern");
                cout << "(" << tok.GetLexeme() << ")" << endl;
                return false;

            default:
                Parser::PushBackToken(tok);
                return true;
        }

    }

    return status;
}

//Expr:= Term {(+|-) Term}
bool Expr(istream& in, int& line, Value & Val1) 
{
    Value val;
    bool flagy = Term(in, line, val);

    if( !flagy )
    {return false;}

    Val1 = val; //remember to assign this

    LexItem  tok = Parser::GetNextToken(in, line);

//    tok = Parser::GetNextToken(in, line);
//    if(tok.GetToken() == ERR){
//        ParseError(line, "Unrecognized Input Pattern");
//        cout << "(" << tok.GetLexeme() << ")" << endl;
//        return false;
//    }

    if(tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while ( tok == PLUS || tok == MINUS )
    {
        flagy = Term(in, line, val);
        if( !flagy )
        {ParseError(line, "Missing operand after operator"); return false; }
        if ( tok == PLUS )
        {Val1 = Val1 + val;}

        else
        {Val1 = Val1 - val;}
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
        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR)
        {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
        if ( Val1.IsErr())
        {
            ParseError ( line, "Illegal addition or subtraction operation.");
            return false;
        }
    }
   Parser::PushBackToken(tok);

    return true;
}

bool Term(istream& in, int& line, Value &Val1)
{
    Value val;
    bool status = SFactor(in, line, val);

    if( !status )
    {return false;}

    Val1 = val;

   LexItem tok	= Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    
    // This is when we check the operands for multiplication and division.
    
    while ( tok == MULT || tok == DIV )
    {
        status = SFactor(in, line, val);

        if( !status )
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }
        else if ( tok == DIV)
        {
            if ( val.IsReal())
            {
                if ( val.GetReal() == 0.0 )
                {
                    ParseError ( line, "Run-Time Error-Illegal Division By Zero");
                    return false;
                }
            }
            else if ( val.IsInt())
            {
                if ( val.GetInt() == 0 )
                {
                    ParseError ( line, "Run-Time Error-Illegal Division By Zero");
                    return false;
                }
            }
            Val1 = Val1/val;
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
        if ( tok == MULT)
        {
            Val1 = Val1*val;
        }
        if ( Val1.IsErr())
        {
            ParseError ( line, "Run-Time Error-Illegal Mixed Type Operands");
            return false;
        }

        tok	= Parser::GetNextToken(in, line);
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

bool idList(istream& in, int& line)
{
    string ident_str;
    bool status = false;

    LexItem tok = Parser::GetNextToken(in, line);
	if(tok == IDENT)
	{
		ident_str = tok.GetLexeme();

		if (!(defvar.find(ident_str)->second))
		{defvar[ident_str] = true;}

		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	}

	else
	{
		Parser::PushBackToken(tok);
		return true;
	}
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA)
    {status = idList(in, line);}

//    if( tok == SCOMA ) {
//        status = StmtList(in, line);
//    }
//    else if (tok == ERR) {
//        ParseError(line, "Unrecognized Input Pattern");
//        cout << "(" << tok.GetLexeme() << ")" << endl;
//        return false;
//    }
	else if(tok == COLON)
	{
		Parser::PushBackToken(tok);
		return true;
	}

	else
    {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	return status;
	
}


bool Var(istream& in, int& line, LexItem & idtok)
{
	string ident_str;

//    if (tok == IDENT){
//        identstr = tok.GetLexeme();
//        if (!(defvar.find(identstr)->second))
//            defvar[identstr] = true;
//        return true;
//    }
	LexItem tok = Parser::GetNextToken(in, line);
	idtok = tok;
	
	if (tok == IDENT)
    {
		ident_str = tok.GetLexeme();
		
		if (!(defvar.find(ident_str)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}	
		return true;
	}

//    else
//    {
//        ParseError(line,"Using Undefined Variable");
//        return false;
//    }

	else if(tok.GetToken() == ERR)
    {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	return false;
}



bool SFactor(istream& in, int& line, Value &Val1)
{
	Value val;
	LexItem t = Parser::GetNextToken(in, line);

	bool status;
	int sign = 0;

	if(t == MINUS )
	{sign = -1;}

	else if(t == PLUS)
	{sign = 1;}
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
	else
		Parser::PushBackToken(t);
		
	status = Factor(in, line, sign, val);
	if ( status)
    {
		Val1 = val;
		if ( sign != 0 )
        {
			if ( val.IsString() || val.IsBool())
            {
				ParseError ( line, "Illegal Operand Type for Sign Operator");
				return false;
			}
			if ( val.IsInt())
            {Val1.SetInt ( sign*val.GetInt());}

			else if ( val.IsReal())
            {Val1.SetReal ( sign*val.GetReal());}

		}
	}
	return status;
}


bool LogicExpr(istream& in, int& line, Value & Val1)
{
	Value val1, val2;
	bool flagy = Expr(in, line, val1);
	LexItem tok;
	
	if( !flagy )
    {return false;}
	
	tok = Parser::GetNextToken(in, line);

    if ( tok == GTHAN  || tok == EQUAL  || tok == LTHAN)
    {
        flagy = Expr(in, line, val2);
        if( !flagy )
        {
            ParseError(line, "Missing expression after relational operator");
            return false;
        }
        if ( tok == GTHAN ) Val1 = (val1 > val2);
        else if ( tok == EQUAL ) Val1 = (val1 == val2);
        else Val1 = ( val1 < val2);

        if ( !Val1.IsBool())
        {
            ParseError(line, "Run-Time Error-Illegal Mixed Type Operands for a Logic Expression");
            return false;
        }
        return true;
    }

//    if ( tok == GTHAN  || tok == EQUAL  || tok == LTHAN)
//    {
//        flagy = Expr(in, line, val2);
//        if( !flagy )
//        {
//            ParseError(line, "Missing expression after relational operator");
//            return false;
//        }
//        if ( tok == GTHAN ) Val1 = (val1 > val2);
//        else if ( tok == EQUAL ) Val1 = (val1 == val2);
//        else Val1 = ( val1 < val2);
//
//        if ( !Val1.IsBool()) {
//            ParseError(line, "Run-Time Error-Illegal Mixed Type Operands for a Logic Expression");
//            return false;
//        }
//        return true;
//    }
    Val1 = val1;
    if (!Val1.IsBool())
    {
        ParseError(line, "Run-Time Error-Illegal Mixed Type Operands for a Logic Expression");
        return false;
    }

	if(tok.GetToken() == ERR)
    {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

    if (tok.GetToken() != GTHAN && tok.GetToken() != EQUAL && tok.GetToken() != LTHAN)
    {
        ParseError(line, "Missing sign");
        return false;
    }

	Parser::PushBackToken(tok);
	return true;
}

bool Factor(istream& in, int& line, int sign, Value & Val1)
{
	Value val;
	LexItem tok = Parser::GetNextToken(in, line);
    string lexeme = tok.GetLexeme();
    //    if( tok == IDENT ) {
//        //check if the var is defined
//        int val;
//        string lexeme = tok.GetLexeme();
//        if (!(defvar.find(lexeme)->second))
//        {
//            ParseError(line, "Undefined Variable");
//            return false;
//        }
//        retVal = symbolsymTable.find(lexeme)->second;
//        return true;
	if( tok == IDENT )
    {

		string lexeme = tok.GetLexeme();
		if (!(defvar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;	
		}

		if ( symTable2.find(lexeme) == symTable2.end())
        {
			ParseError(line, "Using Undeclared Variable");
			return false;
		}
        
		Val1 = symTable2[lexeme];
		return true;
	}
    
    else if( tok == RCONST )
    {
        Val1.SetType(VREAL);
        Val1.SetReal ( stof(lexeme));
        return true;
    }

	else if( tok == ICONST )
    {
		Val1.SetType(VINT); //you HAVE to change the type before you work on anything else
		Val1.SetInt(stoi(lexeme)); //stoi for this case, stof for the next, etc
		return true;
	}

	else if( tok == SCONST )
    {
		Val1.SetType(VSTRING);
		Val1.SetString(lexeme);
		return true;
	}


	else if(tok == LPAREN)
    {
		bool ex = Expr(in, line, val);

		if( !ex )
        {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
        {
			Val1 = val;
			return ex;
		}
		else
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
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

    Parser::PushBackToken(tok);
    return true;
}