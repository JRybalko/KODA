#include "Decoder.hpp"

Decoder::Decoder() {

}

Decoder::~Decoder() {

}

std::vector<unsigned char> Decoder::binary_decode(std::vector<unsigned char> encoded, unsigned int c1, unsigned int c2)
{
	std::vector<unsigned char> decoded;
	
	unsigned int lower = 0;
    // 2^32
	unsigned long long width = 4294967296;

	std::vector<unsigned char> input_array(encoded.begin(), encoded.begin() + 32);
	unsigned int k = 0;
	unsigned int seen_bits = 32;

	unsigned int input_buffer = 0;
	for (int i = 0; i < 32; ++i)
	{
		input_buffer += input_array[i] * std::pow(2, 31 - i);
	}
	
	while (k < c1 + c2) 
	{
		unsigned long long r1 = width * c1 / (unsigned long long)(c1 + c2);
		unsigned long long r2 = width - r1;

		if ((unsigned long long)(input_buffer - lower) >= r2) 
		{
			width = r1;
			lower = lower + r2;
			decoded.push_back(1);
		}
		else 
		{
			width = r2;
			decoded.push_back(0);
		}
		normalize_decode(lower, input_buffer, width, encoded, seen_bits);
		k += 1;
	}
	return decoded;
}

void Decoder::normalize_decode(unsigned int& lower, unsigned int& input_buffer, unsigned long long& width, std::vector<unsigned char>& encoded, unsigned int& seen_bits)
{
	while (width <= QUARTER_BITS)
	{
		if (lower + width <= HALF_BITS);
		else if (lower >= HALF_BITS)
		{
			lower -= HALF_BITS;
			input_buffer -= HALF_BITS;
		}
		else
		{
			lower -= QUARTER_BITS;
			input_buffer -= QUARTER_BITS;
		}
		lower <<= 1;
		width <<= 1;

		unsigned char next_input_bit;
		if (seen_bits < encoded.size())
		{
			next_input_bit = encoded[seen_bits];
		}
		else
		{
			next_input_bit = 0;
		}
		input_buffer <<= 1;
		input_buffer |= next_input_bit;
		seen_bits += 1;
	}
}