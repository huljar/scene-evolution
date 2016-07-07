#ifndef ACTION_H
#define ACTION_H

namespace SEL {
    class Action
    {
    public:
        Action();
        virtual ~Action();

        virtual void exec() const = 0;
    };
}

#endif // ACTION_H
