#pragma once

#ifdef _WIN32
#   define CPPM_OS_IS_WINDOWS true
#else
#   define CPPM_OS_IS_WINDOWS false
#endif

// Unfortunately not all compilers define `__unix__` in a Unix environment,
// “e.g. the xlC or the DEC C/C++ compiler”.
#undef CPPM_OS_IS_UNIX
#if __has_include( <unistd.h> )
#   #include <unistd.h>
#   ifdef _POSIX_VERSION
#       define CPPM_OS_IS_UNIX true
#   endif
#endif
#ifndef CPPM_OS_IS_UNIX
#   define CPPM_OS_IS_UNIX false
#endif

namespace cppm {
    namespace os {
        constexpr bool  is_windows  = CPPM_OS_IS_WINDOWS;
        constexpr bool  is_unix     = CPPM_OS_IS_UNIX;
        
        static_assert( not( is_windows and is_unix ) );
    }  // namespace os
}  // namespace cppm
