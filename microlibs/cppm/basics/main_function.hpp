#pragma once
#include <cppm/basics/type_makers.hpp>
#include <fmt/core.h>

#include <functional>
#include <stdexcept>

#include    <stdlib.h>      // stderr macro
#include    <stdlib.h>      // EXIT_... macro

namespace cppm {
    using   std::function,              // <functional>
            std::exception;             // <stdexcept>

    inline namespace main_function {
        using Simple_main = function<void()>;

        inline auto with_exceptions_displayed( in_<Simple_main> f ) noexcept
            -> int
        {
            try {
                f();
                return EXIT_SUCCESS;
            } catch( in_<exception> x ) {
                fmt::print( stderr, "!{}\n", x.what() );
            } catch( ... ) {
                fmt::print( stderr, "!<A non-standard exception>.\n" );
            }
            return EXIT_FAILURE;
        }
    }  // inline namespace main_function

}  // namespace cppm
