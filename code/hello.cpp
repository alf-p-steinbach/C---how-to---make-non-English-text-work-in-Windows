#include <cstdlib>          // system
#include <iostream>         // cout, endl
using std::system, std::cout, std::endl;
using Byte = unsigned char;

constexpr auto& oe = "ø";

constexpr auto literals_are_utf8()
    -> bool
{ return (Byte( oe[0] ) == 195 and Byte( oe[1] ) == 184 ); }

static_assert( literals_are_utf8(), "With MSVC use option /utf-8." );
#ifdef _WIN32
    const int dummy_for_setting_utf8 = system( "chcp 65001 >nul" );     // UTF-8
#endif

auto main() -> int
{
    cout << "Every 日本国 кошка loves Norwegian blåbærsyltetøy! Yay! 😃" << endl;
}
