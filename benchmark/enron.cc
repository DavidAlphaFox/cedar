#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include <chrono>

#include <cedar_config.h>
#include <cedar.h>

using Trie = cedar::da<int>;

void usage(const char* namep) {
	std::cerr << "Usage:" << std::endl
		<< "\t" << namep << " <file containing list of files>"
		<< std::endl;
}

std::vector<std::string> split(const std::string& str,
		const char delim = ' ') {
	std::vector<std::string> tokens;
	size_t s = 0;
	size_t e = 0;
	while ((e = str.find(delim, s)) != std::string::npos) {
		if (e != s) {
			tokens.emplace_back(str.substr(s, e-s));
		}
		s = e + 1;
	}
	if (e != s) {
		tokens.emplace_back(str.substr(s));
	}
	return tokens;
}

int add_in_trie(Trie& trie, std::set<std::string>& words,
		const std::string& file,
		size_t& nwords, size_t& nchars) {
	std::ifstream ifs(file);
	for (std::string line; std::getline(ifs, line); ) {
		if (line.empty()) {
			continue;
		}

		auto tokens = split(line);
		if (tokens.empty()) {
			continue;
		}

		for (const auto& token : tokens) {
			if (token.empty()) {
				continue;
			}
			trie.update(token.c_str());
			nchars += token.length();
			++nwords;
			words.emplace(token);
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		usage(argv[0]);
		return EINVAL;
	}

	Trie trie;

	auto s = std::chrono::high_resolution_clock::now();
	std::set<std::string> words;
	std::string filename{argv[1]};
	std::ifstream ifs(filename);
	size_t nchars = 0;
	size_t nwords = 0;
	for (std::string line; std::getline(ifs, line); ) {
		auto rc = add_in_trie(trie, words, line, nwords, nchars);
		assert(rc == 0);
	}
	auto e = std::chrono::high_resolution_clock::now();
	auto insert_time = std::chrono::duration_cast<std::chrono::nanoseconds>(e-s).count();

	size_t unique_chars = 0;
	for (const auto& word : words) {
		unique_chars += word.size();
	}

	s = std::chrono::high_resolution_clock::now();
	for (const auto& word : words) {
		Trie::result_triple_type r;
		r = trie.exactMatchSearch<decltype(r)>(word.c_str());
		assert(r.length == word.length());
	}
	e = std::chrono::high_resolution_clock::now();
	auto query_time = std::chrono::duration_cast<std::chrono::nanoseconds>(e-s).count();

	std::cout << "Trie size in bytes " << trie.all_combined_size() << std::endl
		<< "Total number of nodes (used + unused) in trie "
			<< trie.size() << std::endl
		<< "Total number of used nodes " << trie.nonzero_size() << std::endl
		<< "Total number of words added " << nwords << std::endl
		<< "Total number of characters added " << nchars << std::endl
		<< "Total number of unique words " << words.size() << std::endl
		<< "Total number of characters in unique words " << unique_chars
			<< std::endl
		<< "Total insertion time in nanoseconds " << insert_time << std::endl
		<< "Query time for all unique words in nanoseconds " << query_time << std::endl;

	return 0;
}
