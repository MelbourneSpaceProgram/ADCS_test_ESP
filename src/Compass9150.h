#include "MPU9150_fixed.h"

// Maximum and minimum field readings
#define AK8975_H_OL_POS 4095
#define AK8975_H_OL_NEG -4096


// struct MSPvector3{
//     int16_t x;
//     int16_t y;
//     int16_t z;

//     MSPvector3 operator=(int16_t a){
//         x=a;
//         y=a;
//         z=a;
//     }

//     MSPvector3 operator*=(int16_t a){
//         x*=a;
//         y*=a;
//         z*=a;
//     }

// }


class Compass9150 : public MPU9150 {
public:
    Compass9150(uint8_t address) : MPU9150(address){
        state=0x00;
    }
    Compass9150() : MPU9150(){
        state=0x00;
    }
    void calibrate();
    bool isCalibrated(){
        return state&CAL_BIT;
    }
    uint8_t get_mag(int16_t* mx,int16_t* my,int16_t* mz);
private:
    uint8_t state;
    int16_t mx_offset=0;
    int16_t my_offset=0;
    int16_t mz_offset=0;

    const static uint8_t CAL_BIT = 0x01;
    const static uint8_t CAL_RUNNING_BIT = 0x02;
};