#include "ChildHolderElement.h"

#include <algorithm>

std::vector<std::shared_ptr<Element>>& ChildHolderElement::getElements() {
    return this->elements;
}

void ChildHolderElement::setElements(std::vector<std::shared_ptr<Element>> elements) {
    this->elements.clear();
    this->elements = std::move(elements);
}

void ChildHolderElement::removeElement(Element* element) {
    this->elements.erase(
        std::remove_if(
            this->elements.begin(),
            this->elements.end(),
            [element](const std::shared_ptr<Element>& e) {
                return e.get() == element;
            }
        ),
        this->elements.end()
    );
}
