#include <SEL/MoveAction.h>

using namespace SEL;

MoveAction::MoveAction(Object* obj, SearchCondition* searchCond)
    : mObj(obj)
    , mSearchCond(searchCond)
{
}
