#include <bits/stdc++.h>
using namespace std; 
//llp
class Engine{
    public:
    virtual string start()=0;
    virtual ~Engine()=default;
};
class Electric:public Engine{
    public:
    string start(){
        return "with Electric Engine";
    }
};
class Desiel:public Engine{
    public:
    string start(){
        return "with Desiel Engine";
    }
};
class Petrol:public Engine{
    public:
    string start(){
        return "with Petrol Engine";
    }
};

//hlp
class Car{
    protected:
    shared_ptr<Engine> engine;
    public:
    Car(shared_ptr<Engine> engine):engine(engine){}
    virtual void drive()=0;
    virtual ~Car()=default;

};

class Suv:public Car{
    public:
    Suv(shared_ptr<Engine> engine):Car(engine){};
    void drive(){
        cout<<"[suv] "<<engine->start()<<endl;
    }
};
class Sedan:public Car{
    public:
    Sedan(shared_ptr<Engine> engine):Car(engine){};
    void drive(){
        cout<<"[sedan] "<<engine->start()<<endl;
    }
};



int main(){
    shared_ptr<Engine> electric=make_shared<Electric>();
    shared_ptr<Engine> desiel=make_shared<Desiel>();
    shared_ptr<Engine> petrol=make_shared<Petrol>();

    unique_ptr<Car> suv=make_unique<Suv>(desiel);
    unique_ptr<Car> suv1=make_unique<Suv>(petrol);
    suv->drive();
    suv1->drive();

    unique_ptr<Car> sedan=make_unique<Sedan>(electric);
    unique_ptr<Car> sedan1=make_unique<Sedan>(petrol);
    sedan->drive();
    sedan1->drive();
}