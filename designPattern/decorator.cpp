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
    protected:
    shared_ptr<ICharacter> ch;
    public:
    Decorator(shared_ptr<ICharacter> ch):ch(ch){}
    virtual ~Decorator()=default;
};
class GunAbility:public Decorator{
    
    public:
    GunAbility(shared_ptr<ICharacter> ch):Decorator(ch){
    }
    void getAbilities(){
        ch->getAbilities();
        cout<<" with gun ability";
    }
};
class HeightAbility:public Decorator{
    public:
    HeightAbility(shared_ptr<ICharacter> ch):Decorator(ch){
    }
    void getAbilities(){
        ch->getAbilities();
        cout<<" with height ability";
    }
};
class StarAbility:public Decorator{
    public:
    StarAbility(shared_ptr<ICharacter> ch):Decorator(ch){
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