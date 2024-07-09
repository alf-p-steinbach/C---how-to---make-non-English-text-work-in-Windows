#include <winapi/console-util.hpp>          // find_console_window
#include <winapi/process-util.hpp>          // modulename_for, process_id_for
#include <winapi/utf8_from.hpp>             // utf8_from

// Known OK WT version is 1.20, May 2024.

namespace winapi {
    using   std::string, std::wstring;          // <string>

    auto get_console_kind()
        -> wstring
    { return modulename_for( process_id_for( find_console_window() ) ); }
}  // namespace winapi

#include <fmt/core.h>
#include <string_view>
namespace app {
    using   cppm::in_;
    using   std::wstring,               // <string>
            std::wstring_view;          // <string_view>

    auto ascii_uppercased( in_<wstring_view> s )
        -> wstring
    {
        wstring result;
        for( const wchar_t ch: s ) { result += (L'a' <= ch and ch <= L'z'? ch - L'a' + L'A' : ch); }
        return result;
    }

    void run()
    {
        using   winapi::get_console_kind, winapi::utf8_from;

        const wstring console_kind = get_console_kind();
        fmt::print( "Console kind: “{:s}”.\n", utf8_from( console_kind ) );
        fmt::print( "It’s {}Windows Terminal.\n",
            (ascii_uppercased( console_kind ) == L"WINDOWSTERMINAL"? "" : "NOT ")
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
