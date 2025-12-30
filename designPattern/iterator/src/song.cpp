#include "../include/playlist/song.h"
#include <sstream>

Song::Song(const string &name,const string &artist):name(name),artist(artist){}

string Song::getSong(){
        ostringstream oss;
        oss<<"[song]: "<<name<<" by "<<"["<<artist<<"]";
        return oss.str();
    }