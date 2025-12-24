#include <bits/stdc++.h>   //smart pointers are not worth it for singleton class   //thread safe version
using namespace std;
class Singleton{
    static Singleton* instance;
    static mutex mtx;
    Singleton(){
        cout<<"Constructor callled"<<endl;
    }
    public:
    static Singleton* getInstance(){
        if(!instance){
            lock_guard<mutex> lock(mtx);
            if(!instance){
                instance=new Singleton();
            }
        }
        return instance;
    }
    void print(){
        cout<<"i am class 1"<<endl;
    }
};
Singleton* Singleton::instance=nullptr;
mutex Singleton::mtx;
int main(){
    Singleton *ob1=Singleton::getInstance();
    ob1->print();
    Singleton *ob2=Singleton::getInstance();
    ob2->print();

    cout<<(ob1==ob2);

}