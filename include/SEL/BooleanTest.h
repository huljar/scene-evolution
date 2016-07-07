#ifndef BOOLEANTEST_H
#define BOOLEANTEST_H

#include <SEL/Predicate.h>

namespace SEL {
    class SearchCondition; // forward declaration to prevent cycle in inclusion graph

    class BooleanTest
    {
    public:
        BooleanTest(Predicate* pred);
        BooleanTest(SearchCondition* searchCond);

        bool isPredicate() const;

    protected:
        Predicate* mPred;
        SearchCondition* mSearchCond;
    };
}

#endif // BOOLEANTEST_H
