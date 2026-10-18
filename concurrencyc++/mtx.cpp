#include <mutex>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

class Counter{
    private:
    mutex mu;
    int count;
    public:
    Counter(){
        count=0;
    }
    // Approach 1: lock_guard (RAII, preferred)
    void incrementLockGaurd(){
        lock_guard<mutex> lock(mu);
        count++;
    }
    // Approach 2: unique_lock (more flexible)
    void incrementUniqueLock(){
        unique_lock<mutex> lock(mu);
        count++;
    }

     // Approach 3: Manual (avoid!)
     void increment(){
        mu.lock();
        count++;
        mu.unlock();
     }

     int getCount(){
        lock_guard<mutex> lock(mu);
        return count;
     }
};

int main(){
   
   Counter c;

   vector<thread> v;
   for(int i=0;i<10;i++){
    v.emplace_back([&c](){
        for(int j=0;j<1000;j++){
            c.incrementLockGaurd();
        }
    });
   }

   for(auto &t:v){
    t.join();
   }

   cout<<"Final Count "<<c.getCount()<<endl;

   
}