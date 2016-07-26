#ifndef ACTION_H
#define ACTION_H

#include <SEL/BisonSymbol.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/RGBDScene.h>

#include <QVector>

namespace SEL {
    class Action : public BisonSymbol
    {
    public:
        Action();
        virtual ~Action();

        virtual void exec() const = 0;

    protected:
        virtual void cutObject(SceneObject& obj, RGBDScene* rgbdScene) const;
        virtual void cutObjects(QVector<SceneObject>& objs, RGBDScene* rgbdScene) const;
    };
}

#endif // ACTION_H
