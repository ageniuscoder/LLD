#include <bits/stdc++.h>
#include <thread>
#include <chrono>
#include <atomic>

using namespace std;
using namespace std::chrono_literals;

/* ================= ENUMS ================= */

enum class Color { GREEN, YELLOW, RED };
enum class Direction { EAST, WEST, NORTH, SOUTH };
enum class Vehicle { AMBULANCE, FIRE_TRUCK };

/* ================= TRAFFIC SIGNAL ================= */

class TrafficSignal {
    Color color = Color::RED;
public:
    void setColor(Color c) { color = c; }
    Color getColor() const { return color; }
};

/* ================= ROAD ================= */

class Road {
    Direction dir;
    TrafficSignal signal;
public:
    Road(Direction d) : dir(d) {}

    void setSignal(Color c) { signal.setColor(c); }
    Direction getDir() const { return dir; }
    Color getSignal() const { return signal.getColor(); }
};

/* ================= SIGNAL GROUP ================= */

class SignalGroup {
    vector<Road*> roads;
public:
    void addRoad(Road* r) { roads.push_back(r); }

    void setColor(Color c) {
        for (auto r : roads) r->setSignal(c);
    }
};

/* ================= INTERSECTION ================= */

class Intersection {
    SignalGroup eastWest;
    SignalGroup northSouth;
public:
    void addRoad(Road* r) {
        if (r->getDir() == Direction::EAST || r->getDir() == Direction::WEST)
            eastWest.addRoad(r);
        else
            northSouth.addRoad(r);
    }

    SignalGroup& EW() { return eastWest; }
    SignalGroup& NS() { return northSouth; }
};

/* ================= STATE PATTERN ================= */

class SignalState {
public:
    virtual Color color() = 0;
    virtual SignalState* next() = 0;
    virtual ~SignalState() = default;
};

class GreenState : public SignalState {
public:
    Color color() override { return Color::GREEN; }
    SignalState* next() override;
};

class YellowState : public SignalState {
public:
    Color color() override { return Color::YELLOW; }
    SignalState* next() override;
};

class RedState : public SignalState {
public:
    Color color() override { return Color::RED; }
    SignalState* next() override;
};

SignalState* GreenState::next()  { static YellowState y; return &y; }
SignalState* YellowState::next() { static RedState r; return &r; }
SignalState* RedState::next()    { static GreenState g; return &g; }

/* ================= STRATEGY ================= */

class TimeStrategy {
public:
    virtual int greenTime(int load = 0) = 0;
    virtual int yellowTime() { return 5; }
    virtual ~TimeStrategy() = default;
};

class FixedTime : public TimeStrategy {
public:
    int greenTime(int = 0) override { return 10; }
};

/* ================= FORWARD DECL ================= */

class TrafficController;

/* ================= EMERGENCY SENSOR ================= */

class EmergencySensor {
    TrafficController* controller;
public:
    EmergencySensor(TrafficController* c) : controller(c) {}
    void sense(Vehicle, Direction);
};

/* ================= CONTROLLER (SINGLETON) ================= */

class TrafficController {
    Intersection* intersection = nullptr;
    TimeStrategy* strategy = nullptr;

    SignalState* state;
    SignalGroup* active;
    SignalGroup* inactive;

    atomic<bool> emergency{false};
    Direction emergencyDir;

    TrafficController() {
        static RedState r;
        state = &r;
    }

public:
    TrafficController(const TrafficController&) = delete;
    TrafficController& operator=(const TrafficController&) = delete;

    static TrafficController& instance() {
        static TrafficController c;
        return c;
    }

    void setIntersection(Intersection* i) { intersection = i; }
    void setStrategy(TimeStrategy* s) { strategy = s; }

    void triggerEmergency(Direction d) {
        emergency = true;
        emergencyDir = d;
    }

    void handleEmergency() {
        intersection->EW().setColor(Color::RED);
        intersection->NS().setColor(Color::RED);

        if (emergencyDir == Direction::EAST || emergencyDir == Direction::WEST)
            intersection->EW().setColor(Color::GREEN);
        else
            intersection->NS().setColor(Color::GREEN);

        this_thread::sleep_for(5s);
        emergency = false;
    }

    void run() {
        active = &intersection->NS();
        inactive = &intersection->EW();

        while (true) {
            if (emergency) {
                handleEmergency();
                continue;
            }

            // GREEN
            inactive->setColor(state->color());
            state=state->next();
            active->setColor(state->color());
            this_thread::sleep_for(seconds(strategy->greenTime()));

            // YELLOW
            state=state->next();
            active->setColor(state->color());
            this_thread::sleep_for(seconds(strategy->yellowTime()));

            // RED
            state=state->next();
            active->setColor(state->color());
            swap(active, inactive);
        }
    }
};

/* ================= SENSOR IMPLEMENTATION ================= */

void EmergencySensor::sense(Vehicle v, Direction d) {
    switch(v){
        case Vehicle::AMBULANCE:
            controller->triggerEmergency(d);
            break;
        case Vehicle::FIRE_TRUCK:
            controller->triggerEmergency(d);
            break;
    }
}

/* ================= MAIN ================= */

int main() {
    // Roads
    Road east(Direction::EAST), west(Direction::WEST);
    Road north(Direction::NORTH), south(Direction::SOUTH);

    // Intersection
    Intersection intersection;
    intersection.addRoad(&east);
    intersection.addRoad(&west);
    intersection.addRoad(&north);
    intersection.addRoad(&south);

    // Controller
    TrafficController& controller = TrafficController::instance();
    controller.setIntersection(&intersection);

    FixedTime fixed;
    controller.setStrategy(&fixed);

    // Emergency sensor
    EmergencySensor sensor(&controller);

    // Emergency trigger thread
    thread emergencyThread([&]() {
        this_thread::sleep_for(15s);
        sensor.sense(Vehicle::AMBULANCE, Direction::NORTH);
    });

    controller.run();
    emergencyThread.join();
}