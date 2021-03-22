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
#include "I2Cdev.h"
#include "BdotController.h"
#include "Torquer.h"
#include "telemetry.hpp"

#include <Arduino.h>


bool blinkState = false;
// const static uint32_t MAGTASK_STACK_SIZE = 25000;

BDotController magwatcher;
Torquer x_rod(27, 26, 25, 0);
Torquer y_rod(35, 33, 32, 1);

// Container for readings, to be sent as 
const int capacity=JSON_OBJECT_SIZE(3);
StaticJsonDocument<capacity> Hdot;


void setup() {
	uint8_t status = 0;
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    Serial.begin(115200);

	// For testing:
	telemetry::init();


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

const uint16_t B_BUFFER = 100;
vector3_f B_storage[B_BUFFER];
char sendbuf[B_BUFFER*100];

/** Constructs a string holding the JSON-ified B data
 * @param s a character buffer of length at least n
 * @param n length of the buffer
 */
void B_as_json(char s[], uint32_t n){

}

void loop() {
	static vector3_f B;

	magwatcher.poll_magnetometer();

	char buffer[128];

	magwatcher.get_Bdot(B);
	B.str(buffer, 128, "%4.3f %4.3f %4.3f ");
	Serial.println(buffer);

	// Normalise so that dx/dt and dy/dt are at most 1
	float biggest = max(abs(B.x), abs(B.y));
	B /= biggest;

	x_rod.actuate(B.x);
	y_rod.actuate(B.x);

	B_as_json(sendbuf, B_BUFFER*10);
	telemetry::events.send(,"gyro_readings",millis());
    

	digitalWrite(LED_BUILTIN, blinkState);
	blinkState=!blinkState;
	delay(10);
	B_old = B;

}