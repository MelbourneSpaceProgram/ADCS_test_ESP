/*
    Ticker.h - esp32 library that calls functions periodically
                (similar to Ticker.h for esp8266)
    Copyright (C) 2017  Lyudmil Vladimirov
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TICKER_H
#define TICKER_H

#include <Arduino.h>

class Ticker {
private:
    hw_timer_t * _hw_timer = NULL;
    uint16_t _hw_timer_id;
public:
    Ticker(int hw_timer_id);
    ~Ticker();
    void attach(float seconds, void  func());
    void once(float seconds, void  func());
    void once_us(uint32_t microseconds, void  func());
    void attach_us(uint32_t microseconds, void  func(), bool repeat=true);
    void detach();
};
#endif