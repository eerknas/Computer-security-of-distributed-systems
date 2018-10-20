#include "stdafx.h"
#include "gen_func.h"

//#include <vector>
//#include <cassert>
#include <iostream>
#include <cctype>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iterator> 
#include <set>
#include <unordered_map>
#include <valarray>
#include <numeric>

namespace encrypt {

	void vigenere_cipher::encrypt(std::istream& inf, std::ostream& ouf, std::string const& key) {
		std::stringstream buffer;
		// return to the beggining
		buffer << inf.rdbuf();

		std::string str(buffer.str());
		ouf << encrypt(str, key);
	}

	std::string vigenere_cipher::encrypt(const std::string& str, std::string const& key) {
		auto key_length = key.size();
		std::string result;

		long long counter = 0;
		for (auto current : str) {
			if (current != ' ' && current != '\n') {
				current = 'A' + (key.at(counter % key_length) + current - 2 * 'A') % table_length;
			}
			result += current;

			++counter;
		}
		return result;
	}

	std::string get_key_from_console() {
		std::string key;

		std::cout << "Enter a key..." << std::endl;

		while (std::getline(std::cin, key)) {
			if (key.size() != 0 && std::all_of(begin(key),
				end(key),
				[](const char& current) {
				return std::isalpha(current) &&
					std::isupper(current); })) {
				return key;
			}
			std::cout << "Enter a key with only uppercase letters!" << std::endl;
		}
	}
}

namespace decrypt {
	void vigenere_cipher::decrypt(std::istream& inf, std::ostream& ouf, std::string const& key) {
		auto key_length = key.size();

		char current;
		unsigned long long counter = 0;
		while (inf.get(current)) {
			if (current != ' ' && current != '\n') {
				current = 'A' + (table_length + current - key.at(counter % key_length)) % table_length;
			}
			ouf << current;
			++counter;
		}
	}

	std::vector<int> kasiski_examination::get_most_freq_divs(const std::string& str) {
		additional_func::state_autamata state_aut;
		
		std::string sub_str;
		state_aut.init(str);
		std::unordered_map<std::string, std::set<int>> entries;
		
		int length = 3;
		std::pair<std::unordered_map<std::string, std::set<int>>::iterator, bool> insertion;

		// work with subjstring of length 3 (threegrams)
		for (auto i = 0; i < str.size() - length; ++i) {
			//std::cout << length << ' ' << i << std::endl;
			sub_str = str.substr(i, length);
			//std::cout << sub_str << std::endl;
			insertion = entries.insert({ sub_str, std::set<int>() });
			if (insertion.second) {
				auto hit_state = state_aut.get_first_hit_state(sub_str);
				//std::cout << hit_state << std::endl;

				state_aut.output_all_occurences(hit_state, length, insertion.first->second);
				// no doubles, triples or etc.
				if (insertion.first->second.size() == 1) {
					entries.erase(insertion.first);
				}
			}
		}

		int i = 0;
		for (auto it = entries.begin(); it != entries.end(); ++it) {
			sub_str = it->first;
			for (const auto& element : it->second) {
				if (element + sub_str.size() >= str.size()) {
					continue;
				}
				insertion = entries.insert({ sub_str + str[element + sub_str.size()], std::set<int>() });
				if (insertion.second) {
					auto hit_state = state_aut.get_first_hit_state(sub_str + str[element + sub_str.size()]);
					state_aut.output_all_occurences(hit_state, sub_str.size() + 1, insertion.first->second);
					if (insertion.first->second.size() == 1) {
						entries.erase(insertion.first);
					}
				}
			}
		}
		//for (auto length = 4; entries_changed && length < str.size() - 1; ++length) {
		//	entries_changed = false;
		//	for (auto i = 0; i < str.size() - length; ++i) {
		//		//std::cout << length << ' ' << i << std::endl;
		//		sub_str = str.substr(i, length);
		//		//std::cout << sub_str << std::endl;
		//		if (entries.insert({ sub_str, std::set<int>() }).second) {
		//			auto hit_state = state_aut.get_first_hit_state(sub_str);
		//			//std::cout << hit_state << std::endl;
		//			state_aut.output_all_occurences(hit_state, length, entries[sub_str]);
		//			// no doubles, triples or etc.
		//			if (entries[sub_str].size() == 1) {
		//				entries.erase(sub_str);
		//			}
		//			else {
		//				entries_changed = true;
		//			}
		//		}
		//	}
		//}

		/*for (auto it = entries.begin(); it != entries.end(); ++it) {
			std::cout << it->first << ": ";
			for (auto it_vec = it->second.begin(); it_vec != it->second.end(); ++it_vec) {
				std::cout << *it_vec << ' ';
			}
			std::cout << std::endl;
		}*/

		// fckin black magic
		if (entries.size()) {
			std::unordered_map<int, int> frequencies;
			int distance, temp;
			std::pair<std::unordered_map<int, int>::iterator, bool> insertion;
			for (auto key_it = entries.begin(); key_it != entries.end(); ++key_it) {
				for (auto set_it = key_it->second.begin(); set_it != --(key_it->second.end());) {
					distance = -*(set_it)+*(++set_it);
					for (temp = 1; temp * temp < distance; ++temp) {
						if (distance % temp == 0) {
							insertion = frequencies.insert({ temp, 0 });
							insertion.first->second += 1;
							insertion = frequencies.insert({ distance / temp, 0 });
							insertion.first->second += 1;
						}
					}

					if (temp * temp == distance) {
						insertion = frequencies.insert({ temp, 0 });
						insertion.first->second += 1;
					}
				}
			}
			// erase 1 as it's all elements divisor;
			frequencies.erase(1);

			//std::cout << std::endl;
			//for (auto it = frequencies.begin(); it != frequencies.end(); ++it) {
			//	//if (it->second > 100) {
			//		std::cout << it->first << ": " << it->second << std::endl;
			//	//}
			//}

			std::vector<int> values;

			std::transform(
				frequencies.begin(),
				frequencies.end(),
				std::back_inserter(values),
				[](const std::unordered_map<int, int>::value_type& pair) { return pair.second; });

			
			int max_difference = 0, that_value;
			std::sort(values.begin(), values.end(), std::greater<int>());

			//std::copy(values.begin(), values.end(), std::ostream_iterator<int>(std::cout, " "));
			//std::cout << std::endl;

			for (auto i = 0; i < values.size() - 1; ++i) {
				if (max_difference <= values[i] - values[i + 1]) {
					max_difference = values[i] - values[i + 1];
					that_value = values[i];
				}
			}

			values.clear();
			for (auto it = frequencies.begin(); it != frequencies.end(); ++it) {
				if (it->second >= that_value) {
					values.push_back(it->first);
				}
			}
			std::sort(values.begin(), values.end(), [&frequencies](const auto& first, const auto& second) {
				if (frequencies[first] == frequencies[second]) {
					return first < second;
				}
				return frequencies[first] > frequencies[second];
			});

			//std::copy(values.begin(), values.end(), std::ostream_iterator<int>(std::cout, " "));
			//std::cout << std::endl;
			return values;
		}
		return std::vector<int>();
		/*auto max_freq = std::max_element(frequencies.begin(), frequencies.end(),
			[](const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
			return p1.second < p2.second; });

		for (auto key_it = frequencies.begin(); key_it != frequencies.end(); ++key_it) {

		}*/
		/*if (entries.size()) {
			auto lower_bound = entries.begin(), higher_bound = --entries.end();
			
			bool size_changed;
			do {
				size_changed = false;
				for (auto first_it = lower_bound; first_it != std::next(higher_bound); ++first_it) {
					size_changed |= additional_func::check_and_add_set_intersect(first_it, first_it, entries, str.size());
					for (auto second_it = std::next(first_it, 1); second_it != std::next(higher_bound); ++second_it) {
						size_changed |= additional_func::check_and_add_set_intersect(first_it, second_it, entries, str.size());
						size_changed |= additional_func::check_and_add_set_intersect(second_it, first_it, entries, str.size());
					}
				}
				higher_bound = --entries.end();
				lower_bound = std::next(higher_bound);
			} while (size_changed);
		}*/
	}

	int kasiski_examination::get_key_length_lcm_version(std::vector<int>& freq_divs) {
		if (freq_divs.size()) {
			int result = freq_divs[0];
			bool change_operator = false;
			for (auto i = 0; i < freq_divs.size() - 1; ++i) {
				auto temp = result * freq_divs[i + 1] / additional_func::gcd(result, freq_divs[i + 1]);
				if (!change_operator) {
					result = temp;
					if (result == freq_divs[i + 1]) {
						change_operator = true;
					}
				}
				else {
					if (temp != freq_divs[i + 1]) {
						break;
					}
					result = temp;
				}
			}
			return result;
		}
		return -1;
	}

	std::string kasiski_examination::get_key_value(const std::string& str, int key_length) {
		//std::cout << "Entered" << std::endl;
		if (key_length > 0) {
			std::vector<std::valarray<double>> shifts_freq(key_length);
			std::for_each(shifts_freq.begin(), shifts_freq.end(), [](auto& array) {
				array.resize(table_length, 0);
			});
			for (auto i = 0; i < str.size(); ++i) {
				if (str[i] == ' ' || str[i] == '\n') {
					continue;
				}
				++shifts_freq[i % key_length][str[i] - 'A'];
			}
			std::string result;
			double difference, temp;
			int position = 0;
			for (auto i = 0; i < shifts_freq.size(); ++i) {
				/*if (shifts_freq[i].sum() == 0) {
					std::cout << "WTF" << std::endl;
					return "";
				}*/
				shifts_freq[i] /= shifts_freq[i].sum();
				difference = DBL_MAX;
				//std::cout << i << std::endl;
				for (auto j = 0; j < table_length; ++j) {
					temp = additional_func::norm(shifts_freq[i].cshift(j));
					if (difference > temp) {
						position = j;
						difference = temp;
					}
				}
				result += (char)('A' + position);
			}
			//std::cout << "Finished" << std::endl;
			return result;
		}
		return "";
	}
}

namespace additional_func {
	void state_autamata::init(std::string const& str) {
		//assert(str.size() > 0);

		states = new state[2 * str.size()];
		size = last = 0;
		states[0].num = 0;
		states[0].link = -1;
		++size;
		for (auto symbol : str) {
			add(symbol);
		}

		for (int v = 1; v < size; ++v) {
			states[states[v].link].inv_link.push_back(v);
		}
	}

	void state_autamata::add(char c) {
		int cur = size++;
		states[cur].num = states[last].num + 1;
		states[cur].first_pos = states[cur].num - 1;
		states[cur].is_clone = false;
		int p;
		for (p = last; p != -1 && !states[p].next.count(c); p = states[p].link)
			states[p].next[c] = cur;
		if (p == -1)
			states[cur].link = 0;
		else {
			int q = states[p].next[c];
			if (states[p].num + 1 == states[q].num)
				states[cur].link = q;
			else {
				int clone = size++;
				states[clone].is_clone = true;
				states[clone].num = states[p].num + 1;
				states[clone].next = states[q].next;
				states[clone].link = states[q].link;
				states[clone].first_pos = states[q].first_pos;
				for (; p != -1 && states[p].next[c] == q; p = states[p].link)
					states[p].next[c] = clone;
				states[q].link = states[cur].link = clone;
			}
		}
		last = cur;
	}

	int state_autamata::get_first_hit_state(std::string const& str) {
		auto p = 0;
		for (auto symbol : str) {
			if (states[p].next.count(symbol)) {
				p = states[p].next[symbol];
			}
			else {
				return -1;
			}
		}
		return p;
	}

	void state_autamata::output_all_occurences(int v, int p_length, std::set<int>& set) {
		if (!states[v].is_clone) {
			set.insert(states[v].first_pos - p_length + 1);
		}
		for (size_t i = 0; i < states[v].inv_link.size(); ++i) {
			output_all_occurences(states[v].inv_link[i], p_length, set);
		}
	}

	int gcd(int u, int v) {
		if (v) {
			return gcd(v, u % v);
		}
		return u < 0 ? -u : u;
	}

	int multi_lcm(std::vector<int>& num) {
		if (num.size() == 0) return 0;
		else if (num.size() == 1) return num[0];
		else if (num.size() == 2) return num[0] * num[1] / gcd(num[0], num[1]);

		num[1] = num[0] * num[1] / gcd(num[0], num[1]);
		num.erase(num.begin());
		return multi_lcm(num);
	}

	double norm(const std::valarray<double>& shifts_freq) {
		return  std::sqrt(std::pow(engl_letter_freq - shifts_freq, 2.0).sum());
	}

	void validate_and_correct(std::ifstream& file, std::fstream& validated_file) {
		char current;
		bool extra_space = 0;
		while (file.get(current)) {
			if (current == ' ' || current == '\n' ||
				(current >= 'A' && current <= 'Z')) {
				if (current == ' ') {
					if (!extra_space) {
						validated_file << current;
						extra_space = true;
					}
				}
				else {
					validated_file << current;
					extra_space = false;
				}
			}
			else if (current >= 'a' && current <= 'z') {
				validated_file << (char)(current + 'A' - 'a');
				extra_space = false;
			}
		}
	}
}