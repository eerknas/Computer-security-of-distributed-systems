#include "gen_func.h"

#include <iostream>
#include <fstream>
#include <valarray>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <functional>
#include <ios>

int main() {
	std::ifstream inf("input_crypt.txt");
	std::ofstream ouf_decrypt("output_decrypt.txt");
	std::fstream infv, ouf_encrypt;
	infv.open("input_validated.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
	ouf_encrypt.open("ouput_crypt.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
	
	additional_func::validate_and_correct(inf, infv);
	infv.clear();
	infv.seekg(0, std::ios::beg);

	std::string key = encrypt::get_key_from_console();
	encrypt::vigenere_cipher::encrypt(infv, ouf_encrypt, key);

	ouf_encrypt.clear();
	ouf_encrypt.seekg(0, std::ios::beg);
	decrypt::vigenere_cipher::decrypt(ouf_encrypt, ouf_decrypt, key);

	inf.close();
	infv.close();
	ouf_encrypt.close();
	ouf_decrypt.close();

	system("pause");
}