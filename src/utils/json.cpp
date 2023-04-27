#include "json.h"

#include <cstring>
#include <limits>
#include <ostream>

#include "strings/format.h"
#include "strings/symbol_position.h"

void Json::ParseSkipWhitespace(const char *&cur)
{
    while (*cur > '\0' && *cur <= ' ')
        cur++;
}

std::string Json::ParseStringLow(const char *&cur)
{
    ParseSkipWhitespace(cur);

    if (*cur != '"')
        throw std::runtime_error("Expected `\"`.");
    cur++;

    const char *begin = cur;
    bool backslash_preceding = false;

    while (true)
    {
        // Stop on `"`.
        if (*cur == '"' && !backslash_preceding)
            break;

        // Handle `\`.
        backslash_preceding = (*cur == '\\' && !backslash_preceding);

        // Error if no more data.
        if (*cur == '\0')
        {
            cur = begin; // We do this to get a better error message.
            throw std::runtime_error("This string lacks a terminating `\"` character.");
        }

        // Error on non-printable character.
        if (*cur > '\0' && *cur < ' ')
            throw std::runtime_error(STR("Invalid character in a string: 0x", ((unsigned char)*cur)"02x", "."));

        cur++;
    }

    const char *end = cur;

    std::string ret;
    for (cur = begin; cur != end; cur++)
    {
        if (*cur != '\\')
        {
            ret += *cur;
        }
        else
        {
            cur++;
            if (cur == end)
                throw std::runtime_error("Expected an escape character before `\"`.");
            switch (*cur)
            {
              case '\\':
              case '/':
              case '"':
                ret += *cur;
                break;
              case 'b':
                ret += '\b';
                break;
              case 'f':
                ret += '\f';
                break;
              case 'n':
                ret += '\n';
                break;
              case 'r':
                ret += '\r';
                break;
              case 't':
                ret += '\t';
                break;
              case 'u':
                {
                    cur++;
                    if (end - cur < 4)
                        throw std::runtime_error("Expected four hex digits after `\\u`.");
                    int value = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        int digit;
                        if (*cur >= '0' && *cur <= '9')
                            digit = *cur - '0';
                        else if (*cur >= 'a' && *cur <= 'f')
                            digit = *cur - 'a' + 10;
                        else if (*cur >= 'A' && *cur <= 'F')
                            digit = *cur - 'A' + 10;
                        else
                            throw std::runtime_error("Expected four hex digits after `\\u`.");
                        value = value * 16 + digit;
                        cur++;
                    }
                    if (value < 128)
                    {
                        ret += char(value);
                    }
                    else if (value < 2048) // 2048 = 2^11
                    {
                        ret += char(0b1100'0000 + (value >> 6));
                        ret += char(0b1000'0000 + (value & 0b0011'1111));
                    }
                    else
                    {
                        ret += char(0b1110'0000 + (value >> 12));
                        ret += char(0b1000'0000 + ((value >> 6) & 0b0011'1111));
                        ret += char(0b1000'0000 + (value & 0b0011'1111));
                    }
                    cur--; // This is needed because of the auto increment at the end of loop.
                }
            }
        }
    }

    cur++; // Skip the `"`.
    return ret;
}

Json Json::ParseLow(const char *&cur, int allowed_depth)
{
    if (allowed_depth < 0)
        throw std::runtime_error("Too many nested elements.");

    auto TryGetString = [&](std::string string) -> bool
    {
        if (std::strncmp(string.c_str(), cur, string.size()) == 0)
        {
            cur += string.size();
            return true;
        }
        else
        {
            return false;
        }
    };

    ParseSkipWhitespace(cur);

    switch (*cur)
    {
      case 'n': // null
        if (TryGetString("null"))
            return FromVariant(std::monostate{});
        break;

      case 'f': // boolean, false
        if (TryGetString("false"))
            return FromVariant(false);
        break;

      case 't': // boolean, true
        if (TryGetString("true"))
            return FromVariant(true);
        break;

      default: // number
        {
            std::string str;
            bool real = false;

            if (*cur == '-')
            {
                str += '-';
                cur++;
            }

            while (*cur >= '0' && *cur <= '9')
                str += *cur++;

            if (str.empty())
                break;

            if (*cur == '.')
            {
                cur++;

                real = true;
                str += '.';

                while (*cur >= '0' && *cur <= '9')
                    str += *cur++;

                if (str.back() == '.')
                    throw std::runtime_error("Expected a digit after decimal point.");
            }

            if (*cur == 'e' || *cur == 'E')
            {
                cur++;

                real = true;
                str += 'e';

                if (*cur == '+' || *cur == '-')
                    str += *cur++;

                while (*cur >= '0' && *cur <= '9')
                    str += *cur++;

                if (str.back() == 'e' || str.back() == '+' || str.back() == '-')
                    throw std::runtime_error("Expected a digit after `e`, possibly after a sign.");
            }

            if (real)
            {
                char *end = 0;
                double num = std::strtod(str.c_str(), &end);
                if (end == str.c_str())
                    throw std::runtime_error("Unable to parse a number.");

                return FromVariant(num);
            }
            else
            {
                char *end = 0;
                long num = std::strtol(str.c_str(), &end, 10);
                if (end == str.c_str())
                    throw std::runtime_error("Unable to parse a number.");

                if constexpr (sizeof(int) < sizeof(long))
                    if (num < std::numeric_limits<int>::min() || num > std::numeric_limits<int>::max())
                        throw std::runtime_error("Overflow in integral constant.");

                return FromVariant(int(num));
            }
        }
        break;

      case '"': // string
        return FromVariant(ParseStringLow(cur));
        break;

      case '[': // array
        {
            const char *begin = cur;
            cur++; // Skip `[`.

            array_t vec;

            bool first = true;
            while (true)
            {
                ParseSkipWhitespace(cur);

                if (*cur == ']')
                    break;

                if (first)
                {
                    first = false;
                }
                else
                {
                    if (*cur != ',')
                        throw std::runtime_error("Expected `,`.");
                    cur++;
                    ParseSkipWhitespace(cur);

                    if (*cur == ']')
                        break;
                }

                if (*cur == '\0')
                {
                    cur = begin; // We do this to get a better error message.
                    throw std::runtime_error("This array lacks a terminating `]` character.");
                }

                vec.push_back(ParseLow(cur, allowed_depth-1));
            }

            cur++; // Skip `]`.
            return FromVariant(std::move(vec));
        }
        break;

      case '{': // object
        {
            const char *begin = cur;
            cur++; // Skip `{`.

            object_t map;

            bool first = true;
            while (true)
            {
                ParseSkipWhitespace(cur);

                if (*cur == '}')
                    break;

                if (first)
                {
                    first = false;
                }
                else
                {
                    if (*cur != ',')
                        throw std::runtime_error("Expected `,`.");
                    cur++;
                    ParseSkipWhitespace(cur);

                    if (*cur == '}')
                        break;
                }

                if (*cur == '\0')
                {
                    cur = begin; // We do this to get a better error message.
                    throw std::runtime_error("This array lacks a terminating `]` character.");
                }

                std::string name = ParseStringLow(cur);

                ParseSkipWhitespace(cur);

                if (*cur != ':')
                    throw std::runtime_error("Expected `:`.");
                cur++;

                // No need to skip whitespace here, nested ParseLow() will do that.

                map.insert({name, ParseLow(cur, allowed_depth-1)});
            }

            cur++; // Skip `}`.
            return FromVariant(std::move(map));
        }
        break;
    }

    throw std::runtime_error("Unknown entity.");
}

Json::Json(const char *string, int allowed_depth)
{
    const char *begin = string;
    try
    {
        *this = ParseLow(string, allowed_depth);
        ParseSkipWhitespace(string);
        if (*string != '\0')
            throw std::runtime_error("Unexpected data after JSON.");
    }
    catch (std::exception &e)
    {
        auto pos = Strings::GetSymbolPosition(begin, string);
        throw std::runtime_error(FMT("JSON parsing failed, at {}: {}", pos.ToString(), e.what()));
    }
}

void Json::View::DebugPrint(std::ostream &stream) const
{
    switch (Type())
    {
      case null:
        stream << "null";
        break;
      case boolean:
        stream << (GetBool() ? "true" : "false");
        break;
      case num_int:
        stream << GetInt();
        break;
      case num_real:
        stream << GetReal();
        break;
      case string:
        stream << '"' << GetString() << '"';
        break;
      case array:
        {
            const array_t &obj = *std::get_if<int(array)>(&ptr->variant);
            bool first = true;
            stream << '[';
            for (const auto &it : obj)
            {
                if (first)
                    first = false;
                else
                    stream << ',';
                it.GetView().DebugPrint(stream);
            }
            stream << ']';
        }
        break;
      case object:
        {
            const object_t &obj = *std::get_if<int(object)>(&ptr->variant);
            bool first = true;
            stream << '{';
            for (const auto &it : obj)
            {
                if (first)
                    first = false;
                else
                    stream << ',';
                stream << "\"" << it.first << "\":";
                it.second.GetView().DebugPrint(stream);
            }
            stream << '}';
        }
        break;
    }
}
