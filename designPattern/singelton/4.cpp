#include <bits/stdc++.h>   //smart pointers are not worth it for singleton class  //thread safe //Meyer,s Singleton
using namespace std;
class Singleton {
    Singleton() {
        cout << "Constructor called\n";
    }

public:
    Singleton(const Singleton&)=delete;
    Singleton& operator=(const Singleton&)=delete;

    void print() {
        cout << "i am class 1\n";
    }
};
int main(){
    Singleton &ob1=Singleton::getInstance();
    ob1.print();
    Singleton &ob2=Singleton::getInstance();
    ob2.print();

    cout<<(&ob1==&ob2);

}


