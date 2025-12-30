#pragma once
#include <string>
using namespace std;

class Song{
    string name;
    string artist;
    public:
    Song(const string &name,const string &artist);
    string getSong();
};