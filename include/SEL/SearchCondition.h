#ifndef SEARCHCONDITION_H
#define SEARCHCONDITION_H

#include <SEL/BooleanTerm.h>

namespace SEL {
    class SearchCondition
    {
    public:
        SearchCondition(SearchCondition* left, BooleanTerm* right);

    protected:
        SearchCondition* mLeft;
        BooleanTerm* mRight;
    };
}

#endif // SEARCHCONDITION_H
