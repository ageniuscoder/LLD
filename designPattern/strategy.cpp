#include <bits/stdc++.h>         //use unique_ptr in startegy for safety
using namespace std;
class Talkable{
    public:
    virtual void talk()=0;
    virtual ~Talkable()=default;
};

class NormalTalk:public Talkable{
    public:
    void talk(){
        cout<<"Normal Talk"<<endl;
    }
};
class NoTalk:public Talkable{
    public:
    void talk(){
        cout<<"NO Talk"<<endl;
    }
};
class Walkable{
    public:
    virtual void walk()=0;
    virtual ~Walkable()=default;
};
class NormalWalk:public Walkable{
    public:
    void walk(){
         cout<<"Normal Walk"<<endl;
    }
};

class NoWalk:public Walkable{
    public:
    void walk(){
         cout<<"No Walk"<<endl;
    }
};

class Robot{
    unique_ptr<Talkable> t;
    unique_ptr<Walkable> w;
    public:
    Robot(unique_ptr<Talkable> t,unique_ptr<Walkable> w){
        this->t=move(t);
        this->w=move(w);
    }
    void talk(){
        t->talk();
    }
    void walk(){
        w->walk();
    }
    virtual void projection()=0;
    virtual ~Robot()=default;
};

class ComapanionRobot:public Robot{
    public:
    ComapanionRobot(unique_ptr<Talkable> t,unique_ptr<Walkable> w):Robot(move(t),move(w)){}
    void projection(){
        cout<<"I am companion Robot"<<endl;
    }
};

class MurmurRobot:public Robot{
    public:
    MurmurRobot(unique_ptr<Talkable> t,unique_ptr<Walkable> w):Robot(move(t),move(w)){}
    void projection(){
        cout<<"I am murmur Robot"<<endl;
    }
};

int main(){
    unique_ptr<Robot> r1=make_unique<ComapanionRobot>(make_unique<NoTalk>(),make_unique<NormalWalk>());
    r1->talk();
    r1->walk();
    r1->projection();

    unique_ptr<Robot> r2=make_unique<MurmurRobot>(make_unique<NormalTalk>(),make_unique<NormalWalk>());

    r2->talk();
    r2->walk();
    r2->projection();
}