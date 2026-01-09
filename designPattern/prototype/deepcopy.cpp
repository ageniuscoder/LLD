#include <bits/stdc++.h>        //if i have primitive data type means something which is allocated in stack will autmatially get deep copy
using namespace std;
class Address{
    string addr;
    public:
    Address(const string& addr):addr(addr){}

    Address(const Address& other){  //deep copy              //here addr get deep copied
        this->addr=other.addr;
    }
    void setAddr(const string& adr){
        this->addr=adr;
    }
    string getAddr(){
        return addr;
    }
};
class Person{                                        //but if i have memory allocated in heap then we have to enforce deep copy
    string name;
    unique_ptr<Address> address;
    public:
    Person(const string& name,unique_ptr<Address> address):name(name),address(move(address)){}

    Person(const Person & other){
        this->name=other.name;
        this->address=make_unique<Address>(*other.address);              //using new keyword for raw ptr and methods for smart pointers 
    }
    void setAddr(const string& addr){
        address->setAddr(addr);
    }
    string getAddr(){
        return address->getAddr();
    }
    unique_ptr<Person> clone(){
        return make_unique<Person>(*this);
    }
};
int main(){
    unique_ptr<Person> p1=make_unique<Person>("mangal",make_unique<Address>("bhabua"));
    unique_ptr<Person> p2=p1->clone();
    p2->setAddr("Akhlapur");
    cout<<p1->getAddr()<<endl;
    cout<<p2->getAddr()<<endl;
}