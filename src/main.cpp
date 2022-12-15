#include <iostream>
#include "Encoder.hpp"
#include <fstream>
#include <chrono>

int main()
{
	test_files();
	return 0;
}

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

void test_files() 
{
	std::vector<std::string> images = {
		"barbara.pgm", "boat.pgm", "chronometer.pgm", 
		"lena.pgm",	"mandril.pgm", "peppers.pgm"
	};

	std::cout << "Read images" << std::endl;
	int list_image_size = (int)images.size();

	for (int i = 0; i < list_image_size; ++i)
	{
		std::vector<unsigned char> bytes = read_bytes_from_file("data/images/" + images[i]);
		std::cout << "File: " << images[i] << std::endl;
		test_file(bytes);
	}
	std::cout << std::endl;
}

void test_file(std::vector<unsigned char> bytes)
{
    Encoder encoder = Encoder();

	std::pair<unsigned int, unsigned int> counts = calculate_bits(bytes);
	

	auto encode_begin = std::chrono::high_resolution_clock::now();

	std::vector<unsigned char> encoded = encoder.binary_encode(bytes, counts.first, counts.second);

	auto encode_end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> encode_time = std::chrono::duration_cast<std::chrono::nanoseconds>(encode_end - encode_begin);

	std::cout << "Original size: " << bytes.size() * 8 << " bits" << std::endl;
	std::cout << "Encoded size: " << encoded.size() << " bits" << std::endl;
	std::cout << "Compression ratio: " << (double)encoded.size() / (double)(bytes.size() * 8) << std::endl;
	std::cout << "Compression factor: " << (double)((double)bytes.size() * 8) / (double)encoded.size() << std::endl;
	std::cout << "Saving percentage: " << (double)((double)bytes.size() * 8 - (double)encoded.size()) / (double)(bytes.size() * 8) << std::endl;
	std::cout << "Encode time: " << encode_time.count() << " ns" << std::endl;
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

void write_bytes_from_file(std::string filename, std::vector<unsigned char> bytes)
{
	std::ofstream output(filename, std::ios::binary);

	output.write(reinterpret_cast<char const*>(&bytes[0]), bytes.size());

	output.close();
}