#pragma once
#include <winapi/wrapped/windows-h.wide.hpp>

#include <cppm/basics.hpp>

#include <string>

namespace winapi {
    using   cppm::intsize_of, cppm::now, cppm::fail;
    using   std::wstring;           // <string>

    inline auto windowclass_name_of( const HWND window )
        -> wstring
    {
        // Docs of `WNDCLASS`:
        // “The maximum length for lpszClassName is 256. If lpszClassName is greater than the
        // maximum length, the RegisterClass function will fail.”
        //
        wchar_t buffer[256];
        const int length = GetClassName( window, buffer, intsize_of( buffer ) );
        now( length > 0 ) or fail( "GetClassName failed" );
        return wstring( buffer, length );   // Window class names are usually short, probably SSO.
    }
}  // namespace winapi
