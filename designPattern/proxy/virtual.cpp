#include <bits/stdc++.h>
using namespace std;
class Idisplay{
    public:
    virtual void display()=0;
    virtual ~Idisplay()=default;
};
class ImageDisplay:public Idisplay{
    string path;
    public:
    ImageDisplay(string path):path(path){
        //load from disk
        //some expensive opeation
    }
    void display(){
        cout<<"Displaying image from path: "<<path<<endl;
    }
};
class ImageProxy:public Idisplay{
    unique_ptr<ImageDisplay> idis;
    string path;
    public:
    ImageProxy(string path):path(path){
        idis=nullptr;
    }
    void display(){
        if(!idis){
            idis=make_unique<ImageDisplay>(path);
        }
        idis->display();
    }
};
int main(){
    unique_ptr<Idisplay> image=make_unique<ImageProxy>("home/downlaods");
    image->display();

}