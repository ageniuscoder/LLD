#include<iostream>
#include<atomic>
#include<thread>
#include<vector>

using namespace std;

int main(){
  atomic<int> cnt{10};

  vector<thread> v;

  cnt.store(21);  //overwrites the value in atomic counter

  //cnt.fetch_add(1)  return old value and increment atomically
  for(int i=0;i<10;i++){
    v.emplace_back([&cnt](){
        cnt++;
    });
  }

  for(auto &t:v){
    t.join();
  }

  cout<<"Updated Counter: "<<cnt.load()<<endl;
}