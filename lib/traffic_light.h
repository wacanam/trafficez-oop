#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include "light.h"
#include <thread>
#include <chrono>
#include <vector>

class TrafficLight
{
public:
    TrafficLight(int address, int cycleTime, bool debug = false);
    void cycle();
    void terminate();
    void go(int);
    void stop(int);
    void prepareToStop(int);

private:
    int address_;
    int cycleTime_;
    bool isRunning_;
    bool isBlinking_;
    std::thread runningThread_;
    Light redLight_;
    Light greenLight_;
    Light yellowLight_;
    void runningThread();
};

#endif // TRAFFIC_LIGHT_H