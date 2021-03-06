/********************************************************************************
 * Copyright 2009 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
 * Faculty of Engineering, University of Southern Denmark
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ********************************************************************************/

#include "SerialPort.hpp"


#if !(defined __MINGW32__) && !(defined _WIN32)
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#endif
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#include <time.h>
#endif

#if (defined _WIN32) || (defined __CYGWIN__)
#include "WinSerialPort.cppx"
#else
#include "LinuxSerialPort.cppx"
#endif

namespace {
long long currentTimeMs()
{
#ifdef _WIN32
    return (long long) (clock()*((double)1e3/(double)CLOCKS_PER_SEC));
#else
    timeval current;

    gettimeofday(&current, 0);
    return (long long)current.tv_sec * 1000L + current.tv_usec / 1000L;

#endif
}

void sleepMs(int period){
#ifdef _WIN32
    Sleep(period);
#else
    usleep(1000 * period);
#endif
}

}

bool SerialPort::read(
    unsigned char* buf,
    const unsigned int n,
    const unsigned int timeout,
    const unsigned int sInterval)
{
    unsigned int index = 0;

    const unsigned long time =
        currentTimeMs() + timeout;

    unsigned long currTime = time;
    do {
        index += read( &(buf[index]), n-index );
        if(index >= n){
            return true;
        }
        sleepMs(sInterval);
        currTime = currentTimeMs();
    } while( currTime < time );
    return false;
}

void SerialPort::debugPrint(const char* str, const unsigned char* buf, int n) {
	//printf ("%s ", str);

	//for(unsigned int i=0; i<n; ++i) {
	//	printf ("%x ", buf[i]&0xff);
	//}
	//printf ("\n");
}
