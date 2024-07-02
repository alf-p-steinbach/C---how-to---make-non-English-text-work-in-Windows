#// Include <windows.h>
#   ifndef UNICODE
#       error "Gah, no UNICODE".
#       include <stop-compilation>
#   endif
#   define WIN32_MEAN_AND_LEAN
#   define NOMINMAX
#   include <windows.h>

#include <functional>
#include <stdexcept>
#include <string>
#include <utility>

namespace cppm {
    using   std::function,              // <functional>
            std::runtime_error,         // <stdexcept>
            std::string,                // <string>
            std::exchange, std::move;   // <utility>

    template< class T > using in_ = const T&;

    template< class Collection >
    auto intsize_of( in_<Collection> c ) -> int { return int( size( c ) ); }

    constexpr auto now( const bool condition ) -> bool { return condition; }
    [[noreturn]] auto fail( in_<string> message ) -> bool { throw runtime_error( message ); }

    class No_copy_or_move
    {
        using Self = No_copy_or_move;
        No_copy_or_move( in_<Self> )            = delete;
        auto operator=( in_<Self> ) -> Self&    = delete;
        No_copy_or_move( Self&& )               = delete;
        auto operator=( Self&& ) -> Self&       = delete;

    public:
        No_copy_or_move() {}
    };

    class Scope_guard: public No_copy_or_move
    {
        function<void()>    m_cleanup;

    public:
        ~Scope_guard() { m_cleanup(); }
        Scope_guard( function<void()> cleanup ): m_cleanup( move( cleanup ) ) {}
    };
}  // namespace cppm

namespace winapi {
    using   cppm::in_, cppm::intsize_of,
            cppm::now, cppm::fail, cppm::Scope_guard;
    using   std::string, std::wstring;          // <string>

    auto console_title()
        -> wstring
    {
        wstring result( 1024, '\0' );       // From one MS example; another one uses MAX_PATH.
        SetLastError( 0 );
        const int length = int( GetConsoleTitle( result.data(), intsize_of( result ) ) );
        now( GetLastError() == 0 ) or fail( "GetConsoleTitle failed" );
        result.resize( length );
        return result;
    }

    void set_console_title( in_<wstring> new_title )
    {
        SetConsoleTitle( new_title.c_str() ) or fail( "SetConsoleTitle failed" );
    }

    // Microsoft KB 124103: “How to obtain a Console Window Handle (HWND)”
    // Because the `GetConsoleWindow` function usually gives nonsense result in Microsoft Terminal.
    //
    // ❝
    // 1. Call GetConsoleTitle() to save the current console window title.
    // 2. Call SetConsoleTitle() to change the console title to a unique title.
    // 3. Call Sleep(40) to ensure the window title was updated.
    // 4. Call FindWindow(NULL, uniquetitle), to obtain the HWND this call returns the HWND--or NULL if the operation failed.
    // 5. Call SetConsoleTitle() with the value retrieved from step 1, to restore the original window title.
    // ❞
    //
    // <url: https://learn.microsoft.com/en-us/troubleshoot/windows-server/performance/obtain-console-window-handle>
    //
    auto find_console_window()
        -> HWND
    {
        const auto& uuid = L"42DDBEFE-C675-4358-8D48-F6FB05AB85A7";
        const wstring original_title = console_title();
        set_console_title( uuid );
        const auto auto_restore = Scope_guard( [&]{ set_console_title( original_title ); } );

        HWND window = 0;
        for( int i = 1; i <= 5; ++i ) {
            Sleep( 40 );
            window = FindWindow( nullptr, uuid );
            if( window != 0 ) { break; }
        }
        now( window != 0 ) or fail( "FindWindow failed to find the console window." );

        return window;
    }

    auto u8_from( in_<wstring> s )
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

    auto process_id_for( const HWND window )
        -> DWORD
    {
        DWORD result = 0;
        const DWORD thread_id = GetWindowThreadProcessId( window, &result );
        now( thread_id != 0 ) or fail( "GetWindowThreadProcessId failed" );
        return result;
    }

    auto modulename_for( const DWORD process_id )
        -> wstring
    {
        const HANDLE handle = OpenProcess(
            READ_CONTROL | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, process_id
            );
        now( handle != 0 ) or fail( "OpenProcess failed to create a process handle." );
        const auto autoclose_the_handle = Scope_guard( [&]{ CloseHandle( handle ); } );

        auto path = wstring( MAX_PATH, L'\0' );
        for( ;; ) {
            auto size = DWORD( path.size() );
            SetLastError( 0 );
            const bool success = QueryFullProcessImageName( handle, 0, path.data(), &size );
            if( success ) {
                path.resize( size );
                break;
            }
            if( GetLastError() == ERROR_INSUFFICIENT_BUFFER ) {
                path.resize( 2*path.size() );
            } else {
                fail( "QueryFullProcessImageName failed to obtain exe name." );
            }
        }
        const size_t i_last_separator   = path.find_last_of( L'\\' );
        const size_t i_first_char       = i_last_separator + 1;
        const size_t i_last_period      = path.find_last_of( L'.' );
        // TODO: assert things? In particular some doubt about whether a period is guaranteed.
        return path.substr( i_first_char, i_last_period - i_first_char );
    }

}  // namespace winapi

#include <fmt/core.h>
auto main() -> int
{
    using namespace winapi;
    fmt::print( "{:s}\n", u8_from( modulename_for( process_id_for( find_console_window() ) ) ) );
}
