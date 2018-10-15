#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Tokenizer {

private:
    // store parsed input
    vector<string> tokens;

    int token_num;
    bool quote;

public:

    // class methods for parsing input
    vector<string> tokenize(string line);
    vector<string> clean_spaces(vector<string> line);
};

