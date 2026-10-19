#include<condition_variable>
#include<mutex>
#include<thread>
#include<vector>
#include<iostream>
using namespace std;
//counting semaphore
class Sema{   
    mutex mu;
    condition_variable cv;
    int count;
    public:
    explicit Sema(int count):count(count){}

    void accquire(){
        unique_lock<mutex> lock(mu);
        cv.wait(lock,[this](){
            return count>0;
        });
        count--;
    }

    void release(){
        {
            lock_guard<mutex> lock(mu);
            count++;
        }
        cv.notify_one();
    }

    bool tryAccquire(){
        lock_guard<mutex> lock(mu);
        if(count>0){
            count--;
            return true;
        }
        return false;
    }
};

int main(){
    Sema s(5);

    vector<thread> threads;
    mutex cmu;
    for(int i=0;i<7;i++){
        threads.emplace_back([&s,i,&cmu](){
            s.accquire();
            {
                lock_guard<mutex> lock(cmu);
                cout<<"work done: "<<i<<endl;
            }
            s.release();
        });
    }
    for(int i=0;i<7;i++){
        threads[i].join();
    }
    
}