#include <iostream>
#include "lex.h"
#include <map>
#include <vector> //These are similar to maps
#include <cctype>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <type_traits>

using namespace std;


static map<string, Token> tokenPrint
{
                {"IDENT",IDENT},
                {"ICONST",ICONST},
                {"RCONST",RCONST},
                {"SCONST", SCONST},
                {"PLUS",PLUS},
                {"MINUS",MINUS},
                {"MULT",MULT},
                {"DIV",DIV},
                {"ASSOP", ASSOP},
                {"LPAREN", LPAREN},
                {"RPAREN",RPAREN},
                { "PROGRAM", PROGRAM} ,
                {"WRITELN", WRITELN} ,
                {"INTEGER", INTEGER} ,
                {"BEGIN", BEGIN} ,
                {"END",END} ,
                {"IF", IF},
                {"REAL", REAL},
                {"STRING", STRING},
                {"VAR", VAR} ,
                {"ELSE", ELSE},
                {"FOR", FOR},
                {"THEN", THEN},
                {"DO", DO},
                {"TO", TO},
                {"DOWNTO",DOWNTO},
                {"COMMA", COMMA},
                {"EQUAL",EQUAL},
                {"GTHAN", GTHAN},
                {"LTHAN", LTHAN},
                {"SEMICOL", SEMICOL},
                {"COLON", COLON},
                {"DOT",DOT},
                {"ERR",ERR},
                {"DONE",DONE}

}; //This is the map that lists all the tokens and their respective outputs (imported from RA5, but modified).
   //This map is also reversed because it is much simpler to search through it this way

/*
 * Also imported from RA5, but we do not need this in this since we don't need to print this out
 * static map <string, string> idPrint =
        {
                {"circle", "IDENT (circle)"},
                {"square", "IDENT (square)"},
                {"rectangle", "IDENT (rectangle)"},
                {"ellipse", "IDENT (ellipse)"},
                {"REPEAT", "IDENT (REPEAT)"}
        };
 */

static map<char, int> op =
{
        //All or most of these have the same enum which is why they are all mapped to the same int.
                {'+', -1}, //PLUS
                {'-', -1}, //MINUS
                {'*', -1}, //MULT
                {'/', -1}, //DIV
                {'=', -1}, //EQUAL
                {'(', -1}, //LPAREN
                {')', -1} //RPAREN
};

vector <string> Tokarr = //the advantage of using an vector is that we dont have to map it to another value
{
            //THis is similar to the "idPrint" map that we made in RA5

                "PROGRAM","WRITELN","INTEGER","BEGIN","END","IF","REAL", "STRING",
                "VAR", "ELSE","FOR","THEN","DO", "TO","DOWNTO","IDENT","ICONST",
                "RCONST","SCONST","PLUS","MINUS","MULT","DIV","ASSOP","LPAREN","RPAREN",
                "COMMA", "EQUAL","GTHAN","LTHAN","SEMICOL","COLON","DOT","ERR","DONE"

}; //these are in the same order as thr lex.h file for efficiency and also accuracy


LexItem id_or_kw(const string& lexeme, int linenum)
{
    //This is from RA 5 and is for just meant for the separation of an identifier and a key word.

    Token tokentest = IDENT;

    map<string,Token>::iterator i = tokenPrint.find(lexeme); //This is the iterator (found in the LAB assignments) that
    //simply iterates through the tokenPrint map we made.
    if(i != tokenPrint.end())
    {
        tokentest = i -> second;
    }
    else if(i == tokenPrint.end())
    {
        return LexItem(tokentest,lexeme,linenum);
    }

    return LexItem(tokentest,lexeme,linenum);
    //Since this is actually a non-void function, this needs a return type
    // The default (IDENT) outside the if and else statements or else it gives us an error.
}

ostream& operator<<(ostream& out, const LexItem& tok)
{
    /*
     *    if (tok.GetToken() == 15) //enum for IDENT token
    {
        out << idPrint[identlex] << " Line #: " << tok.GetLinenum() << endl;
    }
    else
    {
        out << tokenPrint[tok.GetToken()] << " Line #: " << tok.GetLinenum() << endl;
    }
     */ //Imported from RA5, but our formatting is quite different here.

    //Remember that this is the overload operator for the output.
    //THis has a complete overhaul from RA 5 because our output is completely different here.

    string lextok = tok.GetLexeme();  //this function is defined in the driver file
    Token test = tok.GetToken();
    int linenum = tok.GetLinenum();

    if( test == IDENT || test == ICONST || test == RCONST || test == SCONST)
    {
        out << Tokarr.at(test) << "(" << lextok << ")" << endl; //Remember that tokarr is defined globally and
    }                                                              // can be accessed from anywhere
    if(test == ERR)
    {
        out<<"Error in line " << linenum << " (" << lextok << ")" << endl;
        exit(1);
    }
    if( test != IDENT && test != ICONST && test != RCONST && test != SCONST )
    {
        out << Tokarr.at(test) << endl;
    }

    return out;

}

//This function is the main addition to PA1 from RA5
LexItem getNextToken (istream& in, int& linenum)
{

    enum TokState {START, INID, INSTRING, ININT, INREAL, INCOMMENT}; //These are all the different states that the program
    TokState lexstate = START;                                       //Will be on so that it knows what to focus on and execute

    string lexeme = ""; //The default would simply be an empty string
    char ch;

    while(in.get(ch))
    {
        switch(lexstate)
        {
            case START:
                if(ch == '\n')
                {
                    linenum++;
                    continue;
                }
                else if(isspace(ch))
                {
                    lexeme = ch;
                    continue;
                }
                if(isalpha(ch))
                {
                    lexstate = INID;
                    lexeme = toupper(ch); //This is meant to be uppercase already according
                    continue;                //to the question answered by the TA, but it is not
                }                            //working for some reason so I just converted it.
                if(ch == '_')
                {
                    lexstate = INID;
                    lexeme = toupper(ch);
                    continue;
                }
                else if(isdigit(ch))
                {
                    lexstate = ININT;
                    lexeme = ch;
                    continue;
                }
                else if(ch == '\'')
                {
                    lexstate = INSTRING;
                    lexeme = "";
                    continue;
                }
                else if(ch == '.')
                {
                    lexeme = '.';  //this applies for all the statements, so it is assigned here.

                    if(!isdigit(in.peek())) //This was the function that we talked about in recitation
                    {
                        lexeme += in.get();
                        return LexItem(ERR,lexeme,linenum);
                    }
                    lexstate = INREAL;
                    continue;
                }
                else if(ch == '+')
                {return LexItem(PLUS,"+",linenum);}
                
                else if(ch == '-')
                {return LexItem(MINUS,"-",linenum);}
                
                else if(ch == '*')
                {return LexItem(MULT,"*",linenum);}
                
                else if(ch == '/')
                {return LexItem(DIV,"/",linenum);}
                
                else if(ch == '<')
                {return LexItem(LTHAN,"<",linenum);}
               
                else if(ch == '>')
                {return LexItem(GTHAN,">",linenum);}
               
                else if(ch == '=')
                {return LexItem(EQUAL,"=",linenum);}
                
                else if(ch == ',')
                {return LexItem(COMMA,",",linenum);}
                
                else if(ch == ';')
                {return LexItem(SEMICOL,";",linenum);}
                
                if(ch == '(' && in.peek() == '*' )
                {
                    lexstate = INCOMMENT;
                    continue;
                }
                else if(ch == ':')
                {
                    if(in.peek()== '=')
                    {
                        in.get(ch);
                        return LexItem(ASSOP,":=",linenum);
                    }
                    return LexItem(COLON,":",linenum);
                }
                else if(ch == '.')
                {
                    lexeme = '.';
                    if (!isdigit(in.peek()))
                    {
                        lexeme += in.get();
                        return LexItem(ERR, lexeme, linenum);
                    }
                    lexstate = INREAL;
                }
                else if(ch == '(')
                {
                    return LexItem(LPAREN,"(",linenum);

                }
                else if(ch == ')')
                {
                    return LexItem(RPAREN,")",linenum);

                }
                else
                {
                    lexeme = ch;
                    return LexItem(ERR,lexeme,linenum);
                }
                break;

            case INID:

                /*
                 *           if (isdigit(ch) || isalpha(ch))
                lexeme += ch;
            else{
                in.putback(ch);

                 //more code

            }
            break;
                 */
                if(ch =='\n')
                {
                    linenum ++;
                    return id_or_kw(lexeme, linenum);
                }
                else if(isspace(ch))
                {
                    return id_or_kw(lexeme,linenum);
                }
                else if( isdigit(ch) or ch == '_' )
                {
                    lexeme += ch;
                }
                else if (isalpha(ch))
                {
                    lexeme += toupper(ch);
                }
                else if (ch == '?' || ch == '.' || ch == ',' || ch == ';')
                {
                    in.putback(ch);
                    return id_or_kw(lexeme, linenum);
                }
                else if(op.find(ch) != op.end() || ch == in.eof())
                {
                    in.putback(ch);
                    return id_or_kw(lexeme, linenum);
                }
                else
                {
                    lexeme = ch;
                    return LexItem(ERR, lexeme,  linenum);
                }

                break;
                
                /*
                 *        case ININT:
            if (isdigit(ch) || ch == '.'){
                lexeme += ch;
                if (ch == '.'){
                    if (!isdigit(in.peek()))
                        return LexItem(ERR, lexeme, linenum);
                    else
                        lexstate = INREAL;
                }
            }
            else{
                in.putback(ch);
                return LexItem(ICONST, lexeme, linenum);
            }
            break;
                 */

            case ININT:
                if(isdigit(ch))
                {
                    lexeme += ch;
                }
                else  if(ch == '.')
                {
                    lexeme += ch;
                    lexstate = INREAL;
                    if (isspace(in.peek()) or in.peek() == '\n')
                    {
                        return LexItem(ERR, lexeme, linenum);
                    }
                    else if (!isdigit(in.peek()))
                    {
                        lexeme += in.get();
                        return LexItem(ERR, lexeme, linenum);
                    }

                }
                else if (ch == '\n')
                {
                    linenum ++;
                    return LexItem(ICONST, lexeme, linenum);
                }
                else if (isspace(ch))
                {
                    return LexItem(ICONST, lexeme, linenum);
                }
                else if(ch == ',' || ch == ';')
                {
                    in.putback(ch);
                    return LexItem(ICONST, lexeme, linenum);
                }
                else if(op.find(ch) != op.end() || ch == in.eof())
                {
                    in.putback(ch);
                    return LexItem(ICONST, lexeme, linenum);
                }
                else if(isalpha(ch)|| ch == '_')
                {
                    in.putback(ch);
                    return LexItem(ICONST,lexeme,linenum);
                }

                break;

            case INREAL:
                /*
                 *  if (isdigit(ch))
                lexeme += ch;
            else{
                in.putback(ch);
                return LexItem(RCONST, lexeme, linenum);
            }
            break;
                 */
                if (ch == '\n')
                {

                    linenum ++;
                    return LexItem(RCONST, lexeme, linenum);

                }
                else if (isspace(ch))
                {

                    return LexItem(RCONST, lexeme, linenum);

                }
                else if (isalpha(ch) )
                {

                    lexeme += ch;
                    return LexItem(ERR, lexeme, linenum);

                }
                else if (isdigit(ch))
                {

                    lexeme += ch;

                }

                if (ch == ';' || ch == ',' || ch == in.eof())
                {
                    in.putback(ch);
                    return LexItem(RCONST, lexeme, linenum);
                }
                if (op.find(ch) != op.end())
                {
                    in.putback(ch);
                    return LexItem(RCONST, lexeme, linenum);
                }
                else if(ch == '.')
                {
                    lexeme+= ch;
                    return LexItem(ERR,lexeme,linenum);
                }

                break;

            case INSTRING:
                if (ch == '\'')
                {
                    return LexItem(SCONST, lexeme, linenum);
                }
                else if (ch == '\n')
                {
                    return LexItem(ERR, '\'' + lexeme, linenum);
                }
                else if (ch == in.eof())
                {
                    in.putback(ch);
                    return LexItem(ERR, lexeme, linenum);
                }

                lexeme += ch;
                break;

                /*
                 * 
        case INCOMMENT:
            if (ch == '\n'){
                linenum++;
                lexstate = START;
                break;
                 */
            case INCOMMENT:
                if(ch == ')')
                {
                    lexstate = START;
                    continue;
                }
                if(ch == '\n') //Remember that this is a 'newline' statement
                {
                    linenum++;
                    if(in.peek()==-1)
                    {
                        return LexItem(ERR, "Comment Error", linenum-1);
                        continue;
                    }
                }
        }
    }

    return LexItem(DONE, lexeme, linenum);

}