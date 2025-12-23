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

enum class Type{
    BASIC,
    STANDARD,
    PREMIUM
};

class Factory{
    Type t;
    public:
    static unique_ptr<Burger> create(Type t){
        if(t==Type::BASIC){
            return make_unique<BasicBurger>();
        }else if(t==Type::STANDARD){
            return make_unique<StandardBurger>();
        }else{
            return make_unique<PremiumBurger>();
        }

    }
};
int main(){
    Type t=Type::PREMIUM;
    unique_ptr<Burger> b=Factory::create(t);
    b->prepare();
    
}