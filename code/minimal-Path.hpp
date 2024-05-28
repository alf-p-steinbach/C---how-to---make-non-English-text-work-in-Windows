#pragma once
#include <cppm/stdlib_workarounds/fs_path.hpp>
#include <cppm/basics/type_makers.hpp>                      // in_

#include <filesystem>
#include <string>
#include <string_view>

namespace minimal {
    namespace fs = std::filesystem;
    namespace stdlib_workarounds = cppm::stdlib_workarounds;
    using   cppm::in_;
    using   std::string,            // <string>
            std::string_view;       // <string_view>

    class Path
    {
        fs::path    m_path;

    public:
        Path( in_<string_view> spec ):
            m_path( stdlib_workarounds::path_from_u8( spec ) )  // Also asserts UTF-8 literals.
        {}

        auto str() const -> string { return stdlib_workarounds::to_u8_string( m_path ); }
        operator string () const { return str(); }      // File open & formatting support.
        auto operator-() const -> string { return str(); }          // Reduction to string.
    };
}  // namespace minimal
