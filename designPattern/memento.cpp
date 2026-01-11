#include <bits/stdc++.h>
using namespace std;
class Memento{
    map<string,string> state;
    public:
    Memento(map<string,string> &data):state(data){}
    map<string,string> getState(){
        return state;
    }
};
class Db{
    map<string,string> data;
    public:
    void insert(const string &key,const string& val){
        data[key]=val;
        cout<<"Inserted: ["<<key<<"] -> "<<val<<endl;
    }
    void update(const string &key,const string& val){
        if(data.find(key)==data.end()){
            cout<<"Key can,t be updated"<<endl;
            return;
        }
        data[key]=val;
        cout<<"Updated: ["<<key<<"] -> "<<val<<endl;
    }

    Memento createMemento(){
        return Memento(data);
    }
    void restore(Memento &m){
        data=m.getState();
    }
    void getData(){
        if(data.size()==0){
            cout<<"No data is available"<<endl;
            return;
        }
        for(auto &[a,b]:data){
            cout<<"Data: "<<a<<"="<<b<<endl;
        }
    }
};
class TxnManager{
    optional<Memento> m;
    Db &db;
    public:
    TxnManager(Db &d):db(d){}
    void beginTxn(){
        cout<<"Txn is started"<<endl;
        m=db.createMemento();
    }
    void commit(){
        if(!m){
            cout<<"Txn is not started"<<endl;
            return;
        }
        cout<<"Commited Sucessfully"<<endl;
        m.reset();
    }
    void rollback(){
        if(!m){
            cout<<"Txn is not started"<<endl;
            return;
        }
        cout<<"database is restored to previous state"<<endl;
        db.restore(m.value());  //*m
        m.reset();
    }
};
int main(){
    Db db;
    TxnManager txn(db);
    txn.beginTxn();
    db.insert("mangal","20");
    db.insert("raj","19");
    txn.commit();
    db.getData();

    txn.beginTxn();
    db.insert("rahul","87");

    db.getData();
    txn.rollback();

    db.getData();
}