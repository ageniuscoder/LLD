#include "../include/playlist/playlistiterator.h"

PlaylistIterator::PlaylistIterator(vector<unique_ptr<Song>>& songs):songs(songs),idx(0){};
bool PlaylistIterator::hasNext(){
        return idx<songs.size();
    }

string PlaylistIterator::next(){
        string val=songs[idx]->getSong();
        idx++;
        return val;
    }

bool PlaylistIterator::hasPrev(){
        return idx>0;
    }

string PlaylistIterator::prev(){
        idx--;
        string val=songs[idx]->getSong();
        return val;
    }