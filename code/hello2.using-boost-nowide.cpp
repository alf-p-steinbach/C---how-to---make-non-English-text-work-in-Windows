#include <fmt/core.h>
#include <boost/nowide/iostream.hpp>
#include <string>
using   std::getline, std::string;              // <string>

auto input_line() -> string
{
    string result;
    getline( boost::nowide::cin, result );      // TODO: failure handling.
    return result;
}

auto main() -> int
{
    fmt::print( "Hi, what’s your name? " );
    const string name = input_line();
    fmt::print( "Good to meet you, {}!\n", name );
}
