#include <bits/stdc++.h>
using namespace std;
class Visitor;
class DocumentItem{
    protected:
    string name;
    public:
    DocumentItem(const string &name):name(name){}
    virtual void accept(Visitor* v)=0;
    virtual void getName()=0;
    virtual void getContent()=0;
    virtual ~DocumentItem()=default;
};
class Text:public DocumentItem{
    string content;
    public:
    Text(const string& name,const string& content):DocumentItem(name),content(content){}
    void getContent(){
        cout<<"["<<name<<"]: "<<content<<endl;
    }
    void accept(Visitor* v);
    void getName(){
        cout<<"["<<name<<"]"<<endl;
    }
};
class Image:public DocumentItem{
    string content;
    public:
    Image(const string& name,const string& content):DocumentItem(name),content(content){}
    void getContent(){
        cout<<"["<<name<<"]: "<<content<<endl;
    }
    void accept(Visitor* v);
    void getName(){
        cout<<"["<<name<<"]"<<endl;
    }
};
class Video:public DocumentItem{
    string content;
    public:
    Video(const string& name,const string& content):DocumentItem(name),content(content){}
    void getContent(){
        cout<<"["<<name<<"]: "<<content<<endl;
    }
    void accept(Visitor* v);
    void getName(){
        cout<<"["<<name<<"]"<<endl;
    }
};
class Visitor{
    public:
    virtual void visit(Text* txt)=0;
    virtual void visit(Image* img)=0;
    virtual void visit(Video* vdo)=0;
    virtual ~Visitor()=default;
};
class CalculateSize:public Visitor{
    public:
    void visit(Text* txt){
        cout<<"calculating size of txt file :";
        txt->getName();
    }
    void visit(Image* img){
        cout<<"calculating size of image file :";
        img->getName();
    }
    void visit(Video* vdo){
        cout<<"calculating size of video file :";
        vdo->getName();
    }
};
class Compress:public Visitor{
    public:
    void visit(Text* txt){
        cout<<"compressing text file: ";
        txt->getName();
    }
    void visit(Image* img){
        cout<<"compressing image file: ";
        img->getName();
    }
    void visit(Video* vdo){
        cout<<"compressing video file: ";
        vdo->getName();
    }
};
class Scan:public Visitor{
    public:
    void visit(Text* txt){
        cout<<"Scanning text file: ";
        txt->getName();
    }
    void visit(Image* img){
        cout<<"Scanning image file: ";
        img->getName();
    }
    void visit(Video* vdo){
        cout<<"Scanning video file: ";
        vdo->getName();
    }
};
void Text::accept(Visitor* v){
        v->visit(this);
    }
void Image::accept(Visitor* v){
        v->visit(this);
    }
void Video::accept(Visitor* v){
        v->visit(this);
    }
int main(){
    unique_ptr<DocumentItem> f1=make_unique<Text>("file.txt","hey here is mangal");
    unique_ptr<DocumentItem> f2=make_unique<Image>("sail.png","light image");
    unique_ptr<DocumentItem> f3=make_unique<Video>("st.mp4","comedy");

    unique_ptr<CalculateSize> v1=make_unique<CalculateSize>();
    unique_ptr<Compress> v2=make_unique<Compress>();
    unique_ptr<Scan> v3=make_unique<Scan>();

    f1->accept(v1.get());
    f1->getContent();

    f2->accept(v2.get());
    f2->getContent();

    f3->accept(v3.get());
    f3->getContent();
}