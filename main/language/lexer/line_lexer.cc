#include "line_lexer.h"
#include <unordered_map>
#include <algorithm>
#include <cctype>

static const std::unordered_map<std::string, TokenType> keywords = {
    {"file", TokenType::FILE}, {"output", TokenType::OUTPUT}, {"input", TokenType::INPUT}, {"led", TokenType::LED}, {"button", TokenType::BUTTON}, {"pin", TokenType::PIN}, {"name", TokenType::NAME}, {"write", TokenType::WRITE}, {"read", TokenType::READ}, {"print", TokenType::PRINT},

    // FLOW CONTROL
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"endif", TokenType::ENDIF},
    {"loop", TokenType::LOOP},
    {"dloop", TokenType::DLOOP},
    {"wait", TokenType::WAIT},

    // I2C
    {"i2c", TokenType::I2C},
    {"init", TokenType::INIT},
    {"sda", TokenType::SDA},
    {"scl", TokenType::SCL},

    // OTHERS
    {"->", TokenType::ARROW},
    {"=", TokenType::ASSIGN},
    {"on", TokenType::VALUE},
    {"off", TokenType::VALUE}};

LineLexer::LineLexer(const std::string &line) : source(line) {}

std::vector<Token> LineLexer::process()
{
    while (cursor < source.length())
    {
        char current_char = source[cursor];
        char next_char = peekNext();

        if (std::isspace(current_char))
        {
            flushWord();
            cursor++;
        }
        else if (current_char == '"')
        {
            flushWord();
            consumeString();
        }
        // --- SÍMBOLOS DE 2 CARACTERES ---
        else if (current_char == '-' && next_char == '>')
        {
            flushWord();
            tokens.emplace_back(TokenType::ARROW, "->");
            cursor += 2;
        }
        else if (current_char == '=' && next_char == '=')
        {
            flushWord();
            char third_char = (cursor + 2 < source.length()) ? source[cursor + 2] : '\0';

            if (third_char == '=')
            {
                // Caso "===" invalido.
                // Consumimos todos los '=' seguidos para marcarlos como UNKNOWN
                std::string invalid_op = "==";
                cursor += 2;
                while (cursor < source.length() && source[cursor] == '=')
                {
                    invalid_op += source[cursor];
                    cursor++;
                }
                tokens.emplace_back(TokenType::UNKNOWN, invalid_op);
            }
            else
            {
                tokens.emplace_back(TokenType::IS_EQUAL, "==");
                cursor += 2;
            }
        }
        else if (current_char == '!' && next_char == '=')
        {
            flushWord();
            tokens.emplace_back(TokenType::NOT_EQUAL, "!=");
            cursor += 2;
        }
        else if (current_char == '<' && next_char == '=')
        {
            flushWord();
            tokens.emplace_back(TokenType::LESS_EQUAL, "<=");
            cursor += 2;
        }
        else if (current_char == '>' && next_char == '=')
        {
            flushWord();
            tokens.emplace_back(TokenType::GREATER_EQUAL, ">=");
            cursor += 2;
        }
        // --- SÍMBOLOS DE 1 CARÁCTER ---
        else if (current_char == '=')
        {
            flushWord();
            tokens.emplace_back(TokenType::ASSIGN, "="); // Asignación
            cursor++;
        }
        else if (current_char == '<')
        {
            flushWord();
            tokens.emplace_back(TokenType::LESS_THAN, "<");
            cursor++;
        }
        else if (current_char == '>')
        {
            flushWord();
            tokens.emplace_back(TokenType::GREATER_THAN, ">");
            cursor++;
        }
        // --- TEXTO / NÚMEROS ---
        else
        {
            current_word += current_char;
            cursor++;
        }
    }

    flushWord();
    return tokens;
}

char LineLexer::peekNext() const
{
    if (cursor + 1 < source.length())
    {
        return source[cursor + 1];
    }
    return '\0';
}

void LineLexer::consumeString()
{
    cursor++; // Saltamos la comilla de apertura (")
    std::string literal = "";
    bool closed = false;

    while (cursor < source.length())
    {
        if (source[cursor] == '"')
        {
            closed = true;
            break; // Encontramos la comilla de cierre
        }
        literal += source[cursor];
        cursor++;
    }

    if (closed)
    {
        tokens.emplace_back(TokenType::STRING, literal);
        cursor++; // Saltamos la comilla de cierre (")
    }
    else
    {
        // ERROR: El string se quedó abierto al final de la línea/archivo
        // Lo marcamos como UNKNOWN
        tokens.emplace_back(TokenType::UNKNOWN, "\"" + literal);

#ifndef TEST
        ESP_LOGE("Lexer", "Error: String literal no cerrado: \"%s\"", literal.c_str());
#endif
    }
}

void LineLexer::flushWord()
{
    if (current_word.empty())
        return;

    tokens.emplace_back(generateWordToken(current_word));
    current_word.clear();
}

Token LineLexer::generateWordToken(const std::string &word) const
{
    std::string lower = word;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    // Comprobar palabras clave
    auto it = keywords.find(lower);
    if (it != keywords.end())
    {
        return Token(it->second, word);
    }

    // Comprobar Hexadecimal
    if (word.size() > 2 && word[0] == '0' && (word[1] == 'x' || word[1] == 'X'))
    {
        bool isHex = std::all_of(word.begin() + 2, word.end(), [](unsigned char c)
                                 { return std::isxdigit(c); });
        if (isHex)
            return Token(TokenType::HEX_NUMBER, word);
    }

    // Comprobar Decimal
    if (std::all_of(word.begin(), word.end(), ::isdigit))
    {
        return Token(TokenType::NUMBER, word);
    }

    // Comprobar si es un IDENTIFICADOR VÁLIDO
    // Debe empezar por letra o '_'
    if (!word.empty() && (std::isalpha(word[0]) || word[0] == '_'))
    {
        bool validId = std::all_of(word.begin() + 1, word.end(), [](unsigned char c)
                                   { return std::isalnum(c) || c == '_'; });

        if (validId)
        {
            return Token(TokenType::IDENTIFIER, word);
        }
    }
    return Token(TokenType::UNKNOWN, word);
}
