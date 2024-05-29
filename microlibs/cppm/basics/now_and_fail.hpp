#pragma once

#include <fmt/core.h>
#include <cppm/basics/type_makers.hpp>          // in_

#include <stdexcept>
#include <string_view>
#include <string>

namespace cppm {
    using   std::runtime_error,         // <stdexcept>
            std::string_view,           // <string_view>
            std::string;                // <string>

    inline namespace now_and_fail {
        constexpr auto now( const bool condition ) noexcept
            -> bool
        { return condition; }

        template< class... Args >
        [[noreturn]]
        inline auto fail( in_<string_view> s, in_<Args>... args )
            -> bool
        {
            const fmt::format_args vargs = fmt::make_format_args( +args... );
            throw runtime_error( fmt::vformat( s, vargs ) );   // Note: avoid ADL.
            for( ;; ) {}        // Should never get here. Also, avoid g++ silly-warning.
        }
    }  // inline namespace now_and_fail
}  // namespace cppm
