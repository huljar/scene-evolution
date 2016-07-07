#ifndef QUERY_H
#define QUERY_H

#include <SEL/Action.h>
#include <SEL/SelectStatement.h>
#include <list>

namespace SEL {
    class Query
    {
    public:
        Query(SelectStatement* selStmt, std::list<Action*> actList);
        virtual ~Query();

    protected:
        SelectStatement* mSelectStmt;
        std::list<Action*> mActionList;
    };
}

#endif // QUERY_H
