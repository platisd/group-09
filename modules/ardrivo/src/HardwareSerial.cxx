/*
 *  HardwareSerial.cxx
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

#include "HardwareSerial.h"
#include <iostream>
#include <limits>

extern bool (*write_byte)(unsigned char);
extern size_t (*write_buf)(const unsigned char*, size_t);

void HardwareSerial::begin(unsigned long, uint8_t) {
    begun = true;
}

void HardwareSerial::end() {
    begun = false;
}

int HardwareSerial::available(){
    return std::numeric_limits<int>::max();
}

int HardwareSerial::availableForWrite() {
    return std::numeric_limits<int>::max();
}

bool HardwareSerial::find(char target, size_t length) {

}

bool HardwareSerial::findUntil(char target, char terminal) {

}


void HardwareSerial::flush() {
    std::cout.flush();
}

size_t HardwareSerial::write(uint8_t c) {
    if(!begun)
        return 0;
    write_byte(c);
    std::cout.put(c);
    return 1;
}

size_t HardwareSerial::write(const uint8_t* buf, std::size_t n) {
    if(!begun)
        return 0;
    write_buf(buf, n);
    std::cout.write(reinterpret_cast<const char*>(buf), n);
    return n;
}
