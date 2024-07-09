#pragma once
#include <winapi/wrapped/windows-h.wide.hpp>
#include <cppm/basics.hpp>

#include <string>
#include <utility>

namespace winapi {
    using   cppm::in_, cppm::intsize_of,
            cppm::now, cppm::fail, cppm::Scope_guard,
            cppm::No_copy_or_move;
    using   std::wstring,               // <string>
            std::exchange, std::move;   // <utility>

    inline auto process_id_for( const HWND window )
        -> DWORD
    {
        DWORD result = 0;
        const DWORD thread_id = GetWindowThreadProcessId( window, &result );
        now( thread_id != 0 ) or fail( "GetWindowThreadProcessId failed" );
        return result;
    }

    class Process_handle:
        public No_copy_or_move      // TODO: support moving
    {
        HANDLE  m_handle;
        
    public:
        ~Process_handle() { if( m_handle ) { CloseHandle( m_handle ); } }
        Process_handle( HANDLE&& handle ): m_handle( exchange( handle, {} ) ) {}
        
        auto handle() const -> HANDLE { return m_handle; }
        operator HANDLE() const { return handle(); }
    };

    inline auto read_handle_for_process( const DWORD process_id )
        -> Process_handle
    {
        HANDLE result = OpenProcess(
            READ_CONTROL | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, process_id
            );
        now( result != 0 ) or fail( "OpenProcess failed to create a process handle." );
        return Process_handle( move( result ) );
    }

    inline auto exe_path_for_process( const HANDLE process )
        -> wstring
    {
        auto result = wstring( MAX_PATH, L'\0' );
        for( ;; ) {
            auto size = DWORD( result.size() );
            SetLastError( 0 );
            const bool success = QueryFullProcessImageName( process, 0, result.data(), &size );
            if( success ) {
                result.resize( size );
                break;
            }
            const bool larger_buffer_may_fix_it = (
                GetLastError() == ERROR_INSUFFICIENT_BUFFER and result.size() < 64*1024
            );
            now( larger_buffer_may_fix_it )
                or fail( "QueryFullProcessImageName failed to obtain exe name." );
            result.resize( 2*result.size() );
        }
        return result;
    }

    inline auto modulename_for_process( in_<wstring> path )
        -> wstring
    {
        const size_t i_last_separator   = path.find_last_of( L'\\' );
        assert( i_last_separator != path.npos );
        const size_t i_first_char       = i_last_separator + 1;
        const size_t i_last_period      = path.find_last_of( L'.' );
        const size_t n_chars            = (
            i_last_period == path.npos or i_last_period < i_first_char
            ? path.npos
            : i_last_period - i_first_char
            );
        return path.substr( i_first_char, n_chars );
    }

    inline auto modulename_for_process( const HANDLE process )
        -> wstring
    { return modulename_for_process( exe_path_for_process( process ) ); }

    inline auto modulename_for_process( const DWORD process_id )
        -> wstring
    { return modulename_for_process( read_handle_for_process( process_id ) ); }
}  // namespace winapi
