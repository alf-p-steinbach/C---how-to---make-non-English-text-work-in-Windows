#pragma once

#include <filesystem>
#include <string_view>

namespace fs_path_util {
    namespace fs = std::filesystem;
    using   std::string_view;           // <string_view>

    using Byte = unsigned char;
    template< class Type > using in_ = const Type&;

    constexpr auto literals_are_utf8_encoded()
        -> bool
    {
        constexpr auto& oe = "ø";
        return (Byte( oe[0] ) == 195 and Byte( oe[1] ) == 184);
    }

    static_assert( literals_are_utf8_encoded(), "With Visual C++ use option `/utf-8`." );

    inline auto make_path( in_<string_view> s )
        -> fs::path
    {
        #if __cplusplus >= 202002
            using U8_ptr = const char8_t;       // No such type in C++17, hence `#if`.
            return fs::path( U8_ptr( &*s.begin() ), U8_ptr( &*s.end() ) );
        #else
            return u8path( s );
        #endif
    }


}  // namespace fs_path_util
