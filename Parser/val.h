#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <iomanip>

using namespace std;

enum ValType { VINT, VREAL, VSTRING, VBOOL, VERR };

class Value {
    ValType	T;
    bool    Btemp;
    int		Itemp;
    float   Rtemp;
    string	Stemp;

public:
    Value() : T(VERR), Btemp(false), Itemp(0), Rtemp(0.0), Stemp("") {}
    Value(bool vb) : T(VBOOL), Btemp(vb), Itemp(0), Rtemp(0.0), Stemp("") {}
    Value(int vi) : T(VINT), Btemp(false), Itemp(vi), Rtemp(0.0), Stemp("") {}
    Value(float vr) : T(VREAL), Btemp(false), Itemp(0), Rtemp(vr), Stemp("") {}
    Value(string vs) : T(VSTRING), Btemp(false), Itemp(0), Rtemp(0.0), Stemp(vs) {}


    ValType GetType() const { return T; }
    bool IsErr() const { return T == VERR; }
    bool IsInt() const { return T == VINT; }
    bool IsString() const { return T == VSTRING; }
    bool IsReal() const {return T == VREAL;}
    bool IsBool() const {return T == VBOOL;}

    int GetInt() const { if( IsInt() ) return Itemp; throw "RUNTIME ERROR: Value not an integer"; }

    string GetString() const { if( IsString() ) return Stemp; throw "RUNTIME ERROR: Value not a string"; }

    float GetReal() const { if( IsReal() ) return Rtemp; throw "RUNTIME ERROR: Value not an integer"; }

    bool GetBool() const {if(IsBool()) return Btemp; throw "RUNTIME ERROR: Value not a boolean";}

    void SetType(ValType type)
    {
        T = type;
    }

    void SetInt(int val)
    {
        Itemp = val;
    }

    void SetReal(float val)
    {
        Rtemp = val;
    }

    void SetString(string val)
    {
        Stemp = val;
    }

    void SetBool(bool val)
    {
        Btemp = val;
    }

    //    Value Value::operator+(const Value& op) const {
    ///   if( GetType() == op.GetType() )
    // {
    //	if( IsInt() ) return Value( I + op.GetInt() );
    //if( IsReal() ) return Value( R + op.GetReal() );
    //}
    //else if(IsInt() && op.IsReal())
    //{
//
    //		return Value( (float) GetInt() + op.GetReal());
    //	}
    //else if(IsReal() && op.IsInt())
    //{
    //	return Value(GetReal() + (float) op.GetInt());
    //}
    //else
    //{
    //return Value();
    //}
    //}


    // add op to this
    Value operator+(const Value& op) const
    {
        if( GetType() == op.GetType() )
        {
            if( IsInt() ) return Value( Itemp + op.GetInt() );
            if( IsReal() ) return Value( Rtemp + op.GetReal() );
            if( IsString() ) return Value( Stemp + op.GetString() );
        }

        else if(IsInt() && op.IsReal())
        {
            return Value( (float) GetInt() + op.GetReal());
        }

        else if(IsReal() && op.IsInt()) {
            return Value(GetReal() + (float) op.GetInt());
        }

        else if(op.IsString() && IsString()) {
            return Value(Stemp + op.Stemp);
        }
        return Value();

    }


    // subtract op from this
    Value operator-(const Value& op) const
    {
        float numcheck = IsReal();
        int intcheck = IsInt();

        //This was one of the methods that was not needed yet for RA8

        if (numcheck && op.IsInt())
        {return Value( ( this -> GetReal() - (float) op.GetInt() ) );}

        if( (GetType() != op.GetType()) && (!IsInt() && !op.IsReal()) && (!IsReal() && !op.IsInt()))
        {return Value();}

        else if(this -> GetType() == op.GetType())
        {
            if(IsInt() && !IsReal())
            {return Value(Itemp - op.GetInt());}

            if(numcheck)
            {return Value(Rtemp - op.GetReal());}

        }
//        if( GetType() == op.GetType() )
//        {
//            if( IsInt() ) return Value( I - op.GetInt() );
//            if( IsReal() ) return Value( R - op.GetReal() );
//        }
//        else if(IsInt() && op.IsReal())
//        {
//
//            return Value( (float) GetInt() - op.GetReal());
//        }
        else if(intcheck && op.IsReal())
        {return Value((float)(GetInt() - op.GetReal()));}

        return Value();
    }


    // multiply this by op
    Value operator*(const Value& op) const
    {
        if( GetType() == op.GetType() )
        {
            if( IsInt() ) return Value( Itemp * op.GetInt() );
            if( IsReal() ) return Value( Rtemp * op.GetReal() );
        }


        else if(IsInt() && op.IsReal())
        {
            return Value( (float) GetInt() * op.GetReal());
        }

        else if(IsReal() && op.IsInt())
        {
            return Value(GetReal() * (float) op.GetInt());
        }
        return Value();
    }

    Value operator/(const Value& op) const
    {
        if ( IsInt() && op.IsInt() )
        {
            int x = GetInt();

            if ( op.GetInt() != 0.0)
            {
                x /= op.GetInt();
                return Value(x);
            }
        }

        else if ( IsReal() && op.IsInt() )
        {
            float x = GetReal();

            if ( op.GetInt() != 0.0)
            {
                x /= op.GetInt();
                return Value(x);
            }
        }
            // 		else if(IsInt() && op.IsReal())
// 		{

// 			return Value( (float) GetInt() / op.GetReal());
// 		}
// 		else if(IsReal() && op.IsInt())
// 		{
// 			return Value(GetReal() / ((float) op.GetInt()));
// 		}
// 		else
// 		{
// 			return Value();
        else if ( IsInt() && op.IsReal() )
        {
            float x = GetInt();

            if ( op.GetReal() != 0.0)
            {
                x /= op.GetReal();
                return Value(x);
            }
        }
        else if ( IsReal() && op.IsReal() )
        {
            float x = GetReal();

            if ( op.GetReal() != 0.0)
            {
                x /= op.GetReal();
                return Value(x);
            }
        }

        return Value();
    }

    Value operator==(const Value& op) const
    {

        if( GetType() == op.GetType() )
        {
            if( IsInt() ) return Value( (bool)(Itemp == op.GetInt() ));
            if( IsReal() ) return Value( (bool) (Rtemp == op.GetReal() ));
            if( IsString() ) return Value( (bool) (Stemp == op.GetString()));
        }

        else if(IsInt() && op.IsReal())
        {
            return Value( (bool) (((float) GetInt())== op.GetReal()));
        }
        else if(IsReal() && op.IsInt())
        {
            return Value( (bool) (GetReal() == ((float) op.GetInt())));
        }

        else if(IsString() && op.IsString())
        {
            return Value( (bool) (GetString() == op.GetString()));
        }

        else if( op.IsString() && IsString())
        {
            return Value( (bool) (GetString() == op.GetString()));
        }

        return Value();
    }
    // multiply this with op
//    Value Value::operator*(const Value& op) const {
//        if( GetType() == op.GetType() )
//        {
//        	if( IsInt() ) return Value( I * op.GetInt() );
//        	if( IsReal() ) return Value( R * op.GetReal() );
//		}
//		else if(IsInt() && op.IsReal())
//		{
//
//			return Value( (float) GetInt() * op.GetReal());
//		}
//		else if(IsReal() && op.IsInt())
//		{
//			return Value(GetReal() * (float) op.GetInt());
//		}
//		else
//		{
//			return Value();
//		}
//    }

    Value operator>(const Value& op) const
    {

        int intcheck = IsInt();
        float numcheck = IsReal();

// map<string, Value> symbolTable; //symbol Table of variable names and Value objects
// queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

        if( (GetType() != op.GetType()) && (!IsInt() && !op.IsReal()) && (!IsReal() && !op.IsInt()))
        {return Value();}

        if (numcheck && op.IsInt())
        {return Value( ( this -> GetReal() > (float) op.GetInt())); }

        else if(GetType() == op.GetType())
        {

            if(IsInt() && !IsReal())
            {return Value(Itemp > op.GetInt());}

            else if(numcheck)
            {return Value(Rtemp > op.GetReal());}

        }

        else if(intcheck && op.IsReal())
        {return Value((float)(GetInt() > op.GetReal()));}

        return Value();
    }


    Value operator<(const Value& op) const
    {
        
        // map<string, Value> symbolTable; //symbol Table of variable names and Value objects
        // queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

        int intcheck = IsInt();
        float numcheck = IsReal();

        if ((GetType() != op.GetType()) && (!IsInt() && !op.IsReal()) && (!IsReal() && !op.IsInt())) { return Value(); }

        if (numcheck && op.IsInt()) { return Value((this->GetReal() < (float) op.GetInt())); }

        else if (intcheck && op.IsReal()) { return Value((float) (GetInt() < op.GetReal())); }

        else if (GetType() == op.GetType()) 
        {
            if (IsInt() && !IsReal()) { return Value(Itemp < op.GetInt()); }
            else if (numcheck) { return Value(Rtemp < op.GetReal()); }

        }

        return Value();
    }




    friend ostream& operator<<(ostream& out, const Value& op) {
        if( op.IsInt() ) out << op.Itemp;
        else if( op.IsString() ) out << op.Stemp;
        else if( op.IsReal()) out << fixed << showpoint << setprecision(2) << op.Rtemp;
        else if (op.IsBool()) {
            if(op.GetBool()) cout << "true" ;
            else
                cout << "false" ;
        }
        else out << "ERROR";
        return out;
    }
};


#endif
