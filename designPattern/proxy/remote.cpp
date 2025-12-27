#include <bits/stdc++.h>
using namespace std;
class Icalculator{
    public:
    virtual void add(int a,int b)=0;
    virtual ~Icalculator()=default;
};
class CalculatorProxy:public Icalculator{
    public:
    void add(int a,int b){
        // serialize request
        // send over network
        // receive response
        cout<<"Calling remote server"<<endl;
        cout<<(a+b)<<endl; 
    }
};
int main(){
    shared_ptr<Icalculator> calc=make_shared<CalculatorProxy>();
    calc->add(2,3);
}