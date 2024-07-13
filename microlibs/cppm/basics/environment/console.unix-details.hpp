#pragma once
#include <cppm/basics/environment/os.hpp>
#ifndef CPPM_OS_IS_UNIX
#   error "This header is only for Unix environments."
#   include <stop-compilation>
#endif

#include <cppm/basics/class_kinds/No_copy_or_move.hpp>
#include <cppm/basics/environment/Console_kind-Enum.hpp>

#include <cstdio>

namespace cppm {
    using       std::FILE;          // <cstdio>

    namespace impl {
        struct Unix_console_config: No_copy_or_move {};
        extern auto is_a_unix_console( FILE* f ) -> bool;
        extern auto get_unix_console_kind() -> Console_kind::Enum;      // `none` or `unix`.
    }  // namespace impl

    inline namespace environment {
        class Console_config: public impl::Unix_console_config {};

        inline auto is_a_console( FILE* f )
            -> bool
        { return impl::is_a_unix_console( f ); }

        inline auto get_console_kind()
            -> Console_kind::Enum
        { return impl::get_unix_console_kind(); }
    }  // inline namespace environment
}  // namespace cppm
