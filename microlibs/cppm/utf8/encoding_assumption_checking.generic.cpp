#include <cppm/utf8/encoding_assumption_checking.hpp>

namespace cppm::utf8 {
    auto encoding_assumption_checking::os_api_is_utf8()
        -> bool
    { return true; }
}  // namespace cppm::utf8

