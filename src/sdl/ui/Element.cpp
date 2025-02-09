#include "Element.h"

#include "ChildHolderElement.h"

void Element::destroy() {
    if (const auto lock = this->parent.lock()) {
        lock->removeElement(this);
    } else {
        std::cerr << "No" << std::endl;
    }
}