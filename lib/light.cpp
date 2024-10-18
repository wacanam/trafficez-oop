#include "light.h"
#include <iostream>

Light::Light(int address, bool debug) : address(address), debug(debug) {}

void Light::turnOn()
{
    state = true;
    if (debug)
    {
        std::cout << "Traffic light " << address << " turned on." << std::endl;
    }
}

void Light::turnOff()
{
    state = false;
    if (debug)
    {
        std::cout << "Traffic light " << address << " turned off." << std::endl;
    }
}

void Light::flash(int rate)
{
    this->rate = rate;
    // Implement flashing logic here
    if (debug)
    {
        std::cout << "Traffic light " << address << " flashing at rate: " << rate << std::endl;
    }
}

bool Light::getStatus() const
{
    return state;
}