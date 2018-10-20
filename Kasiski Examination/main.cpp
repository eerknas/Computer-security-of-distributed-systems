#include "gen_func.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <ctime>

int main() {
	srand(time(NULL));
	// tests with key length on 6786
	std::ifstream inf("input.txt");
	std::ofstream ouf("output.txt");
	std::fstream infv;
	infv.open("input_validated.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);

	additional_func::validate_and_correct(inf, infv);
	infv.clear();
	infv.seekg(0, std::ios::beg);

	std::stringstream buffer;
	buffer << infv.rdbuf();
	const std::string str(buffer.str());

	infv.close();
	inf.close();

	const int key_lengths[] = { 4, 5, 8, 9, 10, 16, 18, 19, 20, 22, 24, 25 };
	const int str_lengths[] = { 1000, 1500, 4000, 5000, 6500 };

	auto randchar = []() -> char
	{
		const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		const int max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};

	int kasiski_key_length;
	std::string kasiski_key, str_encrypt, key;
	std::vector<int> freq_divs;
	std::vector<double> probs;

	auto number_of_tries = 10;
	double success;

	for (auto key_length : key_lengths) {
		ouf << "key_length: " << key_length << std::endl;
		for (auto str_length : str_lengths) {
			success = 0;
			for (auto i = 0; i < number_of_tries; ++i) {
				//std::cout << key << std::endl;
				key.resize(key_length, 0);
				std::generate_n(key.begin(), key_length, randchar);
				str_encrypt = encrypt::vigenere_cipher::encrypt(str.substr(0, str_length), key);
				freq_divs = decrypt::kasiski_examination::get_most_freq_divs(str_encrypt);
				//std::copy(freq_divs.begin(), freq_divs.end(), std::ostream_iterator<double>(std::cout, " "));
				//std::cout << std::endl;
				kasiski_key_length = decrypt::kasiski_examination::get_key_length_lcm_version(freq_divs);
				std::cout << key_length << " VS " << kasiski_key_length << std::endl;
				if (key_length == kasiski_key_length) {
					if (key == decrypt::kasiski_examination::get_key_value(str_encrypt, kasiski_key_length)) {
						++success;
					}
				}
			}
			std::cout << str_length << ": iteration finished" << std::endl;
			ouf << str_length << ' ' << success / number_of_tries << std::endl;
		}
		ouf << std::endl;
	}
	std::cout << "The end" << std::endl;
	ouf.close();
	//std::copy(probs.begin(), probs.end(), std::ostream_iterator<double>(std::cout, " "));

	/*auto freq_divs = decrypt::kasiski_examination::get_most_freq_divs(str);
	auto key_length = decrypt::kasiski_examination::get_key_length_lcm_version(freq_divs);

	auto key_value = decrypt::kasiski_examination::get_key_value(str, key_length);
	std::cout << "key_length: " << key_length << std::endl
		<< "key_value: " << key_value << std::endl;*/
	

	/*std::map<int, int> m;
	m.insert({ 1,1 });
	auto it = m.cend();
	m.insert({ 2,2 });
	std::cout << (--it)->first << std::endl;*/
	std::cin.get();
}