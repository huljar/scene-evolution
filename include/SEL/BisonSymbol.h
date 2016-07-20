#ifndef BISONSYMBOL_H
#define BISONSYMBOL_H

#include <iostream>

class BisonSymbol
{
public:
    BisonSymbol() { }

    virtual BisonSymbol* clone() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const BisonSymbol& bs) { bs.print(os); return os; }

protected:
    virtual void print(std::ostream& os) const = 0;

};

#endif // BISONSYMBOL_H
