#include <SEL/Query.h>

using namespace SEL;

Query::Query(SelectStatement* selStmt, std::list<Action*> actList)
    : mSelectStmt(selStmt)
    , mActionList(std::move(actList))
{
}

Query::Query(const Query& other)
    : mSelectStmt(other.mSelectStmt)
    , mActionList(other.mActionList)
{
}

Query& Query::operator=(const Query& other) {
    delete mSelectStmt;
    for(auto it = mActionList.begin(); it != mActionList.end(); ++it)
        delete *it;

    mSelectStmt = other.mSelectStmt;
    mActionList = other.mActionList;

    return *this;
}

Query::~Query() {
    delete mSelectStmt;
    for(auto it = mActionList.begin(); it != mActionList.end(); ++it)
        delete *it;
    std::cerr << "Deleting Query" << std::endl;
}

Query* Query::clone() const {
    return new Query(*this);
}

void Query::print(std::ostream& os) const {
    os << "Query containing " << mActionList.size() << " actions";
}
