// Copyright 2026 UNN-IASR
#include "textgen.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

static std::string clean_word(const std::string& word) {
  std::string result;
  for (char c : word) {
    if (std::isalpha(static_cast<unsigned char>(c)) ||
        c == '-' || c == '\'') {
      result += std::tolower(static_cast<unsigned char>(c));
    }
  }
  return result;
}

std::vector<std::string> split_words(const std::string& text) {
  std::vector<std::string> words;
  std::istringstream iss(text);
  std::string word;
  while (iss >> word) {
    std::string cleaned = clean_word(word);
    if (!cleaned.empty()) {
      words.push_back(cleaned);
    }
  }
  return words;
}

TextGenerator::TextGenerator(unsigned seed) : rng(seed) {}
prefix TextGenerator::build_prefix(
    const prefix& p, const std::string& word) const {
  prefix new_prefix = p;
  if (new_prefix.size() >= static_cast<size_t>(NPREF)) {
    new_prefix.pop_front();
  }
  new_prefix.push_back(word);
  return new_prefix;
}
void TextGenerator::read_file(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }
  std::string line;
  std::vector<std::string> all_words;
  while (std::getline(file, line)) {
    auto words = split_words(line);
    all_words.insert(all_words.end(), words.begin(),
                     words.end());
  }
  file.close();
  if (all_words.size() < static_cast<size_t>(NPREF + 1)) {
    return;
  }
  for (size_t i = 0; i + NPREF < all_words.size(); ++i) {
    prefix p;
    for (int j = 0; j < NPREF; ++j) {
      p.push_back(all_words[i + j]);
    }
    states[p].push_back(all_words[i + NPREF]);
  }
}
std::string TextGenerator::choose_suffix(const prefix& p)
    const {
  auto it = states.find(p);
  if (it == states.end() || it->second.empty()) {
    return "";
  }
  std::uniform_int_distribution<size_t> dist(
      0, it->second.size() - 1);
  return it->second[dist(rng)];
}

prefix TextGenerator::make_prefix(
    const std::vector<std::string>& words) const {
  prefix p;
  for (size_t i = 0;
       i < std::min(words.size(), static_cast<size_t>(NPREF));
       ++i) {
    p.push_back(words[i]);
  }
  return p;
}
std::string TextGenerator::generate(int max_words) {
  if (states.empty()) {
    return "";
  }
  std::uniform_int_distribution<size_t> start_dist(
      0, states.size() - 1);
  auto it = states.begin();
  std::advance(it, start_dist(rng));
  prefix current = it->first;
  std::ostringstream result;
  for (const auto& word : current) {
    result << word << " ";
  }
  int generated = static_cast<int>(current.size());
  while (generated < max_words) {
    std::string suffix = choose_suffix(current);
    if (suffix.empty()) {
      it = states.begin();
      std::advance(it, start_dist(rng));
      current = it->first;
      continue;
    }
    result << suffix << " ";
    current = build_prefix(current, suffix);
    ++generated;
  }
  return result.str();
}
