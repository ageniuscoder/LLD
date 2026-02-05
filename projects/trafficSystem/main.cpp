#include <bits/stdc++.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace std::chrono_literals;

/* ================= ENUMS ================= */

enum class Color { GREEN, YELLOW, RED };
enum class Direction { EAST, WEST, NORTH, SOUTH };
enum class Vehicle { AMBULANCE, FIRE_TRUCK };

/* ================= UTILS ================= */

string colorToStr(Color c) {
    switch (c) {
        case Color::GREEN: return "GREEN";
        case Color::YELLOW: return "YELLOW";
        case Color::RED: return "RED";
    }
    return "";
}

string dirToStr(Direction d) {
    switch (d) {
        case Direction::NORTH: return "NORTH";
        case Direction::SOUTH: return "SOUTH";
        case Direction::EAST:  return "EAST";
        case Direction::WEST:  return "WEST";
    }
    return "";
}

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
};

/* ================= SIGNAL GROUP ================= */

class SignalGroup {
    vector<Road*> roads;
public:
    void addRoad(Road* r) { roads.push_back(r); }

    void setColor(Color c) {
        for (auto r : roads) {
            r->setSignal(c);
            cout << "[SIGNAL] Road "
                 << dirToStr(r->getDir())
                 << " -> " << colorToStr(c) << endl;
        }
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
    virtual int greenTime() = 0;
    virtual int yellowTime() { return 5; }
    virtual ~TimeStrategy() = default;
};

class FixedTime : public TimeStrategy {
public:
    int greenTime() override { return 10; }
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

    mutex mtx;
    condition_variable cv;

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
        {
            lock_guard<mutex> lock(mtx);
            emergency = true;
            emergencyDir = d;
        }
        cout << "\n🚨 EMERGENCY detected at "
             << dirToStr(d) << " 🚨\n";
        cv.notify_one(); // 🔥 immediate preemption
    }
    void resetFSM(Direction greenDir) {
        static RedState red;
        state = &red;

        if (greenDir == Direction::NORTH || greenDir == Direction::SOUTH) {
            active = &intersection->NS();
            inactive = &intersection->EW();
        } else {
            active = &intersection->EW();
            inactive = &intersection->NS();
        }
    }
    void handleEmergency() {
        unique_lock<mutex> lock(mtx);

        cout << "[EMERGENCY] All signals RED\n";
        intersection->EW().setColor(Color::RED);
        intersection->NS().setColor(Color::RED);

        Direction greenDir;

        if (emergencyDir == Direction::EAST || emergencyDir == Direction::WEST) {
            cout << "[EMERGENCY] GREEN for EAST-WEST\n";
            intersection->EW().setColor(Color::GREEN);
            greenDir = emergencyDir;
        } else {
            cout << "[EMERGENCY] GREEN for NORTH-SOUTH\n";
            intersection->NS().setColor(Color::GREEN);
            greenDir = emergencyDir;
        }

        lock.unlock();
        this_thread::sleep_for(5s);

        lock.lock();
        cout << "[EMERGENCY] Cleared\n";

        // 🔥 RESET THE FSM PROPERLY
        resetFSM(greenDir);

        emergency = false;
        cv.notify_one();
    }

    void run() {
        active = &intersection->NS();
        inactive = &intersection->EW();

        cout << "🚦 Traffic Controller Started 🚦\n";

        while (true) {
            unique_lock<mutex> lock(mtx);

            if (emergency) {
                lock.unlock();
                handleEmergency();
                continue;
            }

            cout << "\n=== NEW SIGNAL CYCLE ===\n";

            // GREEN
            state = state->next();
            cout << "[STATE] GREEN\n";
            active->setColor(state->color());
            inactive->setColor(Color::RED);

            if (cv.wait_for(lock, chrono::seconds(strategy->greenTime()),
                            [&]() { return emergency.load(); }))
                continue;

            // YELLOW
            state = state->next();
            cout << "[STATE] YELLOW\n";
            active->setColor(state->color());

            if (cv.wait_for(lock, chrono::seconds(strategy->yellowTime()),
                            [&]() { return emergency.load(); }))
                continue;

            // RED
            state = state->next();
            cout << "[STATE] RED\n";
            active->setColor(state->color());

            swap(active, inactive);
        }
    }
};

/* ================= EMERGENCY SENSOR ================= */

class EmergencySensor {
    TrafficController* controller;
public:
    EmergencySensor(TrafficController* c) : controller(c) {}
    void sense(Vehicle v, Direction d) {
        if (v == Vehicle::AMBULANCE || v == Vehicle::FIRE_TRUCK)
            controller->triggerEmergency(d);
    }
};

/* ================= MAIN ================= */

int main() {
    Road east(Direction::EAST), west(Direction::WEST);
    Road north(Direction::NORTH), south(Direction::SOUTH);

    Intersection intersection;
    intersection.addRoad(&east);
    intersection.addRoad(&west);
    intersection.addRoad(&north);
    intersection.addRoad(&south);

    TrafficController& controller = TrafficController::instance();
    controller.setIntersection(&intersection);

    FixedTime fixed;
    controller.setStrategy(&fixed);

    EmergencySensor sensor(&controller);

    thread emergencyThread([&]() {
        this_thread::sleep_for(15s);
        sensor.sense(Vehicle::AMBULANCE, Direction::NORTH);
    });

    controller.run();
    emergencyThread.join();
}