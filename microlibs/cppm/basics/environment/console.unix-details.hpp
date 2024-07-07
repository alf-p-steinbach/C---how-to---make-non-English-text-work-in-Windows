#pragma once
#include <cppm/basics/environment/os.hpp>
#ifndef CPPM_OS_IS_UNIX
#   error "This header is only for Unix environments."
#   include <stop-compilation>
#endif

#include <cppm/basics/class_kinds/No_copy_or_move.hpp>
#include <cppm/basics/environment/Console_kind-Enum.hpp>

namespace cppm {
    namespace impl {
        struct Unix_console_config: No_copy_or_move {};
        using Console_config = Unix_console_config;

        extern auto get_unix_console_kind() -> Console_kind::Enum;      // `none` or `unix`.
    }  // namespace impl
}  // namespace cppm
