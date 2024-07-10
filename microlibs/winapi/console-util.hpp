#pragma once
#include <winapi/wrapped/windows-h.wide.hpp>

// <<winapi/process-util.hpp> brings in
// modulename_for_process, process_id_for, exe_path_for_process, read_handle_for_process

#include <winapi/gui-util.hpp>                          // windowclass_name_of
#include <winapi/process-util.hpp>                      // (comment above)
#include <winapi/versionresource-inspection.hpp>        // Version

#include <cppm/basics.hpp>

#include <string>
#include <utility>

namespace winapi {
    using   cppm::in_, cppm::intsize_of,
            cppm::now, cppm::fail, cppm::Scope_guard;
    using   std::wstring,               // <string>
            std::move;                  // <utility>

    inline auto console_title()
        -> wstring
    {
        wstring result( 1024, '\0' );       // From one MS example; another one uses MAX_PATH.
        SetLastError( 0 );
        const int length = int( GetConsoleTitle( result.data(), intsize_of( result ) ) );
        now( GetLastError() == 0 ) or fail( "GetConsoleTitle failed" );
        result.resize( length );
        return result;
    }

    inline void set_console_title( in_<wstring> new_title )
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
    inline auto find_console_window()
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

    // ConsoleWindowClass  (powershell, cmd, git bash)
    // DesktopWindowXamlSource (wt)
    // mintty (MSYS2 bash)
    //
    struct Console_host_id
    {
        static constexpr auto&  classic_console     = L"ConsoleWindowClass";    // Window class name.
        static constexpr auto&  mintty              = L"mintty";                // Window class name.
        static constexpr auto&  windows_terminal    = L"WindowsTerminal";       // Executable base name.
        
        wstring     name;
        Version     version;        // All zeroes if not applicable.
    };

    inline auto get_console_host_id()
        -> Console_host_id
    {
        using Id = Console_host_id;
        const HWND window = find_console_window();
        wstring windowclass_name = windowclass_name_of( window );
        const bool reliable_windowclass_as_id =
            windowclass_name == Id::classic_console or windowclass_name == Id::mintty;
        if( not reliable_windowclass_as_id ) {
            const DWORD process_id = process_id_for( window );
            const wstring exe_path = exe_path_for_process( read_handle_for_process( process_id ) );
            wstring modulename = modulename_for_process( exe_path );
            // The module name is usually unrelated, e.g. "Cmd", but is good id for Windows Terminal:
            if( modulename == Id::windows_terminal ) {
                Version version{};
                try { version = Version_info( exe_path ).product_version(); } catch( ... ) {}
                return Id{ move( modulename ), version };
            }
        }
        return Id{ move( windowclass_name ), {} };
    }
}  // namespace winapi
