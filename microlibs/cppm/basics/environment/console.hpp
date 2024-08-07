#pragma once
#include <cppm/basics/environment/os.hpp>

#if CPPM_OS_IS_WINDOWS
#   include <cppm/basics/environment/console.windows-details.hpp>
#elif CPPM_OS_IS_UNIX
#   include <cppm/basics/environment/console.unix-details.hpp>
#else
#   error "Only Windows and Unix are supported."
#   include <stop-compilation>
#endif
static_assert( sizeof( cppm::Console_config ), "OS details header should define Console_config." );

#include <cppm/basics/class_kinds/No_copy_or_move.hpp>
#include <cppm/basics/environment/Console_kind-Enum.hpp>
#include <cppm/basics/exception_handling/now_and_fail.hpp>
#include <cppm/utf8/encoding_assumption_checking.hpp>

#include <cstdio>
#include <optional>

namespace cppm {
    using   std::FILE,          // <cstdio>
            std::optional;      // <optional>

    inline namespace environment {
        // For-exposition redeclarations of stuff from either the Windows or Unix details header.
        class Console_config;
        inline auto is_a_console( FILE* f ) -> bool;
        inline auto get_console_kind() -> Console_kind::Enum;
    }  // inline namespace environment

    inline namespace environment {
        inline auto console_kind()
            -> Console_kind::Enum
        {
            // Cached console kind info, assuming the process doesn't create or remove a console.
            static const Console_kind::Enum     the_console_kind = get_console_kind();
            return the_console_kind;
        }

        inline auto this_process_has_a_console()
            -> bool
        { return (console_kind() != Console_kind::none); }

        inline auto console_has_utf8_input()
            -> bool
        { return (console_kind() >= Console_kind::windows_terminal); }

        inline void assert_utf8_literals()
        {
            assert( literals_are_utf8()
                or !"Use the compiler option(s) for UTF-8 literals, e.g. MSVC `/utf-8`." );
        }

        inline void require_console()
        {
            assert_utf8_literals();
            now( this_process_has_a_console() )
                or fail( "This program requires a console." );
        }

        class Console_usage:
            public No_copy_or_move
        {
            optional<Console_config>    m_config;

        public:
            Console_usage()
            {
                assert_utf8_literals();
                if( this_process_has_a_console() ) { m_config.emplace(); }
            }
        };
    }  // inline namespace environment
}  // namespace cppm
