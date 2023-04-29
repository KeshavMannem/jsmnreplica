#ifndef NEWJSMN_CPP
#define NEWJSMN_CPP
#include <iostream>
#include <vector>
#include <cstddef>
using namespace std;

/* types parser can encounter
Includes objects, arrays, strings, and primitive types, which are integers, bool, etc.
Objects are those that contain a '{', and arrays are those with '['
*/
typedef enum {
  undefinedType = -1,
  objectType = 0,
  arrayType = 1,
  stringType = 2,
  primitiveType = 3
} jsontype;

/* error types the parser can encounter
if there are not enough tokens to place the JSON text into
if the character the parser reads is an invalid character
if the JSON string is not filled with enough bytes it should have
*/ 
enum errorTypes {
  notEnoughTokensErr = -1, // not enough tokens provided
  invalidCharacterErr = -2, // invalid character read
  unfilledJSONErr = -3  // more bytes are expected
};

/* token struct
this is used to create a token, which has attributes of the type (object, array, string, primitive)
also contains the starting position of the token, or the starting index 
contains the ending index for the token as well
if the token has a parent, (for example it is within an object or array), the token also has 
access to the starting index of it's parent
*/
typedef struct token {
  jsontype type; // object, array, string, primitive
  int startPos; // start position in string
  int endPos; // end position in string
  int size;  // size of string
  int parentLink; // index of parent if string has a parent
} jsontoken;


typedef struct parser {
  unsigned int position; // position of parser in string  
  unsigned int nextToken; // next token to be used 
  int parentToken; // parent object of current token      
} jsonparser;

/* fill token function
takes in a token as a parameter, as well as the type and starting and ending position
fills the token with the given type, start and end
sets the size to 0, and this size will increase as the string is parsed
 */
static void fill(jsontoken *token, const jsontype type,
                            const int start, const int end) {
  token->type = type; 
  token->startPos = start;
  token->endPos = end;
  token->size = 0;
}

/*allocate token function
creates a new token for the current string that is being parsed by the parser
if there are not enough tokens to allocate a new token, nullptr is returned
otherwise, a new token is allocated from the tkoens array and sets this token's
start and end to -1, and the size to 0 as well, and it has no parent just yet
*/
static jsontoken *allocateToken(jsonparser *parser, jsontoken *tokens,
                                   const size_t num_tokens) {
  jsontoken *tok;
  if (parser->nextToken >= num_tokens) {
    return nullptr;
  }
  tok = &tokens[parser->nextToken++];
  tok->startPos = -1;
  tok->endPos = -1;
  tok->size = 0;
  tok->parentLink = -1;
  return tok;
}

/* string parsing method
This function takes in the parser, a character, the length, the array 
of tokens, and the number of tokens and fills the next available token
with a string. 
 */
static int stringParsing(jsonparser *parser, const char *ch,
                          const size_t length, jsontoken *tokens,
                          const size_t numTokens) {
  jsontoken *token;

  int start = parser->position;
  
  parser->position++;
  
  while (parser->position < length && ch[parser->position] != '\0') {
    char c = ch[parser->position];

    if (c == '\"') {
      if (tokens == nullptr) {
        return 0;
      }
      token = allocateToken(parser, tokens, numTokens);
      if (token == nullptr) {
        parser->position = start;
        return notEnoughTokensErr;
      }
      fill(token, stringType, start + 1, parser->position);
      token->parentLink = parser->parentToken;
      return 0;
    }

    if (c == '\\' && parser->position + 1 < length) {
      int i;
      parser->position++;
      switch (ch[parser->position]) {
      case '\"':
      case '/':
      case '\\':
      case 'b':
      case 'f':
      case 'r':
      case 'n':
      case 't':
        break;
      case 'u':
        parser->position++;
        for (i = 0; i < 4 && parser->position < length && ch[parser->position] != '\0';
             i++) {
          if (!((ch[parser->position] >= 48 && ch[parser->position] <= 57) ||   /* 0-9 */
                (ch[parser->position] >= 65 && ch[parser->position] <= 70) ||   /* A-F */
                (ch[parser->position] >= 97 && ch[parser->position] <= 102))) { /* a-f */
            parser->position = start;
            return invalidCharacterErr;
          }
          parser->position++;
        }
        parser->position--;
        break;
      default:
        parser->position = start;
        return invalidCharacterErr;
      }
    }
    parser->position++;
  }
  parser->position = start;
  return unfilledJSONErr;
}

/*primitive parsing function
This function takes in the parser, a character, the length, the array 
of tokens, and the number of tokens and fills the next available token
with a primitive type. 
 */
static int primitiveInputParsing(jsonparser *parser, const char *ch,
                                const size_t length, jsontoken *tokens,
                                const size_t numTokens) {
  jsontoken *token;
  int start;

  start = parser->position;

  //switch statements for different cases parser might encounter
  while (parser->position < length && ch[parser->position] != '\0') {
    switch (ch[parser->position]) {
    case ':':
    case '\t':
    case '\r':
    case '\n':
    case ' ':
    case ',':
    case ']':
    case '}':
      goto characterCases;
    }
    if (ch[parser->position] < 32 || ch[parser->position] >= 127) {
      parser->position = start;
      return invalidCharacterErr;
    }
    parser->position++;
  }
  parser->position = start;
  return unfilledJSONErr;

// if one of the cases is encountered, the statement comes here and executes
// the following lines
characterCases:
  if (tokens == nullptr) {
    parser->position--;
    return 0;
  }
  token = allocateToken(parser, tokens, numTokens);
  if (token == nullptr) {
    parser->position = start;
    return notEnoughTokensErr;
  }
  fill(token, primitiveType, start, parser->position);
  token->parentLink = parser->parentToken;
  parser->position--;
  return 0;
}

/* parseInput function
This function is the main parsing function that takes in the parser, 
character, length, array of tokens, and number of Tokens as parameters. 
This function traverses over the input and adjusts the parentLinks of 
the string depending on if they are within an object, or an array. It
also calls the parse primitive and parse string functions whenever the
parser encounters a primitive type or a string type respectively. It 
returns the index of the next available token that is available to be parsed. 
 */
int parseInput(jsonparser *parser, const char *ch, const size_t length,
                        jsontoken *tokens, const unsigned int numTokens) {
  int r;
  int i;
  jsontoken *token;
  int count = parser->nextToken;

  while (parser->position < length && ch[parser->position] != '\0') {
    char c;
    jsontype type;

    c = ch[parser->position];
    switch (c) {
    case '{':
    case '[':
      count++;
      if (tokens == nullptr) {
        break;
      }
      token = allocateToken(parser, tokens, numTokens);
      if (token == nullptr) {
        return notEnoughTokensErr;
      }
      if (parser->parentToken != -1) {
        jsontoken *t = &tokens[parser->parentToken];
        if (t->type == objectType) {
          return invalidCharacterErr;
        }
        t->size++;
        token->parentLink = parser->parentToken;
      }
      token->type = (c == '{' ? objectType : arrayType);
      token->startPos = parser->position;
      parser->parentToken = parser->nextToken - 1;
      break;
    case '}':
    case ']':
      if (tokens == nullptr) {
        break;
      }
      type = (c == '}' ? objectType : arrayType);
      if (parser->nextToken < 1) {
        return invalidCharacterErr;
      }
      token = &tokens[parser->nextToken - 1];
      while (true) {
        if (token->startPos != -1 && token->endPos == -1) {
          if (token->type != type) {
            return invalidCharacterErr;
          }
          token->endPos = parser->position + 1;
          parser->parentToken = token->parentLink;
          break;
        }
        if (token->parentLink == -1) {
          if (token->type != type || parser->parentToken == -1) {
            return invalidCharacterErr;
          }
          break;
        }
        token = &tokens[token->parentLink];
      }
      break;
    case '\"':
      r = stringParsing(parser, ch, length, tokens, numTokens);
      if (r < 0) {
        return r;
      }
      count++;
      if (parser->parentToken != -1 && tokens != nullptr) {
        tokens[parser->parentToken].size++;
      }
      break;
    case '\t':
    case '\r':
    case '\n':
    case ' ':
      break;
    case ':':
      parser->parentToken = parser->nextToken - 1;
      break;
    case ',':
      if (tokens != nullptr && parser->parentToken != -1 &&
          tokens[parser->parentToken].type != arrayType &&
          tokens[parser->parentToken].type != objectType) {
        parser->parentToken = tokens[parser->parentToken].parentLink;
      }
      break;
    // for primitive types, which are numbers and boolean statements
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 't':
    case 'f':
    case 'n':
      if (tokens != nullptr && parser->parentToken != -1) {
        const jsontoken *t = &tokens[parser->parentToken];
        if (t->type == objectType ||
            (t->type == stringType && t->size != 0)) {
          return invalidCharacterErr;
        }
      }
      r = primitiveInputParsing(parser, ch, length, tokens, numTokens);
      if (r < 0) {
        return r;
      }
      count++;
      if (parser->parentToken != -1 && tokens != nullptr) {
        tokens[parser->parentToken].size++;
      }
      break;
    default:
      return invalidCharacterErr;
    }
    parser->position++;
  }

  if (tokens != nullptr) {
    for (i = parser->nextToken - 1; i >= 0; i--) {
      if (tokens[i].startPos != -1 && tokens[i].endPos == -1) {
        return unfilledJSONErr;
      }
    }
  }

  return count;
}

/* init function
Creates a new parser that is to be used for the parser with an array of
tokens that is ready to be used in a buffer.   
 */
void jsmn_init(jsonparser *parser) {
  parser->position = 0;
  parser->nextToken = 0;
  parser->parentToken = -1;
}

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
      cerr << "Failed to parse JSON: " << ret << endl;
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
    cerr << "Failed to parse JSON: " << ret << endl;
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

  // Estimate the number of tokens needed
  const int num_tokens = 5;
  vector<jsontoken> tokens(num_tokens);

  // Parse the JSON string
  const int ret = stringParsing(&parser, jsonString.c_str(), jsonString.length(), tokens.data(), tokens.size());
  if (ret < 0) {
    cerr << "Failed to parse JSON: " << ret << endl;
    exit(1);
  }

  cout << "Parsed successfully!" << endl;
}

int main() {
    parseInput_test();
    parsePrimitive_test();
    parseString_test();
    return 0;
}


#endif 
