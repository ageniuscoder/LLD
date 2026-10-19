#include<thread>
#include<semaphore>
#include<vector>
#include<string>
#include<iostream>
#include<queue>
#include<mutex>
#include<memory>
#include<chrono>
using namespace std;
class Conn{
    int id;
    inline static mutex mu;
    public:
    Conn(int id):id(id){};
    void execute(string query){
        lock_guard<mutex> lock(mu);
        cout<<"Connection id: "<<id<<" -> "<<query<<endl;
    }
};

class ConnPool{
   counting_semaphore<> sem;
   queue<unique_ptr<Conn>> pool;
   mutex connmu;

   public:
   ConnPool(int limit):sem(limit){
     for(int i=0;i<limit;i++){
        pool.push(make_unique<Conn>(i));
     }
   }

   Conn* accquire(){
     sem.acquire();
     lock_guard<mutex> lock(connmu);
     auto p=pool.front().release();
     pool.pop();
     return p;
   }

   void release(Conn* conn){
    {
        lock_guard<mutex> lock(connmu);
        pool.push(unique_ptr<Conn>(conn));
    }
    sem.release();
   }
   
};
int main(){
    ConnPool pool(5);
    vector<thread> v;
    for (int i = 0; i < 10; ++i) {
        v.emplace_back([&pool, i]() {
            Conn* conn = pool.accquire();
            conn->execute("SELECT * FROM users WHERE id = " + to_string(i));
            this_thread::sleep_for(chrono::milliseconds(100));
            pool.release(conn);
        });
    }

    for (auto& t : v) {
        t.join();
    }

}