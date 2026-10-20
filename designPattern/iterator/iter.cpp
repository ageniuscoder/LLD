#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <sstream>

using namespace std;

// --- Core Interfaces ---

template <typename T>
class Iterator {
public:
    virtual bool hasNext() = 0;
    virtual bool hasPrev() = 0;
    virtual T prev() = 0;
    virtual T next() = 0;
    virtual ~Iterator() = default;
};

template <typename T>
class Iterable {
public:
    virtual Iterator<T>* getIterator() = 0;
    virtual ~Iterable() = default;
};


// --- Corrected Linked List ---

// 1. The Node is no longer Iterable. It just holds data.
template <typename T>
class LLNode {
public:
    T val;
    LLNode<T>* next;
    LLNode(T data) : val(data), next(nullptr) {}
};

// 2. The Iterator navigates the nodes.
template <typename T>
class LLIterator : public Iterator<T> {
    LLNode<T>* curr;
    stack<LLNode<T>*> stk;
public:
    LLIterator(LLNode<T>* startNode) : curr(startNode) {}
    
    bool hasNext() override { return curr != nullptr; }
    bool hasPrev() override { return !stk.empty(); }
    
    T next() override {
        T val = curr->val;
        stk.push(curr);
        curr = curr->next;
        return val;
    }
    
    T prev() override {
        auto node = stk.top(); stk.pop();
        curr = node;
        return node->val;
    }
};

// 3. The actual Collection is the Iterable.
template <typename T>
class LinkedList : public Iterable<T> {
    LLNode<T>* head;
    LLNode<T>* tail;
public:
    LinkedList() : head(nullptr), tail(nullptr) {}
    
    // The collection hides the node creation from the user
    void add(T data) {
        LLNode<T>* newNode = new LLNode<T>(data);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }
    
    Iterator<T>* getIterator() override {
        return new LLIterator<T>(head);
    }
};


// --- Tree ---

template <typename T> class TreeNode;

template <typename T>
class TreeIterator : public Iterator<T> {
    stack<TreeNode<T>*> stk; 
    stack<TreeNode<T>*> st;  
    
    void insert(TreeNode<T>* curr) {
        if (curr->getRight()) st.push(curr->getRight());
        if (curr->getLeft()) st.push(curr->getLeft());
    }
public:
    TreeIterator(TreeNode<T>* curr) { if (curr) st.push(curr); }
    
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
        if (node->getLeft()) st.pop();
        if (node->getRight()) st.pop();
        st.push(node);
        return node->getVal();
    }
};

// Note: For a production system, Tree should also be separated into 
// a 'BinaryTree' collection and 'TreeNode' objects, just like the LinkedList!
template <typename T>
class TreeNode : public Iterable<T> {
    T val;
    TreeNode<T>* left;
    TreeNode<T>* right;
public:
    TreeNode(T val) : val(val), left(nullptr), right(nullptr) {}
    T getVal() { return val; }
    
    void setLeft(TreeNode<T>* l) { left = l; }
    void setRight(TreeNode<T>* r) { right = r; }
    TreeNode<T>* getLeft() { return left; }
    TreeNode<T>* getRight() { return right; }
    
    Iterator<T>* getIterator() override {
        return new TreeIterator<T>(this);
    }
};


// --- Playlist & Song ---

class Song {
    string name;
    string artist;
public:
    Song(const string &name, const string &artist) : name(name), artist(artist) {}
    string getSong() {
        ostringstream oss;
        oss << "[song]: " << name << " by " << "[" << artist << "]";
        return oss.str();
    }
};

class PlaylistIterator : public Iterator<string> {
    vector<Song*>& songs;
    int idx;
public:
    PlaylistIterator(vector<Song*>& songs) : songs(songs), idx(0) {}
    
    bool hasNext() override { return idx < songs.size(); }
    
    string next() override {
        string val = songs[idx]->getSong();
        idx++;
        return val;
    }
    
    bool hasPrev() override { return idx > 0; }
    
    string prev() override {
        idx--;
        string val = songs[idx]->getSong();
        return val;
    }
};

class PlayList : public Iterable<string> {
    vector<Song*> songs;
public:
    void addSong(Song* song) {
        songs.push_back(song);
    }
    
    Iterator<string>* getIterator() override {
        return new PlaylistIterator(songs);
    }
};


// --- Main ---

int main(){
    
    // Look how much cleaner this is now! 
    // You don't have to manually link nodes.
    LinkedList<int>* list = new LinkedList<int>();
    list->add(1);
    list->add(2);
    list->add(3);
    list->add(4);
    
    auto it = list->getIterator();
    
    cout<<"first time"<<endl;
    while(it->hasNext()){
        cout<<it->next()<<" ";
    }
    cout<<endl;
    
    cout<<"second time"<<endl;
    while(it->hasPrev()){
        cout<<it->prev()<<" ";
    }
    cout<<endl;
    
    cout<<"third time"<<endl;
    while(it->hasNext()){
        cout<<it->next()<<" ";
    }
    cout<<endl;
    
    PlayList* playlist = new PlayList();
    playlist->addSong(new Song("Sab Tera","Mangal"));
    playlist->addSong(new Song("Husn","Anuv Jain"));
    playlist->addSong(new Song("Bakbak","Shailu"));
    
    auto it1 = playlist->getIterator();
    while(it1->hasNext()){
        cout<<it1->next()<<endl;
    }
    cout<<"prev Mode"<<endl;
    while(it1->hasPrev()){
        cout<<it1->prev()<<endl;
    }
    
    TreeNode<int>* root = new TreeNode<int>(1);
    TreeNode<int>* l1 = new TreeNode<int>(2);
    TreeNode<int>* r1 = new TreeNode<int>(3);
    
    r1->setLeft(new TreeNode<int>(6));
    l1->setLeft(new TreeNode<int>(4));
    l1->setRight(new TreeNode<int>(5));
    
    root->setLeft(l1);
    root->setRight(r1);
    
    auto it2 = root->getIterator();
    cout<<"forward"<<endl;
    while(it2->hasNext()){
        cout<<it2->next()<<" ";
    }
    cout<<endl;
    
    cout<<"backward"<<endl;
    while(it2->hasPrev()){
        cout<<it2->prev()<<" ";
    }
}