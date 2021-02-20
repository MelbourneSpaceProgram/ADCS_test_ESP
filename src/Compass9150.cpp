#include "Compass9150.h"

/** Calibrates compass magnetometer for stray fields
 */
void Compass9150::calibrate(){
    // Flip this bit to TRUE
    state |= CAL_RUNNING_BIT;

    int16_t mx, my, mz;
    int16_t mx_max, my_max, mz_max;
    int16_t mx_min, my_min, mz_min;
    mx_max = AK8975_H_OL_NEG;
    my_max = AK8975_H_OL_NEG;
    mz_max = AK8975_H_OL_NEG;
    mx_min = AK8975_H_OL_POS;
    my_min = AK8975_H_OL_POS;
    mz_min = AK8975_H_OL_POS;

    const int16_t min_delta = 10;

    const uint8_t NREPS = 100;

    for (uint8_t i=0; i<NREPS; i++) {
        getMag(&mx, &my, &mz);
        mx_max = (mx > mx_max) ? mx : mx_max;
        my_max = (my > my_max) ? my : my_max;
        mz_max = (mz > mz_max) ? mz : mz_max;
        mx_min = (mx < mx_min) ? mx : mx_min;
        my_min = (my < my_min) ? my : my_min;
        mz_min = (mz < mz_min) ? mz : mz_min;
        delay(100);
    }

    if (mx_max - mx_min > min_delta && my_max - my_min > min_delta && mz_max - mz_min > min_delta) {
        mx_offset = (mx_max - mx_min) /2;
        my_offset = (my_max - my_min) /2;
        mz_offset = (mz_max - mz_min) /2;
        state |= CAL_BIT;
    }

    state &= ~CAL_RUNNING_BIT;

}

uint8_t Compass9150::get_mag(int16_t* mx,int16_t* my,int16_t* mz){
    uint8_t status = getMag(mx, my, mz);
    mx -= mx_offset;
    my -= my_offset;
    mz -= mz_offset;
    return status;
}