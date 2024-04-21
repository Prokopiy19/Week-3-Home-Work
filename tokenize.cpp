#include <cctype>
#include <iostream>
#include <unordered_map>
#include <variant>
#include <vector>

struct OpeningBracket {};

struct ClosingBracket {};

struct Number {
  int value;
};

struct UnknownToken {
  std::string value;
};

struct MinToken {};

struct MaxToken {};

struct AbsToken {};

struct SqrToken{};

struct Plus {};

struct Minus {};

struct Multiply {};

struct Modulo {};

struct Divide {};

using Token = std::variant<OpeningBracket, ClosingBracket, Number, UnknownToken, MinToken, MaxToken, AbsToken, SqrToken, Plus, Minus, Multiply, Modulo, Divide>;

const std::unordered_map<char, Token> kSymbol2Token{
    {'(', OpeningBracket{}}, {')', ClosingBracket{}}, {'+', Plus{}}, {'-', Minus{}}, {'*', Multiply{}}, {'/', Divide{}}, {'%', Modulo{}}};

const std::unordered_map<std::string, Token> kName2Token{
    {"min", MinToken{}}, {"max", MaxToken{}}, {"abs", AbsToken{}}, {"sqr", SqrToken{}}};

int ToDigit(unsigned char symbol) {
  return symbol - '0';
}

Number ParseNumber(const std::string& input, size_t& pos) {
    int value = 0;
    auto symbol = static_cast<unsigned char>(input[pos]);
    while (std::isdigit(symbol)) {
        value = value * 10 + ToDigit(symbol);
        ++pos;
        if (pos == input.size()) {
            break;
        }
        symbol = static_cast<unsigned char>(input[pos]);
    }
    return Number{value};
}

Token ParseName(const std::string& input, size_t& pos) {
    std::string name;
    auto symbol = static_cast<unsigned char>(input[pos]);
    while (std::isalpha(symbol)) { // проверяем, является ли symbol английской буквой
        name.push_back(std::tolower(symbol)); // std::tolower используем, чтобы не различать прописные буквы от строчных
        ++pos;
        if (pos == input.size()) {
            break;
        }
        symbol = static_cast<unsigned char>(input[pos]);
    }
    const auto it = kName2Token.find(name);
    if (it == kName2Token.end()) {
        return UnknownToken{name};
    }
    return it->second;
}

std::vector<Token> Tokenize(const std::string& input) {
    std::vector<Token> tokens;
    const size_t size = input.size();
    size_t pos = 0;
    while (pos < size) {
        const auto symbol = static_cast<unsigned char>(input[pos]);
        if (std::isspace(symbol)) {
            ++pos;
        } else if (std::isdigit(symbol)) {
            tokens.emplace_back(ParseNumber(input, pos));
        } else if (auto it = kSymbol2Token.find(symbol); it != kSymbol2Token.end()) {
            tokens.emplace_back(it->second);
            ++pos;
        } else if (std::isalpha(symbol)) {
            tokens.emplace_back(ParseName(input, pos));
        } else {
            tokens.emplace_back(UnknownToken{std::string{static_cast<char>(symbol)}});
            ++pos;
        }
    }
    return tokens;
}

void test(const std::string& input)
{
    std::cout << "input: " << input << '\n';
    auto tokens = Tokenize(input);
    std::cout << "tokenized: ";
    for (auto token : tokens) {
        if (std::holds_alternative<OpeningBracket>(token))      std::cout << "(" << ' ';
        else if (std::holds_alternative<ClosingBracket>(token)) std::cout << ")" << ' ';
        else if (std::holds_alternative<Number>(token))         std::cout << std::get<Number>(token).value << ' ';
        else if (std::holds_alternative<UnknownToken>(token))   std::cout << "UnknownToken(" << std::get<UnknownToken>(token).value << ") ";
        else if (std::holds_alternative<MinToken>(token)) std::cout << "min" << ' ';
        else if (std::holds_alternative<MaxToken>(token)) std::cout << "max" << ' ';
        else if (std::holds_alternative<AbsToken>(token)) std::cout << "abs" << ' ';
        else if (std::holds_alternative<SqrToken>(token)) std::cout << "sqr" << ' ';
        else if (std::holds_alternative<Plus>(token))     std::cout << '+' << ' ';
        else if (std::holds_alternative<Minus>(token))    std::cout << '-' << ' ';
        else if (std::holds_alternative<Multiply>(token)) std::cout << '*' << ' ';
        else if (std::holds_alternative<Modulo>(token))   std::cout << '%' << ' ';
        else if (std::holds_alternative<Divide>(token))   std::cout << '/' << ' ';
    }
    std::cout << "\n\n";
}

int main()
{
    test("(1 + 2) * abs(32) / exp(4) + 55 & max(6 - 7)");
    test("(1+2)*abs(32)/4+55*(6-7)3");
    return 0;
}