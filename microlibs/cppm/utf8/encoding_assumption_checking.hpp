#pragma once

#include <cppm/basics/Byte.hpp>

namespace cppm::utf8 {
    inline namespace encoding_assumption_checking {
        static constexpr auto& oe = "ø";

        constexpr auto literals_are_utf8()
            -> bool
        { return (Byte( oe[0] ) == 195 and Byte( oe[1] ) == 184 ); }

        // `os_api_is_utf8` iff `GetACP() = 65001` in Windows, and just true for other OS-es.
        extern auto os_api_is_utf8() -> bool;
    }  // namespace encoding_assumption_checking
}  // namespace cppm::utf8

namespace cppm{ using namespace utf8::encoding_assumption_checking; };
