#include <SEL/SelectStatement.h>

using namespace SEL;

SelectStatement::SelectStatement(std::list<Object*> objList, SearchCondition* searchCond)
    : mObjectList(std::move(objList))
    , mSearchCond(searchCond)
{
}

SelectStatement::~SelectStatement() {
    for(auto it = mObjectList.begin(); it != mObjectList.end(); ++it)
        delete *it;
    delete mSearchCond;
}
