// Copyright 2026 UNN-IASR
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "textgen.h"

TEST(TextGenTest, MakePrefix_TwoWords) {
  TextGenerator gen;
  std::vector<std::string> words = {"hello", "world",
                                     "test"};
  prefix p = gen.make_prefix(words);
  ASSERT_EQ(p.size(), 2);
  EXPECT_EQ(p[0], "hello");
  EXPECT_EQ(p[1], "world");
}

TEST(TextGenTest, MakePrefix_OneWord) {
  TextGenerator gen;
  std::vector<std::string> words = {"hello"};
  prefix p = gen.make_prefix(words);
  ASSERT_EQ(p.size(), 1);
  EXPECT_EQ(p[0], "hello");
}

TEST(TextGenTest, BuildPrefix_Shift) {
  TextGenerator gen;
  prefix p = {"first", "second"};
  prefix new_p = gen.build_prefix(p, "third");
  ASSERT_EQ(new_p.size(), 2);
  EXPECT_EQ(new_p[0], "second");
  EXPECT_EQ(new_p[1], "third");
}

TEST(TextGenTest, ReadFile_BuildsTable) {
  TextGenerator gen(42);
  std::ofstream tmp("test_input.txt");
  tmp << "the quick brown fox";
  tmp.close();
  gen.read_file("test_input.txt");
  const auto& states = gen.get_states();
  prefix p = {"the", "quick"};
  EXPECT_TRUE(states.find(p) != states.end());
  std::remove("test_input.txt");
}

TEST(TextGenTest, ChooseSuffix_SingleOption) {
  TextGenerator gen(123);
  prefix p = {"only", "one"};
  auto& states = const_cast<statetab&>(gen.get_states());
  states[p] = {"choice"};
  std::string suffix = gen.choose_suffix(p);
  EXPECT_EQ(suffix, "choice");
}

TEST(TextGenTest, ChooseSuffix_MultipleOptions) {
  TextGenerator gen(456);
  prefix p = {"test", "prefix"};
  auto& states = const_cast<statetab&>(gen.get_states());
  states[p] = {"option1", "option2", "option3"};
  std::string suffix = gen.choose_suffix(p);
  const auto& options = states[p];
  bool found = false;
  for (const auto& opt : options) {
    if (opt == suffix) {
      found = true;
      break;
    }
  }
  EXPECT_TRUE(found);
}

TEST(TextGenTest, Generate_FixedLength) {
  TextGenerator gen(789);
  auto& states = const_cast<statetab&>(gen.get_states());
  states[{"a", "b"}] = {"c"};
  states[{"b", "c"}] = {"a"};
  states[{"c", "a"}] = {"b"};
  std::string result = gen.generate(10);
  auto words = split_words(result);
  EXPECT_GE(words.size(), 10);
}

TEST(TextGenTest, Generate_EmptyStates) {
  TextGenerator gen;
  std::string result = gen.generate(100);
  EXPECT_TRUE(result.empty());
}

TEST(TextGenTest, SplitWords_CleansPunctuation) {
  std::string text = "Hello, World! It's a test.";
  auto words = split_words(text);
  ASSERT_EQ(words.size(), 5);
  EXPECT_EQ(words[0], "hello");
  EXPECT_EQ(words[1], "world");
  EXPECT_EQ(words[2], "it's");
  EXPECT_EQ(words[3], "a");
  EXPECT_EQ(words[4], "test");
}

TEST(TextGenTest, ReadFile_NonExistent) {
  TextGenerator gen;
  EXPECT_THROW(gen.read_file("nonexistent_file.txt"),
               std::runtime_error);
}
