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
// #include "helper_3dmath.h"
#include "Torquer.h"


#include "BluetoothSerial.h"


BluetoothSerial ESP_BT; // 

Torquer x_rod(27, 26, 25, 0);
Torquer y_rod(35, 33, 32, 1);

BDotController ctrl;



#define BUFFER_LEN 10

#define LED_PIN 2
bool blinkState = false;


void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    ESP_BT.begin("ESP32"); //Name of your Bluetooth Signal
    // Serial.begin(115200);
    
    // Constant offsets are not relevant for dB/dt
    //
    // Serial.println("Calibrating magnetometer...");
    // imu.calibrate();
    // Serial.println( (imu.isCalibrated()) ? "Success" : "Failure");
    // initialize the controller
    ctrl.init();


    // configure LED pin for output
    pinMode(LED_PIN, OUTPUT);

		// Call torquer initialisation routines (these set up the pins and
		// configure PWM channels)
    x_rod.init();
    y_rod.init();

		// Sets maximum allowable PWM duty cycle
		// TODO: Reconfigure this to be a percentage
    x_rod.set_max_power(128);
    y_rod.set_max_power(128);
    

}

// Parametrises numerical low pass filter
// Dimensionless - this only depends on the nth data point in history
float lambda = 0.5;

void loop() {
    unsigned long t = micros();
    // read raw accel/gyro/mag measurements from device
    uint8_t status = imu.getMag(mx, my, mz);
    // compute some averages
    float avg_mx = averageB(mx);
    float avg_my = averageB(my);
    float avg_mz = averageB(mz);

    float dmx_dt = (avg_mx - old_mx) / (t - old_t);
    float dmy_dt = (avg_my - old_my) / (t - old_t);
    float dmz_dt = (avg_mz - old_mz) / (t - old_t);
    
    if (ESP_BT.available() > 0){
        lambda = ESP_BT.parseFloat();
        ESP_BT.print("Set decay constant to ");
        ESP_BT.println(lambda);
    }
    // one = 0.3 microTesla


		// Rescale the control signals so that the larger one is always plus/minus one
    float biggest = max(abs(dmx_dt), abs(dmy_dt));
    dmx_dt /= biggest;
    dmy_dt /= biggest;


    x_rod.actuate(-dmx_dt);
    y_rod.actuate(-dmy_dt);


    ESP_BT.print(avg_mx); ESP_BT.print("\t");
    ESP_BT.print(avg_my); ESP_BT.print("\t");
    ESP_BT.print(avg_mz); ESP_BT.print("\t");

    ESP_BT.print(dmx_dt); ESP_BT.print("\t");
    ESP_BT.print(dmy_dt); ESP_BT.print("\t");
    ESP_BT.print(dmz_dt); ESP_BT.print("\t");
    ESP_BT.println(status);

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
    delay(50);

    if (++idx == BUFFER_LEN){
        idx = 0;
    }

    old_mx = avg_mx;
    old_my = avg_my;
    old_mz = avg_mz;
    old_t = t;
}
