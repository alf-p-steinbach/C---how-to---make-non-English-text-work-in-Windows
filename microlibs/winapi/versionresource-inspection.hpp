#pragma once
#include <winapi/wrapped/windows-h.wide.hpp>
#include <cppm/basics.hpp>

#include <cstdint>
#include <string>
#include <optional>
#include <vector>

namespace windows {
    using   cppm::Byte, cppm::in_,
            cppm::now, cppm::fail;
    using   std::uint32_t,                      // <cstdint>
            std::string, std::wstring,          // <string>
            std::in_place, std::optional,       // <optional>
            std::vector;

    struct Version
    {
        uint32_t parts[4];
        auto major() const -> uint32_t { return parts[3]; }
        auto minor() const -> uint32_t { return parts[2]; }
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
            return {ls & 0xFFFF, ls >> 16, ms & 0xFFFF, ms >> 16};
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
