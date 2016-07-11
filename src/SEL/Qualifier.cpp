#include <SEL/Qualifier.h>

using namespace SEL;

Qualifier::Qualifier(QString text)
    : mText(std::move(text))
{
}

Qualifier::~Qualifier() {

}
