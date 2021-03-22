#include "BdotController.h"
/** Initialization function
 * Connects to IMU and attempts to obtain compass sensitivity calibration
 * 
 */
void BDotController::init(){
    imu.initialize();
    tau=50000; // microseconds!
    hist_idx=0;
    //   retrieve compass sensitivity calibration
    delay(10);
    uint8_t sx, sy, sz;
    imu.getCompassSensitivity(&sx, &sy, &sz);
    sens_correction.x = (sx - 128)*0.5f/128 + 1;
    sens_correction.y = (sy - 128)*0.5f/128 + 1;
    sens_correction.z = (sz - 128)*0.5f/128 + 1;
}

/** Returns average B value based on stored history with an exponential weight
 * @param B Container for Bx By Bz values, stored as floats
 * @param N Number of history points to use
 */ 
void BDotController::get_avg_B(vector3_f& B, uint16_t N=BUFFER_LEN){
    assert(N <= BUFFER_LEN);
    float norm;
    vector3_f tmp;
    norm = 0.0f;
    B = 0.0f;
    float dt;
    float weight;
    // get a lock    
    // portMUX_TYPE MAGNETOMETER_MUTEX;
    // portENTER_CRITICAL(&MAGNETOMETER_MUTEX);
    int64_t curr_time = t_history[hist_idx];
    for (uint16_t n=0; n<N; n++){
        // cast away the voltaile because we are in an atomic block
        tmp = (mag_t&) history[(hist_idx+n) % BUFFER_LEN];
        dt = t_history[((unsigned)(hist_idx+n)) % BUFFER_LEN] - curr_time; // should be negative or zero
        weight = exp(dt/tau);
        tmp *= weight;
        B += tmp;

        norm += weight;
    }
    // portEXIT_CRITICAL(&MAGNETOMETER_MUTEX);
    assert(norm>1e-10);
    B *= 1.0f/norm; // normalise
}

/** Returns finite differenced dB/dt reading, in units of inverse microseconds
 * @param dBdt stores dBdt
 * @param N Number of history points to use
 * Note that the spooky summation stuff below came from rearranging B_1-B_0/t_1-t_0,
 * but it is done within a single atomic block for better speed
 */
void BDotController::get_Bdot(vector3_f& dBdt, uint16_t N){
    assert(N<= BUFFER_LEN-1);
    float norm, norm2, dt, weight;
    vector3_f tmp, B;
    int64_t curr_time, curr_time2;

    B=0;
    norm=0;
    norm2=0;
    // get a lock    
    // portMUX_TYPE MAGNETOMETER_MUTEX;
    // portENTER_CRITICAL(&MAGNETOMETER_MUTEX);
    

    // NOTE: there is room for optimisation of this code to only iterate over the array once, but I felt it
    // more important to write down a correct implementation than a fast one.

    // get a value for current average B
    curr_time = t_history[hist_idx];
    for (uint16_t n=0; n<N; n++){
        // cast away the voltaile because we are in an atomic block
        tmp = (mag_t&) history[(hist_idx+n) % BUFFER_LEN];
        dt = t_history[((unsigned)(hist_idx+n)) % BUFFER_LEN] - curr_time; // should be negative or zero
        weight = exp(dt/tau);
        tmp *= weight;
        dBdt += tmp;

        norm += weight;
    }

    // get a value for old average B
    curr_time2 = t_history[hist_idx+1];
    for (uint16_t n=1; n<N+1; n++){
        // cast away the voltaile because we are in an atomic block
        tmp = (mag_t&) history[(hist_idx+n) % BUFFER_LEN];
        dt = t_history[((unsigned)(hist_idx+n)) % BUFFER_LEN] - curr_time2; // should be negative or zero
        weight = exp(dt/tau);
        tmp *= weight;
        B += tmp;

        norm2 += weight;
    }

    // portEXIT_CRITICAL(&MAGNETOMETER_MUTEX);
    assert(norm>1e-10);
    dBdt /= norm;  // normalise
    B /= norm2;  // normalise
    dBdt -= B;
    dBdt /= (curr_time-curr_time2);
    
}

void BDotController::get_raw_B(vector3_f& B){
    // get a lock    
    // portMUX_TYPE MAGNETOMETER_MUTEX;
    // portENTER_CRITICAL(&MAGNETOMETER_MUTEX);
    B = history[hist_idx];
    // portEXIT_CRITICAL(&MAGNETOMETER_MUTEX);
}

/** Checks for connection and correct configuration of imu.
 * @return bit 0 flags imu result (0 if ok)
 * @return bit 1 flags magnetometer result (0 if ok)
 * 
 */
uint8_t BDotController::hardwareCheck(){
    // verify connection to components
    uint8_t r = 0x00;
    if (!imu.testConnection())      r |= 0x01;
    if (!imu.testMagnetometer())    r |= 0x02;
    return r;
}


void BDotController::poll_magnetometer(){
    mag_t Hreading;
    int64_t time_us = esp_timer_get_time();
    // uint8_t status = imu.getMag(&(tmp.x), &(tmp.y), &(tmp.z));
    int16_t tmp[3];
    imu.getMag(tmp);
    Hreading.x = tmp[0]*sens_correction.x;
    Hreading.y = tmp[1]*sens_correction.y;
    Hreading.z = tmp[2]*sens_correction.z;
    // ATOMIC BLOCK
    // portMUX_TYPE MAGNETOMETER_MUTEX;
    // portENTER_CRITICAL(&MAGNETOMETER_MUTEX);
    if (hist_idx ==0){
        hist_idx = BUFFER_LEN-1;
    } else {
        hist_idx--;
    }
    (mag_t&) history[hist_idx] = Hreading;
    t_history[hist_idx] = time_us;
    // portEXIT_CRITICAL(&MAGNETOMETER_MUTEX);
};