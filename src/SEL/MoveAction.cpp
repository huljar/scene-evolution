#include <SEL/MoveAction.h>

using namespace SEL;

MoveAction::MoveAction(Object* toObj, SearchCondition* searchCond)
    : mObj(toObj)
    , mSearchCond(searchCond)
{
}

MoveAction::~MoveAction() {
    delete mObj;
    delete mSearchCond;
}

void MoveAction::exec() const {
    // TODO: implement
}
