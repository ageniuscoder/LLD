#include <bits/stdc++.h>
using namespace std;

class Item {
    static int cnt;
    string id;
    string name;
    double price;

public:
    Item(const string& name, double price) : name(name), price(price) {
        id = "item" + to_string(cnt++);
    }

    string getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
};

int Item::cnt = 1;

// ================= ItemManager (Singleton) =================
class ItemManager {
    map<string, unique_ptr<Item>> items;
    map<string, string> nameToId;

    ItemManager() {}

public:
    ItemManager(const ItemManager&) = delete;
    ItemManager& operator=(const ItemManager&) = delete;

    static ItemManager& getInstance() {
        static ItemManager instance;
        return instance;
    }

    void addItem(unique_ptr<Item> item) {
        if (!item) return;

        string name = item->getName();

        if (nameToId.count(name)) {
            return; // item already exists
        }

        string id = item->getId();
        nameToId[item->getName()] = id;
        items[id] = move(item);
    }

    Item* getItemByName(const string& name) {
        if (!nameToId.count(name)) {
            cout << "Item does not exist\n";
            return nullptr;
        }
        return items[nameToId[name]].get();
    }

    Item* getItemById(const string& id) {
        if (!items.count(id)) {
            cout << "Item does not exist\n";
            return nullptr;
        }
        return items[id].get();
    }
};

class InventoryObserver{
    public:
    virtual void onOutOfStock(const string& name)=0;
    virtual ~InventoryObserver()=default;
};
class AdminDashBoard:public InventoryObserver{
    public:
    void onOutOfStock(const string& name){
        cout << "\n[ADMIN NOTIFICATION] "
             << name << " is OUT OF STOCK\n";
    }
};

// ================= Inventory =================
class Inventory {
    map<string, Item*> items;   // non-owning
    map<string, int> stock;
    vector<InventoryObserver*> observers;
    void notify(const string& name){
        for(auto &obs:observers){
            obs->onOutOfStock(name);
        }
    }

public:
    void addItem(Item* item, int quan) {
        if (!item) return;
        string id = item->getId();
        items[id] = item;
        stock[id] += max(0, quan);
    }
    void addObservers(InventoryObserver* observer){
        observers.push_back(observer);
    }

    void reduceStock(const string& id, int quan) {
        if (!items.count(id)) {
            cout << "Item not in inventory\n";
            return;
        }
        stock[id] = max(0, stock[id] - quan);
        if(stock[id]==0){
            notify(items[id]->getName());
        }
        
    }

    int getItemStock(const string& id) {
        if (!items.count(id)) {
            cout << "Item not in inventory\n";
            return 0;
        }
        return stock[id];
    }
    void showStock() {
        if (stock.empty()) {
            cout << "Inventory is empty\n";
            return;
        }
        for (auto &it : stock) {
            cout << "Item Name: " << items[it.first]->getName()
                << " | Stock: " << it.second << endl;
        }
    }

};
class CartItem{
    Item* item;
    int quan;
    public:
    CartItem(Item* item,int quan):item(item),quan(quan){}
    double getTotal(){
        return quan*item->getPrice();
    }
    void updateQuantity(int q){  //q can be negative
        quan=max(0,quan+q);
    }
    Item* getItem(){
        return item;
    }
    int getQuan(){
        return quan;
    }
};

class Cart{
    vector<CartItem> items;
    public:
    void addItem(Item* item,int quan){
        bool added=false;
        for(auto &it:items){
            if(it.getItem()->getId()==item->getId()){
                it.updateQuantity(quan);
                added=true;
                break;
            }
        }
        if(!added){
            items.push_back(CartItem(item,quan));
        }
    }
    void removeItem(Item* item,int quan){   //learned use of remove_if with erase
        items.erase(remove_if(items.begin(),items.end(),
             [&](auto &it){
                if(it.getItem()->getId()==item->getId()){
                    it.updateQuantity(-quan);
                    return it.getQuan()==0;
                }
                return false;
             })
            ,items.end());
    }
    void clearCart(){
        items.clear();
    }
    double getTotal(){
        double total=0;
        for(auto &it:items){
            total+=it.getTotal();
        }
        return total;
    }
    bool isEmpty(){
        return items.size()==0;
    }
    void getDetails(){
        for(auto &item:items){
            cout<<"[Item]: "<<item.getItem()->getName()<<" quantity:"<<item.getQuan()<<endl;
        }
        cout<<"Total Amount: "<<getTotal()<<endl;
    }
    vector<CartItem>& getCart(){
        return items;
    }

};

enum class Coins{
    RUPPEE_1=1,
    RUPPEE_2=2,
    RUPPEE_5=5,
    RUPPEE_10=10,
    RUPPEE_20=20
};
class MachineState;
class IdleState;
class HasMoneyState;
class ItemSelectedState;
class DispenseState;
class VendingMachine{
    MachineState* state;
    ItemManager& itemManager;
    unique_ptr<Inventory> inventory;
    unique_ptr<Cart> cart;
    unique_ptr<IdleState> idleState;
    unique_ptr<HasMoneyState> hasMoneyState;
    unique_ptr<ItemSelectedState> itemSelectedState;
    unique_ptr<DispenseState> dispenseState;
    unique_ptr<InventoryObserver> adminObserver;
    int coins;
    VendingMachine();
    public:
    VendingMachine(const VendingMachine&)=delete;
    VendingMachine& operator=(const VendingMachine&)=delete;
    static VendingMachine& getInstance(){
        static VendingMachine instance;
        return instance;
    }
    Inventory* getInventory();
    void updateCoins(int val);
    MachineState* getIdleState();
    MachineState* getHasMoneyState();
    MachineState* getItemSelectedState();
    MachineState* getDispenseState();

    int getCoins();
    void addItem(const string& name,double price);
    Item* getItemByName(const string& name);
    void addToCart(Item* item,int quan);
    void removeFromCart(Item* item,int quan);
    double getTotal();
    bool cartEmpty();
    void internalclearCart();
    void getCartDetails();
    vector<CartItem>& getCart();
    int getItemStock(const string& id);
    void addStock(const string& name,double price,int quan);
    void insertCoin(Coins coin);
    void refundCoin();
    void selectItem(const string& name,int quan);
    void removeSelectedItem(const string& name,int quan);
    void clearCart();
    void dispenseItems();
    void selectDone();
    void showDetails();
    void deductCoins(int val);

};
class MachineState{
    protected:
    VendingMachine& machine;
    public:
    MachineState(VendingMachine& m):machine(m){}
    virtual MachineState* addStock(const string& name,double price,int quan)=0;
    virtual MachineState* insertCoin(int val)=0;
    virtual MachineState* refundCoin()=0;
    virtual MachineState* selectItem(const string& name,int quan)=0;
    virtual MachineState* removeSelectedItem(const string& name,int quan)=0;
    virtual MachineState* clearCart()=0;
    virtual MachineState* dispenseItems()=0;
    virtual MachineState* selectDone()=0;
    virtual ~MachineState()=default;
};
class IdleState:public MachineState{
    public:
    IdleState(VendingMachine& m):MachineState(m){}
    MachineState* addStock(const string& name, double price, int quan) {
        Item* item = machine.getItemByName(name);

        // Create item ONLY if it does not exist
        if (!item) {
            machine.addItem(name, price);
            item = machine.getItemByName(name);
        }

        machine.getInventory()->addItem(item, quan);
        return machine.getIdleState();
    }

    MachineState* insertCoin(int val){
        machine.updateCoins(val);
        return machine.getHasMoneyState();
    }
    MachineState* refundCoin(){
        cout<<"Please insert Coin first"<<endl;
        return machine.getIdleState();
    }
    MachineState* selectItem(const string& name,int quan){
        cout<<"Please insert Coins first"<<endl;
        return machine.getIdleState();
    }
    MachineState* removeSelectedItem(const string& name,int quan){
        cout<<"Please insert Coins first"<<endl;
        return machine.getIdleState();
    }
    MachineState* clearCart(){
        cout<<"Please insert Coins first"<<endl;
        return machine.getIdleState();
    }
    MachineState* dispenseItems(){
        cout<<"Please insert Coins first"<<endl;
        return machine.getIdleState();
    }
    MachineState* selectDone(){
        cout<<"Please insert Coins first"<<endl;
        return machine.getIdleState();
    }

};
class HasMoneyState:public MachineState{
    public:
    HasMoneyState(VendingMachine& m):MachineState(m){}
    MachineState* addStock(const string& name,double price,int quan){
        cout<<"Please select an Item"<<endl;
        return machine.getHasMoneyState();
    }
    MachineState* insertCoin(int val){
        machine.updateCoins(val);
        return machine.getHasMoneyState();
    }
    MachineState* refundCoin(){
        machine.internalclearCart();
        machine.updateCoins(-machine.getCoins());
        return machine.getIdleState();
        
    }
    MachineState* selectItem(const string& name,int quan){
        Item* item=machine.getItemByName(name);
        int stock=machine.getItemStock(item->getId());
        if(stock<quan){
            if(stock==0){
                cout<<"Item is out of Stock"<<endl;
                return machine.getHasMoneyState();
            }else{
                cout<<"Only "<<stock<<" items are available and selected"<<endl;
                quan=stock;
            }
        }
        machine.addToCart(item,quan);
        if(machine.getTotal()>machine.getCoins()){
            machine.removeFromCart(item,quan);
            cout<<"["<<item->getName()<<"] not selected: "<<"Insufficent Coins"<<endl;
            return machine.getHasMoneyState();
        }
        return machine.getItemSelectedState();
    }
    MachineState* removeSelectedItem(const string& name,int quan){
        cout<<"Please select an Item"<<endl;
        return machine.getHasMoneyState();
    }
    MachineState* clearCart(){
        cout<<"Please select an Item"<<endl;
        return machine.getHasMoneyState();
    }
    MachineState* dispenseItems(){
         cout<<"Please select an Item"<<endl;
        return machine.getHasMoneyState();
    }
    MachineState* selectDone(){
        cout<<"Please select an Item"<<endl;
        return machine.getHasMoneyState();
    }
};
class ItemSelectedState:public MachineState{
    public:
    ItemSelectedState(VendingMachine& m):MachineState(m){}
    MachineState* addStock(const string& name,double price,int quan){
        cout<<"Select an Item"<<endl;
        return machine.getItemSelectedState();
    }
    MachineState* insertCoin(int val){
        machine.updateCoins(val);
        return machine.getItemSelectedState();
    }
    MachineState* refundCoin(){
        cout<<"Select an Item"<<endl;
        return machine.getItemSelectedState();
    }
    MachineState* selectItem(const string& name,int quan){
        Item* item=machine.getItemByName(name);
        int stock=machine.getItemStock(item->getId());
        if(stock<quan){
            if(stock==0){
                cout<<"Item is out of Stock"<<endl;
                return machine.getItemSelectedState();
            }else{
                cout<<"Only "<<stock<<" items are available and selected"<<endl;
                quan=stock;
            }
        }
        machine.addToCart(item,quan);
        if(machine.getTotal()>machine.getCoins()){
            machine.removeFromCart(item,quan);
            cout<<"["<<item->getName()<<"] not selected: "<<"Insufficent Coins"<<endl;
            return machine.getItemSelectedState();
        }
        return machine.getItemSelectedState();
    }
    MachineState* removeSelectedItem(const string& name,int quan){
        Item* item=machine.getItemByName(name);
        if(machine.cartEmpty()){
            cout<<"Select an Item"<<endl;
            return machine.getItemSelectedState();
        }
        machine.removeFromCart(item,quan);
        return machine.getItemSelectedState();
    }
    MachineState* selectDone(){
        if(machine.cartEmpty()){
            cout<<"Select an Item"<<endl;
            return machine.getItemSelectedState();
        }
        return machine.getDispenseState();
    }
    MachineState* clearCart(){
        machine.internalclearCart();
        return machine.getItemSelectedState();
    }
     MachineState* dispenseItems(){
        cout<<"Select an Item"<<endl;
        return machine.getItemSelectedState();
     }

};
class DispenseState:public MachineState{
    public:
    DispenseState(VendingMachine& m):MachineState(m){}
     MachineState* addStock(const string& name,double price,int quan){
        cout<<"Dispense Items"<<endl;
        return machine.getDispenseState();
     }
     MachineState* insertCoin(int val){
        cout<<"Dispense Items"<<endl;
        return machine.getDispenseState();
     }
     MachineState* refundCoin(){
        cout<<"Dispense Items"<<endl;
        return machine.getDispenseState();
     }
     MachineState* selectItem(const string& name,int quan){
        cout<<"Dispense Items"<<endl;
        return machine.getDispenseState();
     }
     MachineState* removeSelectedItem(const string& name,int quan){
        cout<<"Dispense Items"<<endl;
        return machine.getDispenseState();
     }
     MachineState* clearCart(){
        cout<<"Dispense Items"<<endl;
        return machine.getDispenseState();
     }
     MachineState* selectDone(){
        cout<<"Dispense Items"<<endl;
        return machine.getDispenseState();
     }
     MachineState* dispenseItems(){
        cout<<"Dispensed Items are: "<<endl;
        Inventory* inv=machine.getInventory();
        vector<CartItem>& cart=machine.getCart();
        for(auto item:cart){
            inv->reduceStock(item.getItem()->getId(),item.getQuan());
        }
        machine.getCartDetails();
        machine.deductCoins(machine.getTotal());
        machine.internalclearCart();
        if(machine.getCoins()>0){
            cout<<"Remaining amount is Refunded: "<<machine.getCoins()<<endl;
            machine.updateCoins(-machine.getCoins());
        }
        return machine.getIdleState();
     }

};
VendingMachine::VendingMachine():itemManager(ItemManager::getInstance()){
        inventory=make_unique<Inventory>();
        cart=make_unique<Cart>();
        idleState=make_unique<IdleState>(*this);
        hasMoneyState=make_unique<HasMoneyState>(*this);
        itemSelectedState=make_unique<ItemSelectedState>(*this);
        dispenseState=make_unique<DispenseState>(*this);
        state=idleState.get();
        adminObserver=make_unique<AdminDashBoard>();
        inventory->addObservers(adminObserver.get());

        coins=0;
    }
Inventory* VendingMachine::getInventory(){
        return inventory.get();
    }
void VendingMachine::updateCoins(int val){
        coins=max(0,coins+val);
    }
MachineState* VendingMachine::getIdleState() {
    return idleState.get();
}

MachineState* VendingMachine::getHasMoneyState() {
    return hasMoneyState.get();
}

MachineState* VendingMachine::getItemSelectedState() {
    return itemSelectedState.get();
}

MachineState* VendingMachine::getDispenseState() {
    return dispenseState.get();
}

int VendingMachine::getCoins(){
    return coins;
}
void VendingMachine::addItem(const string& name,double price){
    itemManager.addItem(make_unique<Item>(name,price));
}
Item* VendingMachine::getItemByName(const string& name){
    return itemManager.getItemByName(name);
}
void VendingMachine::addToCart(Item* item,int quan){
    cart->addItem(item,quan);
}
void VendingMachine::removeFromCart(Item* item,int quan){
    cart->removeItem(item,quan);
}
double VendingMachine::getTotal(){
    return cart->getTotal();
}
bool VendingMachine::cartEmpty(){
    return cart->isEmpty();
}
void VendingMachine::internalclearCart(){
    cart->clearCart();
}
void VendingMachine::getCartDetails(){
    cart->getDetails();
}
vector<CartItem>& VendingMachine::getCart(){
    return cart->getCart();
}
int VendingMachine::getItemStock(const string& id){
    return inventory->getItemStock(id);
}
void VendingMachine::addStock(const string& name,double price,int quan){
        state=state->addStock(name,price,quan);
    }
void VendingMachine::insertCoin(Coins coin){
    int val=static_cast<int>(coin);
    state=state->insertCoin(val);
}
void VendingMachine::refundCoin(){
    state=state->refundCoin();
}
void VendingMachine::selectItem(const string& name,int quan){
    state=state->selectItem(name,quan);
}
void VendingMachine::removeSelectedItem(const string& name,int quan){
    state=state->removeSelectedItem(name,quan);
}
void VendingMachine::clearCart(){
    state=state->clearCart();
}
void VendingMachine::dispenseItems(){
    state=state->dispenseItems();
}
void VendingMachine::selectDone(){
    state=state->selectDone();
}
void VendingMachine::showDetails() {
    cout << "\n========== VENDING MACHINE STATUS ==========\n";

    // Coins
    cout << "Inserted Coins: " << coins << endl;

    // Current State
    cout << "Current State: ";
    if (state == idleState.get()) {
        cout << "IdleState\n";
    } else if (state == hasMoneyState.get()) {
        cout << "HasMoneyState\n";
    } else if (state == itemSelectedState.get()) {
        cout << "ItemSelectedState\n";
    } else if (state == dispenseState.get()) {
        cout << "DispenseState\n";
    } else {
        cout << "Unknown\n";
    }

    // Inventory
    cout << "\n--- Inventory Stock ---\n";
    inventory->showStock();

    // Cart
    cout << "\n--- Cart Details ---\n";
    if (cart->isEmpty()) {
        cout << "Cart is empty\n";
    } else {
        cart->getDetails();
    }

    cout << "===========================================\n";
}
void VendingMachine::deductCoins(int val){
    coins=max(0,coins-val);
}
int main() {
    VendingMachine& vm = VendingMachine::getInstance();

    // =====================================================
    // ADMIN SETUP
    // =====================================================
    cout << "\n========== ADMIN SETUP ==========\n";

    vm.addStock("Kurkure", 20, 2);    // low stock → observer test
    vm.addStock("Biscuit", 15, 5);
    vm.addStock("Pepsi", 30, 1);      // single item → observer test
    vm.addStock("Coke", 45, 3);
    vm.addStock("Lays", 30, 4);
    vm.addStock("Chocolate", 15, 2);  // low stock → observer test
    vm.addStock("Water", 20, 10);

    vm.showDetails();

    // =====================================================
    // USER INSERTS COINS
    // =====================================================
    cout << "\n========== USER INSERTS COINS ==========\n";

    vm.insertCoin(Coins::RUPPEE_20);
    vm.insertCoin(Coins::RUPPEE_20);
    vm.insertCoin(Coins::RUPPEE_10); // total = 50

    vm.showDetails();

    // =====================================================
    // USER SELECTS ITEMS (NORMAL FLOW)
    // =====================================================
    cout << "\n========== USER SELECTS ITEMS ==========\n";

    vm.selectItem("Kurkure", 1);   // ok
    vm.selectItem("Pepsi", 1);     // ok (last Pepsi)
    vm.selectItem("Coke", 1);      // should fail (insufficient coins)

    vm.showDetails();

    // =====================================================
    // USER REMOVES ITEM
    // =====================================================
    cout << "\n========== USER REMOVES ITEM ==========\n";

    vm.removeSelectedItem("Kurkure", 1);

    vm.showDetails();

    // =====================================================
    // USER ADDS MORE COINS
    // =====================================================
    cout << "\n========== USER ADDS MORE COINS ==========\n";

    vm.insertCoin(Coins::RUPPEE_20);

    vm.selectItem("Coke", 1);  // now succeeds

    vm.showDetails();

    // =====================================================
    // USER CONFIRMS & DISPENSES
    // Pepsi stock → 0  ✅ ADMIN NOTIFIED
    // =====================================================
    cout << "\n========== USER CONFIRMS SELECTION ==========\n";

    vm.selectDone();

    cout << "\n========== DISPENSING ==========\n";
    vm.dispenseItems();

    vm.showDetails();

    // =====================================================
    // SECOND PURCHASE – OBSERVER TRIGGERS AGAIN
    // Chocolate stock → 0
    // =====================================================
    cout << "\n========== SECOND PURCHASE ==========\n";

    vm.insertCoin(Coins::RUPPEE_20);
    vm.insertCoin(Coins::RUPPEE_10);

    vm.selectItem("Chocolate", 2); // exhausts stock → ADMIN ALERT
    vm.selectDone();
    vm.dispenseItems();

    vm.showDetails();

    vm.addStock("Chocolate",15,2);
    vm.addStock("Pepsi", 30, 1);

    // =====================================================
    // CLEAR CART TEST
    // =====================================================
    cout << "\n========== CLEAR CART TEST ==========\n";

    vm.insertCoin(Coins::RUPPEE_20);
    vm.selectItem("Water", 1);
    vm.clearCart();   // state + cart reset

    vm.showDetails();

    // =====================================================
    // REFUND FLOW TEST
    // =====================================================
    cout << "\n========== REFUND TEST ==========\n";

    vm.insertCoin(Coins::RUPPEE_10);
    vm.insertCoin(Coins::RUPPEE_5);

    vm.showDetails();

    vm.refundCoin();  // coins refunded, back to idle

    vm.showDetails();

    // =====================================================
    // FINAL STATE
    // =====================================================
    cout << "\n========== FINAL MACHINE STATE ==========\n";
    vm.showDetails();

    cout << "\n========== PROGRAM END ==========\n";
    return 0;
}
