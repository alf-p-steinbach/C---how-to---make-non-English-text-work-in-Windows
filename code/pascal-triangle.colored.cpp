// C++20 code. However, use of `iota_view` can be replaced with ordinary `for` loops for C++17 and earlier.
#include <fmt/color.h>
#include <fmt/core.h>

#include <cassert>              // assert

#include <ranges>
#include <string>
#include <utility>
#include <vector>
using   std::ranges::iota_view,         // <ranges>
        std::string, std::stoi,         // <string>
        std::exchange,                  // <utility>
        std::vector;                    // <vector>

template< class Type > using in_ = const Type&;
auto up_to( const int n ) -> auto { return iota_view( 0, n ); }
auto one_through( const int n ) -> auto { return iota_view( 1, n + 1 ); }
auto vec_with_capacity( const int n ) -> vector<int> { vector<int> v; v.reserve( n ); return v; }
constexpr auto is_digit( const char ch ) -> bool { return ('0' <= ch and ch <= '9'); }
constexpr auto is_odd( const int v ) -> bool { return (v % 2 == 1); }

// Cannot be `constexpr` as long as `up_to` is convenience-defined in terms of ranges lib stuff:
auto binomial( const int n, const int r )
    -> int
{
    int result = 1;
    for( const int k: up_to( r ) ) { result = result*(n - k)/(k + 1); }
    return result;
}

// Evidently due to a bug in the {fmt} library escape sequences interfere with field formatting,
// so here color escape sequences are added after a full output line has been formatted. It is
// a bit of stringly typed code, interpreting parts of the line as numbers, but so be it.
auto colored_pascal_triangle_line( in_<string> line )
    -> string
{
    string result;
    string part;
    char prev_ch = 0;
    assert( line.back() == ' ' );       // An extra space added to make below logic work.
    for( const char ch: line ) {
        if( (ch == ' ') != (prev_ch == ' ') ) {
            if( not part.empty() ) {
                if( is_digit( part.front() ) ) {
                    if( is_odd( stoi( part ) ) ) {
                        part = fmt::format( fg( fmt::color::yellow ), part ); 
                    } else {
                        part = fmt::format( fg( fmt::color::blue ), part ); 
                    }
                }
                result += exchange( part, "" );
            }
        }
        part += ch;
        prev_ch = ch;
    }
    if( not part.empty() ) { result += part; }
    return result;
}

auto main() -> int
{
    const int n             = 16;
    const int max_digits    = int( fmt::format( "{}", binomial( n - 1, n/2 ) ).length() );
    const int field_size    = max_digits + 1;

    // fmt::print( fg( fmt::color::yellow ), "C++20" );
    auto current    = vec_with_capacity( n + 1 );
    auto previous   = vec_with_capacity( n + 1 );
    for( const int y: up_to( 16 ) ) {
        current.clear();            // Retains the capacity.
        current.push_back( 1 );
        if( y > 0 ) {
            for( const int x: one_through( y - 1 ) ) {
                current.push_back( previous[x - 1] + previous[x] );
            }
            current.push_back( 1 );
        }

        string line;
        for( const int v: current ) {
            line += fmt::format( "{:^{}}", v, field_size );
        };
        line = fmt::format( "{:^{}} ", line, n*field_size );    // Centering, adds spaces at the start.
        fmt::print( "{}\n", colored_pascal_triangle_line( line ) );

        swap( current, previous );
    }
}
