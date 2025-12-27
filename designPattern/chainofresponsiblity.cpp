#include <bits/stdc++.h>
using namespace std;
class Dispenser{
    protected:
    shared_ptr<Dispenser> nextHandler;
    int numNotes;
    public:
    Dispenser(int n){
        nextHandler=nullptr;
        numNotes=n;
    }
    virtual int getType()=0;
    virtual void setNext(shared_ptr<Dispenser> next) final{
        nextHandler=next;
    }
    virtual void dispense(int amt) final{
        int denomination=getType();
        int reqNotes=amt/denomination;
        int fullFilled=0;
        if(reqNotes>=numNotes){
            fullFilled=numNotes;
            numNotes=0;
        }else{
            numNotes-=reqNotes;
            fullFilled=reqNotes;
        }
        if(fullFilled>0){
            cout<<fullFilled<<" * "<<denomination<<" Notes dispensed"<<endl;
        }
        int remAmount=amt-fullFilled*denomination;
        if(remAmount>0){
            if(nextHandler){
                 nextHandler->dispense(remAmount);
            }else{
                cout<<remAmount<<" can't be fullfilled"<<endl;
            }
        }
    }
};
class ThousandDispenser:public Dispenser{
    const int type=1000;
    public:
    ThousandDispenser(int n):Dispenser(n){}
    int getType(){
        return type;
    }

};
class FiveHundredDispenser:public Dispenser{
    const int type=500;
    public:
    FiveHundredDispenser(int n):Dispenser(n){};
    int getType(){
        return type;
    }
};
class TwoHundredDispenser:public Dispenser{
    const int type=200;
    public:
    TwoHundredDispenser(int n):Dispenser(n){};
    int getType(){
        return type;
    }
};
class HundredDispenser:public Dispenser{
    const int type=100;
    public:
    HundredDispenser(int n):Dispenser(n){};
    int getType(){
        return type;
    }
};
int main(){
    shared_ptr<Dispenser> thousand=make_shared<ThousandDispenser>(4);
    shared_ptr<Dispenser> fiveHundered=make_shared<FiveHundredDispenser>(4);
    shared_ptr<Dispenser> twoHundered=make_shared<TwoHundredDispenser>(8);
    shared_ptr<Dispenser> hundered=make_shared<HundredDispenser>(8);
    thousand->setNext(fiveHundered);
    fiveHundered->setNext(twoHundered);
    twoHundered->setNext(hundered);

    thousand->dispense(7300);

}