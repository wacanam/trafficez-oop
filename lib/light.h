#ifndef LIGHT_H
#define LIGHT_H

class Light {
public:
	Light(int address, bool debug = false);
	void turnOn();
    void turnOff();
	void flash(int rate);
	bool getStatus() const;
private:
	bool debug = false;
	bool state = false;
	int address = 0;
	int rate = 0;
};
#endif