#include <iostream>
#include "BigInt.hpp"

int main(){
    char str[100];
    BigInt int_t1 = "0b101101101001001101111";
    BigInt int_t2 = "0b1100110101";
    BigInt int_rem;
    BigInt int_t3 = divided("0b101101101001001101111", "0b1100110101", int_rem);
    std::cout << "101101101001001101111 = " <<
    int_t3.to_cstr(str, 2) << " * 1100110101 + " << int_rem.to_cstr(str, 2) << '\n';

    int_t1.div(int_t2, int_t1);
    std::cout << int_t1.to_cstr(str, 2) << " = " <<
    int_t3.to_cstr(str, 2) << " * " << int_t2.to_cstr(str, 2) << " + " << int_rem.to_cstr(str, 2) << '\n';

    int_t1 = "0b 1111 1111";
    int_t2 = "0b 1000";
    int_t3 = int_t1;
    int_t3.div(int_t2, int_rem);
    std::cout << int_t1.to_cstr(str, 2) << " = " <<
    int_t3.to_cstr(str, 2) << " * " << int_t2.to_cstr(str, 2) << " + " << int_rem.to_cstr(str, 2) << '\n';

    int_t1 = "0b 1000 0000";
    int_t2 = "0b 1111";
    int_t3 = int_t1;
    int_t3.div(int_t2, int_rem);
    std::cout << int_t1.to_cstr(str, 2) << " = " <<
    int_t3.to_cstr(str, 2) << " * " << int_t2.to_cstr(str, 2) << " + " << int_rem.to_cstr(str, 2) << '\n';

    char c_str[] = "453547563486745464765467";
    BigInt int1(c_str);
    BigInt int2 = int1;
    auto x = 174361;
    BigInt int3 = x;
    BigInt int4;
    BigInt int6 = 4294967295;
    BigInt int7 = -4294967295;
    BigInt int8 = 4294967296;
    BigInt int5 = -174361l;
    BigInt int9;
    int9 = '0';
    BigInt int10;
    int10 = "0437721";
    BigInt int11 = 4294967296u;
    BigInt int12 = -1;
    int1 = int1;
    BigInt int13 = true;

    std::cout << "1:" << int1.to_cstr(str, 16) << "\n2:" << int2.to_cstr(str, 16) << "\n3:" << int3.to_cstr(str, 16) << "\n4:" <<
    int4.to_cstr(str, 16) << "\n5:" << int5.to_cstr(str, 16) << "\n6:" << int6.to_cstr(str, 16) << "\n7:" <<
    int7.to_cstr(str, 16) << "\n8:" << int8.to_cstr(str, 16) << "\n9:" << int9.to_cstr(str, 16) << "\n10:" << int10.to_cstr(str, 16) << "\n11:" <<
    int11.to_cstr(str, 16) <<  "\n12:" << int12.to_cstr(str, 16) << "\n13:" << int13.to_cstr(str, 16) << "\n13:";
    int13 = false;
    std::cout << int13.to_cstr(str, 16) << "\n";

    std::cout << (-int1).to_cstr(str, 10) << ' ' << (int1++).to_cstr(str, 10) << ' ' << int1.to_cstr(str, 10) << '\n';
    std::cout << (int2--).to_cstr(str, 10) << ' ' << int2.to_cstr(str, 10) << '\n';
    --int7;
    std::cout << (int4 == int9) << ' ' << (int4 == int9) << ' ' << (int7 < int12) << '\n';
    std::cout << "7: " << int7.to_cstr(str, 10) << '\n';

    ++int6;
    --int9;
    int7 += "4294967305";
    int3 -= int1;

    std::cout << int6.to_cstr(str, 16) << '\n' << int9.to_cstr(str, 16) << '\n' << int7.to_cstr(str, 16) << '\n' << int3.to_cstr(str, 16) << '\n';

    int5 *= int5;
    int_t2 = "0b101101101001001101111";
    std::cout << "t2: " << int_t2.to_cstr(str, 10) << '\n';
    int_t2 += int_t2;
    int_t1 -= int_t1;
    std::cout << "5: " << int5.to_cstr(str, 16) << '\n' << "t2: " << int_t2.to_cstr(str, 10) << '\n' << "t1: " << int_t1.to_cstr(str, 16) << '\n';
}
