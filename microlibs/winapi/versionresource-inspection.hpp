#pragma once
#include <winapi/wrapped/windows-h.wide.hpp>
#include <cppm/basics.hpp>

#include <algorithm>
#include <cstdint>
#include <string>
#include <optional>
#include <vector>

namespace winapi {
    using   cppm::Byte, cppm::in_, cppm::intsize_of,
            cppm::now, cppm::fail;
    using   std::all_of,                        // <algorithm>
            std::uint32_t, std::int64_t,        // <cstdint>
            std::string, std::wstring,          // <string>
            std::in_place, std::optional,       // <optional>
            std::vector;

    namespace impl {
        auto is_zero( const unsigned v ) -> bool { return (v == 0); }
    }  // namespace impl

    class Version
    {
        uint32_t parts[4];          // Little-endian.

    public:
        Version( const uint32_t p3, const uint32_t p2, const uint32_t p1, const uint32_t p0 ):
            parts{ p0, p1, p2, p3 }
        {}

        Version( const uint32_t p3, const uint32_t p2 ):
            parts{ 0, 0, p2, p3 }
        {}
        
        Version(): parts{} {}

        auto has_value() const
            -> bool
        { return not all_of( CPPM_ITS_ALL( parts ), impl::is_zero ); }
        
        auto major() const      -> uint32_t { return parts[3]; }
        auto minor() const      -> uint32_t { return parts[2]; }
        auto revision() const   -> uint32_t { return parts[1]; }
        auto build() const      -> uint32_t { return parts[0]; }
        
        friend
        auto compare( in_<Version> a, in_<Version> b )
            -> int
        {
            for( int i = intsize_of( a.parts ) - 1; i >= 0; --i ) {
                if( const auto r = int( int64_t( a.parts[i] ) - b.parts[i] ) ) { return r; }
            }
            return 0;
        }
        
        friend
        auto operator<( in_<Version> a, in_<Version> b ) -> bool { return (compare( a, b ) < 0); }

        friend
        auto operator==( in_<Version> a, in_<Version> b ) -> bool { return (compare( a, b ) == 0); }
    };

    class Version_info
    {   
        vector<Byte>    m_buffer;
        
    public:
        Version_info( in_<wstring> exe_path )
        {
            const auto buffer_size = int( GetFileVersionInfoSize( exe_path.c_str(), {} ) );
            now( buffer_size > 0 )
                or fail( "GetFileVersionInfoSize failed." );
            m_buffer.resize( buffer_size );
            const bool success = GetFileVersionInfo( exe_path.c_str(), {}, buffer_size, m_buffer.data() );
            now( success )
                or fail( "GetFileVersionInfo failed" );
        }

        auto numeric() const
            -> const VS_FIXEDFILEINFO&
        {
            VS_FIXEDFILEINFO* p_buffer{};       // Microsoft constness problem.
            UINT n;
            const bool success = VerQueryValue(
                m_buffer.data(), LR"(\)", reinterpret_cast<void**>( &p_buffer ), &n
                );
            now( success )
                or fail( "VerQueryValue failed" );
            now( p_buffer->dwSignature == 0xFEEF04BD )
                or fail( "Wrong signature value in version info resource." );
            return *p_buffer;
        }

        auto product_version() const
            -> Version
        {
            const VS_FIXEDFILEINFO& info = numeric();
            const DWORD ls = info.dwProductVersionLS;
            const DWORD ms = info.dwProductVersionMS;
            // return {ls & 0xFFFF, ls >> 16, ms & 0xFFFF, ms >> 16};
            return Version( ms >> 16,  ms & 0xFFFF, ls >> 16,  ls & 0xFFFF );
        }
    };

    inline auto opt_version_info_of( in_<wstring> exe_path )
        -> optional<Version_info>
    {
        try {
            return Version_info( exe_path );
        } catch( ... ) {
            return {};
        }
    }
}  // namespace winapi
