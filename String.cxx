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

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstring>
#include <iostream>
#include <regex>
#include <string>

enum StringBaseConv {
    BIN = 2,
    DEC = 10,
    HEX = 16,
};

enum class byte : unsigned char {};

class String {
    std::string m_u;

    String(std::string u) : m_u{std::move(u)} {}

  public:
    String() noexcept = default;
    String(const String&) = default;
    String(String&&) noexcept = default;
    String& operator=(const String&) = default;
    String& operator=(String&&) = default;

    template <std::size_t N> inline String(const char (&charr)[N]) : m_u{charr, N} {}
    inline String(const char* cstr) : m_u{cstr} {}
    inline explicit String(char c) : m_u{1, c} {}
    template <class T, class = std::enable_if_t<std::is_integral<T>::value>> inline String(T val, StringBaseConv base) {
        constexpr static auto base2len_upperbound = [](StringBaseConv base) /*constexpr*/ {
            switch (base) {
            case BIN:
                return 65;
            case DEC:

            case HEX:
                return 17;
            default:
                throw;
            }
        };
        m_u.resize(base2len_upperbound(base));
        const auto res = std::to_chars(&*m_u.begin(), &*m_u.rbegin(), val, +base);
        if (static_cast<int>(res.ec))
            throw;
        m_u.resize(std::strlen(m_u.c_str()));
    }

     // template <class T, class = std::enable_if_t<std::is_floating_point<T>::value>>
    // String(T val, int precision); // unimplemented

    [[nodiscard]] inline auto c_str() const noexcept { return m_u.c_str(); }
    [[nodiscard]] inline auto length() const noexcept { return m_u.length(); }
    [[nodiscard]] inline auto charAt(unsigned idx) const noexcept { return m_u.at(idx); }
    [[nodiscard]] inline auto& charAt(unsigned idx) noexcept { return m_u.at(idx); }
    [[nodiscard]] inline auto operator[](unsigned idx) const noexcept { return m_u[idx]; }
    [[nodiscard]] inline auto& operator[](unsigned idx) noexcept { return m_u[idx]; }

    template <class T>[[nodiscard]] inline bool concat(const T& v) {
        m_u += String(v).m_u;
        return true;
    }

    inline int compareTo(const String& s) const noexcept { return std::memcmp(m_u.c_str(), s.m_u.c_str(), (std::min)(s.m_u.size(), m_u.size())); }

    inline bool startsWith(const String& s) const noexcept {
        if (s.m_u.size() > m_u.size())
            return false;
        return std::memcmp(m_u.c_str(), s.m_u.c_str(), s.m_u.size()) == 0;
    }

    inline bool endsWith(const String& s) const noexcept {
        if (s.m_u.size() > m_u.size())
            return false;
        return std::memcmp(&*m_u.rbegin() - s.m_u.size(), s.m_u.c_str(), s.m_u.size()) == 0;
    }

    inline void getBytes(byte buffer [], unsigned length) { 
        for (int i = 0; i < m_u.length; i++) {
            buffer[i] = (byte)m_u[i];
        }
    }

    inline int indexOf(const char* c) { return m_u.find(c); }

    inline int indexOf(const char* c, unsigned index) { return m_u.find (c, index); }

    inline int indexOf(const String& str) { return m_u.find(str); }

    inline int indexOf(const String& str, unsigned index) { return m_u.find(str, index); }

    inline void remove(unsigned idx) { m_u.erase(index); }

    inline void remove(unsigned idx, unsigned count) { m_u.erase(idx, idx + count - 1); }

    inline void replace(const String& substring1, const String& substring2) { m_u = std::regex_replace (m_u, std::regex(substring1), substring2); }

    inline void reserve(unsigned size) { m_u.reserve(size); }

    inline void setCharAt(unsigned index, const char* c) { m_u[index] = c; }

    inline String substring(unsigned from) { return m_u.substr(from); }

    inline String substring(unsigned from, unsigned to){ return m_u.substr(from, to - from); }

    inline void toCharArray(const char* buffer[], unsigned length) { 
        if (length <= m_u.length) {
            for (int i = 0; i < length; i++) {
                buffer[i] = m_u[i];
            }
        } else {
            for ( i = 0; i < m_u.length; i++) {
                buffer[i] = m_u[i];
            }
        }
    }

    inline int toInt() const noexcept {
        if (isdigit(m_u[0]) {
            return std::stoi(m_u);
        } else return 0L;
    }

    inline double toDouble() const noexcept {
        if (isdigit(m_u[0]) {
            return std::stod(m_u);
        } else return 0;
    }

    inline float toFloat() const noexcept {
        if (isdigit(m_u[0]) {
            return std::stof(m_u);
        } else return 0;
    }

    inline void toLowerCase() {std::transform (m_u.begin(), m_u.end(), m_u.begin(), std::tolower); }

    inline void toUpperCase() {std::transform (m_u.begin(), m_u.end(), m_u.begin(), std::toupper); }

    inline void trim() { m_u = m_u.erase(std::remove (m_u.begin(), m_u.end(), ' '), m_u.end()); }

    friend String operator+(const String&, const String&);
    [[nodiscard]] inline bool equals(const String& s) const noexcept { return m_u == s.m_u; }
    [[nodiscard]] inline bool equalsIgnoreCase(const String& s) const noexcept { 
        String str = m_u;
        return (std::transform(s.begin(), s.end(), s.begin(), std::tolower) == std::transform(str.begin(), str.end(), str.begin(), std::tolower)); 
    }

    [[nodiscard]] inline bool operator==(const String& s) const noexcept { return m_u == s.m_u; }
    [[nodiscard]] inline bool operator!=(const String& s) const noexcept { return m_u != s.m_u; }
    [[nodiscard]] inline bool operator<(const String& s) const noexcept { return m_u < s.m_u; }
    [[nodiscard]] inline bool operator<=(const String& s) const noexcept { return m_u <= s.m_u; }
    [[nodiscard]] inline bool operator>(const String& s) const noexcept { return m_u > s.m_u; }
    [[nodiscard]] inline bool operator>=(const String& s) const noexcept { return m_u >= s.m_u; }

    friend String operator+(const String&, const char*);
    friend String operator+(const char*, const String&);
};

[[nodiscard]] inline String operator+(const String& lhs, const String& rhs) { return lhs + rhs; }
[[nodiscard]] inline String operator+(const String& lhs, const char* rhs) { return lhs + rhs; }
[[nodiscard]] inline String operator+(const char* lhs, const String& rhs) { return lhs + rhs; }

#endif // SMARTCAR_EMUL_STRING_CXX