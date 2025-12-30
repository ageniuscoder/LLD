#pragma once
#include <vector>
#include <memory>
#include "../core/Iterable.h"
#include "song.h" // Required for unique_ptr destructor visibility

using namespace std;

class PlayList : public Iterable<string> {
    vector<unique_ptr<Song>> songs;
public:
    void addSong(unique_ptr<Song> song);
    unique_ptr<Iterator<string>> getIterator() override;
};