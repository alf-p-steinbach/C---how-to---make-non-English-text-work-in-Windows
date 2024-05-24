// C++03 file handling; works with UTF-8 for all encodings.
#include <cppm.hpp>
#include <fmt/core.h>

#include <assert.h>
#include <fstream>
#include <string>

namespace app {
    using namespace cppm::now_and_fail;
    using   cppm::literals_are_utf8, cppm::os_api_is_utf8;
    using   fmt::print;                     // <fmt/core.h>
    using   std::ifstream,                  // <fstream>
            std::getline, std::string;      // <string>

    void run()
    {
        static_assert( literals_are_utf8(), "Use the compiler option(s) for UTF-8 literals." );
        assert( os_api_is_utf8() or !"In Windows use a manifest for UTF-8 as ANSI codepage." );

        const auto& poem_path = "data/æøå-poem.txt";
        ifstream poem( poem_path );
        now( not poem.fail() ) or fail( "Failed to open file “{}”.", poem_path );

        for( string line; getline( poem, line ); ) { print( "{}\n", line ); }

        now( poem.eof() ) or fail( "Something failed reading file “{}”.", poem_path );
    }
}  // namespace app

auto main() -> int { return cppm::with_exceptions_displayed( app::run ); }
