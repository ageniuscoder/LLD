#include <bits/stdc++.h>   //smart pointers are not worth it for singleton class
using namespace std;
class Singleton{
    static Singleton* instance;
    Singleton(){
        cout<<"Constructor callled"<<endl;
    }
    public:
    static Singleton* getInstance(){
        if(!instance){
            instance=new Singleton();
        }
        return instance;
    }
    void print(){
        cout<<"i am class 1"<<endl;
    }
};
Singleton* Singleton::instance=nullptr;
int main(){
    Singleton *ob1=Singleton::getInstance();
    ob1->print();
    Singleton *ob2=Singleton::getInstance();
    ob2->print();

    cout<<(ob1==ob2);

}