#pragma once

#include <cppm/basics/Byte.hpp>
#include <assert.h>

namespace cppm::utf8 {
    inline namespace encoding_assumption_checking {
        static constexpr auto& oe = "ø";

        constexpr auto literals_are_utf8()
            -> bool
        { return (Byte( oe[0] ) == 195 and Byte( oe[1] ) == 184 ); }

        // `os_api_is_utf8` iff `GetACP() = 65001` in Windows, and just true for other OS-es.
        extern auto os_api_is_utf8() -> bool;

        inline void globally_once_assert_utf8_literals()
        {
            static const int dummy = (  // A globally-once check, on first call of this function.
                assert( literals_are_utf8() or !"Use the compiler option(s) for UTF-8 literals." ),
                0
                );
            (void) dummy;
        }
    }  // namespace encoding_assumption_checking
}  // namespace cppm::utf8

namespace cppm{ using namespace utf8::encoding_assumption_checking; };
