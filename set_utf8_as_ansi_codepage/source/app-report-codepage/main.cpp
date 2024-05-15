#include <windows.h>
#include <stdio.h>

auto main() -> int
{
    printf( "Codepage %d.\n", GetACP() );
}
