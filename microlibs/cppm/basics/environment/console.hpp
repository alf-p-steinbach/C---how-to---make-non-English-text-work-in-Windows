#pragma once

#include <cppm/basics/environment/os.hpp>

#include <cstdio>

namespace cppm {
    using   std::FILE;          // <cstdio>

    inline namespace environment {

        // Posix `isatty`, Windows `_isatty`.
        extern auto is_a_console( FILE* f ) -> bool;

        // `windows_classic` includes mintty hosting. `old_windows_terminal` is a version before
        // version 1.20.xxx, May 2024, which (apparently) introduced UTF-8 input.
        struct Console_kind{ enum Enum{
            none, unknown, windows_classic, old_windows_terminal, windows_terminal, unix, _
        }; };

        inline auto get_console_kind()
            -> Console_kind::Enum
        {
            #if CPPM_OS_IS_WINDOWS
                extern auto get_windows_console_kind() -> Console_kind::Enum;   // `none` or `windows_...`
                return get_windows_console_kind();
            #elif CPPM_OS_IS_UNIX
                extern auto get_unix_console_kind() -> Console_kind::Enum;      // `none` or `unix`.
                return Console_kind::unix;      // TODO: check for no console in Unix.
            #else
                return Console_kind::unknown;
            #endif
        }

        inline auto console_kind()
            -> Console_kind::Enum
        {
            static const Console_kind::Enum     the_console_kind = get_console_kind();
            return the_console_kind;
        }
        
        inline auto this_process_has_a_console()
            -> bool
        { return (console_kind() != Console_kind::none); }

        inline auto console_has_u8_input()
            -> bool
        { return (console_kind() >= Console_kind::windows_terminal); }

    }  // inline namespace environment
}  // namespace cppm
