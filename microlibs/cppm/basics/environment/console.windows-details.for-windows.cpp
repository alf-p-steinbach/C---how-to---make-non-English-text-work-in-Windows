#include <cppm/basics/environment/console.windows-details.hpp>
#include <winapi/codepage-util.hpp>     // console_codepage, set_console_codepage
#include <winapi/console-util.hpp>      // get_console_host
#include <winapi/process-util.hpp>      // process_id_for, exe_path_for, modulename_for

#include <io.h>             // _isatty, _fileno

namespace {
    namespace w = winapi;
    using   cppm::in_;
    using   w::console_title, w::set_console_title, w::find_console_window,     // <console-util.hpp>
            w::process_id_for, w::exe_path_for, w::modulename_for;              // <process-util.hpp>
    using   std::string, std::wstring;          // <string>

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

    // ConsoleWindowClass  (powershell, cmd, git bash)
    // DesktopWindowXamlSource (wt)
    // mintty (MSYS2 bash)

    auto get_console_kind()
        -> Console_kind::Enum
    {
        const winapi::Console_host console_host = get_console_host();
        if( (ascii_uppercased( console_host.name ) == L"WINDOWSTERMINAL") ) {
            return (console_kind.version >= winapi::Version( 1, 20 )
                ? Console_kind::old_windows_terminal
                : Console_kind::windows_terminal
                );
        }
        return Console_kind::windows_classic;
    }
}  // namespace <anon>


//------------------------------------------------------------------------------------
//

cppm::impl::Windows_console_config::Codepage_config::~Codepage_config()
{
    winapi::set_console_codepage( m_original_codepage );
}

cppm::impl::Windows_console_config::Codepage_config::Codepage_config()
    : m_original_codepage( winapi::console_codepage() )
{
    winapi::set_console_codepage( CP_UTF8 );
}

auto cppm::impl::is_a_windows_console( FILE* f )
    -> bool
{ return _isatty( _fileno( f ) ); }

auto cppm::impl::get_windows_console_kind()
    -> Console_kind::Enum
{ return {}; }          // TODO:
