#pragma once
#include <winapi/wrapped/windows-h.wide.hpp>
#include <cppm/basics/exception_handling/now_and_fail.hpp>

#include <string>

namespace winapi {
    using namespace cppm::now_and_fail;
    using   std::wstring;               // <string>

    inline auto name_of_codepage( const int cp )
        -> wstring
    {
        CPINFOEX info;
        const bool success = GetCPInfoEx( cp, {}, &info );
        now( success ) or fail( "GetCPInfoEx failed." );
        return info.CodePageName;
    }

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
            reinterpret_cast<wchar_t*>( &result ),
            sizeof( result )
            );
        now( success ) or fail( "GetLocaleInfo failed." );
        return result;
    }

    inline auto console_codepage()
        -> int
    {
        const int cp = GetConsoleCP();
        now( cp != 0 ) or fail( "GetConsoleCP failed." );
        return cp;
    }

    inline void set_console_codepage( const int cp )
    {
        SetConsoleCP( cp ) or fail( "SetConsoleCP failed." );
        SetConsoleOutputCP( cp ) or fail( "SetConsoleOutputCP failed." );
    }
}  // namespace winapi
