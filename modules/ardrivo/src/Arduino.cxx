/*
 *  Arduino.cxx
 *  Copyright 2020 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include <climits>
#include "Arduino.h"
#include "BoardData.hxx"
#include "Entrypoint.hxx"
using namespace std::literals;


void pinMode(uint8_t pin, uint8_t mode) { board_data->pin_modes[pin] = mode; }

void digitalWrite(uint8_t pin, bool val) { board_data->digital_pin_values[pin] = val; }

int digitalRead(uint8_t pin) { return board_data->digital_pin_values[pin]; }

void analogWrite(uint16_t pin, uint16_t val) { board_data->analog_pin_values[pin] = val; }

int analogRead(uint8_t pin) { return board_data->analog_pin_values[pin]; }

void analogReference(uint8_t mode) {}

void noTone(uint8_t pin) {
    constexpr uint8_t noToneFeq = 255;
    board_data->pin_frequency[pin] = noToneFeq;
}

void tone(uint8_t pin, unsigned int frequency, unsigned long duration) {
    board_data->pin_frequency[pin] = frequency;
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    noTone(pin);
}

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout) {
    const std::atomic_bool& pin_state = board_data->digital_pin_values[pin];

    const auto wait_state_change = [&](bool target, int timeout = 0) {
      auto t_max = std::chrono::steady_clock::now() + std::chrono::microseconds(timeout);
      while (HIGH) {
          const auto t = std::chrono::steady_clock::now();
          if (pin_state == target)
              return LOW;
          if (timeout > 0 && t >= t_max)
              return HIGH;
          std::this_thread::sleep_for(1ms);
      }
    };

    if (pin_state == state) {
        if (wait_state_change(!state, timeout)) // timeout reached
            return 0;
    }

    const auto start_time = std::chrono::steady_clock::now();
    wait_state_change(state);
    const auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start_time).count();
}

unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout) { return pulseIn(pin, state, timeout); }

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {

    for (uint8_t i = 0; i < CHAR_BIT; ++i) {
        if (bitOrder == RIGHTMOSTBIT) {
            digitalWrite(dataPin, val & 1);
            val >>= 1;
        } else {
            digitalWrite(dataPin, (val & 128) != 0);
            val <<= 1;
        }

        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }
}
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
    uint8_t value = 0;
    uint8_t hexaDeci = 0x1;

    for (uint8_t i = 0; i < CHAR_BIT; ++i) {
        digitalWrite(clockPin, hexaDeci);
        value |= digitalRead(dataPin) << (bitOrder == RIGHTMOSTBIT ? i : 7 - i);
        digitalWrite(clockPin, LOW);
    }
    return value;
}

void delay(unsigned long duration) { std::this_thread::sleep_for(std::chrono::milliseconds(duration)); }

void delayMicroseconds(unsigned int duration) { std::this_thread::sleep_for(std::chrono::microseconds(duration)); }

unsigned long micros() {
    const auto current_time = std::chrono::steady_clock::now();
    const unsigned long duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time - start_time).count();
    return duration;
}

unsigned long millis() {
    const auto current_time = std::chrono::steady_clock::now();
    const unsigned long duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
    return duration;
}


// Arduino Math functions
long constrain(long amt, long low, long high) { return (amt < low ? low : (amt > high ? high : amt)); }
long map(long x, long in_min, long in_max, long out_min, long out_max) { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }

// Arduino Characters functions
bool isAlpha(char thisChar) { return std::isalpha(thisChar); }
bool isAlphaNumeric(char thisChar) { return std::isalnum(thisChar); }
bool isAscii(char thisChar) { return static_cast<unsigned char>(thisChar) < 128 }
bool isControl(char thisChar) { return std::iscntrl(thisChar); }
bool isDigit(char thisChar) { return std::isdigit(thisChar); }
bool isGraph(char thisChar) { return std::isgraph(thisChar); }
bool isHexadecimalDigit(char thisChar) { return std::isxdigit(thisChar); };
bool isLowerCase(char thisChar) { return std::islower(thisChar); }
bool isPrintable(char thisChar) { return std::isprint(thisChar); }
bool isPunct(char thisChar) { return std::ispunct(thisChar); }
bool isSpace(char thisChar) { return std::isspace(thisChar); }
bool isUpperCase(char thisChar) { return std::isupper(thisChar); }
bool isWhitespace(char thisChar) { return std::isblank(thisChar); }

// Arduino Random Numbers functions
long random(long max) { return std::rand() % max; }
long random(long min, long max) { return std::rand() % (max - min) + min; }
void randomSeed(unsigned long seed) { return std::srand(seed); }

// Arduino External Interrupts functions
void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode) {
    if (interruptNum < interruptsHandlers.size())
        interruptsHandlers[interruptNum] = std::pair{userFunc, mode};
}
void detachInterrupt(uint8_t interruptNum) {
    if (interruptNum < interruptsHandlers.size())
        interruptsHandlers.erase(interruptsHandlers.begin() + interruptNum);
}