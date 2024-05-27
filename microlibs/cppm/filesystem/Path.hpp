#pragma once
#include <cppm/basics/type_makers.hpp>                      // in_
#include <cppm/utf8/encoding_assumption_checking.hpp>       // globally_once_assert_utf8_literals

#include <assert.h>
#include <filesystem>
#include <string>
#include <string_view>

namespace cppm {
    namespace sfs = std::filesystem;
    using   std::string,            // <string>
            std::string_view;       // <string_view>

    #if __cplusplus >= 202002
        using   std::u8string,          // <string>
                std::u8string_view;     // <string_view>
    #endif

    namespace stdlib_workarounds {
        inline auto path_from_u8( in_<string_view> spec )
            -> sfs::path
        {
            globally_once_assert_utf8_literals();
            #if __cplusplus <= 202002   // `<=` b/c `u8path` still exists in C++20, as deprecated.
                return sfs::u8path( spec );
            #else
                using U8 = const char8_t;   // `char8_t` is a distinct type in C++20 and later.
                return sfs::path( u8string_view( reinterpret_cast<U8*>( spec.data() ), s.size() );
            #endif 
        }

        inline auto to_u8_string( in_<sfs::path> p )
            -> string
        {
            globally_once_assert_utf8_literals();
            #if __cplusplus < 202002
                return p.u8string();                    // Returns a `std::string` in C++17.
            #else
                const std::u8string s = p.u8string();
                return string( s.begin(), s.end() );    // Needless copy except for C++20 nonsense.
            #endif 
        }
    }  // namespace stdlib_workarounds

    inline namespace filesystem {
        class Path
        {
            sfs::path   m_path;

        public:
            Path( in_<string_view> spec ):
                m_path( stdlib_workarounds::path_from_u8( spec ) )
            {}

            auto str() const -> string { return stdlib_workarounds::to_u8_string( m_path ); }
            operator string () const { return str(); }      // File open & formatting support.
            auto operator-() const -> string { return str(); }          // Reduction to string.
        };

        inline auto format_as( in_<Path> p ) -> string { return p.str(); }   // Doesn't work. :(
    }  // pseudo-inline namespace filesystem
}  // namespace cppm
