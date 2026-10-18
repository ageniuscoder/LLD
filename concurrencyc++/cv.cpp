#include<iostream>
#include<mutex>
#include<thread>
#include<condition_variable>
using namespace std;
class DataProcessor{
    mutex mu;
    condition_variable cv;
    bool dataready;
    string data;
    public:
    DataProcessor(){
        dataready=false;
        data="";
    }

    string waitdata(){
        unique_lock<mutex> lock(mu);
        cv.wait(lock,[this](){
            return this->dataready;
        });
        dataready=false;
        return data;
    }

    void provideData(string d){
        {
            lock_guard<mutex> lock(mu);
            dataready=true;
            data=d;
        }
        cv.notify_one();
    }
};
int main(){
    DataProcessor d;
    thread consumer([&d](){
        auto data=d.waitdata();
        cout<<"Data: "<<data<<endl;
    });
    thread producer([&d](){
        d.provideData("hello world");
    });
    consumer.join();
    producer.join();
}