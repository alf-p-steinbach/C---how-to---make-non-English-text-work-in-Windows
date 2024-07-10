#include <fmt/core.h>
#include <cppm/basics.hpp>
#include <winapi/utf8_from.hpp>
#include <winapi/versionresource-inspection.hpp>

#include <cstdint>
#include <string>
#include <optional>
#include <vector>

namespace app {
    using   cppm::Byte, cppm::in_,
            cppm::now, cppm::fail;

    using   winapi::utf8_from, winapi::Version, winapi::Version_info;

    using   std::uint32_t,                      // <cstdint>
            std::string, std::wstring,          // <string>
            std::in_place, std::optional,       // <optional>
            std::vector;

    auto opt_version_info_of( in_<wstring> exe_path )
        -> optional<Version_info>
    {
        try {
            return Version_info( exe_path );
        } catch( ... ) {
            return {};
        }
    }

    void run()
    {
        const auto& exe_path = LR"(C:\Windows\System32\notepad.exe)";
        // const auto& exe_path = LR"(C:\Users\Alf P. Steinbach\AppData\Local\Microsoft\WindowsApps\notepad.exe)";
        fmt::print( "{:s}\n", utf8_from( exe_path ) );
        if( const auto opt_info = opt_version_info_of( exe_path ) ) {
            const Version_info& info = opt_info.value();
            const Version ver = info.product_version();
            fmt::print( "Version {:04X}.{:04X}.{:04X}.{:04X}.\n",
                ver.major(), ver.minor(), ver.revision(), ver.build()
                );
            fmt::print( "Version {:d}.{:d}.{:d}.{:d}.\n",
                ver.major(), ver.minor(), ver.revision(), ver.build()
                );
        } else {
            fmt::print( "Bah, no version info.\n" );
        }
    }
}  // namespace app

auto main() -> int { app::run(); }
