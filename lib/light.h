#ifndef LIGHT_H
#define LIGHT_H

#include <thread>
#include <map>

class Light
{
public:
	enum Color
	{
		RED = 0,
		YELLOW = 1,
		GREEN = 2,
		OTHER = 3
	};
	std::map<Color, std::string> colorMap = {
		{Color::RED, "RED"},
		{Color::YELLOW, "YELLOW"},
		{Color::GREEN, "GREEN"},
		{Color::OTHER, "OTHER"}};
	enum State
	{
		ON = 0,
		OFF = 1,
		BLINKING = 2
	};
	std::map<State, std::string> stateMap = {
		{State::ON, "ON"},
		{State::OFF, "OFF"},
		{State::BLINKING, "BLINKING"}};

	Light(Color color, int address, bool debug = false);
	void controlState(State state);
	Color getColor() const;
	int getAddress() const;
	State getState() const;

private:
	Color color_;
	State state_;
	bool isOn_;
	bool isBlinking_;
	int address_;
	bool debug_;
	std::thread blinkingThread_;
	void turnOn();
	void turnOff();
	void startBlinking();
	void stopBlinking();
	void blinkingThread();
};
#endif