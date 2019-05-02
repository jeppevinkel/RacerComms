#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include <Bits.h>
#include <algorithm>
#include <cassert>
//#include <windows.h>
//#include "MainWindows.h"

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

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

void getSerialStuff(SerialPort &serial, string &stringToAppend) {
	while (true)
	{
		unsigned char outPut = readSerial(serial);
		if ((int)outPut != 204)
		{
			stringToAppend += to_string((int)outPut);
		}
	}
}

int main(int argc, char **args)
{
	string input;
	SerialPort serial;
	serial.open("COM3", SerialPort::Baud9600, SerialPort::Data8, SerialPort::None, SerialPort::Stop2_0);

	int speed = 20;

	string rxString = "";

	// Flush
	serial.clean();

	char c[128];

	//
	std::thread t1(getSerialStuff, std::ref(serial), std::ref(rxString));

	while (true) {
		//cout << "Please enter command:\n";
		////cin >> input;
		////cin.getline(input, sizeof(input));
		//vector<string> inArgs(2);
		//getline(cin, input);

		//int i = 0;
		//stringstream ssin(input);
		//while (ssin.good() && i < 4) {
		//	ssin >> inArgs[i];
		//	++i;
		//}

		//bool startVal = false;
		//bool stopThis = false;

		//switch(str2int(inArgs[0].c_str())){
		//case str2int("stop"):
		//	c[0] = 0;
		//	serial.write(c, 1);
		//	break;
		//case str2int("start"):
		//	c[0] = speed;
		//	serial.write(c, 1);
		//	break;
		//case str2int("speed"):
		//	c[0] = clamp(stoi(inArgs[1]), 0, 255);
		//	speed = clamp(stoi(inArgs[1]), 0, 255);
		//	serial.write(c, 1);
		//	break;

		//default:
		//	cout << "Invalid command. Please try again!\n";
		//	break;
		//}
		if (!rxString.empty())
		{
			cout << rxString << endl;
			rxString.clear();
		}
	}

	serial.close();
}