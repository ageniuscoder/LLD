#include <bits/stdc++.h>
using namespace std;
class ICharacter{
    public:
    virtual void getAbilities()=0;
    virtual ~ICharacter()=default;
};
class Mario:public ICharacter{
    public:
    void getAbilities(){
        cout<<"Mario ";
    }
};
class Decorator:public ICharacter{
    public:
    virtual void getAbilities()=0;
    virtual ~Decorator()=default;
};
class GunAbility:public Decorator{
    shared_ptr<ICharacter> ch;
    public:
    GunAbility(shared_ptr<ICharacter> ch){
        this->ch=ch;
    }
    void getAbilities(){
        ch->getAbilities();
        cout<<" with gun ability";
    }
};
class HeightAbility:public Decorator{
    shared_ptr<ICharacter> ch;
    public:
    HeightAbility(shared_ptr<ICharacter> ch){
        this->ch=ch;
    }
    void getAbilities(){
        ch->getAbilities();
        cout<<" with height ability";
    }
};
class StarAbility:public Decorator{
    shared_ptr<ICharacter> ch;
    public:
    StarAbility(shared_ptr<ICharacter> ch){
        this->ch=ch;
    }
    void getAbilities(){
        ch->getAbilities();
        cout<<" with star ability";
    }
};
int main(){
    shared_ptr<ICharacter> c1=make_shared<GunAbility>(
        make_shared<HeightAbility>(
            make_shared<Mario>()
        )
    );
    c1->getAbilities();

    cout<<endl;

    shared_ptr<ICharacter> c2=make_shared<StarAbility>(
        make_shared<GunAbility>(
        make_shared<HeightAbility>(
            make_shared<Mario>()
        )
    ));
    c2->getAbilities();
}