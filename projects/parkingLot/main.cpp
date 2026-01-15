#include <bits/stdc++.h>
using namespace std;
enum class VehicleSize{
    SMALL,
    MEDIUM,
    LARGE
};
enum class VehicleType{
    BIKE,
    CAR,
    TRUCK
};
class Vehicle{
    protected:
    VehicleSize size;
    VehicleType type;
    string number;
    public:
    Vehicle(VehicleSize size,VehicleType type, const string& number):size(size),type(type),number(number){}
    string getVehicleNumber(){
        return number;
    }
    VehicleSize getSize(){
        return size;
    }
    VehicleType getType(){
        return type;
    }
};
class Bike:public Vehicle{
    public:
    Bike(const string &number):Vehicle(VehicleSize::SMALL,VehicleType::BIKE,number){}
};
class Car:public Vehicle{
    public:
    Car(const string &number):Vehicle(VehicleSize::MEDIUM,VehicleType::CAR,number){}
};
class Truck:public Vehicle{
    public:
    Truck(const string &number):Vehicle(VehicleSize::LARGE,VehicleType::TRUCK,number){}
};
class ParkingSpot{
    static int cnt;
    protected:
    string id;
    Vehicle* vehicle;
    VehicleSize size;
    bool occupied;
    public:
    ParkingSpot(VehicleSize size):size(size){
        id="park_"+to_string(cnt); cnt++;
        occupied=false;
        vehicle=nullptr;
    }
    bool isOccupied(){
        return occupied;
    }
    VehicleSize getSize(){
        return size;
    }
    virtual bool canPark(Vehicle* vehicle)=0;
    void park(Vehicle* vehicle){
        if(occupied || !canPark(vehicle)){
            cout<<"Spot is already occupied"<<endl;
            return;
        }
        this->vehicle=vehicle;
        occupied=true;
    }
    void unpark(){
        if(!occupied){ 
            cout<<"Vehicle is not parked"<<endl;
            return;
        }
        this->vehicle=nullptr;
        occupied=false;
    }
    string getId(){
        return id;
    }
    virtual ~ParkingSpot()=default;
};
int ParkingSpot::cnt=1;
class SmallSpot:public ParkingSpot{
    public:
    SmallSpot():ParkingSpot(VehicleSize::SMALL){}
    bool canPark(Vehicle* vehicle){
        if(vehicle->getSize()<=size){
            return true;
        }
        return false;
    }
};
class MediumSpot:public ParkingSpot{
    public:
    MediumSpot():ParkingSpot(VehicleSize::MEDIUM){}
    bool canPark(Vehicle* vehicle){
        if(vehicle->getSize()<=size){
            return true;
        }
        return false;
    }
};
class LargeSpot:public ParkingSpot{
    public:
    LargeSpot():ParkingSpot(VehicleSize::LARGE){}
    bool canPark(Vehicle* vehicle){
        if(vehicle->getSize()<=size){
            return true;
        }
        return false;
    }
};

class ParkingLevel{
    static int cnt;
    int level;
    unordered_map<ParkingSpot*,bool> record;
    unordered_map<string,ParkingSpot*> parkedVehicle;  //vehicle no,spot
    int smallAvail;
    int mediumAvail;
    int largeAvail;
    public:
    ParkingLevel(){
        level=cnt++;
        smallAvail=0;
        mediumAvail=0;
        largeAvail=0;
    }
    unordered_map<ParkingSpot*,bool>& getRecord(){
        return record;
    }
    int totalSpot(){
        return record.size();
    }
    void addSpot(ParkingSpot* spot){
        if(record.find(spot)!=record.end()){
            cout<<"spot is aready added"<<endl;
            return;
        }
        switch(spot->getSize()){
            case VehicleSize::SMALL:
                smallAvail++;
                break;
            case VehicleSize::MEDIUM:
                mediumAvail++;
                break;
            case VehicleSize::LARGE:
                largeAvail++;
                break;
        }
        record[spot]=false;
    }
    void getDetails(){
        cout<<"["<<"Total Available Spot"<<"] :"<<smallAvail+mediumAvail+largeAvail<<endl;
        cout<<"["<<"Total Small Available Spot"<<"] :"<<smallAvail<<endl;
        cout<<"["<<"Total Medium Available Spot"<<"] :"<<mediumAvail<<endl;
        cout<<"["<<"Total Large Available Spot"<<"] :"<<largeAvail<<endl;

        if(parkedVehicle.size()==0){
            cout<<"No vehicle is parked at level: "<<level<<endl;
            return;
        }
        for(auto &it:parkedVehicle){
            cout<<"+++++++++++++ Parked Vehicles at level: "<<level<<"++++++++++++++++"<<endl;
            cout<<"["<<"Vehicle Number"<<"]: "<<it.first<<"       "<<"["<<"parking Spot"<<"]: "<<it.second->getId()<<endl;
        }
    }

    void park(Vehicle* vehicle,ParkingSpot* spot){
        if(record[spot]){
            cout<<"Spot is already filled"<<endl;
            return;
        }
        if(!spot->canPark(vehicle) || spot->isOccupied()){
            cout<<"Vehicle cannot be parked here"<<endl;
            return;
        }
        spot->park(vehicle);
        record[spot]=true;
        parkedVehicle[vehicle->getVehicleNumber()]=spot;
        switch(spot->getSize()){
            case VehicleSize::SMALL:
                smallAvail--;
                break;
            case VehicleSize::MEDIUM:
                mediumAvail--;
                break;
            case VehicleSize::LARGE:
                largeAvail--;
                break;
        }
        cout<<"["<<"Level"<<"]: "<<level<<endl;
        cout<<"["<<"Vehicle"<<"]:"<<vehicle->getVehicleNumber()<<" parked at: "<<spot->getId()<<endl;
    }

    void unpark(Vehicle *vehicle){
        auto it=parkedVehicle.find(vehicle->getVehicleNumber());
        if(it==parkedVehicle.end()){
            cout<<"Vehicle can,t be unparked"<<endl;
            return;
        }
        ParkingSpot* spot=it->second;
        parkedVehicle.erase(it);
        record[spot]=false;
        spot->unpark();
        switch(spot->getSize()){
            case VehicleSize::SMALL:
                smallAvail++;
                break;
            case VehicleSize::MEDIUM:
                mediumAvail++;
                break;
            case VehicleSize::LARGE:
                largeAvail++;
                break;
        }
        cout<<"["<<"Level"<<"]: "<<level<<endl;
        cout<<"["<<"Vehicle"<<"]:"<<vehicle->getVehicleNumber()<<"is unparked"<<endl;
    }
    int getLevel(){
        return level;
    }
};
int ParkingLevel::cnt=1;
class FeeStrategy{
    public:
    virtual double calcFee(ParkingSpot* spot,int hrs)=0;
    virtual ~FeeStrategy()=default;
};
class VehicleSizeBasedFee:public FeeStrategy{
    unordered_map<VehicleSize,double> fee; //per hrs
    public:
    VehicleSizeBasedFee(){
        fee[VehicleSize::SMALL]=10.0;
        fee[VehicleSize::MEDIUM]=15.0;
        fee[VehicleSize::LARGE]=20.0;
    }
    double calcFee(ParkingSpot* spot,int hrs){
        hrs=max(1,hrs);
        return fee[spot->getSize()]*hrs;
    }
};
class ParkingTicket{
    static int cnt;
    string id;
    int level;
    ParkingSpot* spot;
    int enterTime;
    int exitTime;
    Vehicle* vehicle;
    FeeStrategy* feeStrag;
    public: 
    ParkingTicket(int level,ParkingSpot *spot,int enter,Vehicle* vehicle,FeeStrategy* feeStrag):level(level),spot(spot),enterTime(enter),exitTime(enter),vehicle(vehicle),feeStrag(feeStrag){
        id="ticket_"+to_string(cnt++);
    }
    double calcFee(){
        return feeStrag->calcFee(spot,exitTime-enterTime);
    }
    int getLevel(){
        return level;
    }
    void getEntryTicket(){
        cout<<"Ticket Id: "<<id<<endl;
        cout<<"Parking level: "<<level<<"   "<<"Spot: "<<spot->getId()<<endl;
        cout<<"Enter time: "<<enterTime<<endl;
        cout<<"Vehicle :"<<vehicle->getVehicleNumber()<<endl;
    }
    void getExitTicket(){
        cout<<"Ticket Id: "<<id<<endl;
        cout<<"Vehicle :"<<vehicle->getVehicleNumber()<<endl;
        cout<<"Parking level: "<<level<<"   "<<"Spot: "<<spot->getId()<<endl;
        cout<<"Enter time: "<<enterTime<<" "<<"Exit time: "<<exitTime<<endl;
        cout<<"Total Hrs parked: "<<exitTime-enterTime<<endl;
        cout<<"Total Bill: "<<calcFee()<<endl;
    }
    void setExitTime(int time){
        exitTime=time;
    }
};
int ParkingTicket::cnt=1;
class ParkingStrategy{
    public:
    virtual pair<int,ParkingSpot*> findSpot(unordered_map<int,ParkingLevel*> & parkingArea,Vehicle* vehicle)=0;
    virtual ~ParkingStrategy()=default;
};
class BestFit:public ParkingStrategy{
    public:
    pair<int,ParkingSpot*> findSpot(unordered_map<int,ParkingLevel*> & parkingArea,Vehicle* vehicle){
        for(auto &[id,level]:parkingArea){
            for(auto &[spot,avail]:level->getRecord()){
                if((spot->getSize()==vehicle->getSize()) && !avail){
                    return {id,spot};
                }
            }
        }
        return {-1,nullptr};
    }
};
class ParkingLot{
    unordered_map<int,ParkingLevel*> parkingArea; //level no,level
    unordered_map<string,unique_ptr<ParkingTicket>> activeTickets;  //vechicle no,ticket
    FeeStrategy* feeStrat;
    ParkingStrategy* parkingStrat;
    ParkingLot(FeeStrategy* feeStrat,ParkingStrategy* parkingStrat):feeStrat(feeStrat),parkingStrat(parkingStrat){}
    public:
    ParkingLot(const ParkingLot&)=delete;  //avoid copying
    ParkingLot& operator=(const ParkingLot&)=delete;  //avoid assiging
    static ParkingLot& init(FeeStrategy* feeStrat,ParkingStrategy* parkingStrat){
        static ParkingLot instance(feeStrat,parkingStrat);
        return instance;
    }
    static ParkingLot& getInstance(){
        return init(nullptr,nullptr);
    }
    void addLevels(ParkingLevel* level){
        if(parkingArea.find(level->getLevel())!=parkingArea.end()){
            cout<<"This level is already added"<<endl;
            return;
        }
        parkingArea[level->getLevel()]=level;
    }
    void getFullDetails(){
        if(parkingArea.size()==0){
            cout<<"Nothing to show"<<endl;
            return;
        }
        for(auto [id,level]:parkingArea){
            cout<<"[LEVEL]: "<<id<<endl;
            level->getDetails();
            cout<<endl;
        }
    }

    void getLevelDetail(int id){
        if(parkingArea.find(id)==parkingArea.end()){
            cout<<"This floor doesn,t exist"<<endl;
            return;
        }
        cout<<"[LEVEL]: "<<id<<endl;
        parkingArea[id]->getDetails();
        cout<<endl;
    }

    pair<int,ParkingSpot*> findSpot(unordered_map<int,ParkingLevel*> & parkingArea,Vehicle* vehicle){
        return parkingStrat->findSpot(parkingArea,vehicle);
    }
    unique_ptr<ParkingTicket> genrateTicket(int level,ParkingSpot *spot,int enter,int exit,Vehicle* vehicle,FeeStrategy* feeStrat){
        return make_unique<ParkingTicket>(level,spot,enter,exit,vehicle,feeStrat);
    }
    void entry(Vehicle* vehicle,int entryTime){
        auto [level,spot]=findSpot(parkingArea,vehicle);
        if(level==-1){
            cout<<"No valid Parking Spot is Available for this Vehicle"<<endl;
            return;
        }
        unique_ptr<ParkingTicket> ticket=genrateTicket(level,spot,entryTime,vehicle,feeStrat);
        ticket->getEntryTicket();
        parkingArea[level]->park(vehicle,spot);
        activeTickets[vehicle->getVehicleNumber()]=move(ticket);
    }

    void exit(Vehicle* vehicle,int time){
        auto it=activeTickets.find(vehicle->getVehicleNumber());
        if(activeTickets.size()==0 || it==activeTickets.end()){
            cout<<"This vehicle is not parked"<<endl;
            return;
        }
        ParkingTicket* ticket=it->second.get();
        ParkingLevel* floor=parkingArea[ticket->getLevel()];
        floor->unpark(vehicle);
        ticket->setExitTime(time);
        ticket->getExitTicket();
        activeTickets.erase(it);
    } 
};

int main(){
    vector<unique_ptr<Vehicle>> vehicles;  //12
    //3 bikes
    for(int i=0;i<3;i++){
        string num="bike_"+to_string(i+1);
        vehicles.push_back(make_unique<Bike>(num));
    }
    for(int i=0;i<4;i++){
        string num="car_"+to_string(i+1);
        vehicles.push_back(make_unique<Car>(num));
    }
    for(int i=0;i<5;i++){
        string num="truck_"+to_string(i+1);
        vehicles.push_back(make_unique<Truck>(num));
    }
    vector<unique_ptr<ParkingSpot>> spots;
    for(int i=0;i<2;i++){
        spots.push_back(make_unique<SmallSpot>());
    }
    for(int i=0;i<3;i++){
        spots.push_back(make_unique<MediumSpot>());
    }
    for(int i=0;i<3;i++){
        spots.push_back(make_unique<LargeSpot>());
    }
    vector<unique_ptr<ParkingLevel>> levels;
    for(int i=0;i<2;i++){
        levels.push_back(make_unique<ParkingLevel>());
    }

    for(int i=0;i<8;i++){
        if(i%2==0){
            levels[0]->addSpot(spots[i].get());
        }else{
            levels[1]->addSpot(spots[i].get());
        }
    }
    unqiue_ptr<ParkingStrategy> parkingStrat=make_unique<BestFit>();
    unique_ptr<FeeStrategy> feeStrat=make_unique<VehicleSizeBasedFee>();
    ParkingLot & place=ParkingLot::init(feeStrat.get(),parkingStrat.get());
    for(auto level:levels){
        place.addLevels(level.get());
    }
    place.getFullDetails();
    place.entry(vehicles[2].get(),7);
    place.entry(vehicles[9].get(),5);
    place.entry(vehicles[4].get(),5);
    place.entry(vehicles[8].get(),4);
    place.exit(vehicles[2].get(),9);
    place.entry(vehicles[6].get(),10);
    place.entry(vehicles[0].get(),10);
    place.exit(vehicles[9].get(),8);
    place.exit(vehicles[4].get(),7);
    place.exit(vehicles[0].get(),11);

}