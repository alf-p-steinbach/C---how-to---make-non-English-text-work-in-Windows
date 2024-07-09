#include <cppm/basics/environment/console.windows-details.hpp>
#include <winapi/codepage-util.hpp>     // console_codepage, set_console_codepage
#include <winapi/console-util.hpp>      // console_title, set_console_title, find_console_window
#include <winapi/process-util.hpp>      // process_id_for, exe_path_for, modulename_for

#include <io.h>             // _isatty, _fileno

namespace {
    namespace w = winapi;
    using   w::console_title, w::set_console_title, w::find_console_window,     // <console-util.hpp>
            w::process_id_for, w::exe_path_for, w::modulename_for;              // <process-util.hpp>

    void foo()
    {
        const DWORD process_id = process_id_for( find_console_window() );
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
