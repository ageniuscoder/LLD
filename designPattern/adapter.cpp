#include <bits/stdc++.h>  //i have existing paypalSdk(third party), i have target interface paymentProcessor which can,t talk to third party so i need adapter 
using namespace std;
class PaypalSdk{  //third party
    public:
    void makePayment(double amount){
        cout<<"$ "<<amount<<" is payed using Paypal"<<endl;
    }
};

class paymentProcessor{  //target and is expected by client
    public:
    virtual void pay(double amount)=0;
    virtual ~paymentProcessor()=default;
};

class PaypalAdapter:public paymentProcessor{    //here unique pointer of paypalSdk in not acceptalble bcz it is third party , it my shared and long-lived, so use refrence here
    PaypalSdk& paypal;
    public:
    PaypalAdapter(PaypalSdk& paypal):paypal(paypal){}
    void pay(double amount){
        paypal.makePayment(amount);
    }
};

class Client{
    unique_ptr<paymentProcessor> payment;
    double amount;
    public:
    Client(unique_ptr<paymentProcessor> payment,double amount):payment(move(payment)),amount(amount){}
    void pay(){
        payment->pay(amount);
    }
};

int main(){
    PaypalSdk paypal;
    auto payment=make_unique<PaypalAdapter>(paypal);
    auto client=make_unique<Client>(move(payment),300);
    client->pay();
}