#pragma once

#include <fmt/core.h>
#include <cppm/basics/type_makers.hpp>          // in_

#include <stdexcept>
#include <string_view>

namespace cppm {
    using   fmt::make_format_args, fmt::vformat;
    using   std::runtime_error,         // <stdexcept>
            std::string_view;           // <string>

    inline namespace now_and_fail {
        constexpr auto now( const bool condition ) noexcept
            -> bool
        { return condition; }

        template< class... Args >
        [[noreturn]]
        inline auto fail( in_<string_view> s, in_<Args>... args )
            -> bool
        { throw runtime_error( vformat( s, make_format_args( args... ) ) ); }
    }  // inline namespace now_and_fail
}  // namespace cppm
