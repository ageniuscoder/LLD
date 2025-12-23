#include <bits/stdc++.h>
using namespace std;
class Burger{
    public:
    virtual void prepare()=0;
    virtual ~Burger()=default;
};
class BasicBurger:public Burger{
    public:
    void prepare(){
        cout<<"Basic Burger"<<endl;
    }
};
class StandardBurger:public Burger{
    public:
    void prepare(){
        cout<<"Standard Burger"<<endl;
    }
};
class PremiumBurger:public Burger{
    public:
    void prepare(){
        cout<<"Premium Burger"<<endl;
    }
};

class BasicWheatBurger:public Burger{
    public:
    void prepare(){
        cout<<"Basic Wheat Burger"<<endl;
    }
};

class StandardWheatBurger:public Burger{
    public:
    void prepare(){
        cout<<"Standard Wheat Burger"<<endl;
    }
};

class PremiumWheatBurger:public Burger{
    public:
    void prepare(){
        cout<<"Premium Wheat Burger"<<endl;
    }
};

enum class Type{
    BASIC,
    STANDARD,
    PREMIUM
};

class Factory{
    public:
    virtual unique_ptr<Burger> create(Type t)=0;
    virtual ~Factory()=default;
};

class SinghBurger:public Factory{
    public:
    unique_ptr<Burger> create(Type t){
        switch(t){
            case Type::BASIC:
                return make_unique<BasicBurger>();
            case Type::STANDARD:
                return make_unique<StandardBurger>();
            case Type::PREMIUM:
                return make_unique<PremiumBurger>();
            default:
                return nullptr;

        }
    }
};

class KingBurger:public Factory{
    public:
    unique_ptr<Burger> create(Type t){
        switch(t){
            case Type::BASIC:
                return make_unique<BasicWheatBurger>();
            case Type::STANDARD:
                return make_unique<StandardWheatBurger>();
            case Type::PREMIUM:
                return make_unique<PremiumWheatBurger>();
            default:
                return nullptr;

        }
    }
};


int main(){
    Type t=Type::STANDARD;
    unique_ptr<Factory> f=make_unique<SinghBurger>();
    unique_ptr<Burger> b=f->create(t);
    b->prepare();
    
}