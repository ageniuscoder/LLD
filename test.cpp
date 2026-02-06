#include <thread>
#include <iostream>
#include <mutex>

int counter = 0;
std::mutex m;
void increment() {
    counter++;
    std::lock_guard<std::mutex> lock(m);
    std::cout<<counter<<std::endl;
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout << counter << "\n"; 
}