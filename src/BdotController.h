#include "MPU9150_fixed.h"
#include "vectormath.h"
#include "Ticker.h"
#include "MPU9150_fixed.h"
#include "esp_timer.h"
#include <algorithm>

// lock to avoid multiple access
static portMUX_TYPE my_mutex;

// Maximum and minimum field readings
#define AK8975_H_OL_POS 4095
#define AK8975_H_OL_NEG -4096

typedef Vector3<int16_t> mag_t;
typedef Vector3<float> vector3_f;

class BDotController {
public:
    BDotController() {};
    void init(){
        pollCaller.attach_us(1000, poll_magnetometer);
        lambda=2;
        hist_idx=0;
        imu.initialize();
    }
    bool hardwareCheck(){
        // verify connection to components
        // TODO: expand this function
        return imu.testConnection();
    }
    void calc_average(vector3_f& B){
        float norm;
        vector3_f tmp;
        norm = 0.0f;
        B = 0.0f;
        int64_t curr_time = t_history[hist_idx];
        float dt;
        float weight;
        vTaskEnterCritical(&my_mutex);
        for (uint8_t n=0; n<BUFFER_LEN; n++){
            tmp = history[(hist_idx-n) % BUFFER_LEN];
            dt = t_history[(hist_idx-n) % BUFFER_LEN] - curr_time; // should be negative or zero
            weight = exp(dt/tau);
            tmp *= weight;
            B += tmp;

            norm += weight;
        }
        vTaskExitCritical(&my_mutex);
        B *= 1.0f/norm; // normalise
    }


private:
    static Ticker pollCaller;
    static void poll_magnetometer(){
        mag_t tmp;
        int64_t time_us = esp_timer_get_time();
        uint8_t status = imu.getMag(&(tmp.x), &(tmp.y), &(tmp.z));
        // ATOMIC BLOCK
        vTaskEnterCritical(&my_mutex);
        hist_idx = (hist_idx + 1)%BUFFER_LEN;
        history[hist_idx] = tmp;
        t_history[hist_idx] = time_us;
        vTaskExitCritical(&my_mutex);
    };

    // class default I2C address is 0x68
    // specific I2C addresses may be passed as a parameter here
    // AD0 low = 0x68 (default for InvenSense evaluation board)
    // AD0 high = 0x69
    static MPU9150 imu;

    static float tau; // time constant of filter, microseconds

    static Vector3<float> avg_m;


    static const int BUFFER_LEN = 128;
    static volatile mag_t history[BUFFER_LEN];
    static volatile int64_t t_history[BUFFER_LEN];
    static volatile uint8_t hist_idx; // points to a the index of the most recent valid read, avoiding the need for rippling
    // Storage format: [ 8 9 10 1 2 3 4 5 6 7 ]
    // i.e. n^th most recent record found at history[(hist_idx-n) % BUFFER_LEN], 
    static_assert(BUFFER_LEN < (uint8_t) 255, "buffer is too long for index datatype to store"); // needed to ensure we don't have weird overflows

    static void update_log();

};