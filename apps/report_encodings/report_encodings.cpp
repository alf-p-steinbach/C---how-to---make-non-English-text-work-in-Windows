#include <cppm.hpp>
#include <winapi/codepage_checking.hpp>
static_assert( IS_NARROW_WINAPI(), "Define (only) UTF8_WINAPI please." );

#include <iostream>
#include <filesystem>

namespace app {
    namespace fs = std::filesystem;
    using   cppm::Byte, cppm::intsize_of,
            cppm::literals_are_utf8, cppm::os_api_is_utf8;
    auto& out = std::cout;

    void run()
    {
        system( "chcp 65001 >nul" );
        static_assert( literals_are_utf8(), "Use the compiler option(s) for UTF-8 literals." );
        assert( os_api_is_utf8() or !"In Windows use a manifest for UTF-8 as ANSI codepage." );

        const int process_cp = winapi::process_ansi_codepage();
        out << "Process ANSI codepage: " << winapi::codepage_name( process_cp ) << ".\n";

        const int system_cp = winapi::system_ansi_codepage();
        out << "System ANSI codepage:  " << winapi::codepage_name( system_cp ) << ".\n";

        const auto  p                   = fs::path( "ø" );
        const auto  p_codes             = p.u32string();
        const int   n_code_points       = intsize_of( p_codes );
        const bool  path_assumes_utf8   = (n_code_points == 1);
        const bool  path_assumes_ansi   = (
            (not path_assumes_utf8) and p_codes[0] == Byte( "ø"[0] )
            );
        assert( path_assumes_utf8 + path_assumes_ansi == 1 );

        out << "`std::filesystem::path` assumes a `char` string is "
            << (path_assumes_utf8? "PROCESS" : "SYSTEM") << " ANSI encoded"
            << " (UTF-8 “ø” yields " << n_code_points << " code point(s):";
        for( int i = 0; i < n_code_points; ++ i ) { out << " " << +Byte( p_codes[i] ); }
        out << ").\n";
        out << "ø roundtripped: '" << p.u8string() << "'\n";
    }
}  // namespace app

auto main() -> int { return cppm::with_exceptions_displayed( app::run ); }
