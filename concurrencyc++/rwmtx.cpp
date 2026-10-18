#include<iostream>
#include<shared_mutex>
#include<thread>
#include<vector>
#include<unordered_map>
#include<mutex>
#include<string>
using namespace std;
class Cache{
    unordered_map<string,string> cache;
    shared_mutex mtx;
    public:
    void put(string key,string value){
        unique_lock<shared_mutex> lock(mtx);
        cache[key]=value;
    }

    string get(string key){
        shared_lock<shared_mutex> lock(mtx);
        return cache[key];
    }
};

int main(){
    Cache c;
    vector<thread> reader;
    vector<thread> writer;

    mutex coutmtx;

    for(int i=0;i<5;i++){
        writer.emplace_back([&c,i](){
            string key="key"+ to_string(i);
            string value= "value"+to_string(i);
            c.put(key,value);
        });
    }

     for(auto &w:writer){
        w.join();
    }

    for(int i=0;i<100;i++){
        reader.emplace_back([&c,i,&coutmtx](){
            string key="key"+to_string(i%5);
            lock_guard<mutex> lock(coutmtx);
            cout<< c.get(key) << endl;
        });
    }

    for(auto &r:reader){
        r.join();
    }

}