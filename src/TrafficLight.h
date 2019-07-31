#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
    T receive();
    void send(T &&msg);

private:
    std::condition_variable _condition;
    std::mutex _mtx;
    std::deque<T> _queue;
};

typedef enum {
    red,
    green
} TrafficLightPhase;

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void simulate();
    void waitForGreen();


private:
    // typical behaviour methods
    void cycleThroughPhases();
    void setCurrentPhase(TrafficLightPhase val);

    MessageQueue<TrafficLightPhase> _msg;
    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif