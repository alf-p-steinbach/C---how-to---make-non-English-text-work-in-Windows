#pragma once
#include <cppm/utf8/encoding_assumption_checking.hpp>

#include <winapi/utf8-checking.hpp>

auto cppm::utf8::encoding_assumption_checking::os_api_is_utf8()
    -> bool
{ return winapi::is_utf8_codepage(); }
