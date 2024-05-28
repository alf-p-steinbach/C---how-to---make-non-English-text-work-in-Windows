#pragma once
#include <cppm/stdlib_workarounds/fs_path.hpp>
#include <cppm/basics/type_makers.hpp>                      // in_

#include <assert.h>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

namespace cppm {
    namespace fs = std::filesystem;
    using   std::string,            // <string>
            std::string_view,       // <string_view>
            std::move;              // <utility>

    inline namespace filesystem {
        class Path
        {
            fs::path    m_path;

        public:
            Path() noexcept {}

            Path( in_<string_view> spec ):
                m_path( stdlib_workarounds::path_from_u8( spec ) )
            {}

            Path( in_<Path> other ): m_path( other.m_path ) {}
            Path( Path&& other ) noexcept : m_path( move( other.m_path ) ) {}

            friend
            void swap( Path& a, Path& b ) noexcept
            {
                a.m_path.swap( b.m_path );
            }

            auto operator=( in_<Path> other ) -> Path& { m_path = other.m_path; return *this; }
            auto operator=( Path&& other ) noexcept -> Path& { m_path = move( other.m_path ); return *this; }

            auto is_empty() const noexcept -> bool { return m_path.empty(); }

            auto str() const -> string { return stdlib_workarounds::to_u8_string( m_path ); }
            operator string () const { return str(); }      // File open & formatting support.
            auto operator-() const -> string { return str(); }          // Reduction to string.
        };

        // inline auto format_as( in_<Path> p ) -> string { return p.str(); }   // Doesn't work. :(
    }  // pseudo-inline namespace filesystem
}  // namespace cppm
