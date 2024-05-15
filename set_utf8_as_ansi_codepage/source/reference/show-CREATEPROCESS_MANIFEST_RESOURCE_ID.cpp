#include <windows.h>
#include <stdio.h>

#define X( m ) X_( m )
#define X_( m ) #m

auto main() -> int
{
    printf( "CREATEPROCESS_MANIFEST_RESOURCE_ID = %s\n", X(CREATEPROCESS_MANIFEST_RESOURCE_ID) );
}
