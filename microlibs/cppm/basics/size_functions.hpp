#pragma once

#include <iterator>

namespace cppm {
    using   std::size;          // <iterator>

    inline namespace size_functions {

        template< class Collection >
        constexpr auto intsize_of( in_<Collection> c )
            -> int
        { return static_cast<int>( size( c ) ); }

    }  // inline namespace size_functions
}  // namespace cppm
