#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include <Bits.h>
#include <algorithm>
#include <fstream>
#include <ctime>

#include "SerialPort.hpp"

using namespace std;
using namespace rwhw;

constexpr unsigned int str2int(const char* str, int h = 0)
{
	return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

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

const char* hex2byte(const char* buf)
{
	char bufHex[2];

	for (int j = 0; j < 2; j++)
		sprintf(bufHex, "%X", buf[j]);

	return bufHex;
}

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
	int mode = -1;
	std::cout << "Please select mode.\nType 1 to send commands.\nType 2 to receive information." << std::endl;
	std::cin >> mode;

	std::string comPort = "";
	std::cout << "Please select COM port.\nJust type the number of the port you use!" << std::endl;
	std::cin >> comPort;

	time_t t = time(0);   // get time now
	struct tm* now = localtime(&t);

	char dt[80];
	strftime(dt, 80, "Sample-%b%d_kl_%H%M.csv", now);

	std::string input;
	SerialPort serial;
	serial.open("COM" + comPort, SerialPort::Baud9600, SerialPort::Data8, SerialPort::None, SerialPort::Stop2_0);

	std::ofstream myFile;
	myFile.open(std::string(dt));

	int speed = 20;

	std::string rxString = "";

	// Flush
	serial.clean();

	unsigned char c[128];

	//
	std::thread t1(getSerialStuff, std::ref(serial), std::ref(rxString));

	while (true) {
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

			switch(str2int(inArgs[0].c_str())){
			case str2int("stop"):
				c[0] = 0;
				serial.write(c, 1);
				break;
			case str2int("start"):
				c[0] = speed;
				serial.write(c, 1);
				break;
			case str2int("speed"):
				c[0] = std::clamp(stoi(inArgs[1]), 0, 255);
				speed = std::clamp(stoi(inArgs[1]), 0, 255);
				serial.write(c, 1);
				break;

			default:
				std::cout << "Invalid command. Please try again!\n";
				break;
			}
		}
		else {
			if (!rxString.empty())
			{
				std::cout << rxString << std::endl;
				myFile << rxString << std::endl;
				rxString.clear();
			}
		}
	}

	serial.close();
}