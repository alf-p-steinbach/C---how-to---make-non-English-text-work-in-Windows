#ifndef UNICODE
#   error "Define the preprocessor symbol UNICODE in the build (e.g. option `-D UNICODE`)."
#   include <stop-compilation>      // For e.g. the g++ compiler.
#endif
#include "windows-h.hpp"
static_assert( IS_WIDE_WINAPI(), IS_WIDE_WINAPI_TEXT() );
#include <shellapi.h>               // CommandLineToArgvW

#include <assert.h>                 // assert
#include <stddef.h>                 // size_t
#include <stdio.h>                  // stderr, fprintf, printf

#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
using   std::size,                              // <iterator>
        std::optional,                          // <optional>
        std::runtime_error,                     // <stdexcept>
        std::string,                            // <string>
        std::string_view, std::wstring_view,    // <string_view>
        std::vector;                            // <vector>
using namespace std::string_literals;  // ""s

template< class T >
using T_ = T;

struct No_copying_or_moving
{
    No_copying_or_moving( const No_copying_or_moving& ) = delete;
    auto operator=( const No_copying_or_moving& ) -> No_copying_or_moving& = delete;
    
    No_copying_or_moving( No_copying_or_moving&& ) = delete;
    auto operator=( No_copying_or_moving&& ) -> No_copying_or_moving& = delete;
    
    No_copying_or_moving() {}
};

struct Bool_param { constexpr operator bool() const { return true; } };

template< class C >
auto int_size( const C& c ) -> int { return static_cast<int>( size( c ) ); }

auto hopefully( const bool condition ) -> bool { return condition; }
auto fail( const string& s ) -> bool { throw runtime_error( s ); }

#define FAIL( s ) fail( string() + __func__  + " - " + (s) )

using C_wstr = const wchar_t*;

auto to_utf8( const wstring_view& ws )
    -> string
{
    if( ws.empty() ) { return ""; }
    assert( ws.size() <= size_t( INT_MAX ) );

    const DWORD flags = 0;
    const int buffer_size = ::WideCharToMultiByte(
        CP_UTF8, flags, ws.data(), int_size( ws ), nullptr, 0, nullptr, nullptr
        );
    hopefully( buffer_size != 0 )
        or FAIL( "Windows’ WideCharToMultiByte failed" );

    auto result = string( buffer_size, '\0' );
    const int string_size = ::WideCharToMultiByte(
        CP_UTF8, flags, ws.data(), int_size( ws ), result.data(), buffer_size, nullptr, nullptr
        );
    result.resize( string_size );       // Not clear if this is necessary.
    return result;
}

namespace app {
    class Resource_module: No_copying_or_moving
    {
        HMODULE     m_handle;
        
    public:
        ~Resource_module() { ::FreeLibrary( m_handle ); }

        Resource_module( const C_wstr file_path ):
            m_handle( ::LoadLibraryEx( file_path, 0, LOAD_LIBRARY_AS_DATAFILE ) )
        {
            hopefully( m_handle != 0 )
                or FAIL( "Windows’ LoadLibraryEx failed" );
        }
        
        auto manifest() const
            -> optional<string>
        {
            const HRSRC res_handle = ::FindResource( m_handle, CREATEPROCESS_MANIFEST_RESOURCE_ID, RT_MANIFEST );
            if( res_handle == 0 ) {
                return {};
            }
            const HGLOBAL data_handle = ::LoadResource( m_handle, res_handle );
            hopefully( data_handle != 0 )
                or FAIL( "Windows’ LoadResource failed (after FindResource succeeded)" );
            const DWORD n_bytes = ::SizeofResource( m_handle, res_handle );
            hopefully( n_bytes != 0 )
                or FAIL( "Windows’ SizeofResource failed (after FindResource and LoadResource succeeded)" );
            const T_<void*> p_data = ::LockResource( data_handle );
            hopefully( p_data != nullptr )
                or FAIL( "Windows’ LockResource failed (after FindResource, LoadResource and SizeofResource succeeded)" );
            return string( static_cast<char*>( p_data ), n_bytes );
        }
    };

    class Resource_updater: No_copying_or_moving
    {
        static constexpr auto delete_existing_resources = Bool_param();
        static constexpr auto discard_updates           = Bool_param();

        HANDLE  m_update_handle;
        
        auto begin_update( const C_wstr file_path )
            -> HANDLE
        { return ::BeginUpdateResource( file_path, not delete_existing_resources ); }
 
        auto end_update()
            -> bool
        { return ::EndUpdateResource( m_update_handle, not discard_updates ); }

        auto discard_changes_and_end_update()
            -> bool
        { return ::EndUpdateResource( m_update_handle, discard_updates ); }

    public:
        ~Resource_updater()
        {
            if( m_update_handle != 0 ) {
                discard_changes_and_end_update();
            }
        }

        Resource_updater( const C_wstr file_path ):
            m_update_handle( begin_update( file_path ) )
        {
            hopefully( m_update_handle != 0 )
                or FAIL( "Windows’ BeginUpdateResource failed" );
        }
        
        auto add_update_of_manifest( const string_view& text )
            -> Resource_updater&
        {
            assert( m_update_handle != 0 );
            ::UpdateResource(
                m_update_handle,
                RT_MANIFEST,
                MAKEINTRESOURCE( 1 ),
                0,      // language identifier, n/a
                const_cast<char*>( text.data() ),
                static_cast<DWORD>( text.size() )
                )
                or FAIL( "Specifying the update via Windows’ UpdateResource failed." );
            return *this;
        }
        
        void commit()
        {
            const bool ok = end_update();
            if( not ok ) {
                FAIL( "Committing changes via Windows’ EndUpdateResource failed." );
            }
            m_update_handle = 0;
        }
    };

    void add_manifest_to( const C_wstr file_path )
    {
        constexpr auto xml = string_view(
            R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>)" "\r\n"
            R"(<assembly manifestVersion="1.0" xmlns="urn:schemas-microsoft-com:asm.v1">)" "\r\n"
            R"(  <assemblyIdentity type="win32" name="appname" version="1.0.0.0"/>)" "\r\n"
            R"(  <application>)" "\r\n"
            R"(    <windowsSettings>)" "\r\n"
            R"(      <activeCodePage xmlns="http://schemas.microsoft.com/SMI/2019/WindowsSettings">UTF-8</activeCodePage>)" "\r\n"
            R"(    </windowsSettings>)" "\r\n"
            R"(  </application>)" "\r\n"
            R"(</assembly>)" "\r\n"
            );

        if( const auto opt_manifest = Resource_module( file_path ).manifest() ) {
            FAIL( "The file already has a manifest:\n\n" + opt_manifest.value() );
        }
        
        Resource_updater( file_path ).add_update_of_manifest( xml ).commit();

        const auto module = Resource_module( file_path );
        const auto opt_manifest = module.manifest();
        hopefully( opt_manifest.has_value() )
            or FAIL( "Failed to store a manifest resource." );
        hopefully( opt_manifest.value() == xml )
            or FAIL( "The update failed to store the new manifest exactly." );
    }

    void run( const C_wstr cmd_verb, const vector<C_wstr>& cmd_args )
    {
        hopefully( int_size( cmd_args ) == 1 )
            or fail( ""s + "Usage: " + to_utf8( cmd_verb ) + " EXEFILENAME" );
        const C_wstr file_path = cmd_args[0];
        add_manifest_to( file_path );
    }
}  // namespace app


#include <stdlib.h>         // EXIT_...

auto wchar_based_main( const int n_cmd_parts, const T_<const C_wstr*> cmd_parts ) noexcept
    -> int
{
    using std::exception;
    try {
        const C_wstr    cmd_verb        = cmd_parts[0];
        const int       n_cmd_args      = n_cmd_parts - 1;
        const auto      cmd_args        = cmd_parts + 1;
        app::run( cmd_verb, vector<C_wstr>( cmd_args, cmd_args + n_cmd_args ) );
        return EXIT_SUCCESS;
    } catch( const exception& x ) {
        fprintf( stderr, "!%s\n", x.what() );
    }
    return EXIT_FAILURE;
}

auto main()
    -> int
{
    struct Exit_code{ enum Enum: int {}; };

    struct Command_line: No_copying_or_moving
    {
        int         n_parts     = 0;
        wchar_t**   parts       = nullptr;

        ~Command_line() { ::LocalFree( reinterpret_cast<HLOCAL>( parts ) ); }

        Command_line()
        {
            ::SetLastError( 0 );
            parts = ::CommandLineToArgvW( ::GetCommandLine(), &n_parts );
            if( parts == nullptr ) {
                const int err = ::GetLastError();
                throw Exit_code::Enum( err? err : EXIT_FAILURE );
            }
        }
    };

    try {
        const Command_line cmd;
        return wchar_based_main( cmd.n_parts, cmd.parts );
    } catch( const Exit_code::Enum exit_code ) {
        return exit_code;
    }
}
