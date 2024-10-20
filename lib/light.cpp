#include "light.h"
#include "utils.h"
#include <iostream>
#include <fmt/format.h>

Light::Light(Color color, int address, bool debug) : color_(color), state_(State::OFF), isBlinking_(false), address_(address), debug_(debug) {}

void Light::turnOn()
{
    isOn_ = true;
    if (debug_)
    {
        std::string message = fmt::format("{} light is on", colorMap[color_]);
        std::cout << "\r\033[K" << message << std::endl
                  << std::flush;
    }
}

void Light::turnOff()
{
    isOn_ = false;
    stopBlinking();
}

void Light::startBlinking()
{
    if (isBlinking_)
    {
        return;
    }
    isBlinking_ = true;
    blinkingThread_ = std::thread(&Light::blinkingThread, this);
    blinkingThread_.detach();
}

void Light::stopBlinking()
{
    isBlinking_ = false;
    if (blinkingThread_.joinable())
    {
        blinkingThread_.join();
    }
}

Light::Color Light::getColor() const
{
    return color_;
}

int Light::getAddress() const
{
    return address_;
}

Light::State Light::getState() const
{
    return state_;
}

void Light::controlState(State state)
{
    if (debug_)
    {
        std::string message = fmt::format("Controlling light: color={}, state={}", colorMap[color_], stateMap[state]);
        std::cout << "\r\033[K" << message << std::flush;
    }

    if (state == State::ON)
    {
        turnOn();
    }
    else if (state == State::OFF)
    {
        turnOff();
    }
    else if (state == State::BLINKING)
    {
        startBlinking();
    }
}

void Light::blinkingThread()
{
    while (isBlinking_)
    {
        isOn_ = !isOn_;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (debug_)
        {

            std::string message = fmt::format("{} light is {}", colorMap[color_], isOn_ ? "on" : "off");
            std::cout << "\r\033[K" << message << std::flush;
        }
    }
}
