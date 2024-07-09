#include <winapi/console-util.hpp>                  // find_console_window
#include <winapi/process-util.hpp>                  // modulename_for, process_id_for
#include <winapi/utf8_from.hpp>                     // utf8_from
#include <winapi/versionresource-inspection.hpp>    // Version, opt_version_info_of

// Known OK WT version is 1.20, May 2024.

namespace winapi {
    using   std::string, std::wstring;          // <string>

    struct Console_kind
    {
        wstring     name;
        Version     version;        // All zeroes if not applicable.
    };

    auto get_console_kind()
        -> Console_kind
    {
        Console_kind result{};
        const DWORD process_id = process_id_for( find_console_window() );
        const wstring exe_path = exe_path_for_process( read_handle_for_process( process_id ) );
        result.name = modulename_for_process( exe_path );
        try {
            result.version = Version_info( exe_path ).product_version();
        } catch( ... ) {
            // Ignore, use all zeroes result.
        }
        return result;
    }
}  // namespace winapi

#include <fmt/core.h>
#include <string_view>
namespace app {
    using   cppm::in_;
    using   std::string, std::wstring,  // <string>
            std::wstring_view;          // <string_view>

    auto ascii_uppercased( in_<wstring_view> s )
        -> wstring
    {
        wstring result;
        for( const wchar_t ch: s ) { result += (L'a' <= ch and ch <= L'z'? ch - L'a' + L'A' : ch); }
        return result;
    }

    auto to_string( in_<winapi::Version> ver )
        -> string
    { return fmt::format( "{}.{}.{}.{}", ver.major(), ver.minor(), ver.revision(), ver.build() ); }

    auto is_windows_terminal( in_<winapi::Console_kind> console_kind )
        -> bool
    { return (ascii_uppercased( console_kind.name ) == L"WINDOWSTERMINAL"); };

    auto is_u8complete_windows_terminal( in_<winapi::Console_kind> console_kind )
        -> bool
    {
        using namespace std::rel_ops;       // operator>=
        return (is_windows_terminal( console_kind ) and console_kind.version >= winapi::Version( 1, 20 ));
    }

    void run()
    {
        using   winapi::get_console_kind, winapi::utf8_from;

        const winapi::Console_kind console_kind = get_console_kind();

        fmt::print( "Console kind: “{:s}”", utf8_from( console_kind.name ) );
        if( console_kind.version.has_value() ) {
            fmt::print( " version {}", to_string( console_kind.version ) );
        }
        fmt::print( ".\n" );

        fmt::print( "It’s a {}Windows Terminal.\n",
            (is_windows_terminal( console_kind )? "" : "NOT ")
            );
        fmt::print( "It’s {}an UTF-8 complete Windows Terminal.\n",
            (is_u8complete_windows_terminal( console_kind )? "" : "NOT ")
            );
    }
}

#include <cstdlib>          // EXIT_...
#include <stdexcept>        // std::exception
auto main() -> int
{
    using cppm::in_;
    using std::exception;

    try {
        app::run();
        return EXIT_SUCCESS;
    } catch( in_<exception> x ) {
        fmt::print( stderr, "!{}\n", x.what() );
    }
    return EXIT_FAILURE;
}
