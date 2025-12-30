#include "../include/playlist/playlist.h"
#include "../include/playlist/playlistiterator.h"

void PlayList::addSong(unique_ptr<Song> song){
        songs.push_back(move(song));
    }

unique_ptr<Iterator<string>> PlayList::getIterator(){
    return make_unique<PlaylistIterator>(songs);
}