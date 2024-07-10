#include <cppm/basics/environment/console.windows-details.hpp>
#include <winapi/console-util.hpp>      // get_console_host

#include <io.h>             // _isatty, _fileno

namespace w = winapi;

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
{
    if( not GetConsoleWindow() ) {                      return Console_kind::none; }
    using Id = w::Console_host_id;
    const Id host_id = w::get_console_host();
    if( host_id.name == Id::windows_terminal ) {
        // Known WT version supporting UTF-8 input is 1.20, May 2024.
                                                        return (host_id.version >= winapi::Version( 1, 20 )
                                                            ? Console_kind::windows_terminal
                                                            : Console_kind::old_windows_terminal
                                                            );
    } else if( host_id.name == Id::mintty ) {           return Console_kind::mintty;
    } else if( host_id.name == Id::classic_console ) {  return Console_kind::windows_classic;
    } else {                                            return Console_kind::unknown;
    }
}
