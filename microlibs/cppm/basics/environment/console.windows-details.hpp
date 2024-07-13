#pragma once
#include <cppm/basics/environment/os.hpp>
#ifndef CPPM_OS_IS_WINDOWS
#   error "This header is only for Windows environments."
#   include <stop-compilation>
#endif

#include <cppm/basics/class_kinds/No_copy_or_move.hpp>
#include <cppm/basics/environment/Console_kind-Enum.hpp>

#include <cstdio>

namespace cppm {
    using   std::FILE;          // <cstdio>

    namespace impl {
        // Ensures UTF-8 encoding assumption + enables ANSI escape sequence support.
        class Windows_console_config:
            No_copy_or_move
        {
            struct Codepage_config: No_copy_or_move
            {
                int m_original_codepage;
                ~Codepage_config();
                Codepage_config();
            };
                
            struct Ansi_escapes_config: No_copy_or_move
            {
                bool m_original_ansi_escapes_enabled;
                ~Ansi_escapes_config();
                Ansi_escapes_config();
            };

            Codepage_config         m_codepage_config;
            Ansi_escapes_config     m_ansi_escapes_config;

        public:
            ~Windows_console_config() = default;    // Restores original cp + ANSI escape support state.
            Windows_console_config() = default;
        };

        extern auto is_a_windows_console( FILE* f ) -> bool;
        extern auto get_windows_console_kind() -> Console_kind::Enum;   // none, mintty or windows_...
    }  // namespace impl

    inline namespace environment {
        class Console_config: public impl::Windows_console_config {};

        inline auto is_a_console( FILE* f )
            -> bool
        { return impl::is_a_windows_console( f ); }

        inline auto get_console_kind()
            -> Console_kind::Enum
        { return impl::get_windows_console_kind(); }

    }  // inline namespace environment
}  // namespace cppm
