// trafficez-oop.cpp : Defines the entry point for the application.
//
#include "lib/traffic_light.h"
#include "lib/global.h"

std::string host = "192.168.1.5";
std::string command = "reset";
int port = 23;
std::string username = "ezadmin";
std::string password = "ez@dmin";


void initializeTelnetClient(const std::string &host, int port)
{
	// TelnetClient numatoRelay(host, port);
	// numatoRelay.connectClient(username, password);
	// numatoRelay.sendCommand("relay writeall ffff");
	// numatoRelay.sendCommand("reset");
	
	numatoRelay = new TelnetClient("192.168.1.5", 23);

	if (numatoRelay == nullptr)
	{
		std::cerr << "Error creating TelnetClient" << std::endl;
		return;
	}
	numatoRelay->connectClient("ezadmin", "ez@dmin");
	
	// , [](const std::string &response)
	// 					
}

int main(int argc, const char *argv[])
{
	// Run the TelnetClient initialization in a separate thread
	std::thread telnetThread(initializeTelnetClient, host, port);
	telnetThread.detach(); // Detach the thread to allow it to run independently

	TrafficLight trafficLight1(1, 60, true);
	TrafficLight trafficLight2(2, 60, true);
	trafficLight1.terminate();
	trafficLight2.terminate();
	trafficLight1.cycle();
	trafficLight2.cycle();

	// keep it running because we are using threads until we press a key
	std::cin.get();
}
