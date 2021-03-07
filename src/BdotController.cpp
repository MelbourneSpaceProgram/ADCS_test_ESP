#include "BdotController.h"

/* Calculates a weighted average of B
*/
float averageB(int16_t (&array)[BUFFER_LEN]){
    float sum = 0;
    // float norm = 0;
    for(int i=0; i<BUFFER_LEN; i++){
        sum += exp(-i*lambda)*array[i];
        // norm += exp(-i*lambda);
    }
    float output = sum * (1-exp(-lambda))/(1-exp(-lambda*BUFFER_LEN));
    return output;
}

