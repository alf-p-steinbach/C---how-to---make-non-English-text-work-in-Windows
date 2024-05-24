#include "windows-h.hpp"
static_assert( IS_NARROW_WINAPI(), "Define (only) UTF8_WINAPI please." );

#include <iostream>
#include <iterator>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace cppm {
    using   std::size,                  // <iterator>
            std::runtime_error,         // <stdexcept>
            std::string;                // <string>

    using Byte = unsigned char;

    template< class Type >  using   in_     = const Type&;

    template< class Collection >
    constexpr auto intsize_of( in_<Collection> c ) -> int { return static_cast<int>( size( c ) ); }

    inline namespace x_handling {
        constexpr auto hopefully( const bool cond ) -> bool { return cond; }
        [[noreturn]] auto fail( in_<string> s ) -> bool { throw runtime_error( s ); }
    }
}  // namespace cppm

namespace winapi {
    using namespace cppm::x_handling;
    using   std::string;                // <string>

    auto process_ansi_codepage() -> int { return GetACP(); }
    
    auto system_ansi_codepage()
        -> int
    {
        const LCTYPE cp_info_id = LOCALE_IDEFAULTANSICODEPAGE;
        DWORD result = 0;
        const bool success = !!GetLocaleInfo(
            LOCALE_SYSTEM_DEFAULT,
            cp_info_id | LOCALE_RETURN_NUMBER,
            reinterpret_cast<char*>( &result ),
            sizeof( result )
            );
        hopefully( success ) or fail( "GetLocaleInfo failed." );
        return result;
    }

    auto assert_utf8_process_codepage()
        -> bool
    {
        assert( process_ansi_codepage() == 65001 or
            !"The process ANSI codepage isn't UTF-8 (old Windows?)." );
        return true;
    }
    
    auto codepage_name( const int cp )
        -> string
    {
        CPINFOEX    info;
        GetCPInfoEx( cp, {}, &info );
        return info.CodePageName;
    }
}  // namespace winapi

namespace app {
    namespace fs = std::filesystem;
    using   cppm::Byte, cppm::intsize_of;
    auto& out = std::cout;

    void run()
    {
        const int process_cp = winapi::process_ansi_codepage();
        out << "Process ANSI codepage: " << winapi::codepage_name( process_cp ) << ".\n";

        const int system_cp = winapi::system_ansi_codepage();
        out << "System ANSI codepage:  " << winapi::codepage_name( system_cp ) << ".\n";
        
        const fs::path dummy( "ø" );
        const int n_code_points = intsize_of( dummy.u32string() );
        const bool path_assumes_utf8 = (n_code_points == 1);
        const bool path_assumes_ansi = ((not path_assumes_utf8) and dummy.u32string()[0] == Byte( "ø"[0] ));

        assert( path_assumes_utf8 + path_assumes_ansi == 1 );

        out << "`std::filesystem::path` assumes a `char` string is "
            << (path_assumes_utf8? "process" : "system") << " ANSI encoded"
            << " (UTF-8 “ø” yields " << n_code_points << " code points:";
        for( int i = 0; i < n_code_points; ++ i ) { out << " " << +Byte( dummy.u32string()[i] ); }
        out << ").\n";
        out << "Roundtripped: '" << dummy.u8string() << "'\n";
    }
}  // namespace app

#include <stdlib.h>     // system, EXIT_...
using   cppm::in_;
using   std::cerr, std::exception;

auto main() -> int
{
    winapi::assert_utf8_process_codepage();
    system( "chcp 65001 >nul" );
    try {
        app::run();
        return EXIT_SUCCESS;
    } catch( in_<exception> x ) {
        cerr << "!" << x.what() << "\n";
    }
    return EXIT_FAILURE;
}
