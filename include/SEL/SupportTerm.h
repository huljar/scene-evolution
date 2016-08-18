#ifndef SUPPORTTERM_H
#define SUPPORTTERM_H

#include <SEL/BinaryPredicate.h>
#include <SEL/Object.h>

namespace SEL {
    class SupportTerm : public BinaryPredicate
    {
    public:
        SupportTerm(Object* obj);
        SupportTerm(const SupportTerm& other);
        SupportTerm& operator=(const SupportTerm& other);
        virtual ~SupportTerm();

        virtual bool eval(SceneObjectManager* sceneObjMgr, const Scene& currentScene, const SceneObject& obj, const DatasetManager::LabelMap& labels) const;

        virtual SupportTerm* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        Object* mObj;
    };
}

#endif // SUPPORTTERM_H
