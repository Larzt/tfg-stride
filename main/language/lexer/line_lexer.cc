#include "line_lexer.h"
#include <unordered_map>
#include <algorithm>
#include <cctype>

static const std::unordered_map<std::string, TokenType> keywords = {
    {"file", TokenType::FILE},     {"output", TokenType::OUTPUT},
    {"input", TokenType::INPUT},   {"led", TokenType::LED},
    {"button", TokenType::BUTTON}, {"pin", TokenType::PIN},
    {"name", TokenType::NAME},     {"write", TokenType::WRITE},
    {"read", TokenType::READ},     {"print", TokenType::PRINT},

    // FLOW CONTROL
    {"if", TokenType::IF},         {"else", TokenType::ELSE},
    {"endif", TokenType::ENDIF},   {"loop", TokenType::LOOP},
    {"dloop", TokenType::DLOOP},   {"wait", TokenType::WAIT},

    // I2C
    {"i2c", TokenType::I2C},       {"init", TokenType::INIT},
    {"sda", TokenType::SDA},       {"scl", TokenType::SCL},

    // OTHERS
    {"->", TokenType::ARROW},      {"=", TokenType::ASSIGN},
    {"on", TokenType::VALUE},      {"off", TokenType::VALUE}
};

LineLexer::LineLexer(const std::string& line) : source(line) {}

std::vector<Token> LineLexer::process() {
    while (cursor < source.length()) {
        char current_char = source[cursor];
        char next_char = peekNext();

        if (std::isspace(current_char)) {
            flushWord();
            cursor++;
        }
        else if (current_char == '"') {
            flushWord();
            consumeString();
        }
        // --- SÍMBOLOS DE 2 CARACTERES ---
        else if (current_char == '-' && next_char == '>') {
            flushWord();
            tokens.emplace_back(TokenType::ARROW, "->");
            cursor += 2;
        }
        else if (current_char == '=' && next_char == '=') {
            flushWord();
            tokens.emplace_back(TokenType::IS_EQUAL, "==");
            cursor += 2;
        }
        else if (current_char == '!' && next_char == '=') {
            flushWord();
            tokens.emplace_back(TokenType::NOT_EQUAL, "!=");
            cursor += 2;
        }
        else if (current_char == '<' && next_char == '=') {
            flushWord();
            tokens.emplace_back(TokenType::LESS_EQUAL, "<=");
            cursor += 2;
        }
        else if (current_char == '>' && next_char == '=') {
            flushWord();
            tokens.emplace_back(TokenType::GREATER_EQUAL, ">=");
            cursor += 2;
        }
        // --- SÍMBOLOS DE 1 CARÁCTER ---
        else if (current_char == '=') {
            flushWord();
            tokens.emplace_back(TokenType::ASSIGN, "="); // Asignación
            cursor++;
        }
        else if (current_char == '<') {
            flushWord();
            tokens.emplace_back(TokenType::LESS_THAN, "<");
            cursor++;
        }
        else if (current_char == '>') {
            flushWord();
            tokens.emplace_back(TokenType::GREATER_THAN, ">");
            cursor++;
        }
        // --- TEXTO / NÚMEROS ---
        else {
            current_word += current_char;
            cursor++;
        }
    }

    flushWord();
    return tokens;
}

char LineLexer::peekNext() const {
    if (cursor + 1 < source.length()) {
        return source[cursor + 1];
    }
    return '\0';
}

void LineLexer::consumeString() {
    cursor++; // Saltar la comilla inicial
    std::string literal = "";

    while (cursor < source.length() && source[cursor] != '"') {
        literal += source[cursor];
        cursor++;
    }

    tokens.emplace_back(TokenType::STRING, literal);

    if (cursor < source.length() && source[cursor] == '"') {
        cursor++; // Saltar la comilla final si existe
    }
}

void LineLexer::flushWord() {
    if (current_word.empty()) return;

    tokens.emplace_back(generateWordToken(current_word));
    current_word.clear();
}

Token LineLexer::generateWordToken(const std::string& word) const {
    std::string lower = word;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    auto it = keywords.find(lower);
    if (it != keywords.end()) {
        return Token(it->second, word);
    }

    if (word.size() > 2 && word[0] == '0' && (word[1] == 'x' || word[1] == 'X')) {
        return Token(TokenType::HEX_NUMBER, word);
    }

    if (std::all_of(word.begin(), word.end(), ::isdigit)) {
        return Token(TokenType::NUMBER, word);
    }

    return Token(TokenType::IDENTIFIER, word);
}


