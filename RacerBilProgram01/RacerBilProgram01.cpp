#include <iostream>
#include <string>

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
	string outPut = ".";
#ifdef _WIN32
	Sleep(100);
#else
	sleep(0.1);
#endif
	char buf[1];
	serial.read(buf, 1);
	/*while((int)outPut.back() != 45){
		if ((int)buf[0] != -52 && (int)buf[0] != 10)
		{
			outPut += ((int)buf[0]);
		}
	}
	return outPut;*/
	return buf[0];
}

int main(int argc, char **args)
{
	char input[56];
	SerialPort serial;
	serial.open("COM3", SerialPort::Baud9600);

	// Flush
	serial.clean();

	char c[128];

	while (true) {
		cout << "Please enter command:\n";
		cin >> input;

		bool startVal = false;
		bool stopThis = false;

		switch(str2int(input)){
		case str2int("start"):
			c[0] = 'A';
			serial.write(c, 1);
			while (!stopThis)
			{
				int outPut = (int)readSerial(serial);
				if (outPut != -52 && startVal == false)
				{
					startVal = true;
				}
				else if ((startVal && outPut == -52) || (startVal && outPut == 10))
				{
					stopThis = true;
				}
				if (outPut != -52 && outPut != 10)
				{
					cout << (char) outPut << endl;
				}
			}
			break;
		case str2int("stop"):
			c[0] = 'B';
			serial.write(c, 1);
			while (!stopThis)
			{
				int outPut = (int)readSerial(serial);
				if (outPut != -52 && startVal == false)
				{
					startVal = true;
				}
				else if ((startVal && outPut == -52) || (startVal && outPut == 10))
				{
					stopThis = true;
				}
				if (outPut != -52 && outPut != 10)
				{
					cout << (char)outPut << endl;
				}
			}
			break;
		default:
			cout << "Invalid command. Please try again!\n";
			break;
		}
	}

	serial.close();
}