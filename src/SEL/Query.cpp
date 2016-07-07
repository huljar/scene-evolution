#include <SEL/Query.h>

using namespace SEL;

Query::Query(SelectStatement* selStmt, std::list<Action*> actList)
    : mSelectStmt(selStmt)
    , mActionList(std::move(actList))
{
}

Query::~Query() {
    delete mSelectStmt;
    for(auto it = mActionList.begin(); it != mActionList.end(); ++it)
        delete *it;
}
