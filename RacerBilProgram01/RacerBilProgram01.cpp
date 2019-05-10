#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <ctime>

#include "SerialPort.hpp"

using namespace rwhw;

// Function to for an integer based on the ascii values of the characters in the string
constexpr unsigned int str2int(const char* str, int h = 0)
{
	return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

// Returns the read from the serial
char readSerial(SerialPort & serial) {
#ifdef _WIN32
	Sleep(100);
#else
	sleep(0.1);
#endif
	unsigned char buf[1];
	serial.read(buf, 1);
	return buf[0];
}

// Saves the received data from serial to a string
void getSerialStuff(SerialPort &serial, std::string &stringToAppend) {
	while (true)
	{
		unsigned char outPut = readSerial(serial);
		if ((int)outPut != 204)
		{
			stringToAppend += std::to_string((int)outPut);
		}
	}
}

int main(int argc, char **args)
{
	//Enter the mode.
	int mode = -1;
	std::cout << "Please select mode.\nType 1 to send commands.\nType 2 to receive information." << std::endl;
	std::cin >> mode;

	//Input the desired COM port.
	std::string comPort = "";
	std::cout << "Please select COM port.\nJust type the number of the port you use!" << std::endl;
	std::cin >> comPort;

	time_t t = time(0);   // get time now
	struct tm* now = localtime(&t);

	char dt[80];
	strftime(dt, 80, "Sample-%b%d_kl_%H%M.csv", now);

	//Opens the COM port that was entered.
	std::string input;
	SerialPort serial;
	serial.open("COM" + comPort, SerialPort::Baud9600, SerialPort::Data8, SerialPort::None, SerialPort::Stop2_0);

	//Opens a file, and names it after the time.
	std::ofstream myFile;
	myFile.open(std::string(dt));

	//Default speed at the beginning of the program, mode 1.
	int speed = 20;

	//The string where the data from the car is stored and outputted. For mode 1.
	std::string rxString = "";

	// Flush
	serial.clean();

	//Variable for storing serial output
	unsigned char c[128];

	//Extra thread that repetedly check for, and saves serial input
	std::thread t1(getSerialStuff, std::ref(serial), std::ref(rxString));

	//A loop that continuously run the program
	while (true) {
		//An if statement that selects which one of the two modes are run
		if (mode == 1) {
			std::cout << "Please enter command:\n";
			std::vector<std::string> inArgs(2);
			std::getline(std::cin, input);

			int i = 0;
			std::stringstream ssin(input);
			while (ssin.good() && i < 4) {
				ssin >> inArgs[i];
				++i;
			}

			bool startVal = false;
			bool stopThis = false;

			//Switches between the options based on input
			switch(str2int(inArgs[0].c_str())){
			case str2int("stop"): // Sends command to set the speed to 0
				c[0] = 0;
				serial.write(c, 1);
				break;
			case str2int("start"): // Sends command to start car with latest saved speed
				c[0] = speed;
				serial.write(c, 1);
				break;
			case str2int("speed"): // Sends command to set speed to the value of the 2nd input
				c[0] = std::clamp(stoi(inArgs[1]), 0, 255);
				speed = std::clamp(stoi(inArgs[1]), 0, 255);
				serial.write(c, 1);
				break;

			default:
				std::cout << "Invalid command. Please try again!\n";
				break;
			}
		}
		//Mode 2 is just in an else statement, so anything other than 1 runs this mode.
		else {
			if (!rxString.empty())
			{
				std::cout << rxString << std::endl; // Writes serial input to console
				myFile << rxString << std::endl; // Writes serial input to file
				rxString.clear();
			}
		}
	}
	serial.close();
}