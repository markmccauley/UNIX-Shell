#include "Tokenizer.h"

vector<string> Tokenizer::tokenize(string line) {
    tokens = {""};
    token_num = 0;
    quote = false;
    for (int i = 0; i < line.length(); i++) {
        if (!quote && (line[i] == '\"' || line[i] == '\'')) { // check for quotes
            quote = true;
        }
        else if (quote && (line[i] == '\"' || line[i] == '\'')) {
            quote = false;
        }
        else if (!quote && line[i] == ' ') { // check white space
            tokens.push_back(""); // remove white space
            token_num++;
        }
        else if (!quote && (line[i] == '<' || line[i] == '>' || line[i] == '|')) { // check special characters
            tokens.push_back(string(1,line[i]));
            tokens.push_back("");
            token_num += 2;
        } else {
            tokens[token_num] = tokens[token_num] + line[i];
        }
    }
    tokens = clean_spaces(tokens);
    return tokens;
}

vector<string> Tokenizer::clean_spaces(vector<string> line) { // further remove spaces
    for (int i = 0; i < line.size(); i++) {
        if (line[i] == "") {
            line.erase(line.begin() + i);
        }
    }
    return line;
}