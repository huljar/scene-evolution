#include <SEL/MoveAction.h>

using namespace SEL;

MoveAction::MoveAction(Object* toObj, SearchCondition* searchCond)
    : mObj(toObj)
    , mSearchCond(searchCond)
{
}

MoveAction::MoveAction(const MoveAction& other)
    : mObj(other.mObj)
    , mSearchCond(other.mSearchCond)
{
}

MoveAction& MoveAction::operator=(const MoveAction& other) {
    delete mObj;
    delete mSearchCond;

    mObj = other.mObj;
    mSearchCond = other.mSearchCond;

    return *this;
}

MoveAction::~MoveAction() {
    delete mObj;
    delete mSearchCond;
    std::cerr << "Deleting MoveAction" << std::endl;
}

void MoveAction::exec() const {
    // TODO: implement
}

MoveAction* MoveAction::clone() const {
    return new MoveAction(*this);
}

void MoveAction::print(std::ostream& os) const {
    os << "MoveAction";
}
