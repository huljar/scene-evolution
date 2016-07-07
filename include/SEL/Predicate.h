#ifndef PREDICATE_H
#define PREDICATE_H

namespace SEL {
    class Predicate
    {
    public:
        Predicate();
        virtual ~Predicate();

        virtual bool eval() const = 0;
    };
}

#endif // PREDICATE_H
