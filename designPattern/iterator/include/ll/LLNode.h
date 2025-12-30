#pragma once
#include <memory>
#include "../core/Iterable.h"

using namespace std;

// Forward declaration to resolve circular dependency
template <typename T> class LLIterator;

template <typename T>
class LLNode : public Iterable<T> {
    T val;
    unique_ptr<LLNode<T>> next;
public:
    LLNode(T data) : val(data), next(nullptr) {}
    T getVal() { return val; }
    void setNext(unique_ptr<LLNode<T>> node) { next = move(node); }
    LLNode<T>* getNext() { return next.get(); }

    // Removed LLNode<T>:: prefix inside class block
    unique_ptr<Iterator<T>> getIterator() override;
};

// Include iterator implementation at bottom for template visibility
#include "LLIterator.h"

template <typename T>
unique_ptr<Iterator<T>> LLNode<T>::getIterator() {
    return make_unique<LLIterator<T>>(this);
}