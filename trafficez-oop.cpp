// trafficez-oop.cpp : Defines the entry point for the application.
//

#include "trafficez-oop.h"
#include "lib/traffic_light.h"
using namespace std;

int main()
{

	TrafficLight trafficLight(1, true);
	trafficLight.flash(2);
	trafficLight.cycle();

	return 0;
}
