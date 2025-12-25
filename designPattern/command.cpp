#include <bits/stdc++.h>    //reciever(fan,light) is own by client only  //command is own by invoker(remot) //command has reference of reciever(fan,ligj)
using namespace std;
class Icommand{
    public:
    virtual void execute()=0;
    virtual void undo()=0;
    virtual ~Icommand()=default;
};
class Light{
    public:
    void on(){
        cout<<"Light is On"<<endl;
    }
    void off(){
        cout<<"Light is off"<<endl;
    }
};
class Fan{
    public:
     void on(){
        cout<<"Fan is On"<<endl;
    }
    void off(){
        cout<<"Fan is off"<<endl;
    }
};
class LightCommand:public Icommand{
    Light &light;
    public:
    LightCommand(Light& light):light(light){} //refernce must be passed in intializer list only
    
    void execute(){
        light.on();
    }
    void undo(){
        light.off();
    }
};
class FanCommand:public Icommand{
    Fan &fan;
    public:
    FanCommand(Fan& fan):fan(fan){}  //refernce must be passed in intializer list only
    void execute(){
        fan.on();
    }
    void undo(){
        fan.off();
    }
};
class Remote{
    const int size=2;
    vector<unique_ptr<Icommand>> buttons;
    vector<bool> isPressed;
    public:
    Remote(){
        //size=2;
        buttons.resize(size);
        isPressed.resize(size,false);
    }

    void setButton(int idx,unique_ptr<Icommand> cmd){
        if(idx>=0 && idx<size){
            buttons[idx]=move(cmd);
        }else{
            cout<<"Invalid button "<<endl;
        }
    }

    void pressButton(int idx){
        if(idx>=0 && idx<size){
            if(!buttons[idx]) {
                cout<<"button is not set yet"<<endl;
                return;
            }
            if(!isPressed[idx]){
                buttons[idx]->execute();
            }else{
                buttons[idx]->undo();
            }
            isPressed[idx]=!isPressed[idx];
        }else{
            cout<<"Invalid button is pressed"<<endl;
        }
    }



};
int main(){
    Light light;
    Fan fan;
    unique_ptr<Icommand> lc=make_unique<LightCommand>(light);
    unique_ptr<Icommand> fc=make_unique<FanCommand>(fan);

    unique_ptr<Remote> remote=make_unique<Remote>();

    remote->setButton(0,move(lc));
    remote->setButton(1,move(fc));

    remote->pressButton(0);
    remote->pressButton(0);
    remote->pressButton(0);


    remote->pressButton(1);
    remote->pressButton(1);

    remote->pressButton(2);
}

/*
vector<bool> is not a normal vector.

It is a bit-packed specialization

operator[] does not return bool&

You can’t take its address

It can behave strangely with references, lambdas, templates

Example surprise:

bool* p = &isPressed[0];  // ❌ does NOT compile for vector<bool>


That’s why many C++ devs say:

“Avoid vector<bool> unless you really need bit packing.”

vector<bool> is a specialized bitset that doesn’t behave like a normal container.
I prefer vector<char> or vector<uint8_t> for predictable behavior.
*/