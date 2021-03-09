/*
    Ticker.cpp - esp32 library that calls functions periodically
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

#include "Ticker.h"

Ticker::Ticker(int hw_timer_id)
{
    if(hw_timer_id<0 || hw_timer_id>3){
        Serial.println("[Ticker.h] ERROR - hw_timer_id must be in the range 0-3!");
        return;
    }
    _hw_timer_id = hw_timer_id;
    _hw_timer = timerBegin(_hw_timer_id, 80, true);
}

Ticker::~Ticker()
{
    detach();
    free(_hw_timer);
}


void Ticker::attach(float seconds, void  func())
{
  // Repeat the alarm (third parameter)
  attach_us(seconds*1000000L, func);
}

void Ticker::once(float seconds, void  func())
{
  // Repeat the alarm (third parameter)
  attach_us(seconds*1000000L, func, false);
}

void Ticker::once_us(uint32_t microseconds, void  func())
{
  // Repeat the alarm (third parameter)
  attach_us(microseconds, func, false);
}

void Ticker::attach_us(uint32_t microseconds, void  func(), bool repeat)
{

  // Attach onTimer function to our timer.
  timerAttachInterrupt(_hw_timer, func, true);

  // Set alarm to fire per given interval
  timerAlarmWrite(_hw_timer, microseconds, repeat);

  timerStart(_hw_timer);
  // Start an alarm
  timerAlarmEnable(_hw_timer);
}

void Ticker::detach()
{
  // Stop alarm
  timerAlarmDisable(_hw_timer);
}
