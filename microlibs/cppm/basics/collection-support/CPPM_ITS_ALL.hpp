#pragma once
#include <iterator>     // std::begin, std::end

#define CPPM_ITS_ALL( collection )  \
    (::cppm::guaranteed_lvalue( collection ), std::begin( collection )), std::end( collection )

namespace cppm {
    template< class Type > void guaranteed_lvalue( const Type& ) {}
    template< class Type > void guaranteed_lvalue( Type&& ) = delete;
}  // namespace cppm
