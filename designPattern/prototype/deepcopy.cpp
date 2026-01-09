#include <bits/stdc++.h>
using namespace std;
class Address{
    string addr;
    public:
    Address(const string& addr):addr(addr){}

    Address(const Address& other){  //deep copy
        this->addr=other.addr;
    }
    void setAddr(const string& adr){
        this->addr=adr;
    }
    string getAddr(){
        return addr;
    }
};
class Person{
    string name;
    unique_ptr<Address> address;
    public:
    Person(const string& name,unique_ptr<Address> address):name(name),address(move(address)){}

    Person(const Person & other){
        this->name=other.name;
        this->address=make_unique<Address>(*other.address);
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