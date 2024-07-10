﻿#pragma once

namespace cppm {
    inline namespace environment {

        // `windows_classic` is a classic Windows console.
        // `old_windows_terminal` is a version before version 1.20.xxx, May 2024,
        // the one which apparently introduced UTF-8 input.
        struct Console_kind{ enum Enum{
            none,
            unknown,
            windows_classic, windows_mintty, old_windows_terminal, windows_terminal,
            unix,
            _
        }; };

    }  // inline namespace environment
}  // namespace cppm
