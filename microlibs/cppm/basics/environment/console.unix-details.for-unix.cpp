#include <cppm/basics/environment/console.unix-details.hpp>

#include <cppm/basics/type_makers.hpp>          // const_
#include <stdio.h>          // fileno
#include <unistd.h>         // isatty

auto cppm::impl::is_a_unix_console( FILE* f )
    -> bool
{ return isatty( fileno( f ) ); }

auto cppm::impl::get_unix_console_kind()
    -> Console_kind::Enum
{
    const_<FILE*> f = fopen( "/dev/tty", "r" );
    const bool has_console = !!f;
    fclose( f );
    return (has_console? Console_kind::unix : Console_kind::none);
}
