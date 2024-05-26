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



}  // namespace fs_path_util
