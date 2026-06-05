#ifndef __STEntry_h
#define __STEntry_h

#include "Token.h"

class STEntry 
{
    public:
        Token* token;
        bool reserved;
    
        STEntry();
        STEntry(Token*);   
        STEntry(Token*, bool);
};
#endif