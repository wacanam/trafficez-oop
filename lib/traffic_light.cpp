#include "traffic_light.h"
#include <iostream>
#include <fmt/format.h>

TrafficLight::TrafficLight(int address, int cycleTime, bool debug) : address_(address),
                                                                     cycleTime_(cycleTime),
                                                                     isRunning_(false),
                                                                     isBlinking_(false),
                                                                     redLight_(Light(Light::Color::RED, address + Light::Color::RED, debug)),
                                                                     greenLight_(Light(Light::Color::GREEN, address + Light::Color::GREEN, debug)),
                                                                     yellowLight_(Light(Light::Color::YELLOW, address + Light::Color::YELLOW, debug)) {};

void TrafficLight::cycle()
{
    if (isRunning_)
    {
        return;
    }
    isRunning_ = true;
    runningThread_ = std::thread(&TrafficLight::runningThread, this);
    runningThread_.detach();
}

void TrafficLight::go(int duration)
{
    // Set the current light to green
    fmt::print("\nGo for {} seconds\n", duration);
    redLight_.controlState(Light::State::OFF);
    yellowLight_.controlState(Light::State::OFF);
    greenLight_.controlState(Light::State::ON);
    std::this_thread::sleep_for(std::chrono::seconds(duration));
}

void TrafficLight::stop(int duration)
{
    // Set the current light to red
    fmt::print("\nStop for {} seconds\n", duration);
    greenLight_.controlState(Light::State::OFF);
    yellowLight_.controlState(Light::State::OFF);
    redLight_.controlState(Light::State::ON);
    std::this_thread::sleep_for(std::chrono::seconds(duration));
}

void TrafficLight::prepareToStop(int duration)
{
    // Set the current light to green and start blinking
    fmt::print("\nPrepare to stop for {} seconds\n", duration);
    redLight_.controlState(Light::State::OFF);
    yellowLight_.controlState(Light::State::OFF);
    greenLight_.controlState(Light::State::BLINKING);
    std::this_thread::sleep_for(std::chrono::seconds(duration));
    greenLight_.controlState(Light::State::OFF);
    yellowLight_.controlState(Light::State::ON);
    std::this_thread::sleep_for(std::chrono::seconds(duration));
}

void TrafficLight::terminate()
{
    isRunning_ = false;
    if (runningThread_.joinable())
    {
        runningThread_.join();
    }
    redLight_.controlState(Light::State::OFF);
    yellowLight_.controlState(Light::State::OFF);
    greenLight_.controlState(Light::State::OFF);
}

void TrafficLight::runningThread()
{
    while (isRunning_)
    {
        // start the go light
        go(10);
        // prepare to stop
        prepareToStop(3);
        // stop
        stop(10);
        // repeat
    }
}
