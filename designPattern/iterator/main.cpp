#include <iostream>
#include "include/ll/LLNode.h"
#include "include/tree/TreeNode.h"
#include "include/playlist/playlist.h"
#include "include/playlist/song.h"

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