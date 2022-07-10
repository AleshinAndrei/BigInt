#ifndef BigInt_TPP
#define BigInt_TPP

#include "BigInt.hpp"
#include <type_traits>

//template <typename INT_T, class=typename std::enable_if<is_int<INT_T>>::type>
template <typename INT_T, typename std::enable_if<int_is<INT_T>::value, bool>::type = true>
BigInt::BigInt(INT_T num) {
    operator=(num);
}

template <typename SMALL_INT_T, typename std::enable_if<int_is<SMALL_INT_T>::small_v, bool>::type = true>
BigInt& BigInt::operator=(SMALL_INT_T num) & {
    if (num < 0){
        num = ~num+1;
        operator=(static_cast<uint32_t>(num));
        neg = true;
    } else {
        data.clear();
        neg = false;
        if (num){
            data.push_back(static_cast<uint32_t>(num));
        }
    }
    return *this;
}

template <typename BIG_INT_T, typename std::enable_if<int_is<BIG_INT_T>::big_v, bool>::type = true>
BigInt& BigInt::operator=(BIG_INT_T num) & {
    if (num < 0){
        num = ~num+1;
        operator=(static_cast<uint64_t>(num));
        neg = true;
    } else {
        data.clear();
        neg = false;
        while(num){
            data.push_back(static_cast<uint32_t>(num));
            num >>= 32;
        }
    }
    return *this;
}

#endif
