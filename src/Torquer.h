#include "Arduino.h"

class Torquer {
public:
    Torquer(uint8_t EN_pin, uint8_t fwd_pin, uint8_t bwd_pin, uint8_t channel) : 
        pwm_channel(channel), EN(EN_pin), fwd(fwd_pin), bwd(bwd_pin){
    }

    void init(){
        pinMode(fwd, OUTPUT);
        pinMode(bwd, OUTPUT);

        digitalWrite(fwd, LOW);
        digitalWrite(bwd, LOW);

        ledcSetup(pwm_channel, freq, resolution);
        ledcAttachPin(EN, pwm_channel);
    }

    void actuate(float mu){
        float modmu = abs(mu);
        if (modmu > 1.){
            mu /= modmu;
        } else if (modmu < deadzone_cutoff) {
            digitalWrite(fwd, LOW);
            digitalWrite(bwd, LOW);
            ledcWrite(pwm_channel, 0);
            return;
        }

        if (mu > 0) {
            digitalWrite(fwd, HIGH);
            digitalWrite(bwd, LOW);
        }
        if (mu < 0) {
            digitalWrite(fwd, LOW);
            digitalWrite(bwd, HIGH);
        }

        uint8_t sig = mu*max_power;
        ledcWrite(pwm_channel, sig);
    }

    void set_max_power(uint16_t pow){
        max_power = pow;
    }

private:
    const uint8_t pwm_channel;
    const uint8_t EN, fwd, bwd;
    float deadzone_cutoff = 0.005; // 0.5% power

    uint8_t resolution = 8; // bits
    uint16_t max_power = 255;
    uint16_t freq = 5000; // Hz

};