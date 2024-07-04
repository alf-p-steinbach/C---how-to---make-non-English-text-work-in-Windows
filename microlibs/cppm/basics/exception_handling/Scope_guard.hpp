#pragma once

#include <functional>

namespace cppm {
    using   std::function;          // <functional>

    namespace impl {
        struct Scope_guard_base:
            public No_copy_or_move
        {
            function<void()> m_cleanup;
            void dismiss() { m_cleanup = []{}; }
            ~Scope_guard_base() { m_cleanup(); }
            Scope_guard_base( function<void()> cleanup ): m_cleanup( move( cleanup ) ) {}
        };
    }  // namespace impl

    inline namespace exception_handling {
        class Scope_guard:
            private impl::Scope_guard_base
        {
        public:
            using impl::Scope_guard_base::Scope_guard_base;     // Inherit the constructor.
        };

        class Dismissable_scope_guard:
            private impl::Scope_guard_base
        {
        public:
            using impl::Scope_guard_base::Scope_guard_base;     // Inherit the constructor.
            using impl::Scope_guard::dismiss;
        };
    }  // inline namespace exception_handling
}  // namespace cppm
