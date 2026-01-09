#include <bits/stdc++.h>
using namespace std;
class Cloneable{
    public:
    virtual unique_ptr<Cloneable> clone(const string& name)=0;
    virtual ~Cloneable()=default;
};
class Npc:public Cloneable{
    string name;
    int health;
    int attack;
    int defence;
    public:
    Npc(const string& name,int health,int attack,int defence):name(name),health(health),attack(attack),defence(defence){
        //expensive operations
    }
    Npc(const string&name,const Npc& other){
        this->name=name;
        this->health=other.health;
        this->attack=other.attack;
        this->defence=other.defence;
    }
    unique_ptr<Cloneable> clone(const string& name){
        return make_unique<Npc>(name,*this);
    }

    void setAttack(int val){
        this->attack=val;
    }
    void setDefence(int val){
        this->defence=val;
    }

    void describe(){
        cout<<"NPC: name:"<<name<<" | health:"<<health<<" | attack:"<<attack<<" | defence:"<<defence<<endl;
    }

};
int main(){
    unique_ptr<Npc> n1=make_unique<Npc>("alien",30,5,2);
    n1->describe();
    unique_ptr<Cloneable> baseClone=n1->clone("Powerful Alien");
    Npc* rawNpc=dynamic_cast<Npc*>(baseClone.get());
    if (!rawNpc) {
        throw runtime_error("Clone is not an Npc");
    }   
    unique_ptr<Npc> n2(rawNpc);
    baseClone.release();
    n2->setDefence(5);
    n2->describe();
}