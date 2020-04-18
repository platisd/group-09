/*
 *  String.cxx
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
 */

#include "WString.h"

[[nodiscard]] inline auto String::c_str() const noexcept { return m_u.c_str(); }
[[nodiscard]] inline auto String::length() const noexcept { return m_u.length(); }
[[nodiscard]] inline auto String::charAt(unsigned idx) const noexcept { return m_u.at(idx); }
[[nodiscard]] inline auto& String::charAt(unsigned idx) noexcept { return m_u.at(idx); }
[[nodiscard]] inline auto operator[](unsigned idx) const noexcept { return m_u[idx]; }
[[nodiscard]] inline auto& operator[](unsigned idx) noexcept { return m_u[idx]; }

[[nodiscard]] int String::compareTo(const String& s) const noexcept {
    return std::memcmp(m_u.c_str(), s.m_u.c_str(), (std::min)(s.m_u.size(), m_u.size()));
}

[[nodiscard]] bool String::startsWith(const String& s) const noexcept {
    if (s.m_u.size() > m_u.size())
        return false;
    return std::memcmp(m_u.c_str(), s.m_u.c_str(), s.m_u.size()) == 0;
}

[[nodiscard]] bool String::endsWith(const String& s) const noexcept {
    if (s.m_u.size() > m_u.size())
        return false;
    return (m_u.compare(m_u.length() - s.m_u.length(), s.m_u.length(), s.m_u) == 0);
}

void String::getBytes(byte buffer[], unsigned length) const noexcept {
    for (int i = 0; i < m_u.length; i++) {
        buffer[i] = (byte)m_u[i];
    }
}

[[nodiscard]] inline int String::indexOf(const char* c) const noexcept { return m_u.find(c); }

[[nodiscard]] inline int String::indexOf(const char* c, unsigned index) const noexcept { return m_u.find(c, index); }

[[nodiscard]] inline int String::indexOf(const String& str) const noexcept { return m_u.find(str); }

[[nodiscard]] inline int String::indexOf(const String& str, unsigned index) const noexcept { return m_u.find(str, index); }

void String::remove(unsigned idx) { m_u.erase(index); }

void String::remove(unsigned idx, unsigned count) { m_u.erase(idx, idx + count - 1); }

void String::replace(const String& substring1, const String& substring2) {
    size_t position = m_u.find(substring1.m_u);

    while (position != std::string::npos) {
        m_u.replace(position, m_u.size(), substring2.m_u);
        position = m_u.find(substring1.m_u, position + substring2.m_u.size());
    }
}

void String::reserve(unsigned size) { m_u.reserve(size); }

inline void String::setCharAt(unsigned index, const char* c) { m_u[index] = c; }

[[nodiscard]] String String::substring(unsigned from) const { return m_u.substr(from); }

[[nodiscard]] String String::substring(unsigned from, unsigned to) const { return m_u.substr(from, to - from); }

void String::toCharArray(const char* buffer[], unsigned length) noexcept {
    if (length <= m_u.length) {
        for (int i = 0; i < length; i++) {
            buffer[i] = m_u[i];
        }
    } else {
        for (i = 0; i < m_u.length; i++) {
            buffer[i] = m_u[i];
        }
    }
}

[[nodiscard]] long String::toInt() const noexcept {
        if (isdigit(m_u[0]) {
        return std::stoi(m_u);
        } 
        else return 0;
}

[[nodiscard]] double String::toDouble() const noexcept {
        if (isdigit(m_u[0]) {
        return std::stod(m_u);
        } 
        else return 0;
}

[[nodiscard]] float String::toFloat() const noexcept {
        if (isdigit(m_u[0]) {
        return std::stof(m_u);
        } 
        else return 0;
}

void String::toLowerCase() noexcept{ std::transform(m_u.begin(), m_u.end(), m_u.begin(), std::tolower); }

void String::toUpperCase() noexcept{ std::transform(m_u.begin(), m_u.end(), m_u.begin(), std::toupper); }

void String::trim() { m_u = m_u.erase(std::remove(m_u.begin(), m_u.end(), ' '), m_u.end()); }

[[nodiscard]] bool String::equals(const String& s) const noexcept { return m_u == s.m_u; }

[[nodiscard]] bool String::equalsIgnoreCase(const String& s) const noexcept {
    String str = m_u;
    return (std::transform(s.begin(), s.end(), s.begin(), std::tolower) == std::transform(str.begin(), str.end(), str.begin(), std::tolower));
}

[[nodiscard]] inline bool operator==(const String& s) const noexcept { return m_u == s.m_u; }
[[nodiscard]] inline bool operator!=(const String& s) const noexcept { return m_u != s.m_u; }
[[nodiscard]] inline bool operator<(const String& s) const noexcept { return m_u < s.m_u; }
[[nodiscard]] inline bool operator<=(const String& s) const noexcept { return m_u <= s.m_u; }
[[nodiscard]] inline bool operator>(const String& s) const noexcept { return m_u > s.m_u; }
[[nodiscard]] inline bool operator>=(const String& s) const noexcept { return m_u >= s.m_u; }

[[nodiscard]] inline String operator+(const String& lhs, const String& rhs) { return lhs + rhs; }
[[nodiscard]] inline String operator+(const String& lhs, const char* rhs) { return lhs + rhs; }
[[nodiscard]] inline String operator+(const char* lhs, const String& rhs) { return lhs + rhs; }