#pragma once

#include <cppm/basics/type_makers.hpp>          // in_

namespace cppm {
    inline namespace class_kinds {

        class No_copy_or_move
        {
            using Self = No_copy_or_move;
            No_copy_or_move( in_<Self> )            = delete;
            auto operator=( in_<Self> ) -> Self&    = delete;
            No_copy_or_move( Self&& )               = delete;
            auto operator=( Self&& ) -> Self&       = delete;

        public:
            constexpr No_copy_or_move() noexcept {}
        };

    }  // inline namespace class_kinds
}  // namespace cppm
