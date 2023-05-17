#include "jsmnreplica.h"

/* parse testing function
testing if the parseInput() function works correctly
*/
void parseInput_test() {
  string jsonString;
  cout << "Enter a JSON string: ";
  getline(cin, jsonString);

  jsonparser parser;
  jsmn_init(&parser);

  // Estimate the number of tokens needed
  const int num_tokens = 128;
  vector<jsontoken> tokens(num_tokens);

  // Parse the JSON string
  const int ret = parseInput(&parser, jsonString.c_str(), jsonString.length(), tokens.data(), tokens.size());
  if (ret < 0) {
      cerr << "Failed to parse JSON Input: " << ret << endl;
      exit(1);
  }

  cout << "Parsed successfully!" << endl;

  // Access the parsed tokens
  for (int i = 0; i < ret; ++i) {
      cout << "Token " << i << ": "
                << "type=" << tokens[i].type << " "
                << "start=" << tokens[i].startPos << " "
                << "end=" << tokens[i].endPos << " "
                << "size=" << tokens[i].size << " "
                << "parent=" << tokens[i].parentLink << endl;
  }
}

/* primitive parsing test function
testing if the primitiveInputParsing() function works correctly
*/
void parsePrimitive_test() {
  string jsonString;
  cout << "Enter a JSON string: ";
  getline(cin, jsonString);

  jsonparser parser;
  jsmn_init(&parser);

  const int num_tokens = 5;
  vector<jsontoken> tokens(num_tokens);

  const int ret = primitiveInputParsing(&parser, jsonString.c_str(), jsonString.length(), tokens.data(), tokens.size());
  if (ret < 0) {
    cerr << "Invalid Primitive Type Entry: " << ret << endl;
    exit(1);
  }

  cout << "Parsed successfully!" << endl;
}

/* string parsing test function
testing if the stringParsing() function works correctly
*/
void parseString_test() {
  string jsonString;
  cout << "Enter a JSON string: ";
  getline(cin, jsonString);

  jsonparser parser;
  jsmn_init(&parser);

  const int num_tokens = 5;
  vector<jsontoken> tokens(num_tokens);

  // Parse the JSON string
  const int ret = stringParsing(&parser, jsonString.c_str(), jsonString.length(), tokens.data(), tokens.size());
  if (ret < 0) {
    cerr << "Invalid String Type Entry: " << ret << endl;
    exit(1);
  }

  cout << "Parsed successfully!" << endl;
}

/* testing input from a file system, reads files and parses, printing out tokens, 
types, sizes, and parent links 
*/
void inputFileTest() {
  ifstream inFS("tests/example1.json");
  string jsonString;

  jsonString.assign( (istreambuf_iterator<char>(inFS)), (istreambuf_iterator<char>()));

  jsonparser parser;
  jsmn_init(&parser);

  const int num_tokens = 128;
  vector<jsontoken> tokens(num_tokens);

  const int ret = parseInput(&parser, jsonString.c_str(), jsonString.length(), tokens.data(), tokens.size());
  if (ret < 0) {
      cerr << "Failed to parse JSON Input: " << ret << endl;
      exit(1);
  }

  cout << "Parsed successfully!" << endl;

  for (int i = 0; i < ret; ++i) {
      cout << "Token " << i << ": "
                << "type=" << tokens[i].type << " "
                << "start=" << tokens[i].startPos << " "
                << "end=" << tokens[i].endPos << " "
                << "size=" << tokens[i].size << " "
                << "parent=" << tokens[i].parentLink << endl;
  }
}

int main() {

  // parse Input test
  // parseInput_test();

  // // parse Primitive values test
  // parsePrimitive_test();

  // // parse String values test
  // parseString_test();

  inputFileTest();
  
  return 0;
}