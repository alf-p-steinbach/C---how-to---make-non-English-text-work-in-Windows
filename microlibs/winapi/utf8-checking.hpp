#pragma once
#include <winapi/wrapped/windows-h.hpp>

namespace winapi {
    inline auto is_utf8_codepage()
        -> bool
    { return (GetACP() == 65001); }

    inline auto winapi_h_assert_utf8_codepage()
        -> bool
    {
        assert( is_utf8_codepage() or !"The process codepage isn't UTF-8 (old Windows?)." );
        return true;
    }
}  // namespace winapi
