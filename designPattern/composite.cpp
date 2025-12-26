#include <bits/stdc++.h>
using namespace std;
class FileSystem{
    public:
    virtual void ls(int indent=0)=0;
    virtual void openAll(int indent=0)=0;
    virtual shared_ptr<FileSystem> cd(string name)=0;
    virtual int getSize()=0;
    virtual string getName()=0;
    virtual bool isFolder()=0;
    virtual ~FileSystem()=default;
};
class File:public FileSystem{
    string name;
    int size;
    public:
    File(string name,int size):name(name),size(size){}
    string getName(){
        return name;
    }
    bool isFolder(){
        return false;
    }
    void ls(int indent=0){
        cout<<string(indent,' ')<<getName()<<endl;
    }
    void openAll(int indent=0){
        cout<<string(indent,' ')<<getName()<<endl;
    }

    shared_ptr<FileSystem> cd(string name){
        return nullptr;
    }

    int getSize(){
        return size;
    }
};
class Folder:public FileSystem{
    string name;
    vector<shared_ptr<FileSystem>> items;
    public:
    Folder(string name):name(name){};
    void addItems(shared_ptr<FileSystem> fs){
        items.push_back(fs);
    }
    string getName(){
        return name;
    }
    bool isFolder(){
        return true;
    }
    int getSize(){
        int total=0;
        for(auto item:items){
            total+=item->getSize();
        }
        return total;
    }

    void ls(int indent=0){
        cout<<string(indent,' ')<<"+ "<<getName()<<endl;
        for(auto item:items){
            if(item->isFolder()){
                cout<<string(indent+4,' ')<<"+ "<<item->getName()<<endl;
            }else{
                cout<<string(indent+4,' ')<<item->getName()<<endl;
            }
        }
    }
    void openAll(int indent=0){
        cout<<string(indent,' ')<<"+"<<getName()<<endl;
        for(auto item:items){
           item->openAll(indent+4);
        }
    }

    virtual shared_ptr<FileSystem> cd(string name){
        for(auto item:items){
            if(item->isFolder() && item->getName()==name){
                return item;
            }
        }
        cout<<"NO such folder exists in "<<getName()<<endl;
        return nullptr;
    }

};
int main(){
    shared_ptr<Folder> root=make_shared<Folder>("Root");
    shared_ptr<File> f1=make_shared<File>("f1.txt",3);
    shared_ptr<File> f2=make_shared<File>("f2.txt",4);
    shared_ptr<Folder> music=make_shared<Folder>("Music");
    root->addItems(f1);
    root->addItems(f2);
    root->addItems(music);

    shared_ptr<File> f3=make_shared<File>("m1.mp3",5);
    shared_ptr<Folder> image=make_shared<Folder>("Image");
    music->addItems(f3);
    music->addItems(image);

    shared_ptr<File> f4=make_shared<File>("i1.jpg",3);
    shared_ptr<File> f5=make_shared<File>("i2.jpg",4);

    image->addItems(f4);
    image->addItems(f5);

    //root->ls();

    root->openAll();

    //cout<<root->getSize();

    cout<<"____________________________________"<<endl;

    auto nd=root->cd("Music");
    nd->openAll();

}