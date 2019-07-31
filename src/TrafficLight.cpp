#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // wait, using a unique_lock, until the queue is not empty
    std::unique_lock<std::mutex> lock(_mtx);
    _condition.wait(lock, [this] { return !_queue.empty(); });

    // return the first element of the queue (FIFO)
    T msg = std::move(_queue.front());
    _queue.pop_front();

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // use a lock_guard to store elements in the back of the queue
    std::lock_guard<std::mutex> lock(_mtx);
    _queue.emplace_back(msg);

    // notify that the message has been inserted
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

TrafficLight::~TrafficLight()
{

}


void TrafficLight::waitForGreen()
{
    // keep on reading from the message queue until a green signal is read
    while (true) {
        if (_msg.receive() == TrafficLightPhase::green) {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::setCurrentPhase(TrafficLightPhase val)
{
    _currentPhase = val;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // create a random number between 4000 and 6000
    std::default_random_engine rnd;
    rnd.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    double cycleDuration = (rnd() % 2001) + 4000; // duration of a single simulation cycle in ms
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // init stop watch
    lastUpdate = std::chrono::system_clock::now();
    while (true) {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= cycleDuration) {
            getCurrentPhase() == TrafficLightPhase::red ? setCurrentPhase(TrafficLightPhase::green) :
                                                          setCurrentPhase(TrafficLightPhase::red);

            TrafficLightPhase phase = getCurrentPhase();
            _msg.send(std::move(phase));

            // reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();
        }
    }
}