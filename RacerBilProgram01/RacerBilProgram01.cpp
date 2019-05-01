#include <iostream>
#include <string>
#include <thread>
#include <windows.h>
#include "MainWindows.h"

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
	char buf[1];
	serial.read(buf, 1);
	return buf[0];
}

void getSerialStuff(SerialPort &serial, string &stringToAppend) {
	while (true)
	{
		char outPut = readSerial(serial);
		if (outPut != -52 && outPut != 10)
		{
			stringToAppend += outPut;
		}
	}
}

int main(int argc, char **args)
{
	/*char input[56];
	SerialPort serial;
	serial.open("COM4", SerialPort::Baud9600, SerialPort::Data8, SerialPort::None, SerialPort::Stop2_0);

	string rxString = "";

	// Flush
	serial.clean();

	char c[128];

	//
	std::thread t1(getSerialStuff, std::ref(serial), std::ref(rxString));
	
	MainWindow *myWindow = new MainWindow();

	/*while (true) {
		cout << "Please enter command:\n";
		cin >> input;

		bool startVal = false;
		bool stopThis = false;

		switch(str2int(input)){
		case str2int("slow"):
			c[0] = 'A';
			serial.write(c, 1);
			break;
		case str2int("fast"):
			c[0] = 'z';
			serial.write(c, 1);
			break;
		default:
			cout << "Invalid command. Please try again!\n";
			break;
		}
		/*if (!rxString.empty())
		{
			cout << stoi(rxString) << endl;
			rxString.clear();
		}
	}

	serial.close();*/
}