#include <iostream>
#include "lex.h"
#include <map>
#include <vector>
#include <cctype>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <type_traits>

using namespace std; //Keep forgetting to add this!

//This needs to have a main function in it so that we can call and test the other file we made.

int main(int argc, char *argv[])
{
    //remember that we have to initialize these values to what we want in the beginning, it won't do it for us like java.
    int tokcount = 0;
    int linecount = 1;

    ifstream Filein;
    string file = "";

    bool vFLag = false;
    bool idsFlag = false;
    bool iconsts = false;
    bool rconsts = false;
    bool sconsts = false;
    bool comm =false;

    //These are all the maps that we are going to be iterating though so that we can read and edit them
    //and also use them for our output. It is neater and is more efficient.

    //We can even create a map within a map, but this makes it easier to differentiate and debug,
    map<string, int> strMap;
    map<int, int> intMap;
    map<double, int> realMap;
    map<string, int> identMap;
    map <Token, map<string, int>> myMap;


    for(int i = 1; i < argc; i++) //The first command line argument that we take in
    {
        string arg = argv[i];

        if(arg == "-v")
        {vFLag = true;}
        else if (arg == "-iconst")
        {iconsts = true;}
        else if(arg == "-rconst")
        {rconsts = true;}
        else if(arg == "-sconst")
        {sconsts = true;}
        else if(arg == "-ident")
        {idsFlag = true;}

        /*
         * else if (arg.substr(0,1) == "-"){
            cout << "UNRECOGNIZED FLAG " << argv[i];
            exit(1);
        }
         //We dont need to separate and make substrings, it will work
         */

        else if (arg[0] == '-')
        {
            //This refers to the "badflag" test case.
            cout << "UNRECOGNIZED FLAG " << arg << endl;
            return(0);
        }
        else if(file != "") //this basically detects if there are multiple arguments (file names) elegantly.
        {
            cout << "ONLY ONE FILE NAME ALLOWED." << endl;
            return 1;
        }
        else
        {
            file = arg;
        }

    }

    Filein.open(file.c_str()); //This is where we open the file and check for errors.

    if(file == "")
    {
        cout << "NO SPECIFIED INPUT FILE NAME." << endl;
        return 1;
    }
    else if(!Filein)
    {
        cerr << "CANNOT OPEN the File " << file << endl;
        return 1;
    }

    while(true) //we kinda want this to run no matter what
    {
        tokcount++;

        LexItem lex = getNextToken(Filein, linecount); //THIS is the method that we created in the other lex.cpp file.

        //All three of these are defined in the lex.h file we included
        string lextest = lex.GetLexeme();
        Token tok = lex.GetToken();
        linecount = lex.GetLinenum();


        if (tok == ICONST)
        {intMap[stoi(lex.GetLexeme())] = -1;}
        else if (tok == RCONST)
        {realMap[stod(lex.GetLexeme())] = -1;}
        else if (tok == SCONST)
        {strMap[lex.GetLexeme()] = -1;}
        else if (tok == IDENT)
        {identMap[lex.GetLexeme()] = -1;}

        //Dealing with the 'DONE' token
        if (tok != DONE && vFLag && lextest != "Comment Error")
        {cout << lex;}
        else if (tok == DONE)
        {
            //we just subtract one here because we have the DONE token
            tokcount = tokcount -1;
            linecount = linecount -1;
            break;
        }

        if (tok == ERR) //This is if we raise an ERR token
        {
            /*
             * if (vFlag)
            cout << lex << "(" << lex.GetLexeme() << ") Line #: " << linenum << "\n\n";
        cout << "Error on line " << linenum << " (" << lex.GetLexeme() << ")" << "\n";
        exit(1);
             */
            if(lextest == "Comment Error")
            {
                comm = true;
                tokcount -= 1;
                cout << endl << "Missing a comment end delimiters '*)' at line " << linecount << endl;
            }
            else
                cout << lex;
            break;
        }

        if(comm)
        {
            cout << endl << "Missing a comment end delimiters '*)' at line " << linecount << endl;

        }

    }
    /*
     *     cout << "Lines: " << linenum <<endl;
    if (linenum == 0)
        exit(1);
    else
        cout << "Tokens: " << tokCount << endl;
    cout << "\n";
     */

    cout << "Lines: " << linecount << endl;

    if (tokcount > 0)
    {cout << "Tokens: " << tokcount << endl;}

    if(sconsts)
    {
        map<string, int>::iterator itr;
        cout << "STRINGS:" << endl;

        //itrerate through the string map
        while (strMap.begin() != strMap.end()) //While the map is not empty
        {
            itr = strMap.begin();
            string smallest = itr -> first;

            while (itr != strMap.end())
            {
                if (!(itr -> first).compare(smallest))
                {

                    smallest = itr -> first;

                }
                ++itr;
            }

            cout << '\'' << smallest << '\'' << endl;
            strMap.erase(smallest);

        }

    }

    /*
     *    if (iconstsFlag){
        cout << "INTEGERS:" << endl;
        for (auto it = myMap[ICONST].begin(); it != myMap[ICONST].end(); it++)
            cout << it->first << endl;
        return 1;
    }
     */

    if(iconsts) //while the integer constants flag is true
    {
        map<int, int>::iterator itr; //we iterate through our map
        cout << "INTEGERS:" << endl;

        while (intMap.begin() != intMap.end())
        {

            itr = intMap.begin();
            int smallest = itr -> first;

            while (itr != intMap.end())
            {

                if (itr -> first <= smallest)
                {

                    smallest = itr -> first;

                }
                ++itr;
            }

            cout << smallest << endl;
            intMap.erase(smallest);

        }

    }
    if(rconsts)
    {
        cout << "REALS:" << endl;
        map<double, int>::iterator itr;

        while (realMap.begin() != realMap.end())
        {

            itr = realMap.begin();
            double smallest = itr -> first;

            while (itr != realMap.end())
            {

                if (itr -> first <= smallest)
                {

                    smallest = itr -> first;

                }
                ++itr;
            }

            cout << smallest << endl;
            realMap.erase(smallest);


        }

    }



    if (idsFlag)
    {

        cout << "IDENTIFIERS:" << endl;
        map<string, int>::iterator itr;

        /*
 *   if (idsFlag){
    cout << "IDENTIFIERS:" << endl;
    for (auto it = myMap[IDENT].begin(); it != myMap[IDENT].end(); it++){
        cout << it->first;
        if (it != prev(myMap[IDENT].end()))
            cout << ", ";
}
 */
        while (identMap.begin() != identMap.end())
        {
            itr = identMap.begin();
            string smallest = itr -> first;

            while(itr != identMap.end())
            {
                if (!(itr -> first).compare(smallest))
                {smallest = itr -> first;}
                ++itr;
            }

            identMap.erase(smallest); //.erase() is essentially the del function for a map

            if (identMap.begin() != identMap.end())
            {cout << smallest << ", ";}

            else if(identMap.begin() == identMap.end())
            {cout << smallest << endl;}

        }

    }

    return 0;
}