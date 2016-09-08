#ifndef COMPPREDICATE_H
#define COMPPREDICATE_H

#include <SEL/Predicate.h>
#include <SEL/CompElement.h>

namespace SEL {
    class CompPredicate : public Predicate
    {
    public:
        enum class Operator {
            EQ,
            NE,
            LT,
            GT,
            LE,
            GE
        };

        CompPredicate(CompElement* left, Operator op, CompElement* right);
        CompPredicate(const CompPredicate& other);
        CompPredicate& operator=(const CompPredicate& other);
        virtual ~CompPredicate();

        virtual bool eval(SceneObjectManager* sceneObjMgr, int sceneIdx, SceneObject& obj, const DatasetManager::LabelMap& labels) const;

        virtual CompPredicate* clone() const;

    protected:
        template<typename T>
        bool comp(T left, T right) const {
            switch(mOp) {
                case Operator::EQ: return left == right;
                case Operator::NE: return left != right;
                case Operator::LT: return left < right;
                case Operator::GT: return left > right;
                case Operator::LE: return left <= right;
                case Operator::GE: return left >= right;
                default: return false;
            }
        }

        virtual void print(std::ostream& os) const;

        CompElement* mLeft, *mRight;
        Operator mOp;
    };
}

#endif // COMPPREDICATE_H
