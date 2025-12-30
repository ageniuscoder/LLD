#pragma once
#include <stack>
#include "../core/Iterator.h"

using namespace std;

template <typename T> class LLNode;

template <typename T>
class LLIterator : public Iterator<T> {
    LLNode<T>* curr;
    stack<LLNode<T>*> stk;
public:
    LLIterator(LLNode<T>* node) : curr(node) {};
    bool hasNext() override { return curr != nullptr; }
    bool hasPrev() override { return !stk.empty(); }
    
    T next() override {
        T val = curr->getVal();
        stk.push(curr);
        curr = curr->getNext();
        return val;
    }
    
    T prev() override {
        auto node = stk.top(); stk.pop();
        curr = node;
        return node->getVal();
    }
};