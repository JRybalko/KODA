#include <iostream>
#include "Encoder.hpp"
#include "Decoder.hpp"
#include <fstream>
#include <map>
#include <random>
#include <chrono>

std::pair<unsigned int, unsigned int> calculate_bits(std::vector<unsigned char> bytes)
{
	unsigned int c1 = 0;
	unsigned int c2 = 0;
	for (auto byte : bytes)
	{
		std::bitset<8> b(byte);
		c1 += b.count();
		c2 += 8 - b.count();
	}
	return std::make_pair(c1, c2);
}

std::vector<unsigned char> read_bytes_from_file(std::string filename)
{
	std::ifstream input(filename, std::ios::binary);

	std::vector<unsigned char> bytes(
		(std::istreambuf_iterator<char>(input)),
		(std::istreambuf_iterator<char>()));

	input.close();
	return bytes;
}

long double binary_entropy(unsigned int c1, unsigned int c2)
{
	long double p1 = (long double)c1 / (long double)(c1 + c2);
	long double p2 = (long double)c2 / (long double)(c1 + c2);
	return -(p1 * std::log2(p1) + p2 * std::log2(p2));
}

double entropy(std::vector<unsigned char> bytes)
{
	std::map<unsigned char, unsigned int> histogram;
	for (auto byte : bytes)
	{
		histogram[byte] += 1;
	}
	double entropy = 0;
	for (auto& pair : histogram)
	{
		double p = (double)pair.second / (double)bytes.size();
		entropy += p * log2(p);
	}
	return -entropy;
}

void write_bytes_from_file(std::string filename, std::vector<unsigned char> bytes)
{
	std::ofstream output(filename, std::ios::binary);

	output.write(reinterpret_cast<char const*>(&bytes[0]), bytes.size());

	output.close();
}

void test_file(std::vector<unsigned char> bytes)
{
    Encoder encoder = Encoder();
	Decoder decoder = Decoder();

	std::pair<unsigned int, unsigned int> counts = calculate_bits(bytes);
	

	auto encode_begin = std::chrono::high_resolution_clock::now();
	std::vector<unsigned char> encoded = encoder.binary_encode(bytes, counts.first, counts.second);
	auto encode_end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> encode_time = std::chrono::duration_cast<std::chrono::nanoseconds>(encode_end - encode_begin);

	auto decode_begin = std::chrono::high_resolution_clock::now();
	std::vector<unsigned char> decoded = decoder.binary_decode(encoded, counts.first, counts.second);
	auto decode_end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> decode_time = std::chrono::duration_cast<std::chrono::nanoseconds>(decode_end - decode_begin);


	std::vector<unsigned char> decoded_file_bytes;
	int offset = 0;
	while (decoded.size() >= offset + 8)
	{
		std::vector<unsigned char> bytes(decoded.begin() + offset, decoded.begin() + offset + 8);
		unsigned char byte = 0;

		for (int i = 0; i < 8; ++i)
		{
			byte += bytes[i] * std::pow(2, 7 - i);
		}
		decoded_file_bytes.push_back(byte);
		offset += 8;
	}
	write_bytes_from_file("out_file", decoded_file_bytes);

	std::cout << "Original size: 			" << bytes.size() * 8 << " bits" << std::endl;
	std::cout << "Encoded size: 			" << encoded.size() << " bits" << std::endl;
	std::cout << "Compression ratio: 		" << (double)encoded.size() / (double)(bytes.size() * 8) << std::endl;
	std::cout << "Compression factor: 		" << (double)((double)bytes.size() * 8) / (double)encoded.size() << std::endl;
	std::cout << "Saving percentage: 		" << (double)((double)bytes.size() * 8 - (double)encoded.size()) / (double)(bytes.size() * 8) << std::endl;
	std::cout << "Encode time: 				" << encode_time.count() << " ns" << std::endl;
	std::cout << "Entropy (8 bit words): 	" << entropy(bytes) << std::endl;
	std::cout << "Entropy (1 bit words): 	" << binary_entropy(counts.first, counts.second) << std::endl;
	std::cout << "Decode time: 				" << decode_time.count() << " ns" << std::endl;

	if (bytes == decoded_file_bytes)
	{
		std::cout << "Decoding successful" << std::endl;
	}
	else
	{
		std::cout << "Decoding failed" << std::endl;
	}

	std::cout << std::endl;
}

void test_files() 
{
	std::vector<std::string> images = {
		"barbara.pgm", "boat.pgm", "chronometer.pgm", 
		"lena.pgm",	"mandril.pgm", "peppers.pgm"
	};

	std::cout << "-- READ IMAGES --\n" << std::endl;
	int list_image_size = (int)images.size();

	for (int i = 0; i < list_image_size; ++i)
	{
		std::vector<unsigned char> bytes = read_bytes_from_file("data/images/" + images[i]);
		std::cout << "------------------------------------" << std::endl;
		std::cout << "File: " << images[i] << std::endl;
		test_file(bytes);
	}
	std::cout << std::endl;
}



int main()
{
	test_files();
	return 0;
}