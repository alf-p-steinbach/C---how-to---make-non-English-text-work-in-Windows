#pragma once
#include <winapi/wrapped/windows-h.wide.hpp>

namespace winapi {
    inline auto is_utf8_codepage()
        -> bool
    { return (GetACP() == 65001); }
}  // namespace winapi
