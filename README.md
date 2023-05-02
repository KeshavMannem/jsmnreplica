# jsmnreplica
Replica for jsmn parser.
This replica uses similar processes to the JSMN parser that was previously published. A prominent feature of the parser is the token feature, which allows for the parser to keep track of parent links, current tokens, and allocate future ones for upcoming characters to be stored in. The parser also parses through 4 types of data, Objects, Arrays, Strings, and Primitive Data which includes numbers, booleans, or null. 

The jsmn replica uses very similar methodology that the real jsmn parser has, and it also contains similar functions. The cases that the JSMN parser encounters are also similar in the jsmnreplica. This version also does not block parent links, and implements them as a key part of the parser. The same goes for strict mode, as strict mode does not need to be enabled, the jsmnreplica is already designed to work with strict mode cases. 

There are three test functions testing if the primitive, string, and input parsing functions work. 