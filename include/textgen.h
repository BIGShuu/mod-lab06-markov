// Copyright 2026 UNN-IASR
#ifndef INCLUDE_TEXTGEN_H_
#define INCLUDE_TEXTGEN_H_
#include <string>
#include <deque>
#include <map>
#include <vector>
#include <random>
const int NPREF = 2;
const int MAXGEN = 1000;
typedef std::deque<std::string> prefix;
typedef std::map<prefix, std::vector<std::string>> statetab;

class TextGenerator {
 private:
  statetab states;
  mutable std::mt19937 rng;

 public:
  explicit TextGenerator(unsigned seed = 42);
  void read_file(const std::string& filename);
  std::string generate(int max_words = MAXGEN);
  const statetab& get_states() const { return states; }
  prefix make_prefix(const std::vector<std::string>& words) const;
  std::string choose_suffix(const prefix& p) const;
  prefix build_prefix(const prefix& p, const std::string& word) const;
};
std::vector<std::string> split_words(const std::string& text);
#endif  // INCLUDE_TEXTGEN_H_
