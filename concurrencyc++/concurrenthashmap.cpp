
#include <iostream>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <optional>
#include <string>
using namespace std;

template<typename K, typename V>
class HashMap {
    int strips;
    vector<unordered_map<K, V>> maps;
    vector<mutex> locks;

    int strip(const K& key) {
        return hash<K>{}(key) % strips;
    }

public:
    HashMap(int n = 16): strips(n), maps(n), locks(n) {}

    void put(const K& key, const V& value) {
        int i = strip(key);
        lock_guard<mutex> lock(locks[i]);
        maps[i][key] = value;
    }

    optional<V> get(const K& key) {
        int i = strip(key);
        lock_guard<mutex> lock(locks[i]);

        auto it = maps[i].find(key);

        if (it == maps[i].end())
            return nullopt;

        return it->second;
    }
};

int main() {
    HashMap<string, string> mp(9);

    vector<thread> threads;

    mutex cmu;

    // 4 threads insert 100 keys each
    for (int t = 0; t < 4; t++) {
        threads.emplace_back([t, &mp]() {
            for (int i = 0; i < 100; i++) {
                string key = "key_" + to_string(t * 100 + i);
                string value = "value_" + to_string(t * 100 + i);

                mp.put(key, value);
            }
        });
    }

    // Wait until all insertions finish
    for (auto& t : threads)
        t.join();

    threads.clear();

    cout << "Insertion completed\n";

    // 4 threads concurrently read values
    for (int t = 0; t < 4; t++) {
        threads.emplace_back([t, &mp,&cmu]() {
            for (int i = 0; i < 100; i++) {
                string key = "key_" + to_string(t * 100 + i);
                auto result = mp.get(key);

                if (result.has_value()) {
                    lock_guard<mutex> lock(cmu);
                    cout << "Thread " << t
                         << " read " << key
                         << " = " << *result << '\n';
                }
            }
        });
    }

    for (auto& t : threads)
        t.join();

    cout << "All threads completed\n";

    return 0;
}
