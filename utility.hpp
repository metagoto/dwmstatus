#pragma once

#include <string>
#include <exception>
#include <boost/spirit/include/karma.hpp>


namespace utility
{


template <typename T>
bool lex_cast(std::string& str, T const& value)
{
    namespace karma = boost::spirit::karma;
    std::back_insert_iterator<std::string> sink(str);
    return karma::generate(sink, value);
}


void format(std::string& buf, char const* s)
{
    while (s && *s) {
        if (*s == '%' && *++s != '%')
            throw std::runtime_error("invalid format: missing arguments");
        buf += *s++;
    }
}


template<typename T, typename... Args> // note the "..."
void format(std::string& buf, const char* s, T value, Args... args)
{
    while (s && *s) {
        if (*s=='%' && *++s!='%') {
            lex_cast(buf, value);
            return format(buf, ++s, args...);
        }
        buf += *s++;
    }
    throw std::runtime_error("extra arguments provided to format");
}

}
