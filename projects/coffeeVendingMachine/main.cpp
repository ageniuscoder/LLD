#include <bits/stdc++.h>
using namespace std;
enum class CoffeeType {
    LATTE,
    ESPRESSO,
    CAPPUCCINO
};

enum class Ingredients {
    MILK,
    COFFEE_BEANS,
    WATER,
    SUGAR,
    CARAMEL_SYRUP
};
enum class Topping{
    NONE,
    CARAMEL_SYRUP,
    EXTRA_SUGAR
};
enum class Money:int{
    TEN=10,
    TWENTY=20,
    FIFTY=50
};
class Utility{
    public:
    static string toStr(Ingredients type){
        switch(type){
            case Ingredients::MILK:
                return "MILK";
            case Ingredients::SUGAR:
                return "SUGAR";
            case Ingredients::WATER:
                return "WATER";
            case Ingredients::COFFEE_BEANS:
                return  "COFFEE_BEANS";
            case Ingredients::CARAMEL_SYRUP:
                return "CARAMEL_SYRUP"; 
        }
        return "NONE";
    }
    static string toStr(CoffeeType type) {
        switch(type){
            case CoffeeType::LATTE: return "LATTE";
            case CoffeeType::ESPRESSO: return "ESPRESSO";
            case CoffeeType::CAPPUCCINO: return "CAPPUCCINO";
        }
        return "NONE";
    }
};
class Observer{
    public:
    virtual void update(string type)=0;
    virtual ~Observer()=default;
};
class InApp:public Observer{
    public:
    void update(string type){
        cout<<"[NOTIFICATION]: ";
        cout<<"["<<type<<"] is out of stock"<<endl;
    }
};
class Inventory{
    unordered_map<Ingredients,int> stock;
    vector<Observer*> observers;
    
    void notify(Ingredients type){
        for(auto &o:observers){
            o->update(Utility::toStr(type));
        }
    }
    Inventory(){
        static InApp obs;
        observers.push_back(&obs);
    }
    public:
    Inventory(const Inventory&)=delete;
    Inventory& operator=(const Inventory&)=delete;
    static Inventory& getInstance(){
        static Inventory instance;
        return instance;
    }
    void addStock(Ingredients type,int quan){
         if(quan>=0){
            stock[type]+=quan;
            cout<<"["<<Utility::toStr(type)<<"]: stock added by: "<<quan<<endl;
         }
    }
    void reduceStock(Ingredients type,int quan){
        if(quan>=0){
            stock[type]=max(0,stock[type]-quan);
            if(stock[type]==0){
                notify(type);
            }
            cout<<"["<<Utility::toStr(type)<<"]: stock reduced by: "<<quan<<endl;
         }
    }
    bool isSufficient(Ingredients type,int quan){
        return stock[type]>=quan;
    }
    void getStock(){
        for(auto &[item,quan]:stock){
            cout<<"["<<Utility::toStr(item)<<"] -> "<<quan<<endl;
        }
    }
};
class Coffee {
protected:
    unordered_map<Ingredients, int> recipe;
    double price;
    CoffeeType type;

public:
    Coffee(CoffeeType type) : type(type), price(0) {}

    virtual double getPrice() const = 0;
    virtual CoffeeType getTypeEnum() const=0;
    virtual string getType() const = 0;
    virtual unordered_map<Ingredients, int> getRecipe() const = 0;

    virtual ~Coffee() = default;
};
class Latte : public Coffee {
public:
    Latte() : Coffee(CoffeeType::LATTE) {
        price = 30;

        recipe[Ingredients::COFFEE_BEANS] = 1;
        recipe[Ingredients::WATER] = 1;
        recipe[Ingredients::MILK] = 3;
        recipe[Ingredients::SUGAR] = 1;
    }

    double getPrice() const override {
        return price;
    }

    string getType() const override {
        return Utility::toStr(type);
    }
    CoffeeType getTypeEnum() const override{
        return type;
    }
    unordered_map<Ingredients, int> getRecipe() const override {
        return recipe;
    }
};
class Espresso : public Coffee {
public:
    Espresso() : Coffee(CoffeeType::ESPRESSO) {
        price = 20;

        recipe[Ingredients::COFFEE_BEANS] = 1;
        recipe[Ingredients::WATER] = 1;
    }

    double getPrice() const override {
        return price;
    }
    CoffeeType getTypeEnum() const override{
        return type;
    }
    string getType() const override {
        return Utility::toStr(type);
    }

    unordered_map<Ingredients, int> getRecipe() const override {
        return recipe;
    }
};
class Cappuccino : public Coffee {
public:
    Cappuccino() : Coffee(CoffeeType::CAPPUCCINO) {
        price = 35;

        recipe[Ingredients::COFFEE_BEANS] = 1;
        recipe[Ingredients::WATER] = 1;
        recipe[Ingredients::MILK] = 2;
        recipe[Ingredients::SUGAR] = 1;
    }

    double getPrice() const override {
        return price;
    }

    string getType() const override {
        return Utility::toStr(type);
    }
    CoffeeType getTypeEnum() const override{
        return type;
    }
    unordered_map<Ingredients, int> getRecipe() const override {
        return recipe;
    }
};

class Decorator:public Coffee{
    protected:
    unique_ptr<Coffee> coffee;
    public:
    Decorator(unique_ptr<Coffee> c):Coffee(c->getTypeEnum()),coffee(move(c)){}
    virtual double getPrice() const = 0;
    virtual string getType() const = 0;
    CoffeeType getTypeEnum() const override {
        return coffee->getTypeEnum();
    }
    virtual unordered_map<Ingredients, int> getRecipe() const = 0;
};
class CaremelSyrup:public Decorator{
    public:
    CaremelSyrup(unique_ptr<Coffee> c):Decorator(move(c)){}
    double getPrice() const override {
        return coffee->getPrice()+5;
    }

    string getType() const override {
        return Utility::toStr(type)+"+"+"Caremel Syrup";
    }

    unordered_map<Ingredients, int> getRecipe() const override {
        auto old=coffee->getRecipe();
        old[Ingredients::CARAMEL_SYRUP]+=1;
        return old;
    }
};
class Extra_Sugar:public Decorator{
    public:
    Extra_Sugar(unique_ptr<Coffee> c):Decorator(move(c)){}
    double getPrice() const override {
        return coffee->getPrice()+2;
    }

    string getType() const override {
        return Utility::toStr(type)+"+"+"Extra Sugar";
    }

    unordered_map<Ingredients, int> getRecipe() const override {
        auto old=coffee->getRecipe();
        old[Ingredients::SUGAR]+=1;
        return old;
    }
};
class CoffeeFactory{
    protected:
    static vector<unique_ptr<Coffee>> factory;
    public:
    virtual ~CoffeeFactory()=default;
};
vector<unique_ptr<Coffee>> CoffeeFactory::factory;
class SimpleCoffee:public CoffeeFactory{
    public:
    static Coffee* create(CoffeeType ctype){
        switch(ctype){
            case CoffeeType::CAPPUCCINO:
                factory.push_back(make_unique<Cappuccino>());
                return factory.back().get();
            case CoffeeType::ESPRESSO:
                factory.push_back(make_unique<Espresso>());
                return factory.back().get();
            case CoffeeType::LATTE:
                factory.push_back(make_unique<Latte>());
                return factory.back().get();
        }
        return nullptr;
    }
};
class ExtraSugarCoffee:public CoffeeFactory{
    public:
    static Coffee* create(CoffeeType ctype){
        switch(ctype){
            case CoffeeType::CAPPUCCINO:{
                unique_ptr<Extra_Sugar> es=make_unique<Extra_Sugar>(make_unique<Cappuccino>());
                factory.push_back(move(es));
                return factory.back().get();
            }
            case CoffeeType::ESPRESSO:{
                unique_ptr<Extra_Sugar> es=make_unique<Extra_Sugar>(make_unique<Espresso>());
                factory.push_back(move(es));
                return factory.back().get();
            }
            case CoffeeType::LATTE:{
                unique_ptr<Extra_Sugar> es=make_unique<Extra_Sugar>(make_unique<Latte>());
                factory.push_back(move(es));
                return factory.back().get();
            }
        }
        return nullptr;
    }
};
class CaremelSyrupCoffee:public CoffeeFactory{
    public:
    static Coffee* create(CoffeeType ctype){
        switch(ctype){
            case CoffeeType::CAPPUCCINO:{
                unique_ptr<CaremelSyrup> es=make_unique<CaremelSyrup>(make_unique<Cappuccino>());
                factory.push_back(move(es));
                return factory.back().get();
            }
            case CoffeeType::ESPRESSO:{
                unique_ptr<CaremelSyrup> es=make_unique<CaremelSyrup>(make_unique<Espresso>());
                factory.push_back(move(es));
                return factory.back().get();
            }
            case CoffeeType::LATTE:{
                unique_ptr<CaremelSyrup> es=make_unique<CaremelSyrup>(make_unique<Latte>());
                factory.push_back(move(es));
                return factory.back().get();
            }
        }
        return nullptr;
    }
};
class MachineState;
class Idle;
class HasCoin;
class Dispense;
class CoffeeMachine{
    MachineState* state;
    Coffee* selected;
    int coins;
    unique_ptr<Idle> idle;
    unique_ptr<HasCoin> hasCoin;
    unique_ptr<Dispense> dispense;
    Inventory& inventory;
    CoffeeMachine();
    public:
    CoffeeMachine(const CoffeeMachine&)=delete;
    CoffeeMachine& operator=(const CoffeeMachine&)=delete;
    static CoffeeMachine& getInstance();
    MachineState* getIdleState();
    MachineState* getHasCoinState();
    MachineState* getDispenseState();
    void getMenu();
    void setStock(Ingredients type,int quan);
    void addCoin(int c);
    int getCoin();
    bool ckStock(Ingredients type,int quan);
    void setSelected(Coffee* c);
    Coffee* getSelected();
    void reduceStock(Ingredients type,int quan);
    void reduceCoin(int c);
    void displayMenu();
    void insertCoin(Money type);
    void addStock(Ingredients type,int quan);
    void selectItem(CoffeeType ctype,Topping ttype);
    void dispenseItem();
    void showMachineStatus();
};
class MachineState{
    protected:
    CoffeeMachine* machine;
    public:
    MachineState(CoffeeMachine* m):machine(m){}
    virtual MachineState* displayMenu()=0;
    virtual MachineState* insertCoin(Money type)=0;
    virtual MachineState* addStock(Ingredients type,int quan)=0;
    virtual MachineState* selectItem(CoffeeType ctype,Topping ttype)=0;
    virtual MachineState* dispenseItem()=0;
};
class Idle:public MachineState{
    public:
    Idle(CoffeeMachine* m):MachineState(m){}
    MachineState* displayMenu(){
        machine->getMenu();
        return machine->getIdleState();
    }
    MachineState* addStock(Ingredients type,int quan){
        machine->setStock(type,quan);
        return machine->getIdleState(); 
    }
    MachineState* insertCoin(Money type){
        machine->addCoin(static_cast<int>(type));
        return machine->getHasCoinState();
    }
    MachineState* selectItem(CoffeeType ctype,Topping ttype){
        cout<<"[Idle]: Insert Coin first"<<endl;
        return machine->getIdleState(); 
    }
    MachineState* dispenseItem(){
        cout<<"[Idle]: Insert Coin first"<<endl;
        return machine->getIdleState();
    }
};
class HasCoin:public MachineState{
    void create(CoffeeType ctype,Topping ttype, Coffee* & c){
        switch(ttype){
            case Topping::NONE:
                c=SimpleCoffee::create(ctype);
                break;
            case Topping::EXTRA_SUGAR:
                c=ExtraSugarCoffee::create(ctype);
                break;
            case Topping::CARAMEL_SYRUP:
                c=CaremelSyrupCoffee::create(ctype);
                break;
        }
    }
    bool ck(unordered_map<Ingredients, int> r){
        bool flag=true;
        for(auto &[a,b]:r){
            if(!machine->ckStock(a,b)){
                flag=false;
                break;
            }
        }
        return flag;
    }
    public:
    HasCoin(CoffeeMachine* m):MachineState(m){}
    MachineState* displayMenu(){
        cout<<"Select an Item"<<endl;
        return machine->getHasCoinState();
    }
    MachineState* addStock(Ingredients type,int quan){
        cout<<"Select an Item"<<endl;
        return machine->getHasCoinState();
    }
    MachineState* insertCoin(Money type){
        machine->addCoin(static_cast<int>(type));
        return machine->getHasCoinState();
    }
    MachineState* selectItem(CoffeeType ctype,Topping ttype){
        Coffee* c;
        create(ctype,ttype,c);
        auto reciep=c->getRecipe();
        if(!ck(reciep) || machine->getCoin()<c->getPrice()){
            cout<<"Insufficent Fund or Item not Available"<<endl;
            return machine->getHasCoinState();
        }
        machine->setSelected(c);
        return machine->getDispenseState();
    }
    MachineState* dispenseItem(){
        cout<<"Select Item first"<<endl;
        return machine->getHasCoinState();
    }
};
class Dispense:public MachineState{
    void reduceStock(unordered_map<Ingredients, int> r){
        for(auto &[a,b]:r){
            machine->reduceStock(a,b);
        }
    }
    public:
    Dispense(CoffeeMachine* m):MachineState(m){}
    MachineState* displayMenu(){
        cout<<"Dispense an Item"<<endl;
        return machine->getDispenseState();
    }
    MachineState* addStock(Ingredients type,int quan){
        cout<<"Dispense an Item"<<endl;
        return machine->getDispenseState();
    }
    MachineState* insertCoin(Money type){
        cout<<"Dispense an Item"<<endl;
        return machine->getDispenseState();
    }
    MachineState* selectItem(CoffeeType ctype,Topping ttype){
        cout<<"Dispense an Item"<<endl;
        return machine->getDispenseState();
    }
    MachineState* dispenseItem(){
        Coffee* c=machine->getSelected();
        int price=c->getPrice();
        auto r=c->getRecipe();
        reduceStock(r);
        int remain=machine->getCoin()-price;
        machine->reduceCoin(machine->getCoin());
        if(remain>0){
            cout<<"[Amount] refunded: "<<remain<<endl;
        }
        machine->setSelected(nullptr);
        cout<<"[Coffee] Dispensed: "<<c->getType()<<endl;
        return machine->getIdleState();
    }
};
CoffeeMachine::CoffeeMachine():inventory(Inventory::getInstance()){
    idle=make_unique<Idle>(this);
    hasCoin=make_unique<HasCoin>(this);
    dispense=make_unique<Dispense>(this);
    state=idle.get();
    selected=nullptr;
    coins=0;
}
CoffeeMachine& CoffeeMachine::getInstance(){
    static CoffeeMachine instance;
    return instance;
}
MachineState* CoffeeMachine::getIdleState(){
    return idle.get();
}
MachineState* CoffeeMachine::getHasCoinState(){
    return hasCoin.get();
}
MachineState* CoffeeMachine::getDispenseState(){
    return dispense.get();
}
void CoffeeMachine::getMenu() {
    cout << "=========== COFFEE MENU ===========" << endl;
    cout << "Coffee Types:" << endl;
    cout << "1. LATTE        - 30" << endl;
    cout << "2. ESPRESSO     - 20" << endl;
    cout << "3. CAPPUCCINO   - 35" << endl;
    cout << endl;

    cout << "Toppings:" << endl;
    cout << "1. CARAMEL_SYRUP  (+5)" << endl;
    cout << "2. EXTRA_SUGAR    (+2)" << endl;
    cout << "===================================" << endl;
}
void CoffeeMachine::setStock(Ingredients type,int quan){
    inventory.addStock(type,quan);
}
void CoffeeMachine::addCoin(int c){
    coins+=c;
}
int CoffeeMachine::getCoin(){
    return coins;
}
bool CoffeeMachine::ckStock(Ingredients type,int quan){
    return inventory.isSufficient(type,quan);
}
void CoffeeMachine::setSelected(Coffee* c){
    selected=c;
}
Coffee* CoffeeMachine::getSelected(){
    return selected;
}
void CoffeeMachine::reduceStock(Ingredients type,int quan){
    inventory.reduceStock(type,quan);
}
void CoffeeMachine::reduceCoin(int c){
    coins=max(0,coins-c);
}
void CoffeeMachine::displayMenu(){
    state=state->displayMenu();
}
void CoffeeMachine::insertCoin(Money type){
    state=state->insertCoin(type);
}
void CoffeeMachine::addStock(Ingredients type,int quan){
    state=state->addStock(type,quan);
}
void CoffeeMachine::selectItem(CoffeeType ctype,Topping ttype){
    state=state->selectItem(ctype,ttype);
}
void CoffeeMachine::dispenseItem(){
    state=state->dispenseItem();
}
void CoffeeMachine::showMachineStatus() {
    cout << "\n========= MACHINE STATUS =========\n";

    // State
    if (state == idle.get())
        cout << "Current State : IDLE\n";
    else if (state == hasCoin.get())
        cout << "Current State : HAS_COIN\n";
    else if (state == dispense.get())
        cout << "Current State : DISPENSE\n";

    // Coins
    cout << "Inserted Coins : " << coins << "\n";

    // Selected Coffee
    if (selected)
        cout << "Selected Coffee : " << selected->getType() << "\n";
    else
        cout << "Selected Coffee : NONE\n";

    cout << "\n--- Inventory ---\n";
    inventory.getStock();

    cout << "==================================\n";
}
int main() {

    CoffeeMachine& machine = CoffeeMachine::getInstance();

    cout << "\n===== ADMIN: Loading Initial Stock =====\n";
    machine.addStock(Ingredients::MILK, 5);
    machine.addStock(Ingredients::COFFEE_BEANS, 5);
    machine.addStock(Ingredients::WATER, 5);
    machine.addStock(Ingredients::SUGAR, 5);
    machine.addStock(Ingredients::CARAMEL_SYRUP, 2);

    machine.showMachineStatus();


    cout << "\n===== USER 1: Exhausting CARAMEL SYRUP =====\n";

    // First caramel
    machine.insertCoin(Money::FIFTY);
    machine.selectItem(CoffeeType::ESPRESSO, Topping::CARAMEL_SYRUP);
    machine.dispenseItem();

    // Second caramel -> should trigger notification
    machine.insertCoin(Money::FIFTY);
    machine.selectItem(CoffeeType::ESPRESSO, Topping::CARAMEL_SYRUP);
    machine.dispenseItem();

    machine.showMachineStatus();


    cout << "\n===== ADMIN: Refilling CARAMEL SYRUP =====\n";
    machine.addStock(Ingredients::CARAMEL_SYRUP, 3);
    machine.showMachineStatus();


    cout << "\n===== USER 2: Exhausting CARAMEL Again =====\n";

    for(int i = 0; i < 3; i++) {
        machine.insertCoin(Money::FIFTY);
        machine.selectItem(CoffeeType::ESPRESSO, Topping::CARAMEL_SYRUP);
        machine.dispenseItem();
    }

    machine.showMachineStatus();


    cout << "\n===== USER 3: Exhausting MILK =====\n";

    // Latte uses 3 milk
    machine.insertCoin(Money::FIFTY);
    machine.selectItem(CoffeeType::LATTE, Topping::NONE);
    machine.dispenseItem();

    // This should push milk to 0
    machine.insertCoin(Money::FIFTY);
    machine.selectItem(CoffeeType::LATTE, Topping::NONE);
    machine.dispenseItem();

    machine.showMachineStatus();


    cout << "\n===== ADMIN: Refilling MILK =====\n";
    machine.addStock(Ingredients::MILK, 6);
    machine.showMachineStatus();


    cout << "\n===== USER 4: Exhausting MILK Again =====\n";

    for(int i = 0; i < 2; i++) {
        machine.insertCoin(Money::FIFTY);
        machine.selectItem(CoffeeType::LATTE, Topping::NONE);
        machine.dispenseItem();
    }

    machine.showMachineStatus();

    return 0;
}