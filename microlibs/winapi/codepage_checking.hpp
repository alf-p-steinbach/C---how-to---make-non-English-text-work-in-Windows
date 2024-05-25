#pragma once
#include <winapi/wrapped/windows-h.hpp>

#include <cppm/basics/now_and_fail.hpp>

namespace winapi {
    using namespace cppm::now_and_fail;
    using   std::string;                // <string>

    inline auto process_ansi_codepage()
        -> int
    { return GetACP(); }
    
    inline auto system_ansi_codepage()
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
        now( success ) or fail( "GetLocaleInfo failed." );
        return result;
    }

    inline auto codepage_name( const int cp )
        -> string
    {
        CPINFOEX info;
        GetCPInfoEx( cp, {}, &info );
        return info.CodePageName;
    }
}  // namespace winapi
