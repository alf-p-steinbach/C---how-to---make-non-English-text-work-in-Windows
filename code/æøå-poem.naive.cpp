#include <filesystem>
#include <fstream>
#include <string>

#include <assert.h>
#include <cppm/basics.hpp>
#include <cppm/utf8/encoding_assumption_checking.hpp>
#include <fmt/core.h>

namespace app {
    namespace fs = std::filesystem;
    using   cppm::hopefully, cppm::fail;
    using   std::ifstream,      // <fstream
            std::string;        // <string>

    void run()
    {
        static_assert( cppm::literals_are_utf8(), "Use the compiler option(s) for UTF-8 literals." );
        assert( cppm::os_api_is_utf8() or !"In Windows use a manifest for UTF-8 as ANSI codepage." );

        const fs::path  data_folder     = "data";
        const fs::path  poem_path       = data_folder / "æøå-poem.txt";
        ifstream poem( poem_path );
        hopefully( not poem.fail() )
            or fail( fmt::format( "Failed to open file “{}”.", poem_path.string() ) );

        for( string line; getline( poem, line ); ) { fmt::print( "{}\n", line ); }

        hopefully( poem.eof() )
            or fail( fmt::format( "Something failed reading file “{}”.", poem_path.string()) );
    }
}  // namespace app

auto main() -> int { return cppm::with_exceptions_displayed( app::run ); }
