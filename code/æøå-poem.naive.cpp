// Naïve use of `std::filesystem::path`, intentionally bug-ridden:
#include <cppm.hpp>
#include <fmt/core.h>

#include <assert.h>
#include <filesystem>
#include <fstream>
#include <string>


namespace app {
    namespace fs = std::filesystem;
    using namespace cppm::now_and_fail;
    using   cppm::literals_are_utf8, cppm::os_api_is_utf8, cppm::in_;
    using   fmt::print;                     // <fmt/core.h>
    using   std::ifstream,                  // <fstream>
            std::getline, std::string;      // <string>

    //! Bug: per the standard the default `.string()` produces system Windows ANSI encoding.
    auto operator-( in_<fs::path> p ) -> string { return p.string(); }  // `-`: stripped.

    void run()
    {
        static_assert( literals_are_utf8(), "Use the compiler option(s) for UTF-8 literals." );
        assert( os_api_is_utf8() or !"In Windows use a manifest for UTF-8 as ANSI codepage." );

        //! Bug: the `fs::path` constructor assumes a `char`-based string is system Windows ANSI.
        const auto poem_path = fs::path( "data/æøå-poem.txt" );
        ifstream poem( poem_path );
        now( not poem.fail() ) or fail( "Failed to open file “{}”.", -poem_path );

        for( string line; getline( poem, line ); ) { print( "{}\n", line ); }

        now( poem.eof() ) or fail( "Something failed reading file “{}”.", -poem_path );
    }
}  // namespace app

auto main() -> int { return cppm::with_exceptions_displayed( app::run ); }
