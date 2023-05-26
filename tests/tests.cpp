#include "../lib/jsmnreplica.cpp"
#include <fstream>
#include "gtest/gtest.h"

TEST(parseInputTests, firstParseInputTest) {
  ifstream inFS("tests/example1.json");
  string jsonString;

  jsonString.assign( (istreambuf_iterator<char>(inFS)), (istreambuf_iterator<char>()));

  jsonparser parser;
  jsmn_init(&parser);

  const int num_tokens = 128;
  vector<jsontoken> tokens(num_tokens);

  const int ret = parseInput(&parser, jsonString.c_str(), jsonString.length(), tokens.data(), tokens.size());
  EXPECT_TRUE(ret >= 0);
}

TEST(parseStringTests, firstStringTest) {
  string jsonString;
  cout << "Enter a JSON string: ";
  getline(cin, jsonString);

  jsonparser parser;
  jsmn_init(&parser);

  const int num_tokens = 5;
  vector<jsontoken> tokens(num_tokens);

  // Parse the JSON string
  const int ret = stringParsing(&parser, jsonString.c_str(), jsonString.length(), tokens.data(), tokens.size());
  EXPECT_TRUE(ret >= 0);
}