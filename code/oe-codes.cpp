#include <iostream>
using   std::cout;

auto main() -> int
{
    using Byte = unsigned char;
    const auto& oe = "ø";
    for( const Byte v: oe ) { cout << +v << ' '; }
    cout << '\n';
}
