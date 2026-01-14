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

};
int ParkingLevel::cnt=1;
int main(){}