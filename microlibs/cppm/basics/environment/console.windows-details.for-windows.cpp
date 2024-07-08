#include <cppm/basics/environment/console.windows-details.hpp>
#include <winapi/codepage-util.hpp>

#include <io.h>             // _isatty, _fileno


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
