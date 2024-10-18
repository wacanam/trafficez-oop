#include "traffic_light.h"
#include <iostream>
#include <thread>
#include <future>

using namespace std;

enum COLOR
{
    RED = 1,
    YELLOW = 2,
    GREEN = 3
};

TrafficLight::TrafficLight(int address, bool debug) : redLight(RED, debug), yellowLight(YELLOW, debug), greenLight(GREEN, debug), currentColor(RED), debug(debug) {}

void TrafficLight::turnRed()
{
    redLight.turnOn();
    yellowLight.turnOff();
    greenLight.turnOff();
    currentColor = RED;
    if (debug)
    {
        cout << "Traffic light turned red." << endl;
    }
}

void TrafficLight::turnYellow()
{
    redLight.turnOff();
    yellowLight.turnOn();
    greenLight.turnOff();
    currentColor = YELLOW;
    if (debug)
    {
        cout << "Traffic light turned yellow." << endl;
    }
}

void TrafficLight::turnGreen()
{
    redLight.turnOff();
    yellowLight.turnOff();
    greenLight.turnOn();
    currentColor = GREEN;
    if (debug)
    {
        cout << "Traffic light turned green." << endl;
    }
}

void TrafficLight::flash(int rate)
{
    const int flashInterval = 1000 / rate; // Calculate flash interval in milliseconds
    const int numFlashes = 5;              // Adjust the number of flashes as needed

    std::future<void> future = std::async(std::launch::async, [this, flashInterval, numFlashes]()
                                          {
        for (int i = 0; i < numFlashes; ++i) {
            yellowLight.turnOn();
            std::this_thread::sleep_for(std::chrono::milliseconds(flashInterval));
            yellowLight.turnOff();
            std::this_thread::sleep_for(std::chrono::milliseconds(flashInterval));
        }

        if (debug) {
            std::cout << "Traffic light flashing at rate: " << std::endl;
        } });

    // Continue with other tasks while the flashing is in progress
    // ...

    // Wait for the flashing to finish if necessary
    future.wait();
}

int TrafficLight::getCurrentColor()
{
    return currentColor;
}

void TrafficLight::cycle()
{
    turnRed();
    // Adjust the timing as needed
    std::this_thread::sleep_for(std::chrono::seconds(10));

    turnYellow();
    std::this_thread::sleep_for(std::chrono::seconds(3));

    turnGreen();
    std::this_thread::sleep_for(std::chrono::seconds(15));
}
