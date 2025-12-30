#include <bits/stdc++.h>      //.get() returns the raw pointer managed by the smart pointer, without transferring ownership.
using namespace std;
template <typename T>
class Iterator{
    public:
    virtual bool hasNext()=0;
    virtual bool hasPrev()=0;
    virtual T prev()=0;
    virtual T next()=0;
    virtual ~Iterator()=default;
};

template <typename T>
class Iterable{
    public:
    virtual unique_ptr<Iterator<T>> getIterator()=0;
    virtual ~Iterable()=default;
};
template <typename T>
class LLNode:public Iterable<T>{
    T val;
    unique_ptr<LLNode<T>> next;
    public:
    LLNode(T data):val(data),next(nullptr){}
    T getVal(){
        return val;
    }
    void setNext(unique_ptr<LLNode<T>> node){
        next=move(node);
    }

   LLNode<T>* getNext(){
        return next.get();
   }

   unique_ptr<Iterator<T>> getIterator();

};
template <typename T>
class TreeNode:public Iterable<T>{
    T val;
    unique_ptr<TreeNode<T>> left;
    unique_ptr<TreeNode<T>> right;
    public:
    TreeNode(T val):val(val),left(nullptr),right(nullptr){}
    T getVal(){
        return val;
    }
    void setLeft(unique_ptr<TreeNode> l){
        left=move(l);
    }
    void setRight(unique_ptr<TreeNode> r){
        right=move(r);
    }

    TreeNode<T>* getLeft(){
        return left.get();
    }
    TreeNode<T>* getRight(){
        return right.get();
    }

    unique_ptr<Iterator<T>> getIterator();
};
class Song{
    string name;
    string artist;
    public:
    Song(const string &name,const string &artist):name(name),artist(artist){}
    string getSong(){
        ostringstream oss;
        oss<<"[song]: "<<name<<" by "<<"["<<artist<<"]";
        return oss.str();
    }
};
class PlayList:public Iterable<string>{
    vector<unique_ptr<Song>> songs;
    public:
    void addSong(unique_ptr<Song> song){
        songs.push_back(move(song));
    }
    unique_ptr<Iterator<string>> getIterator();
};
template <typename T>
class LLIterator:public Iterator<T>{
    LLNode<T>* curr;
    stack<LLNode<T>*> stk;
    public:
    LLIterator(LLNode<T>* node):curr(node){};
    bool hasNext(){
        return curr!=nullptr;
    }
    bool hasPrev(){
        return !stk.empty();
    }
    T next(){
        T val=curr->getVal();
        stk.push(curr);
        curr=curr->getNext();
        return val;
    }
    T prev(){
        auto node=stk.top(); stk.pop();
        curr=node;
        T val=node->getVal(); 
        return val;
    }
};

class PlaylistIterator:public Iterator<string>{
    vector<unique_ptr<Song>>& songs;
    int idx;
    public:
    PlaylistIterator(vector<unique_ptr<Song>>& songs):songs(songs),idx(0){}
    bool hasNext(){
        return idx<songs.size();
    }
    string next(){
        string val=songs[idx]->getSong();
        idx++;
        return val;
    }
    bool hasPrev(){
        return idx>0;
    }
    string prev(){
        idx--;
        string val=songs[idx]->getSong();
        return val;
    }
};
template <typename T>
class TreeIterator:public Iterator<T>{
    stack<TreeNode<T>*> stk;
    stack<TreeNode<T>*> st;
    void insert(TreeNode<T>* curr){
        if(curr->getRight()){
            st.push(curr->getRight());
        }
        if(curr->getLeft()){
            st.push(curr->getLeft());
        }
    }
    public:
    TreeIterator(TreeNode<T>* curr){
        st.push(curr);
    };
    bool hasNext(){
        return !st.empty();
    }
    T next(){
        auto node=st.top(); st.pop();
        stk.push(node);
        insert(node);
        return node->getVal();
    }
    bool hasPrev(){
        return !stk.empty();
    }
    T prev(){
        auto node=stk.top(); stk.pop();
        return node->getVal();
    }
};

template <typename T>
unique_ptr<Iterator<T>> LLNode<T>::getIterator(){
    return make_unique<LLIterator<T>>(this);
}
template <typename T>
unique_ptr<Iterator<T>> TreeNode<T>::getIterator(){
    return make_unique<TreeIterator<T>>(this);
}

unique_ptr<Iterator<string>> PlayList::getIterator(){
    return make_unique<PlaylistIterator>(songs);
}
int main(){
    unique_ptr<LLNode<int>> head=make_unique<LLNode<int>>(1);
    unique_ptr<LLNode<int>> second=make_unique<LLNode<int>>(2);
    unique_ptr<LLNode<int>> third=make_unique<LLNode<int>>(3);

    third->setNext(make_unique<LLNode<int>>(4));

    second->setNext(move(third));
    head->setNext(move(second));


    auto it=head->getIterator();


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

    unique_ptr<PlayList> playlist=make_unique<PlayList>();
    playlist->addSong(make_unique<Song>("Sab Tera","Mangal"));
    playlist->addSong(make_unique<Song>("Husn","Anuv Jain"));
    playlist->addSong(make_unique<Song>("Bakbak","Shailu"));

    auto it1=playlist->getIterator();

    while(it1->hasNext()){
        cout<<it1->next()<<endl;
    }
    cout<<"prev Mode"<<endl;
    while(it1->hasPrev()){
        cout<<it1->prev()<<endl;
    }

    unique_ptr<TreeNode<int>> root=make_unique<TreeNode<int>>(1);
    unique_ptr<TreeNode<int>> l1=make_unique<TreeNode<int>>(2);
    unique_ptr<TreeNode<int>> r1=make_unique<TreeNode<int>>(3);

    r1->setLeft(make_unique<TreeNode<int>>(6));
    l1->setLeft(make_unique<TreeNode<int>>(4));
    l1->setRight(make_unique<TreeNode<int>>(5));

    root->setLeft(move(l1));
    root->setRight(move(r1));

    auto it2=root->getIterator();
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