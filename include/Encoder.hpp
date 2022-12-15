#pragma once
#include <vector>
#include <bitset>
#include <stack>

#define HALF_BITS 0x80000000
#define QUARTER_BITS 0x40000000

class Encoder {

public:
    Encoder();
    ~Encoder();
    std::vector<unsigned char> binary_encode(std::vector<unsigned char> bytes, unsigned int c1, unsigned int c2);
private:
    void add_bits_from_lower(unsigned int& lower, unsigned int& scale3, std::vector<unsigned char>& encoded);
    void normalize_encode(unsigned int& lower, unsigned int& scale3, unsigned long long& width, std::vector<unsigned char>& encoded);
};