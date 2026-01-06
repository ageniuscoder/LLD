#include <bits/stdc++.h>
using namespace std;
class VendingMachine;
class VendingState{
    public:
    virtual VendingState* insertCoin(VendingMachine* machine,int coins)=0;
    virtual VendingState* returnCoin(VendingMachine* machine )=0;
    virtual VendingState* selectItem(VendingMachine* machine,const string& name)=0;
    virtual VendingState* dispenseItem(VendingMachine* machine)=0;
    virtual VendingState* refill(VendingMachine* machine,const string& name,int quantity)=0;
    virtual string getState()=0;
    virtual ~VendingState()=default;
};
class NoCoin;
class HasCoin;
class Dispense;
class SoldOut;
class Product{
    string name;
    int price;
    int quantity;
    public:
    Product(const string &name,int price,int quantity):name(name),price(price),quantity(quantity){}
    string getProduct(){
        return name;
    }
    int getPrice(){
        return price;
    }
    int getQuantity(){
        return quantity;
    }
    void addQuantity(int quantity){
        this->quantity+=quantity;
    }
    bool checkQuantity(){
        return quantity>0;
    }
};
class VendingMachine{
    VendingState* state;
    unordered_map<string,unique_ptr<Product>> products;
    int coins;
    vector<Product*> selectedItem;

    unique_ptr<NoCoin> nocoin;
    unique_ptr<HasCoin> hascoin;
    unique_ptr<Dispense> dispense;
    unique_ptr<SoldOut> soldout;

    public:
    VendingMachine();
    void insertCoin(int coins);
    void returnCoin();
    void selectItem(const string &name);
    void dispenseItem();
    void refill(const string &name,int quantity);

    int getCoins();
    void addCoins(int coins);
    void setCoins(int coins);
    int getPrice(const string &name);
    void addQuantity(const string& name,int quantity);
    void getStatus();
    void addProduct(unique_ptr<Product> product);
    void addSelected(Product* product);
    void clearSelected();
    bool hasAnyStock();

    Product* getProduct(const string &name);
    vector<Product*>& getSelectedItem();
    VendingState* getNoCoin(); 
    VendingState* getHasCoin(); 
    VendingState* getDispense(); 
    VendingState* getSoldOut(); 
};

class NoCoin:public VendingState{
    public:
    VendingState* insertCoin(VendingMachine* machine,int coins){
        machine->addCoins(coins);
        return machine->getHasCoin();
    }
    VendingState* returnCoin(VendingMachine* machine){
        cout<<"[NOCOIN]Insert Some Coins First"<<endl;
        return machine->getNoCoin();
    }
    VendingState* selectItem(VendingMachine* machine,const string &name){
        cout<<"[NOCOIN]Insert Some Coins First"<<endl;
        return machine->getNoCoin();
    }
    VendingState* dispenseItem(VendingMachine* machine){
        cout<<"[NOCOIN]Insert Some Coins First"<<endl;
        return machine->getNoCoin();
    }

    VendingState* refill(VendingMachine* machine,const string& name,int quantity){
        machine->addQuantity(name,quantity);
        return machine->getNoCoin();
    }

    string getState(){
        return "NO_COIN";
    }

};
class HasCoin:public VendingState{
    public:
    VendingState* insertCoin(VendingMachine* machine,int coins){
        machine->addCoins(coins);
        return machine->getHasCoin();
    }
    VendingState* returnCoin(VendingMachine* machine){
        cout<<"[HASCOIN]Returning all coins"<<machine->getCoins()<<endl;
        machine->setCoins(0);
        return machine->getNoCoin();
    }
    VendingState* selectItem(VendingMachine* machine,const string &name){
        int totalCost=0;
        for(auto &item:machine->getSelectedItem()){
            totalCost+=item->getPrice();
        }
        if(machine->getCoins()<totalCost+ machine->getProduct(name)->getPrice()){
            cout<<"[HASCOIN]Please insert more coins"<<endl;
            return machine->getHasCoin();
        }
        if(!machine->getProduct(name)->checkQuantity()){
            cout<<"Item is out of Stock, It can,t be selected"<<endl;
            return machine->getHasCoin();
        }
        cout<<"[PRODUCT]: "<<machine->getProduct(name)->getProduct()<<" is Selected"<<endl;
        machine->addSelected(machine->getProduct(name));
        return machine->getHasCoin();
    }
    VendingState* dispenseItem(VendingMachine* machine){
        cout<<"[HASCOIN]Selected Items are Ready to Dispense"<<endl;
        return machine->getDispense();
    }
    VendingState* refill(VendingMachine* machine,const string& name,int quantity){
        cout<<"[HASCOIN]First Select an Item"<<endl;
        return machine->getHasCoin();
    }
    string getState(){
        return "HAS_COIN";
    }

};

class Dispense:public VendingState{
    public:
    VendingState* insertCoin(VendingMachine* machine,int coins){
        cout<<"[DISPENSE]Coins can,t be Inserted "<<endl;
        return machine->getDispense();
    }
    VendingState* returnCoin(VendingMachine* machine){
        cout<<"[DISPENSE]Coins can,t be Returend "<<endl;
        return machine->getDispense();
    }
    VendingState* selectItem(VendingMachine* machine,const string &name){
        cout<<"[DISPENSE]Items can,t be selected"<<endl;
        return machine->getDispense();
    }
    VendingState* dispenseItem(VendingMachine* machine){
        for(auto product:machine->getSelectedItem()){
            cout<<"Dispensing Item: "<<product->getProduct()<<endl;
            product->addQuantity(-1);
            machine->addCoins(-product->getPrice());
        }
        machine->clearSelected();
        if(machine->getCoins()>0){
            cout<<"[DISPENSE]Returning remaining Coins: "<<machine->getCoins()<<endl;
        }
        machine->setCoins(0);
        if(machine->hasAnyStock()){
            return machine->getNoCoin();
        }
        return machine->getSoldOut();
    }
    VendingState* refill(VendingMachine* machine,const string& name,int quantity){
        cout<<"[DISPENSE]Items can,t be refilled"<<endl;
        return machine->getDispense();
    }
    string getState(){
        return "DISPENSE";
    }
};

class SoldOut:public VendingState{
    public:
    VendingState* insertCoin(VendingMachine* machine,int coins){
        cout<<"[SOLDOUT]Coins can,t be Inserted "<<endl;
        return machine->getSoldOut();
    }
    VendingState* returnCoin(VendingMachine* machine){
        cout<<"[SOLDOUT]Coins can,t be Returend "<<endl;
        return machine->getSoldOut();
    }
    VendingState* selectItem(VendingMachine* machine,const string& name){
        cout<<"[SOLDOUT]Items are out of stock"<<endl;
        return machine->getSoldOut();
    }
    VendingState* dispenseItem(VendingMachine* machine){
        cout<<"[SOLDOUT]Item is out of stock"<<endl;
        return machine->getSoldOut();
    }

    VendingState* refill(VendingMachine* machine,const string& name,int quantity){
        machine->addQuantity(name,quantity);
        return machine->getNoCoin();
    }
    string getState(){
        return "SOLD_OUT";
    }
};
VendingMachine::VendingMachine():coins(0){
    nocoin=make_unique<NoCoin>();
    hascoin=make_unique<HasCoin>();
    dispense=make_unique<Dispense>();
    soldout=make_unique<SoldOut>();
    products["water bottle"]=make_unique<Product>("water bottle",20,2);

    if(products.size()>0){
        state=nocoin.get();
    }else{
        state=soldout.get();
    }
}
void VendingMachine::insertCoin(int coins){
    state=state->insertCoin(this,coins);
}
void VendingMachine::returnCoin(){
    state=state->returnCoin(this);
}
void VendingMachine::selectItem(const string &name){
    state=state->selectItem(this,name);
}
void VendingMachine::dispenseItem(){
    state=state->dispenseItem(this);
    state=state->dispenseItem(this);
}

void VendingMachine::refill(const string& name,int quantity){
    state=state->refill(this,name,quantity);
}
int VendingMachine::getCoins(){
    return coins;
}
void VendingMachine::addCoins(int coins){
    this->coins+=coins;
}
void VendingMachine::setCoins(int coins){
    this->coins=coins;
}
int VendingMachine::getPrice(const string &name){
    return products[name]->getPrice();
}
void VendingMachine::addQuantity(const string& name,int quantity){
    products[name]->addQuantity(quantity);
}
VendingState* VendingMachine::getNoCoin(){
    return nocoin.get();
}
VendingState* VendingMachine::getHasCoin(){
    return hascoin.get();
}
VendingState* VendingMachine::getDispense(){
    return dispense.get();
}
VendingState* VendingMachine::getSoldOut(){
    return soldout.get();
}
void VendingMachine::getStatus(){
    cout<<"=========  Machine Status: ========="<<endl;
    cout<<"Machine State: "<<state->getState()<<endl;
    cout<<"Total coin Inserted: "<<coins<<endl;
    if(selectedItem.size()>0){
        cout<<"List of selected Items are: "<<endl;
        for(auto item:selectedItem){
            cout<<item->getProduct()<<"  :Rs "<<item->getPrice()<<endl;
        }
    }
}
void VendingMachine::addProduct(unique_ptr<Product> product){
    products[product->getProduct()]=move(product);
}
Product* VendingMachine::getProduct(const string &name){
    auto it = products.find(name);
    if (it == products.end()) return nullptr;
    return it->second.get();
}
void VendingMachine::addSelected(Product* product){
    selectedItem.push_back(product);
}
vector<Product*>& VendingMachine::getSelectedItem(){
    return selectedItem;
}
void VendingMachine::clearSelected() {
    selectedItem.clear();
}
bool VendingMachine::hasAnyStock() {
    for (auto& [_, p] : products) {
        if (p->getQuantity() > 0) return true;
    }
    return false;
}

int main(){

    unique_ptr<VendingMachine> machine=make_unique<VendingMachine>();
    machine->addProduct(make_unique<Product>("chips",10,3));
    machine->addProduct(make_unique<Product>("kurkure",20,3));
    machine->insertCoin(10);
    machine->getStatus();
    machine->selectItem("chips");
    machine->selectItem("water bottle");
    machine->getStatus();
    machine->insertCoin(20);
    machine->getStatus();
    machine->selectItem("kurkure");
    machine->selectItem("kurkure");
    machine->getStatus();
    machine->dispenseItem();
    machine->getStatus();
    machine->insertCoin(20);
    machine->getStatus();
    machine->selectItem("kurkure");
    machine->getStatus();
    machine->dispenseItem();
    machine->getStatus();
    machine->insertCoin(20);
    machine->getStatus();
    machine->selectItem("kurkure");
    machine->getStatus();
    machine->dispenseItem();
    machine->getStatus();
    machine->refill("kurkure",1);
    machine->getStatus();
    machine->selectItem("kurkure");
    machine->getStatus();
    machine->dispenseItem();
    machine->getStatus();
}