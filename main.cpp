// trafficez-oop.cpp : Defines the entry point for the application.
//
#include "lib/traffic_light.h"
#include "lib/telnet_client.hpp"

std::string host = "localhost";
std::string command = "relay on 1";
int port = 23;

int main()
{

	// TrafficLight trafficLight(1, true);
	// trafficLight.flash(2);
	// trafficLight.cycle();

	TelnetClient client(host, port);

	if (client.connectClient())
	{
		client.sendCommand(command, [](const std::string &response)
						   { std::cout << "Server response: " << response << std::endl; });
	}

	return 0;
}
