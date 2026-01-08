#include <bits/stdc++.h>
using namespace std;
class Collegues;
class IMediator{
    public:
    virtual void sendAll(const string& from,const string &msg)=0;
    virtual void sendTo(const string &from,const string& to,const string &msg)=0;
    virtual void registerUser(Collegues* c)=0;
    virtual void muteUser(const string &who,const string &whom)=0;
    virtual ~IMediator()=default;
};
class Collegues{
    protected:
    IMediator* mediator;
    public:
    Collegues(IMediator* mediator):mediator(mediator){}
    virtual string getName()=0;
    virtual void sendAll(const string& msg)=0;
    virtual void sendTo(const string& to,const string& msg)=0;
    virtual void recieve(const string &from,const string& msg)=0;
    virtual ~Collegues()=default;
};
class ChatMediator:public IMediator{
    vector<Collegues*> users;
    unordered_map<string,unordered_set<string>> muteList; //who->{whom}
    public:
    void sendAll(const string& from,const string &msg){
        for(auto &user:users){
            string name=user->getName();
            if(name==from) continue;
            if(muteList[name].count(from)){
                cout<<name<<" mutes "<<from<<endl;
                continue;
            }
            user->recieve(from,msg);
        }
    }

    void sendTo(const string &from,const string& to,const string &msg){
        Collegues* user=nullptr;
        for(auto &u:users){
            if(u->getName()==to){
                user=u;
                break;
            }
        }
        if(!user){
            cout<<to<<" is not registered"<<endl;
            return;
        }
        if(muteList[to].count(from)){
            cout<<to<<" mutes "<<from<<endl;
            return;
        }
        user->recieve(from,msg);
    }
    void registerUser(Collegues* c){
        users.push_back(c);
    }
    void muteUser(const string &who,const string &whom){
        muteList[who].insert(whom);
    }
};
class User:public Collegues{
    string name;
    public:
    User(IMediator* mediator,const string &name):Collegues(mediator),name(name){
        mediator->registerUser(this);
    }
    string getName(){
        return name;
    }
    void sendAll(const string& msg){
        cout<<"..................BroadCast message........................"<<endl;
        mediator->sendAll(name,msg);
    }
    void sendTo(const string& to,const string& msg){
        cout<<"-----------private message------------------"<<endl;
        mediator->sendTo(name,to,msg);
    }
    void recieve(const string &from,const string& msg){
        cout<<"["<<from<<" -> "<<name<<"]: "<<msg<<endl;
    }

};
int main(){
    unique_ptr<IMediator> chatRoom=make_unique<ChatMediator>();
    unique_ptr<User> u1=make_unique<User>(chatRoom.get(),"mangal");
    unique_ptr<User> u2=make_unique<User>(chatRoom.get(),"shailu");
    unique_ptr<User> u3=make_unique<User>(chatRoom.get(),"akarsh");
    unique_ptr<User> u4=make_unique<User>(chatRoom.get(),"abhay");

    chatRoom->muteUser("mangal","abhay");
    chatRoom->muteUser("abhay","mangal");
    u1->sendAll("Hello Everyone");
    
    u4->sendAll("Hello Group");

    u1->sendTo("abhay","kaise ho abhay bhai");
    u4->sendTo("mangal","kaise ho mangal bhai");

}