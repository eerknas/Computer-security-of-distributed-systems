#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <valarray>

const auto table_length = 'Z' - 'A' + 1;
const std::valarray<double> engl_letter_freq = {	
	0.08167, 0.01492, 0.02782, 0.04253, 0.12702,
	0.0228, 0.02015, 0.06094, 0.06966, 0.00153,
	0.00772, 0.04025, 0.02406, 0.06749, 0.07507,
	0.01929, 0.00095, 0.05987, 0.06327, 0.09056,
	0.02758, 0.00978, 0.0236, 0.0015, 0.01974,
	0.00074, };

namespace encrypt {
	class vigenere_cipher {
	public:
		static void encrypt(std::istream& inf, std::ostream& ouf, std::string const& key);
		static std::string encrypt(const std::string& str, std::string const& key);
	};

	std::string get_key_from_console();
}

namespace decrypt {
	class vigenere_cipher {
	public:
		static void decrypt(std::istream& inf, std::ostream& ouf, std::string const& key);
	};

	class kasiski_examination {
	public:
		static std::vector<int> get_most_freq_divs(const std::string& str);
		static int get_key_length_lcm_version(std::vector<int>& freq_divs);
		static std::string get_key_value(const std::string& str, int key_length);
	};

	
}

namespace additional_func {
	class state_autamata {
	private:
		struct state {
			int num, link, first_pos;
			bool is_clone;
			std::vector<int> inv_link;
			std::map<char, int> next;
		};
		state* states;
		int size, last;

		void add(char c);
	public:
		void init(std::string const& str);
		int get_first_hit_state(std::string const& str);
		void output_all_occurences(int v, int p_length, std::set<int>& set);
	};

	int gcd(int u, int v);
	int multi_lcm(std::vector<int>& num);
	double norm(const std::valarray<double>& shifts_freq);
	void validate_and_correct(std::ifstream& file, std::fstream& validated_file);
}

