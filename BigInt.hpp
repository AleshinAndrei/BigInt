#ifndef BigInt_HPP
#define BigInt_HPP

#include <vector>
#include <string>
#include <iostream>

class BigInt;

template<typename INT_T>
struct int_is {
    static constexpr bool u_small_v = (
        std::is_same<INT_T, uint8_t>::value ||
        std::is_same<INT_T, uint16_t>::value ||
        std::is_same<INT_T, uint32_t>::value ||
        (std::is_same<INT_T, unsigned long>::value && sizeof(unsigned long) <= 4)
    );
    static constexpr bool s_small_v = (
        std::is_same<INT_T, int8_t>::value ||
        std::is_same<INT_T, int16_t>::value ||
        std::is_same<INT_T, int32_t>::value ||
        (std::is_same<INT_T, long>::value && sizeof(long) <= 4)
    );
    static constexpr bool small_v = (
        int_is<INT_T>::u_small_v || int_is<INT_T>::s_small_v
    );
    static constexpr bool u_big_v = (
        std::is_same<INT_T, uint64_t>::value ||
        (std::is_same<INT_T, unsigned long>::value && sizeof(unsigned long) > 4)
    );
    static constexpr bool s_big_v = (
        std::is_same<INT_T, int64_t>::value ||
        (std::is_same<INT_T, long>::value && sizeof(long) > 4)
    );
    static constexpr bool big_v = (
        int_is<INT_T>::u_big_v || int_is<INT_T>::s_big_v
    );
    static constexpr bool bool_v = (std::is_same<INT_T, bool>::value);
    static constexpr bool value = (
        int_is<INT_T>::big_v || int_is<INT_T>::small_v || int_is<INT_T>::bool_v
    );
};

std::ostream& operator<<(std::ostream& os, const BigInt& bigint);
std::istream& operator>>(std::istream& is, const BigInt& bigint);

class BigInt{
    std::vector<uint32_t> data;
    bool neg = false; // sign
    uint8_t def_base = 10; // default base
    int8_t chtoi(const char& ch) const;
    char itoch(const uint8_t i) const;
public:
    BigInt();
    BigInt(const BigInt& other);
    BigInt(const BigInt& other, const char& sign);
    BigInt(BigInt&& other);
    BigInt(BigInt&& other, const char& sign);
    BigInt(const std::string& str, uint8_t base=0);
    BigInt(const char* p_str, uint8_t base=0);
    BigInt(const char& ch, uint8_t base=0);
    BigInt(bool flag);
    template <typename INT_T, typename std::enable_if<int_is<INT_T>::value, bool>::type = true>
    BigInt(INT_T num);
    //template <typename INT_T, class=typename std::enable_if<int_is<INT_T>>::type>

    operator std::string() const;

    BigInt& operator=(const BigInt& other) & ;
    BigInt& operator=(BigInt&& other) & ;
    BigInt& operator=(const std::string& str) & ;
    BigInt& operator=(const char* p_str) & ;
    BigInt& operator=(const char& ch) & ;
    BigInt& operator=(bool flag) & ;
    template <typename SMALL_INT_T, typename std::enable_if<int_is<SMALL_INT_T>::small_v, bool>::type = true>
    BigInt& operator=(SMALL_INT_T num) & ;
    template <typename BIG_INT_T, typename std::enable_if<int_is<BIG_INT_T>::big_v, bool>::type = true>
    BigInt& operator=(BIG_INT_T num) & ;

    BigInt& operator+=(const BigInt& other) & ;
    BigInt& operator-=(const BigInt& other) & ;
    // BigInt& operator+=(const uint64_t& other);

    BigInt& operator*=(const BigInt& other) & ;
    BigInt& operator/=(const BigInt& other) & ;
    BigInt& operator%=(const BigInt& other) & ;

    BigInt& operator>>=(uint64_t shift) & ;
    BigInt& operator<<=(uint64_t shift) & ;

    BigInt& operator++() & ;
    BigInt& operator--() & ;
    BigInt operator++(int) & ;
    BigInt operator--(int) & ;


    BigInt operator+() const;
    BigInt operator-() const;
    friend BigInt operator+(const BigInt& int1, const BigInt& int2);
    friend BigInt operator-(const BigInt& int1, const BigInt& int2);

    friend BigInt operator*(const BigInt& int1, const BigInt& int2);
    friend BigInt operator/(const BigInt& int1, const BigInt& int2);
    friend BigInt operator%(const BigInt& int1, const BigInt& int2);

    friend BigInt operator>>(const BigInt big_int, uint64_t shift);
    friend BigInt operator<<(const BigInt big_int, uint64_t shift);


    friend bool operator<(const BigInt& int1, const BigInt& int2);
    friend bool operator>(const BigInt& int1, const BigInt& int2);
    friend bool operator<=(const BigInt& int1, const BigInt& int2);
    friend bool operator>=(const BigInt& int1, const BigInt& int2);
    friend bool operator==(const BigInt& int1, const BigInt& int2);
    friend bool operator!=(const BigInt& int1, const BigInt& int2);


    void from_string(std::string& str, uint8_t base=0) & ;
    void from_cstr(const char* p_str, uint8_t base=0) & ;
    void from_char(const char& ch) & ;
    std::string to_string(uint8_t base=0) const;
    char* to_cstr(char* p_str, uint8_t base=0) const;

    BigInt& plus_minus_op(const BigInt& other, const char& sign);
    BigInt& div(const BigInt& other, BigInt& remain);
    friend BigInt divided(const BigInt& int1, const BigInt& int2, BigInt& remain);

    size_t bit_size() const;
    const BigInt abs() const;
    bool is_valid() const;
    void set_def_base(uint8_t base);
    uint8_t get_def_base() const;
};
#include "BigInt.tpp"

#endif // BigInt_HPP
