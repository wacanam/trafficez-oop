// trafficez-oop.cpp : Defines the entry point for the application.
//
#include "lib/traffic_light.h"
#include "lib/global.h"

std::string host = "localhost";
std::string command = "relay on 1\n";
int port = 23;

void initializeTelnetClient(const std::string &host, int port)
{
	telnet_client = new TelnetClient(host, port);
	if (telnet_client == nullptr)
	{
		std::cerr << "Error creating TelnetClient" << std::endl;
		return;
	}
	if (telnet_client->connectClient())
	{
		telnet_client->sendCommand(command, [](const std::string &response)
								   { std::cout << "Server response: " << response << std::endl; });
	}
	delete telnet_client;
	telnet_client = nullptr;
}

int main(int argc, const char *argv[])
{
	// Run the TelnetClient initialization in a separate thread
	std::thread telnetThread(initializeTelnetClient, host, port);
	telnetThread.detach(); // Detach the thread to allow it to run independently

	TrafficLight trafficLight(1, 60, true);
	trafficLight.terminate();
	trafficLight.cycle();

	// keep it running because we are using threads until we press a key
	std::cin.get();
}
