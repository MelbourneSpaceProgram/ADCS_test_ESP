// I2C device class (I2Cdev) demonstration Arduino sketch for MPU9150
// 1/4/2013 original by Jeff Rowberg <jeff@rowberg.net> at https://github.com/jrowberg/i2cdevlib
//          modified by Aaron Weiss <aaron@sparkfun.com>
//
// Changelog:
//     2011-10-07 - initial release
//     2013-1-4 - added raw magnetometer output

/* ============================================
I2Cdev device library code is placed under the MIT license

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"

// I2Cdev and MPU9150 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "BdotController.h"
#include "Torquer.h"

bool blinkState = false;
const static uint32_t MAGTASK_STACK_SIZE = 25000;

// Super secret wifi credentials
const char* ssid = "ACRUX-0";
const char* password = "ultra_secure";

// // Set web server port number to 80
// WiFiServer server(80);
BDotController magwatcher;
Torquer x_rod(27, 26, 25, 0);
Torquer y_rod(35, 33, 32, 1);


void setup() {
	uint8_t status = 0;
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    Serial.begin(115200);


    // Initialize the magnetometer viewer
    magwatcher.init();

	status = magwatcher.hardwareCheck();
	if (status != 0){
		if (status&0x01) Serial.println("IMU connection failure");
		if (status&0x02) Serial.println("magnetometer connection failure");
	}


    // configure LED pin for output
    pinMode(LED_BUILTIN, OUTPUT);

	// Call torquer initialisation routines (these set up the pins and
	// configure PWM channels)
    x_rod.init();
    y_rod.init();

	// 	// Sets maximum allowable PWM duty cycle
    x_rod.set_max_power(0.5);
    y_rod.set_max_power(0.5);
}

void loop() {
	magwatcher.poll_magnetometer();

	vector3_f B;
	char buffer[128];

	magwatcher.get_avg_B(B);
	B.str(buffer, 128, "%4.3f %4.3f %4.3f ");
	Serial.println(buffer);

	// x_rod.actuate(-dmx_dt);
	// y_rod.actuate(-dmy_dt);

	digitalWrite(LED_BUILTIN, blinkState);
	blinkState=!blinkState;
	delay(10);

}