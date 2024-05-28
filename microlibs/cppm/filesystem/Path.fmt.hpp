#pragma once
#include <cppm/filesystem/Path.hpp>

#include <fmt/core.h>
#include <fmt/format.h>         // fmt::formatter<T>::format

template<>
struct fmt::formatter<cppm::filesystem::Path>:
    formatter<std::string>
{
    // parse is inherited from `formatter<string>`.

    auto format( const cppm::filesystem::Path& p, format_context& ctx ) const
        -> auto
    { return formatter<std::string>::format( p.str(), ctx ); }
};
