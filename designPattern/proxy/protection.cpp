#include <bits/stdc++.h>
using namespace std;
class IdocUnlocker{
    public:
    virtual void unlockPdf(string file,string pwd)=0;
    virtual ~IdocUnlocker()=default;

};
class PdfUnlocker:public IdocUnlocker{
    public:
    void unlockPdf(string file,string pwd){
        cout<<file<<" is unlock using password: "<<pwd<<endl;
    }
};
class User{
    string name;
    bool p;
    public:
    User(string name,bool p):name(name),p(p){}
    string getName(){
        return name;
    }
    void setPremium(){
        p=true;
    }
    bool isPremium(){
        return p;
    }
};
class PdfUnlockerProxy: public IdocUnlocker{
    shared_ptr<User> user;
    unique_ptr<IdocUnlocker> pdf;
    public:
    PdfUnlockerProxy(shared_ptr<User> user,unique_ptr<IdocUnlocker> pdf):user(user),pdf(move(pdf)){}
    void unlockPdf(string file,string pwd){
        if(user->isPremium()){
            cout<<user->getName()<<" has unlocked the ";
            pdf->unlockPdf(file,pwd);
        }else{
            cout<<"This feature is for premium user only,move to premium"<<endl;
        }
    }

};

int main(){
    shared_ptr<User> mangal=make_shared<User>("mangal",false);
    //mangal->setPremium();
    unique_ptr<PdfUnlocker> pdf=make_unique<PdfUnlocker>();
    unique_ptr<IdocUnlocker> tool=make_unique<PdfUnlockerProxy>(mangal,move(pdf));
    tool->unlockPdf("abc.pdf","mangal");
}