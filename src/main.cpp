// Copyright 2026 UNN-IASR
#include <iostream>
#include <fstream>
#include <string>
#include "textgen.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0]
              << " <input_file> <output_file> [seed]"
              << std::endl;
    return 1;
  }
  std::string input_file = argv[1];
  std::string output_file = argv[2];
  unsigned seed = (argc >= 4) ? std::stoi(argv[3]) : 42;
  try {
    TextGenerator gen(seed);
    gen.read_file(input_file);
    std::string text = gen.generate(MAXGEN);
    std::ofstream out(output_file);
    if (!out.is_open()) {
      std::cerr << "Cannot open output file: "
                << output_file << std::endl;
      return 2;
    }
    out << text;
    out.close();
    std::cout << "Generated " << MAXGEN
              << " words to " << output_file
              << std::endl;
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 3;
  }
}
