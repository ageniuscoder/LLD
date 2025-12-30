#pragma once
#include <vector>
#include <memory>
#include "../core/Iterable.h"
#include "song.h"
using namespace std;


class PlaylistIterator:public Iterator<string>{
    vector<unique_ptr<Song>>& songs;
    int idx;
    public:
    PlaylistIterator(vector<unique_ptr<Song>>& songs);
    bool hasNext();
    string next();
    bool hasPrev();
    string prev();
};