#ifndef LIGHT_H
#define TRAFIIC_LIGHT_H

#include "light.h"

class TrafficLight {
public:
	TrafficLight(int address, bool debug = false);
	// Methods for controlling the traffic light
	void turnRed();
	void turnYellow();
	void turnGreen();
	void flash(int rate);
	void cycle();
	int getCurrentColor();
private:
	bool debug = false;
	Light redLight;
	Light yellowLight;
	Light greenLight;
	int currentColor;
};
#endif