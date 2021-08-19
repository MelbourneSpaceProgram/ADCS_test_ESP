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
#include <Arduino.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "BdotController.h"
#include "Torquer.h"
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run 'make menuconfig' to and enable it
#endif

BDotController magwatcher;
Torquer x_rod(27, 26, 25, 0);
Torquer y_rod(35, 33, 32, 1);

BluetoothSerial SerialBT;

bool blinkState = true;

void setup() {
	uint8_t status = 0;
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

	// Initialize Bluetooth
	Serial.begin(115200);
	SerialBT.begin("ESP32");
	Serial.println("Bluetooth Enabled! Ready to pair...");

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

	// Sets maximum allowable PWM duty cycle
    x_rod.set_max_power(0.5);
    y_rod.set_max_power(0.5);

	// Mode selection information
	SerialBT.println("Please enter a mode, or write \"help\" for options: ");
}

const uint BUFLEN = 128;
char B_buffer[BUFLEN];
char big_buffer[BUFLEN*2];

String cmd = "none";

void loop() {

	// Read command
	if (SerialBT.available() > 0) {
		cmd = SerialBT.readString();
		cmd.trim();
		Serial.println(cmd);
	}

	if (cmd == "help") {
		SerialBT.println("1. test-detumble");
		cmd = "none";
	}

	// Determine Mode
	else if (cmd == "test-detumble") {
		static vector3_f B;

		magwatcher.poll_magnetometer();

		magwatcher.get_Bdot(B);
		B.str(B_buffer, BUFLEN, "%4.3f %4.3f %4.3f ");
		SerialBT.println(B_buffer);

		// Normalise so that dx/dt and dy/dt are at most 1
		float biggest = max(abs(B.x), abs(B.y));
		B /= biggest;

		x_rod.actuate(B.x);
		y_rod.actuate(B.x);

		magwatcher.get_raw_B(B);
		B.str(B_buffer, BUFLEN, "%2.2f\t%2.2f\t%2.2f");
		snprintf(big_buffer, BUFLEN,"%5d\t", (int) millis());
		strncat(big_buffer, B_buffer, BUFLEN);
	}

	digitalWrite(LED_BUILTIN, blinkState);
	blinkState=!blinkState;
	delay(1000);
}