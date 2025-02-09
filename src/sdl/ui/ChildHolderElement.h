#ifndef CHILD_HOLDER_ELEMENT_H
#define CHILD_HOLDER_ELEMENT_H

#include <memory>
#include <vector>

#include "Element.h"

class ChildHolderElement : public Element {
protected:
    std::vector<std::shared_ptr<Element>> elements;
public:
    explicit ChildHolderElement(const std::weak_ptr<ChildHolderElement>& parent) : Element(parent) {}

    void setElements(std::vector<std::shared_ptr<Element>> elements);
    void removeElement(Element* element);
};

#endif
