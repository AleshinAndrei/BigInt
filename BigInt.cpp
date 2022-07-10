#include "BigInt.hpp"
#include <cstdint>
#include <cctype>
#include <stdexcept>
#include <cstring>

int8_t BigInt::chtoi(const char& ch) const {
    if (ch <= '9')      return (ch - '0');
    else if (ch <= 'Z') return (ch - 'A' + 10);
    else if (ch <= 'z') return (ch - 'a' + 10);
    throw std::invalid_argument("invalid char " + ch);
}
char BigInt::itoch(const uint8_t i) const {
    if (i < 10)      return ('0' + i);
    else if (i < 37) return ('a' + i - 10);
    throw std::invalid_argument("invalid int");
}


BigInt::BigInt(){}
BigInt::BigInt(const BigInt& other)
: data(other.data), neg(other.neg), def_base(other.def_base){
}
BigInt::BigInt(const BigInt& other, const char& sign)
: data(other.data), def_base(other.def_base) {
    if (sign == '-') neg = true;
}
BigInt::BigInt(BigInt&& other)
: data(std::move(other.data)), neg(other.neg), def_base(other.def_base) {
    other.neg = false; // other in a valid but unspecified state (also if &other == this)
}
BigInt::BigInt(BigInt&& other, const char& sign)
: data(std::move(other.data)), def_base(other.def_base){
    if (sign == '-') neg = true;
    other.neg = false;
}
BigInt::BigInt(const std::string& str, uint8_t base){
    if (base)
        set_def_base(base);
    from_cstr(str.c_str(), base);
}
BigInt::BigInt(const char* p_str, uint8_t base){
    if (base)
        set_def_base(base);
    from_cstr(p_str, base);
}
BigInt::BigInt(const char& ch, uint8_t base){
    if (base)
        set_def_base(base);
    from_char(ch);
}
BigInt::BigInt(bool flag){
    if (flag) data.push_back(0x1);
}


BigInt::operator std::string() const {
    return to_string();
}


BigInt& BigInt::operator=(const BigInt& other) & {
    if (this == &other) return *this;
    data = other.data;
    neg = other.neg;
    return *this;
}
BigInt& BigInt::operator=(BigInt&& other) & {
    data = std::move(other.data);
    neg = other.neg;
    other.neg = false;
    return *this;
}
BigInt& BigInt::operator=(const std::string& str) & {
    from_cstr(str.c_str());
    return *this;
}
BigInt& BigInt::operator=(const char* p_str) & {
    from_cstr(p_str);
    return *this;
}
BigInt& BigInt::operator=(const char& ch) & {
    from_char(ch);
    return *this;
}
BigInt& BigInt::operator=(bool flag) & {
    data.clear();
    neg = false;
    if (flag) data.push_back(0x1);
    return *this;
}

BigInt& BigInt::operator+=(const BigInt& other) & {
    return plus_minus_op(other, '+');
}
BigInt& BigInt::operator-=(const BigInt& other) & {
    return plus_minus_op(other, '-');
}

BigInt& BigInt::operator*=(const BigInt& other) & {
    // selfAssignment supported
    if (other.data.empty()){
        neg = false;
        data.clear();
    } else if (!data.empty()){
        BigInt res;
        size_t i = 0, j = 0, size1 = data.size(), size2 = other.data.size();
        res.data.resize(size1, 0);
        uint64_t t, ovfl;
        for(;j<size2; ++j){
            t = 0;
            ovfl = 0;
            for(;i<size1; ++i){
                t = static_cast<uint64_t>(data[i]) * other.data[j] + res.data[i+j] + ovfl;
                res.data[i+j] = static_cast<uint32_t>(t);
                ovfl = (t>>32);
            }
            res.data.push_back(static_cast<uint32_t>(ovfl));
        }
        if (res.data.back() == 0x0) res.data.pop_back();
        neg ^= other.neg;
        data = res.data;
    }
    return *this;
}
BigInt& BigInt::operator/=(const BigInt& other) & {
    BigInt tmp;
    return (this->div(other, tmp));
}
BigInt& BigInt::operator%=(const BigInt& other) & {
    BigInt remain;
    this->div(other, remain);
    *this = remain;
    return *this;
}

BigInt& BigInt::operator>>=(uint64_t shift) & {
    if (!data.empty() && (shift != 0)){
        uint64_t t = shift % 32;
        size_t i = 0, j = shift / 32;
        if (t != 0){
            uint64_t t1 = 32-t;
            while (j+1<data.size()){
                data[i] = (data[j] >> t);
                data[i] |= (data[j+1] << t1);
                ++i;
                ++j;
            }
        } else {
            while (j+1<data.size()){
                data[i] = data[j];
                ++i;
                ++j;
            }
        }
        data[i] = (data.back() >> t);
        if (data[i]) data.resize(i+1);
        else data.resize(i);
        if (data.empty()) neg = false;
    }
    return *this;
}
BigInt& BigInt::operator<<=(uint64_t shift) & {
    if (!data.empty() && (shift != 0)){
        // data can be equal to {0} and shift == 1 (when calling from this->div()),
        // then result must be equal to {0} exactly
        int t = shift % 32;
        size_t i = data.size(), j = data.size() + ((shift-1) / 32);
        data.resize(j+1, 0);
        if (t != 0){
            int t1 = 32-t;
            while(i){
                --i;
                data[j] |= (data[i] >> t1);
                --j;
                data[j] = (data[i] << t);
            }
            if (data.back() == 0) data.pop_back();
        } else {
            while(i){
                --i;
                --j;
                data[j] = data[i];
            }
        }
        while(j){
            --j;
            data[j] = 0;
        }
    }
    return *this;
}


BigInt& BigInt::operator++() & {
    if (neg){
        if (data[0] != 0x0){
            --data[0];
        } else {
            size_t i = 1;
            while (true) {
                data[i-1] = 0xFFFFFFFF;
                if (data[i] != 0x0){
                    --data[i];
                    break;
                }
                ++i;
            }
        }
        if (data.back() == 0x0){
            data.pop_back();
            neg = !(data.empty());
        }
    } else if (data.empty()) {
        data.push_back(0x1);
    } else if (data[0] != 0xFFFFFFFF){
        ++data[0];
    } else {
        data[0] = 0x0;
        size_t i = 1;
        for (;i<data.size(); ++i){
            if (data[i] != 0xFFFFFFFF){
                ++data[i];
                return *this;
            }
            data[i] = 0x0;
        }
        if (data.back() == 0x0) data.push_back(0x1);
    }
    return *this;
}
BigInt& BigInt::operator--() & {
    if (neg){
        if (data[0] != 0xFFFFFFFF){
            ++data[0];
        } else {
            data[0] = 0x0;
            for (size_t i=1;i<data.size(); ++i){
                if (data[i] != 0xFFFFFFFF){
                    ++data[i];
                    break;
                }
                data[i] = 0x0;
            }
            if (data.back() == 0x0) data.push_back(0x1);
        }
    } else if (data.empty()){
        data.push_back(0x1);
        neg = true;
    } else {
        if (data[0] != 0x0) {
            --data[0];
        } else {
            size_t i = 1;
            while (true) {
                data[i-1] = 0xFFFFFFFF;
                if (data[i] != 0x0){
                    --data[i];
                    break;
                }
                ++i;
            }
        }
        if (data.back() == 0x0) data.pop_back();
    }
    return *this;
}
BigInt BigInt::operator++(int) & {
    BigInt res = *this;
    ++(*this);
    return res;
}
BigInt BigInt::operator--(int) & {
    BigInt res = *this;
    --(*this);
    return res;
}

BigInt BigInt::operator+() const {return BigInt(*this);} //
BigInt BigInt::operator-() const {
    BigInt res(*this);
    if (data.size()) res.neg = !res.neg;
    return res;
}

BigInt operator+(const BigInt& int1, const BigInt& int2){
    return BigInt(int1).plus_minus_op(int2, '+');
}
BigInt operator-(const BigInt& int1, const BigInt& int2){
    return BigInt(int1).plus_minus_op(int2, '-');
}

BigInt operator*(const BigInt& int1, const BigInt& int2){
    BigInt res = int1;
    res *= int2;
    return res;
}
BigInt operator/(const BigInt& int1, const BigInt& int2){
    BigInt tmp;
    return BigInt(int1).div(int2, tmp);
}
BigInt operator%(const BigInt& int1, const BigInt& int2){
    BigInt remain;
    BigInt(int1).div(int2, remain);
    return remain;
}

BigInt operator>>(const BigInt big_int, uint64_t shift){
    BigInt res = big_int;
    res >>= shift;
    return res;
}
BigInt operator<<(const BigInt big_int, uint64_t shift){
    BigInt res = big_int;
    res <<= shift;
    return res;
}


bool operator<(const BigInt& int1, const BigInt& int2){
    if (int1.neg != int2.neg) return int1.neg;
    if (int1.data.size() == int2.data.size()){
        size_t i = int1.data.size();
        while (i){
            --i;
            if (int1.data[i] != int2.data[i])
                return (int1.neg != (int1.data[i] < int2.data[i]));
        }
        return false;
    }
    return (int1.neg != (int1.data.size() < int2.data.size()));
}
bool operator>(const BigInt& int1, const BigInt& int2){
    return (int2 < int1);
}
bool operator<=(const BigInt& int1, const BigInt& int2){
    return !(int2 < int1);
}
bool operator>=(const BigInt& int1, const BigInt& int2){
    return !(int1 < int2);
}
bool operator==(const BigInt& int1, const BigInt& int2){
    if ((int1.neg == int2.neg) && (int1.data.size() == int2.data.size())){
        size_t i = int1.data.size();
        while (i){
            --i;
            if (int1.data[i] != int2.data[i]) return false;
        }
        return true;
    }
    return false;
}
bool operator!=(const BigInt& int1, const BigInt& int2){
    return !(int2 == int1);
}

void BigInt::from_string(std::string& str, uint8_t base) & {
    from_cstr(str.c_str(), base);
}
void BigInt::from_cstr(const char* p_str, uint8_t base) & {
    if ((base == 1) || (base > 36))
        throw std::invalid_argument("invalid base (must be {0, 2, 3, ..., 36})");
    data = {0};
    neg = false;

    while (isspace(*p_str)) {
        ++p_str;
    }
    if (*p_str == '-'){
        neg = true;
        ++p_str;
    }
    if (*p_str == '0'){
        ++p_str;
        if ((*p_str == 'x') || (*p_str == 'X')){ // prefix "0x..."
            if (base == 0) base = 16;
            if (base == 16){
                ++p_str;
            } else {
                --p_str;
            }
        } else if ((*p_str == 'b') || (*p_str == 'B')){ // prefix "0b..."
            if (base == 0) base = 2;
            if (base == 2){
                ++p_str;
            } else {
                --p_str;
            }
        } else { // prefix "0..."
            --p_str;
            if (base == 0) base = 8;
        }
    } else if (base == 0) {
        base = def_base;
    }

    std::vector<int8_t> dig;
    while (true){
        while (isblank(*p_str)) ++p_str;
        if (base < 11){
            if ((*p_str < '0') || ('0'+base <= *p_str)) break;
        } else if (!(
            (('0' <= *p_str) && (*p_str < '0'+10)) ||
            (('A' <= *p_str) && (*p_str < 'A'+base-10)) ||
            (('a' <= *p_str) && (*p_str < 'a'+base-10))
        )) break;
        dig.push_back(chtoi(*p_str));
        ++p_str;
    }

    std::vector<uint32_t> spl{1}; // представление base^k в виде числа с 2^32 основанием
    size_t k = dig.size()-1; // k - то число, в которое возводим base
    size_t i; // i - счётчик spl и data, i <= spl.size() = data.size()
    uint64_t t = 0, ovfl_data, ovfl_spl;
    while (true){
        ovfl_data = 0;
        ovfl_spl = 0;
        for (i=0; i<spl.size(); ++i){
            t = static_cast<uint64_t>(spl[i]) * dig[k] + data[i] + ovfl_data;
            ovfl_data = (t >> 32);
            data[i] = static_cast<uint32_t>(t);

            t = static_cast<uint64_t>(spl[i]) * base + ovfl_spl;
            ovfl_spl = (t >> 32);
            spl[i] = static_cast<uint32_t>(t);

            if ((i == spl.size()-1) && (ovfl_data || ovfl_spl)){
                spl.push_back(0);
                data.push_back(0x0);
            }
        }
        if (k == 0) break;
        --k;
    }
    while (data.size() && (data.back() == 0x0)) data.pop_back();
    if (data.empty()) neg = false;
}
void BigInt::from_char(const char& ch) & {
    if (ch < '0') throw std::invalid_argument("invalid char " + ch);
    else if (ch == '0') data.clear();
    else if (ch <= '9') data = {static_cast<uint32_t>(ch) - '0'};
    else if (ch < 'A') throw std::invalid_argument("invalid char " + ch);
    else if (ch <= 'Z') data = {static_cast<uint32_t>(ch) - 'A' + 10};
    else if (ch < 'a') throw std::invalid_argument("invalid char " + ch);
    else if (ch <= 'z') data = {static_cast<uint32_t>(ch) - 'a' + 10};
    else throw std::invalid_argument("invalid char " + static_cast<uint32_t>(ch));
    neg = false;
}
std::string BigInt::to_string(uint8_t base) const {
    if (base == 0)
        base = def_base;
    else if ((base == 1) || (36 < base))
        throw std::invalid_argument("invalid base (must be {0, 2, 3, ..., 36})");


    std::vector<uint8_t> dig_res{0};
    std::vector<uint8_t> spl{1}; // представление 2^(32*k) в виде числа с основанием base
    uint64_t t = 0; // универсальная переменная
    size_t k = 0; // k - то число, в которое возводим base
    size_t i; // i - счётчик spl и data, i <= spl.size() = dig_res.size()
    uint64_t ovfl_res; // <= base
    uint64_t ovfl_spl; // <= base
    while (k < data.size()){
        ovfl_res = 0;
        ovfl_spl = 0;
        for (i=0; i<spl.size(); ++i){
            t = static_cast<uint64_t>(spl[i]) * data[k] + dig_res[i] + ovfl_res;
            ovfl_res = t / base;
            dig_res[i] = static_cast<uint8_t>(t % base);

            t = (static_cast<uint64_t>(spl[i]) << 32) + ovfl_spl;
            ovfl_spl = t / base;
            spl[i] = static_cast<uint8_t>(t % base);

            if ((i == spl.size()-1) && (ovfl_res || ovfl_spl)){
                spl.push_back(0);
                dig_res.push_back(0);
            }
        }
        ++k;
    }
    while((dig_res.back() == 0) && (dig_res.size() > 1)) dig_res.pop_back();

    std::string res;
    res.reserve(dig_res.size()+1);
    if (neg)
        res.push_back('-');
    i = dig_res.size();
    while (i){
        --i;
        res.push_back(itoch(dig_res[i]));
    }
    return res;
}

char* BigInt::to_cstr(char* p_str, uint8_t base) const {
    return std::strcpy(p_str, to_string(base).c_str());
}

BigInt& BigInt::plus_minus_op(const BigInt& other, const char& sign){
    // selfAssignment supported
    int64_t k = ((neg == other.neg) ? 1 : -1);
    if (sign == '-'){
        k = -k;
    } else if (sign != '+'){
        throw std::invalid_argument("invalid sign in plus_minus_op (mast be '+' or '-')");
    }
    int64_t temp;
    uint32_t ovfl = 0;
    size_t i = 0, size_ = other.data.size();

    if (size_ > data.size())
        data.resize(other.data.size(), 0x0);
    for (;i<size_; ++i){
        temp = data[i] + k*(other.data[i] + ovfl); // 0xffffffff00000000 <= temp <= 0x1ffffffff
        ovfl = (temp >> 32)&0x1; // 0 or 1.
        // Warning! negative rshift is implementation-defined until C++20
        data[i] = static_cast<uint32_t>(temp); // temp % 2^32
    }
    if (ovfl){
        if (k == 1){
            data.push_back(0x1);
        } else {
            for (i=0; i<size_; ++i){
                if (data[i] != 0){
                    data[i] = (~data[i]) + 1;
                    break;
                }
            }
            for (;i<size_; ++i){
                data[i] = ~data[i];
            }
            neg = !neg;
        }
    }
    while (data.size() && (data.back() == 0x0)) data.pop_back();
    if (data.empty()) neg = false;
    return *this;
}
BigInt& BigInt::div(const BigInt& other, BigInt& remain) { //
    // selfAssignment supported
    if (other.data.size() == 0){
        throw std::invalid_argument("division by zero");
    }
    if (&other == this){
        remain = 0;
        *this = 1;
        return *this;
    }
    if (&remain == this){
        BigInt tmp;
        div(other, tmp);
        *this = tmp;
        return *this;
    }
    BigInt tmp(other, '+');
    bool this_neg = this->neg;
    remain.data = std::move(this->data);
    remain.neg = false;

    this->neg = false;
    (this->data).clear();
    if (remain < tmp){
        remain.neg = this_neg;
         // *this == 0
        return *this;
    }

    uint64_t bit_size0 = tmp.bit_size();
    uint64_t shift0 = (remain.bit_size() - bit_size0); // >= 0
    uint64_t shift1 = 0;
    tmp <<= shift0;
    (this->data).push_back(0);
    while (true){
        if (remain >= tmp){ // remain.bit_size() == tmp.bit_size()
            ++((this->data)[0])
            remain -= tmp;
            shift1 = (bit_size0 + shift0 - remain.bit_size()); // >= 1
        } else {
            shift1 = 1;
        }
        if (shift0 < shift1)
            break;
        tmp >>= shift1;
        shift0 -= shift1;
        *this <<= shift1; // *this can be invalid, but it's ok
    }
    *this <<= shift0;

    remain.neg = (this_neg && (!remain.data.empty()));
    this->neg = (this_neg != other.neg); // data.size() > 0
    return *this;
}
BigInt divided(const BigInt& int1, const BigInt& int2, BigInt& remain){
    return BigInt(int1).div(int2, remain);
}

size_t BigInt::bit_size() const {
    // super fast version
    if (data.empty()) return 0;
    size_t res = (data.size()-1)*sizeof(uint32_t);
    uint32_t last = data.back();
    uint32_t k;
    if (last&0xffff0000){
        if (last&0xff000000) {
            res += 28;
            k = 0xf0000000;
        } else {
            res += 20;
            k = 0xfff00000;
        }
    } else {
        if (last&0xffffff00) {
            res += 12;
            k = 0xfffff000;
        } else {
            res += 4;
            k = 0xfffffff0;
        }
    }
    size_t shift = 2;
    while (shift){
        if (last & k){
            k <<= shift;
            res += shift;
        } else {
            k >>= shift;
            res -= shift;
        }
        shift >>= 1;
    }
    if (last & k) ++res;
    return res;
}
const BigInt BigInt::abs() const {return BigInt(*this, false);}
bool BigInt::is_valid() const {
    if (data.empty() && neg){
        throw std::runtime_error("oops, something went worth: BigInt is not valid (*this == 0, but is negative)");
    } else if (data.back() == 0){
        throw std::runtime_error("oops, something went worth: BigInt is not valid (this->data.back() == 0)");
    }
    return true;
}
void BigInt::set_def_base(uint8_t base){
    if ((base < 2) || (base > 36))
        throw std::invalid_argument("invalid base (must be {2, 3, ..., 36})");
    def_base = base;
}
uint8_t BigInt::get_def_base() const {return def_base;}
