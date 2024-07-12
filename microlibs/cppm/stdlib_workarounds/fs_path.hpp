#pragma once
#include <cppm/basics/type_makers.hpp>                      // in_
#include <cppm/utf8/encoding_assumption_checking.hpp>       // globally_once_assert_utf8_literals

#include <filesystem>
#include <string>
#include <string_view>

namespace cppm {
    namespace fs = std::filesystem;
    using   std::string,            // <string>
            std::string_view;       // <string_view>

    #if __cplusplus >= 202002
        using   std::u8string,          // <string>
                std::u8string_view;     // <string_view>
    #endif

    inline namespace stdlib_workarounds {
        inline auto path_from_u8( in_<string_view> spec )
            -> fs::path
        {
            globally_once_assert_utf8_literals();
            #if __cplusplus < 202002    // `<` b/c `u8path` is deprecated in C++20; ⇨ warnings. 
                return fs::u8path( spec );
            #else
                using U8 = const char8_t;   // `char8_t` is a distinct type in C++20 and later.
                return fs::path( u8string_view( reinterpret_cast<U8*>( spec.data() ), spec.size() ) );
            #endif 
        }

        inline auto to_u8_string( in_<fs::path> p )
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
    }  // inline namespace stdlib_workarounds
}  // namespace cppm
