#include <winapi/console-util.hpp>                  // find_console_window
#include <winapi/utf8_from.hpp>                     // utf8_from
#include <winapi/versionresource-inspection.hpp>    // Version, opt_version_info_of

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

    auto is_windows_terminal( in_<winapi::Console_host_id> host_id )
        -> bool
    { return (ascii_uppercased( host_id.name ) == L"WINDOWSTERMINAL"); };

    auto is_u8complete_windows_terminal( in_<winapi::Console_host_id> host_id )
        -> bool
    {
        using namespace std::rel_ops;       // operator>=
        return (is_windows_terminal( host_id ) and host_id.version >= winapi::Version( 1, 20 ));
    }

    void run()
    {
        using   winapi::get_console_host_id, winapi::utf8_from;

        const winapi::Console_host_id   host_id = get_console_host_id();

        fmt::print( "Console kind: “{:s}”", utf8_from( host_id.name ) );
        if( host_id.version.has_value() ) {
            fmt::print( " version {}", to_string( host_id.version ) );
        }
        fmt::print( ".\n" );

        fmt::print( "It’s a {}Windows Terminal.\n",
            (is_windows_terminal( host_id )? "" : "NOT ")
            );
        fmt::print( "It’s {}an UTF-8 complete Windows Terminal.\n",
            (is_u8complete_windows_terminal( host_id )? "" : "NOT ")
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
