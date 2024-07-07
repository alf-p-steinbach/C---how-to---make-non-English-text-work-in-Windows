// Using a DIY `std::filesystem::path` wrapper that corrects its text garbling.
#include "minimal-Path.hpp"

#include <cppm.hpp>
#include <fmt/core.h>

#include <assert.h>
#include <fstream>
#include <string>

namespace app {
    using namespace cppm::now_and_fail;
    using   cppm::os_api_is_utf8, cppm::in_;
    using   fmt::print;                     // <fmt/core.h>
    using   minimal::Path;
    using   std::ifstream,                  // <fstream>
            std::getline, std::string;      // <string>

    void run()
    {
        assert( os_api_is_utf8() or !"In Windows use a manifest for UTF-8 as ANSI codepage." );

        const auto poem_path = Path( "data/æøå-poem.txt" );     // Asserts UTF-8 literals.
        ifstream poem( poem_path );
        now( not poem.fail() ) or fail( "Failed to open file “{}”.", -poem_path );

        for( string line; getline( poem, line ); ) {
            print( "{}\n", line );
        }

        now( poem.eof() ) or fail( "Something failed reading file “{}”.", -poem_path );
    }
}  // namespace app

auto main() -> int { return cppm::with_exceptions_displayed( app::run ); }
