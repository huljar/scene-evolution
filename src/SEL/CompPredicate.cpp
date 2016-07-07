#include <SEL/CompPredicate.h>

using namespace SEL;

CompPredicate::CompPredicate(CompElement* left, Operator op, CompElement* right)
    : mLeft(left)
    , mRight(right)
    , mOp(op)
{
}
