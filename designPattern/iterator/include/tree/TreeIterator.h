#pragma once
#include <stack>
#include "../core/Iterator.h"

using namespace std;

template <typename T> class TreeNode;

template <typename T>
class TreeIterator : public Iterator<T> {
    stack<TreeNode<T>*> stk; // History for prev
    stack<TreeNode<T>*> st;  // Current traversal
    
    void insert(TreeNode<T>* curr) {
        if (curr->getRight()) st.push(curr->getRight());
        if (curr->getLeft()) st.push(curr->getLeft());
    }
public:
    TreeIterator(TreeNode<T>* curr) { if (curr) st.push(curr); };
    
    bool hasNext() override { return !st.empty(); }
    bool hasPrev() override { return !stk.empty(); }

    T next() override {
        auto node = st.top(); st.pop();
        stk.push(node);
        insert(node);
        return node->getVal();
    }

    T prev() override {
        auto node = stk.top(); stk.pop();
        // Logic fix: Undo the insertions made during next() to restore stack state
        if (node->getLeft()) st.pop();
        if (node->getRight()) st.pop();
        st.push(node);
        return node->getVal();
    }
};