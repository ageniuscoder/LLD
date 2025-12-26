#include <bits/stdc++.h>     //The facade does not need to be heap-allocated unless required.   //Facade already manages heap objects internally  Stack allocation is simpler and safer
using namespace std;
class Projector{
    public:
    void on(){
        cout<<"projector is on"<<endl;
    }
};
class Amplifier{
    public:
    void on(){
        cout<<"amplifier is on"<<endl;
    }
    void setVolume(int val){
        cout<<"Volume is set to "<<val<<endl;
    }
};
class Dvd{
    public:
    void on(){
        cout<<"Dvd is on"<<endl;
    }
    void play(){
        cout<<"Video is playing"<<endl;
    }
};
class TheaterFacade{     //facade
    unique_ptr<Projector> p;
    unique_ptr<Amplifier> a;
    unique_ptr<Dvd> d;
    public:
    TheaterFacade(){
        p=make_unique<Projector>();
        a=make_unique<Amplifier>();
        d=make_unique<Dvd>();
    }
    void playMovie(int val){
        p->on();
        a->on();
        a->setVolume(val);
        d->on();
        d->play();
    }
};
int main(){
    TheaterFacade th;
    th.playMovie(30);
}