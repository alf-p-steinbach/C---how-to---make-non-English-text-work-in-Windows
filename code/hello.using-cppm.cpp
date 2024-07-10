#include <cppm/basics.hpp>
#include <iostream>         // cout, endl

using   cppm::Console_usage;
using   std::system, std::cout, std::endl;

auto main() -> int
{
    const Console_usage console_usage;
    cout << "Every 日本国 кошка loves Norwegian blåbærsyltetøy! Yay!" << endl;
}
