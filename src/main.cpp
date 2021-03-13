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
#include "Compass9150.h"
// #include "helper_3dmath.h"
#include "Torquer.h"


#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
Compass9150 imu;

BluetoothSerial ESP_BT; // 

Torquer x_rod(27, 26, 25, 0);
Torquer y_rod(35, 33, 32, 1);

// #define ESP_BT Serial

#define BUFFER_LEN 10

#define LED_PIN 2
bool blinkState = false;

int16_t mx_avg, my_avg, mz_avg;
int16_t mx[BUFFER_LEN];
int16_t my[BUFFER_LEN];
int16_t mz[BUFFER_LEN];
uint16_t idx = 0;

// Shift left function for array
void shiftRight(int16_t (&array)[BUFFER_LEN]) {
    for(int i=BUFFER_LEN-1; i>0; i--){
        array[i] = array[i-1];
    }
}

/* Calculates a weighted average of B
*/
float averageB(int16_t (&array)[BUFFER_LEN], float lambda){
    float sum = 0;
    // float norm = 0;
    for(int i=0; i<BUFFER_LEN; i++){
        sum += exp(-i*lambda)*array[i];
        // norm += exp(-i*lambda);
    }
    float output = sum * (1-exp(-lambda))/(1-exp(-lambda*BUFFER_LEN));
    // float output = sum / norm;
    return output;
}

unsigned long old_t;

float old_mx=0;
float old_my=0;
float old_mz=0;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    ESP_BT.begin("ESP32"); //Name of your Bluetooth Signal
    // Serial.begin(115200);

    // initialize device
    // Serial.println("here...");
    ESP_BT.println("Initializing I2C devices...");
    imu.initialize();

    // verify connection
    ESP_BT.println("Testing device connections...");
    ESP_BT.println(imu.testConnection() ? "MPU9150 connection successful" : "MPU9150 connection failed");

    // Constant offsets are not relevant for dB/dt
    //
    // Serial.println("Calibrating magnetometer...");
    // imu.calibrate();
    // Serial.println( (imu.isCalibrated()) ? "Success" : "Failure");

    // configure LED pin for output
    pinMode(LED_PIN, OUTPUT);

    for (int i = BUFFER_LEN-1; i >=0; i--){
        imu.getMag(mx+i, my+i, mz+i);
        delay(100);
    }


		// Call torquer initialisation routines (these set up the pins and
		// configure PWM channels)
    x_rod.init();
    y_rod.init();

		// Sets maximum allowable PWM duty cycle
		// TODO: Reconfigure this to be a percentage
    x_rod.set_power_maximum(128);
    y_rod.set_power_maximum(128);
    
    Serial.println("rods initialised...");
    old_t = micros();
}

float decay = 0.5;

void loop() {
    // read raw accel/gyro/mag measurements from device
    shiftRight(mx);
    shiftRight(my);

    unsigned long t = micros();
    uint8_t status = imu.getMag(mx, my, mz);
    float avg_mx = averageB(mx, decay);
    float avg_my = averageB(my, decay);
    float avg_mz = averageB(mz, decay);

    float dmx_dt = (avg_mx - old_mx) / (t - old_t);
    float dmy_dt = (avg_my - old_my) / (t - old_t);
    float dmz_dt = (avg_mz - old_mz) / (t - old_t);
    
    if (ESP_BT.available() > 0){
        decay = ESP_BT.parseFloat();
        ESP_BT.print("Set decay constant to ");
        ESP_BT.println(decay);
    }
    // one = 0.3 microTesla


		// Rescale the control signals so that the larger one is always plus/minus one
    float biggest = max(abs(dmx_dt), abs(dmy_dt));
    if (biggest > 0){
      dmx_dt /= biggest;
      dmy_dt /= biggest;
    }
    


    x_rod.actuate(-dmx_dt);
    y_rod.actuate(-dmy_dt);


    ESP_BT.print(avg_mx); ESP_BT.print("\t");
    ESP_BT.print(avg_my); ESP_BT.print("\t");
    ESP_BT.print(avg_mz); ESP_BT.print("\t");

    ESP_BT.print(mx[0]); ESP_BT.print("\t");
    ESP_BT.print(my[0]); ESP_BT.print("\t");
    ESP_BT.print(mz[0]); ESP_BT.print("\t");
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
