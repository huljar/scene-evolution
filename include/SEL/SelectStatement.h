#ifndef SELECTSTATEMENT_H
#define SELECTSTATEMENT_H

#include <SEL/Object.h>
#include <SEL/SearchCondition.h>
#include <list>

namespace SEL {
    class SelectStatement
    {
    public:
        SelectStatement(std::list<Object*> objList, SearchCondition* searchCond);
        virtual ~SelectStatement();

    protected:
        std::list<Object*> mObjectList;
        SearchCondition* mSearchCond;
    };
}

#endif // SELECTSTATEMENT_H
