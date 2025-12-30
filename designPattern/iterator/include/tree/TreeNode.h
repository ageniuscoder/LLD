#pragma once
#include <memory>
#include "../core/Iterable.h"

using namespace std;

template <typename T> class TreeIterator;

template <typename T>
class TreeNode : public Iterable<T> {
    T val;
    unique_ptr<TreeNode<T>> left;
    unique_ptr<TreeNode<T>> right;
public:
    TreeNode(T val) : val(val), left(nullptr), right(nullptr) {}
    T getVal() { return val; }
    
    // Added missing <T> arguments
    void setLeft(unique_ptr<TreeNode<T>> l) { left = move(l); }
    void setRight(unique_ptr<TreeNode<T>> r) { right = move(r); }

    TreeNode<T>* getLeft() { return left.get(); }
    TreeNode<T>* getRight() { return right.get(); }

    unique_ptr<Iterator<T>> getIterator() override;
};

#include "TreeIterator.h"

template <typename T>
unique_ptr<Iterator<T>> TreeNode<T>::getIterator() {
    return make_unique<TreeIterator<T>>(this);
}