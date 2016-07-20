#include <SEL/Qualifier.h>

using namespace SEL;

Qualifier::Qualifier(QString text)
    : mText(std::move(text))
{
}

Qualifier::Qualifier(const Qualifier& other)
    : mText(other.mText)
{
}

Qualifier& Qualifier::operator=(const Qualifier& other) {
    mText = other.mText;

    return *this;
}

Qualifier::~Qualifier() {
    std::cerr << "Deleting Qualifier" << std::endl;
}

Qualifier* Qualifier::clone() const {
    return new Qualifier(*this);
}

void Qualifier::print(std::ostream& os) const {
    os << "Qualifier containing " << mText.toStdString();
}
