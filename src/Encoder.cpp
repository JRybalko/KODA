#include "Encoder.hpp"

Encoder::Encoder() {

}

Encoder::~Encoder() {

}

std::vector<unsigned char> Encoder::binary_encode(std::vector<unsigned char> bytes, unsigned int c1, unsigned int c2)
{
	std::vector<unsigned char> encoded;
	unsigned int lower = 0;
    // 2^32
	unsigned long long width = 4294967296; 
	unsigned int scale3 = 0;

	for (auto byte : bytes)
	{
		std::bitset<8> b(byte);
		for (int i = 7; i >= 0; --i)
		{
			bool bit = b[i];
			unsigned long long r1 = width * c1/(unsigned long long)(c1 + c2);
			unsigned long long r2 = width - r1;

			if (bit)
			{
				width = r1;
				lower = lower + r2;
			}
			else
			{
				width = r2;
			}
			normalize_encode(lower, scale3, width, encoded);
		}
	}
	add_bits_from_lower(lower, scale3, encoded);
	return encoded;
}

void Encoder::add_bits_from_lower(unsigned int& lower, unsigned int& scale3, std::vector<unsigned char>& encoded)
{
	std::stack<unsigned char> ending;
	for (int i = 0; i < 32; ++i)
	{
		unsigned int last_bit = lower & 1;
		lower >>= 1;
		if (i == 31)
		{
			while (scale3 > 0)
			{
				ending.push((unsigned int)1 - last_bit);
				scale3 -= 1;
			}
		}
		ending.push(last_bit);
	}
	while (!ending.empty())
	{
		encoded.push_back(ending.top());
		ending.pop();
	}
}

void Encoder::normalize_encode(unsigned int& lower, unsigned int& scale3, unsigned long long& width, std::vector<unsigned char>& encoded)
{
	while (width <= QUARTER_BITS)
	{
		if (lower >= HALF_BITS)
		{
			encoded.push_back(1);
			while (scale3 > 0)
			{
				encoded.push_back(0);
				scale3 -= 1;
			}
			lower -= HALF_BITS;
		}
		else if (lower + width <= HALF_BITS)
		{
			encoded.push_back(0);
			while (scale3 > 0)
			{
				encoded.push_back(1);
				scale3 -= 1;
			}
		}
		else
		{
			scale3 += 1;
			lower -= QUARTER_BITS;
		}
		lower <<= 1;
		width <<= 1;
	}
}

