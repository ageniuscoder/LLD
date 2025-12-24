#include <bits/stdc++.h>
using namespace std;
class Isubscriber{
    public:
    virtual void update()=0;
    virtual ~Isubscriber()=default;
};
class Ichannel{
    public:
    virtual void subscribe(shared_ptr<Isubscriber> sub)=0;
    virtual void unsubscribe(shared_ptr<Isubscriber> sub)=0;
    virtual void notify()=0;
    virtual ~Ichannel()=default;
};

class Striver:public Ichannel{
    string title;
    vector<weak_ptr<Isubscriber>> subscribers;
    public:
    void subscribe(shared_ptr<Isubscriber> sub){
        subscribers.push_back(sub);
    }
    void unsubscribe(shared_ptr<Isubscriber> sub){
        for(auto it=subscribers.begin(); it!=subscribers.end();++it){
            if(it->lock()==sub){
                subscribers.erase(it);
                break;
            }
        }
    }
    void notify(){
        for(auto it=subscribers.begin(); it!=subscribers.end();){
            if(auto sp=it->lock()){
                sp->update();
                ++it;
            }else{
                it=subscribers.erase(it);
            }
        }
    }
    void addVideo(string title){
        this->title=title;
        notify();
    }

    string getVideo(){
        return title;
    }
    string getName(){
        return "Striver";
    }

};
class Mangal:public Isubscriber{
    shared_ptr<Striver> chan;
    public:
    Mangal(shared_ptr<Striver> chan){
        this->chan=chan;
    }
    void update(){
        cout<<"[Mangal] New video is uplaoded by "<<chan->getName()<<" [Title:] "<<chan->getVideo()<<endl;
    }
};
class Shailu:public Isubscriber{
    shared_ptr<Striver> chan;
    public:
    Shailu(shared_ptr<Striver> chan){
        this->chan=chan;
    }
    void update(){
        cout<<"[Shailu] New video is uplaoded by "<<chan->getName()<<" [Title:] "<<chan->getVideo()<<endl;
    }
};
int main(){
    shared_ptr<Striver> st=make_shared<Striver>();
    shared_ptr<Isubscriber> mangal=make_shared<Mangal>(st);
    shared_ptr<Isubscriber> shailu=make_shared<Shailu>(st);
    st->subscribe(mangal);
    st->subscribe(shailu);
    st->addVideo("Dynamic Programming");
    st->unsubscribe(shailu);
    st->addVideo("Array");
}