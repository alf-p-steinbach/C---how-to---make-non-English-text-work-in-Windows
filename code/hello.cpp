#include <cstdlib>          // system
#include <iostream>         // cout, endl

namespace app{
    using std::system, std::cout, std::endl;
    using Byte = unsigned char;
    constexpr auto& oe = "ø";

    constexpr auto literals_are_utf8()
        -> bool
    { return (Byte( oe[0] ) == 195 and Byte( oe[1] ) == 184 ); }

    void run()
    {
        static_assert( literals_are_utf8(), "With MSVC use option /utf-8." );

        #ifdef _WIN32
            system( "chcp 65001 >nul" );    // UTF-8
        #endif
        cout << "Proper Norwegian blåbærsyltetøy! Yay!" << endl;
    }
}  // namespace app

auto main() -> int { app::run(); }
