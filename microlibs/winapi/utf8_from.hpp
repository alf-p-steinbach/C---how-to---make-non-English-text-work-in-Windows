#pragma once
#include <winapi/wrapped/windows-h.wide.hpp>
#include <cppm/basics.hpp>

#include <string>

namespace winapi {
    using   cppm::in_, cppm::now, cppm::fail, cppm::intsize_of;
    using   std::string, std::wstring;                  // <string>
 
    auto utf8_from( in_<wstring> s )
        -> string
    {
        if( s.empty() ) { return ""; }

        const DWORD flags = WC_ERR_INVALID_CHARS;
        const int buffer_size = WideCharToMultiByte(
            CP_UTF8, flags, s.data(), intsize_of( s ), nullptr, 0, nullptr, nullptr
            );
        now( buffer_size > 0 ) or fail( "WideCharToMultiByte failed to obtain buffer size" );

        auto result = string( buffer_size, '\0' );
        const int result_length = WideCharToMultiByte(
            CP_UTF8, flags, s.data(), intsize_of( s ), result.data(), buffer_size, nullptr, nullptr
            );
        now( result_length > 0 ) or fail( "WideCharToMultiByte failed to convert to UTF-8." );

        result.resize( result_length );         // Just for good measure.
        return result;
    }
}  // namespace winapi
