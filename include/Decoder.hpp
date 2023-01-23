#pragma once
#include <vector>
#include <random>

#define HALF_BITS 0x80000000
#define QUARTER_BITS 0x40000000

class Decoder {

public:
    Decoder();
    ~Decoder();
    std::vector<unsigned char> binary_decode(std::vector<unsigned char> encoded, unsigned int c1, unsigned int c2);
private:
    void normalize_decode(unsigned int& lower, unsigned int& input_buffer, unsigned long long& width, std::vector<unsigned char>& encoded, unsigned int& seen_bits);
};