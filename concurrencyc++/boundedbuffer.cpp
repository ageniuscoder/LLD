#include<mutex>
#include<condition_variable>
#include<thread>
#include<queue>
#include<iostream>
using namespace std;
template <typename T>
class BoundedBuffer{
    queue<T> buffer;
    size_t cap;
    mutex mu;
    condition_variable notEmpty;
    condition_variable notFull;
    public:
    explicit BoundedBuffer(size_t cap):cap(cap){}

    void put(T &item){
        {
            unique_lock<mutex> lock(mu);
            notFull.wait(lock,[this](){
                return cap>buffer.size();
            });
            buffer.push(item);
       }
       notEmpty.notify_one();
    }

    T get(){
        T item;
        {
            unique_lock<mutex> lock(mu);
            notEmpty.wait(lock,[this](){
                return buffer.size()!=0;
            });
            item=buffer.front();
            buffer.pop();
        }
        notFull.notify_one();
        return item;
    }

};

int main(){

    mutex cmu;

    BoundedBuffer<int> buffer(20);
    thread producer([&buffer,&cmu](){
        for(int i=0;i<100;i++){
            buffer.put(i);
            lock_guard<mutex> lock(cmu);
            cout<<"Produced Data: "<<i<<endl;
        }
    });

    thread consumer1([&buffer,&cmu](){
        for(int i=0;i<50;i++){
            auto data=buffer.get();
            lock_guard<mutex> lock(cmu);
            cout<<"Consumed Data: "<<data<<endl;
        }
    });

    thread consumer2([&buffer,&cmu](){
        for(int i=0;i<35;i++){
            auto data=buffer.get();
            lock_guard<mutex> lock(cmu);
            cout<<"Consumed Data: "<<data<<endl;
        }
    });

    producer.join();
    consumer1.join();
    consumer2.join();

}