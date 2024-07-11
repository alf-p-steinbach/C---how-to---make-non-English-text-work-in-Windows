#include <fmt/core.h>

#include <string>
using   std::string;                // <string>

#ifdef _WIN32
#   // Include <windows.h>:
#   define STRICT                   // General C++ compatibility. As of 2024 is now the default but.
#   define NOMINMAX                 // No C++-incompatible `min` and `max` macros, please.
#   define WIN32_LEAN_AND_MEAN      // Reduces header size by roughly 50%, e.g. 300'000 → 150'000 lines.
#   define UNICODE                  // Function macros resolve to ...W functions, the `wchar_t` functions.
#   include <windows.h>

    #include <iterator>
    using   std::size;              // <iterator>

    using Byte = unsigned char;

    auto input_line() -> string
    {
        static const HANDLE input = ::GetStdHandle( STD_INPUT_HANDLE );
        
        wchar_t buffer[256];        // Sufficient for interactive line input.
        DWORD n_chars_read = 0;
        ::ReadConsole( input, buffer, size( buffer ), &n_chars_read, nullptr ); // TODO: failure handling.
        const int n_u8_bytes = ::WideCharToMultiByte(
            CP_UTF8, 0, buffer, n_chars_read, nullptr, 0, nullptr, nullptr
            );
        auto result = string( n_u8_bytes, '\0' );
        ::WideCharToMultiByte(
            CP_UTF8, 0, buffer, n_chars_read, result.data(), n_u8_bytes, nullptr, nullptr
            );
        while( not result.empty() and Byte( result.back() ) < ' ' ) { result.pop_back(); }      // CRLF.
        return result;
    }
#else
    #include <iostream>
    using   std::cin,               // <iostream>
            std::getline;           // <string>

    auto input_line() -> string
    {
        string result;
        getline( cin, result );     // TODO: failure handling.
        return result;
    }
#endif

auto main() -> int
{
    fmt::print( "Hi, what’s your name? " );
    const string name = input_line();
    fmt::print( "Good to meet you, {}!\n", name );
}
