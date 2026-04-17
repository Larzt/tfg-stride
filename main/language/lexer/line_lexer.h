#pragma once

#include "lexer.h"
#include <string>
#include <vector>

class LineLexer {
private:
    const std::string& source;
    size_t cursor = 0;
    std::string current_word = "";
    std::vector<Token> tokens;

    // Métodos auxiliares privados
    char peekNext() const;
    void consumeString();
    void flushWord();
    Token generateWordToken(const std::string& word) const;

public:
    explicit LineLexer(const std::string& line);
    std::vector<Token> process();
};
