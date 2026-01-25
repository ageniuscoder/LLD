#include <bits/stdc++.h>
using namespace std;
class Item{
    static int cnt;
    string id;
    string name;
    double price;
    public:
    Item(const string& name,double price):name(name),price(price){
        id="item"+to_string(cnt++);
    }
    string getId(){
        return id;
    }
    string getName(){
        return name;
    }
    double getPrice(){
        return price;
    }
};
class ItemManager{
    map<string,unqiue_ptr<Item>> items;
    map<string,string> namedItem;
    ItemManager(){}
    public:
    void addItem(unqiue_ptr<Item> item){
        string id=item->getId();
        if(items.count(id)){
            cout<<"This item is already added"<<endl;
            return;
        }
        string name=item->getName();
        items[id]=move(item);
        namedItem[name]=id;
    }
    Item* getItemByName(const string& name){
        if(!namedItem.count(name)){
            cout<<"Item not exists"<<endl;
            return;
        }
        return items[namedItem[name]].get();
    }
    Item* getItemById(const string& id){
        if(!items.count(id)){
            cout<<"Item not exists"<<endl;
            return;
        }
        return items[id].get();
    }

};
class Inventory{
    map<string,Item*> items;
    map<string,int> stock;
    public:
    void addItem(Item* item,int quan){
       string id=item->getId();
       if(items.count(id)){
           stock[id]+=max(0,quan);
       }else{
            items[id]=item;
            stock[id]+=max(0,quan);
       }
    }
    void reduceStock(const string& id,int quan){
        if(!item.count(id)){
            cout<<"Item doesn,t exist in Inventory"<<endl;
            return;
        }
        int newQuan=stock[id]-quan;
        stock[id]=max(0,newQuan);
    }
    int getItemStock(const string& id){
        if(!item.count(id)){
            cout<<"Item doesn,t exist in Inventory"<<endl;
            return;
        }
        return stock[id];
    }
};
int Item::cnt=1;
int main(){}