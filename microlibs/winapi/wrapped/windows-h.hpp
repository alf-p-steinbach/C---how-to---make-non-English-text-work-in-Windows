﻿#pragma once
// Ideal usage:
// define `WINAPI_VERSION` in the build settings, or anyway before including this file.

#ifdef MessageBox
#   error "<windows.h> has already been included, possibly with undesired options."
#   include <stop-compilation>
#endif

#include <assert.h>
#ifdef _MSC_VER                         // TODO: Check version.
#   include <iso646.h>                  // Standard `and` etc. also with MSVC.
#endif


//----------------------------------------------- Convenience macros.
//
// The macros are usable in macro-land; constants would not be.

#define IS_NARROW_WINAPI()      (sizeof(*GetCommandLine()) == 1)
#define IS_WIDE_WINAPI()        (sizeof(*GetCommandLine()) > 1)


//----------------------------------------------- Select UTF-8 xxxA or UTF-16 xxxW API.

// UTF8_WINAPI is a custom macro for this file. UNICODE, _UNICODE and _MBCS are MS macros.
#if defined( UTF8_WINAPI ) and defined( UNICODE )
#   error "Inconsistent, both UNICODE (UTF-16) and UTF8_WINAPI (UTF-8) are defined."
#   include <stop-compilation>
#endif

#if defined( _MBCS ) and defined( UNICODE )
#   error "Inconsistent, both UNICODE (UTF-16) and _MBCS (Windows multibyte) are defined."
#   include <stop-compilation>
#endif

#if not( defined( UTF8_WINAPI ) or defined( UNICODE ) )
#   error "Define either UTF8_WINAPI or UNICODE, for respectively UTF-8 and UTF-16."
#   include <stop-compilation>
#endif

#undef UNICODE
#undef _UNICODE
#ifdef UTF8_WINAPI
#   undef   _MBCS
#   define  _MBCS       // Mainly for 3rd party code that uses it for platform detection.
#else
#   define UNICODE
#   define _UNICODE     // Mainly for 3rd party code that uses it for platform detection.
#endif


//----------------------------------------------- Make it less incompatible with C++.

#undef  STRICT
#define STRICT          // C++-compatible “strongly typed” declarations, please.
#undef NOMINMAX
#define NOMINMAX        // No C++-sabotaging “min” and “max” macros, please.


//----------------------------------------------- Reduce the size.
//
// Reduce the size of the <windows.h>, to the degree practically possible.
// Also, with WIN32_LEAN_AND_MEAN an `#include <winsock2.h>` will actually include that header.

#undef  WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#undef  NOCOMM
#define NOCOMM          // No serial comms API please.
#define NOMCX
#define NOMCX           // No modem configuration API please.
#undef  NOOPENFILE
#define NOOPENFILE      // No OpenFile function please; it's limited and long deprecated.


//----------------------------------------------- Select which API versions to support.
//
// With much stuff introduced in Windows Vista and later one needs to set version indicators.
// NTDDI_VERSION (newest macro), _WIN32_WINNT and WINVER (oldest macro) are used by <windows.h>.
// WINAPI_VERSION and WINAPI_SERVICE_PACK are macros used by and optionally defined by this header.
//
// Experience shows that one needs to take full control of this, especially with g++, so:
//
#if defined( WINAPI_VERSION )
#   undef   NTDDI_VERSION
#   undef   _WIN32_WINNT
#   undef   WINVER
#else
#   undef WINAPI_SERVICE_PACK
#   if defined( NTDDI_VERSION )
#       undef   _WIN32_WINNT
#       undef   WINVER
#       define  WINAPI_VERSION          (NTDDI_VERSION >> 16)
#       define  WINAPI_SERVICE_PACK     (NTTDI_VERSION & 0xFFFF)
#   elif defined( _WIN32_WINNT )
#       undef   WINVER
#       define  WINAPI_VERSION          _WIN32_WINNT
#   elif defined( WINVER )
#       define  WINAPI_VERSION          WINVER
#   else
#       define  WINAPI_VERSION          0x0600      // Default, Windows Vista.
#   endif
#endif

#ifndef WINAPI_SERVICE_PACK
#   define  WINAPI_SERVICE_PACK         0
#endif

#ifndef NTDDI_VERSION
#   define NTDDI_VERSION    ((WINAPI_VERSION << 16) | WINAPI_SERVICE_PACK)
#endif

#ifndef _WIN32_WINNT
#   define _WIN32_WINNT     WINAPI_VERSION
#endif

#ifndef WINVER
#   define WINVER           WINAPI_VERSION
#endif


//----------------------------------------------- The actual include.

#include <windows.h>
