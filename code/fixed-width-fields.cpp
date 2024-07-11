#include <fmt/core.h>       // <url: https://fmt.dev/>
#include <string_view>

using namespace std::string_view_literals;
using std::string_view;
using Byte = unsigned char;

constexpr auto is_u8_tailbyte( const Byte v ) -> bool { return (128 <= v and v < 192); }

auto u8_seq_length( const Byte first_byte )
    -> int
{
    return (0?0
        : first_byte < 128?     1   // < 0b1000'0000  I.e. just an ASCII character.
        : first_byte < 192?     -1  // < 0b1100'0000  Invalid as first byte, it's a tailbyte.
        : first_byte < 224?     2   // < 0b1110'0000
        : first_byte < 240?     3   // < 0b1111'0000
        : first_byte < 248?     4   // < 0b1111'1000
        :                       -2  // Invalid as first byte because max UTF-8 seq length is 4.
        );
}

auto main() -> int
{
    const auto s        =  "日本国 кошка’s FTW!"sv;
    const auto u32_s    = U"日本国 кошка’s FTW!"sv;

    // Display the UTF-8 encoded characters in a sequence of fixed width fields.
    for( const auto& code: s ) {
        if( not is_u8_tailbyte( code ) ) {
            const auto u8_char = string_view( &code, u8_seq_length( code ) );
            fmt::print( "{:>5s}", u8_char );
        }
    }
    fmt::print( "\n" );
    
    // Display the Unicode codes vertically aligned with the characters.
    for( const unsigned code: u32_s ) { fmt::print( "{:>5X}", code ); }
    fmt::print( "\n" );
}
