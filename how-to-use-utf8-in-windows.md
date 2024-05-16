##  C++ *how* to &mdash; make non-English text work in Windows.

If special characters `ø`, `¿`, `á` or similars don’t display correctly then you’re probably using Windows.

Console input and output of non-English characters Just Works&trade; in \*nix environments because \*nix environments in practice use UTF-8 encoding for all text. Unfortunately that’s not so in Windows. In Windows you must *make it* work.

And it’s not just console i/o that’s problematic in Windows:

international characters in filesystem paths, or in environment variables, or in command line arguments, whatever, are problematic in Windows, by default resulting in gobbledygook garbage and/or failed operations.

*Most of these problems can be fixed.*

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](https://github.com/thlorenz/doctoc)*

- [1. *How* to display non-English characters in the console.](#1-how-to-display-non-english-characters-in-the-console)
- [2. *How* to format fixed width fields (regardless of Windows/\*nix/whatever platform).](#2-how-to-format-fixed-width-fields-regardless-of-windows%5Cnixwhatever-platform)
- [3. *How* to input non-English characters from the console.](#3-how-to-input-non-english-characters-from-the-console)
- [4. *How* to get the `main` arguments UTF-8 encoded.](#4-how-to-get-the-main-arguments-utf-8-encoded)
- [5. *How* to make `std::filesystem::path` guaranteed work.](#5-how-to-make-stdfilesystempath-guaranteed-work)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->


### 1. *How* to display non-English characters in the console.

To make ordinary output via e.g. `cout` work in Windows you need to take control of four text encodings (how text is represented as a sequence of byte values):

1.    The encoding your editor saves source code files with.
2.    The encoding the compiler assumes for a source code file.
3.    The encoding the compiler uses to store `char` based literals.
4.    The encoding the console assumes for a program’s byte stream output.

The simplest is to set all these to UTF-8. With the g++ compiler (2) and (3) are already UTF-8 by default, so with g++ you only need to fix (4), the console’s encoding assumption. Which you can do via a code snippet such as

```cpp
#ifdef _WIN32
    system( "chcp 65001 >nul" );        // Set the console to expect codepage 65001 = UTF-8.
#endif
```

Here `_WIN32` is a macro that’s automatically defined in 32-bit and 64-bit Windows. When this is done via Windows API functions instead of a command it’s easy to restore the original encoding assumption. Anyway you can put this system specific code in a separate source file, by doing it in the initializer for some dummy object.

But instead of changing the console’s expectation for byte stream output yourself, you can use e.g. [the {fmt} library](https://github.com/fmtlib/fmt)’s `fmt::print` for output, which if possible presents UTF-8 correctly. This functionality is also available as `std::print` in the C++23 standard library.

Other compilers than g++ won’t necessarily assume and use UTF-8 for respectively source code and literals. In particular Visual C++ by default assumes and uses Windows ANSI, which depends on the configuration of Windows. That means that the encoding of literals by default depends on in which country you compiled, which as of 2024 is silly and counter productive.

For Visual C++ you can use option [**`/utf-8`**](https://learn.microsoft.com/en-us/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8) to set both the source code encoding assumption and the encoding of literals.

As of 2024 this option is not yet supported by Visual Studio’s project property dialogs, but in those dialogs you can set it “manually” as (part of) the text for the “advanced” options.

I believe but do not know that the same needs to be done for clang++ in Windows, because in Windows it’s positioned itself roughly as a replacement for Visual C++.


### 2. *How* to format fixed width fields (regardless of Windows/\*nix/whatever platform).

There is no good way to make fixed width field ***formatting*** with `std::setw` work, because it has an assumption of 1 byte = 1 character, which doesn’t hold with UTF-8 (assumptions, again). The same goes for `printf` and family. And likely for DIY solutions.

But the aforementioned [{fmt} library](https://github.com/fmtlib/fmt) does proper fixed width field formatting with UTF-8.

In addition to understanding a variable number of bytes per character, the {fmt} library takes into account whether a character such as an emoji will occupy two character cells in the console. The widths you specify are number of character cells, *display widths*, not number of characters. It Just Works&trade;.

{fmt} supports left-, right- and center-alignment of a text in a field of a (possibly dynamic) specified number of cells. But unfortunately the library doesn’t directly expose its internal estimate of the text’s display width, which you need for e.g. displaying a box of dynamic width suitable for containing a given text. However you can obtain that number by formatting the text + e.g. `!` (any non-space single cell character) as left-adjusted in a field *n* cells wide, where *n* is at least 1 + the number of bytes in the text representation; count the number of trailing spaces in the result; and do the math:

```cpp
// Can be optimized in umpteen ways, but if that's a concern better use a 3rd party library.
auto display_width_of( in_<string_view> s )
    -> int
{
    const string    sx                  = string( s ) + '!';    // A non-space single cell character.
    const int       n_sx_bytes          = intsize_of( sx );
    const int       field_width         = n_sx_bytes;
    const string    field               = format( "{:{}s}", sx, field_width );
    const int       n_padding_spaces    = static_cast<int>( field.size() - field.rfind( '!' ) - 1 );
    const int       sx_width            = field_width - n_padding_spaces;

    return sx_width - 1;
}
```

Alternatively there are 3rd party libraries that do accurate display width estimates, more efficiently, for Unicode text.

### 3. *How* to input non-English characters from the console.

For ***input*** it doesn’t suffice to set the Windows console to assume UTF-8.

UTF-8 for console byte stream input is unsupported down at the Windows API level: non-ASCII input characters just produce nullbytes.

However, you can do UTF-8 console input via third party libraries such as [Boost NoWide](https://www.boost.org/doc/libs/1_85_0/libs/nowide/doc/html/index.html).

Or you can implement it yourself via Windows’ console API, e.g. [`ReadConsoleW`](https://learn.microsoft.com/en-us/windows/console/readconsole).

For, the problem is not Windows or Windows consoles, but merely low Quality of Implementation for this in all (I believe) Windows C++ standard library implementations, where byte stream oriented code that works fine in *nix systems has been blindly, and incorrectly, copied over to the Windows implementation. That’s sort of like the Ariane rocket failure. In both cases caused by management or possibly coders doing the easy reuse thing and disregarding critical assumptions.


### 4. *How* to get the `main` arguments UTF-8 encoded.

In Windows the `main` arguments `argv` are the original UTF-16 encoded command line arguments (which can be obtained via `GetCommandLine()` + `CommandLineToArgvW()`) translated to the process’ Windows ANSI encoding (the result of `GetACP()`). And by default `GetACP()` returns Windows’ global Windows ANSI encoding, in Western countries usually codepage 1252 = Windows ANSI Western, which is a single byte per character Latin-1 extension. So by  default this is a **lossy conversion** where e.g. filenames specified as command line arguments, can be irrevocably trashed.

The AFAIK only way make `GetACP` return the UTF-8 encoding, codepage 65001, so that the `main` arguments get UTF-8 encoded, is (this works since June 2019) to [equip the executable with a suitable **manifest** resource](https://learn.microsoft.com/en-us/windows/apps/design/globalizing/use-utf8-code-page#set-a-process-code-page-to-utf-8).

The manifest is a simple UTF-8 encoded text file which can go like this:

*app-manifest.xml*:
```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly manifestVersion="1.0" xmlns="urn:schemas-microsoft-com:asm.v1">
  <assemblyIdentity type="win32" name="¤" version="1.0.0.0"/>
  <application>
    <windowsSettings>
      <activeCodePage xmlns="http://schemas.microsoft.com/SMI/2019/WindowsSettings">UTF-8</activeCodePage>
    </windowsSettings>
  </application>
</assembly>
```

In an IDE the way to add this as a manifest resource, i.e. as a properly identified resource data blob in the executable, depends on the IDE. Visual Studio uses a special Microsoft tool [`mt`](https://learn.microsoft.com/en-us/windows/win32/sbscs/mt-exe) to combine manifest snippets and embed them as a manifest resource. I’m not familiar with the details of that except that it makes it a challenge to have the same source code for all toolchains. A practical solution is to simply use different files, for this aspect, with different toolchains. It can be a maintenance problem to keep these synchronized but needs must.

For a command line build, e.g. for the MinGW g++ compiler, and for that matter also for Visual C++, you can do it by using a Windows ANSI encoded **resource script** (filename extension `.rc`) to direct a **resource compiler** (GNU `windres` or Microsoft `rc`) to produce a binary version of the resource, where this binary version (GNU `.o`, Microsoft `.res`) then can just be passed to the C++ linker.

A minimal resource script for a manifest resource named “app-manifest.xml”:

*app.rc*:
```txt
#include <windows.h>    // RT_MANIFEST
// CREATEPROCESS_MANIFEST_RESOURCE_ID is defined as 1 cast to `char*`; not directly usable here.

1  RT_MANIFEST "app-manifest.xml"
```
That’s a lot of magic incantations &mdash; long reams of just semi-documented apparently arbitrary technobabble text &mdash; just to flip a binary decision in the direction that should be the default. At worst it should have been a simple yes/no compiler option. But, again, Microsoft.

Positive: this also fixes the encoding of environment variables as viewed by [`getenv()`](https://en.cppreference.com/w/cpp/utility/program/getenv), and with luck it also fixes naïve use of `std::filesystem::path`.

Negative: in C++ it does not necessarily fix the encoding assumption for `char` based strings passed to wide streams, and in the Windows API GDI graphics (e.g. `TextOutA()`) does not honor the process ANSI codepage.

A program that assumes UTF-8 as the process’ ANSI codepage should better assert that it really is so;

```cpp
#ifdef _WIN32
    assert( GetACP() == 65001 or !"Need UTF-8 as the process’ ANSI code page." );
#endif
```

&hellip; which requires a declaration of `GetACP`, e.g. via the `<windows.h>` header.

Also worth noting, instead of adding these files and toolchain dependent tool usage to every micro-project it’s possible to create a little program that inserts a suitable application manifest resource in an existing executable, and just run that at the end of every successful build. Windows has a number of API functions that do the basic update-the-executable for you, i.e. there’s no need to go down to the dark art level of patching binaries. For example, you can use Windows’ `UpdateResource`. When I once did this I chose to let that UTF-8 enabling program itself use UTF-16 encoded `wchar_t` based text. [The code](apps/set_utf8_as_ansi_codepage/source/app/main.cpp) can be worth looking at: it exemplifies general UTF-16 to UTF-8 conversion in Windows; how to retrieve UTF-16 encoded command line arguments (instead of using possibly trashed `main` arguments); and of course, it shows how to update or create an application manifest resource in an existing executable, though without complete fail-safety.

### 5. *How* to make `std::filesystem::path` guaranteed work.

By so far taking charge of &mdash; or alternatively working around &mdash; 5 text encodings,

1. the encoding your editor saves source code files with;
2. the encoding the compiler assumes for a source code file;
3. the encoding the compiler uses to store `char` based literals;
4. the encoding the console assumes for a program’s byte stream output; and
5. the process ANSI encoding that Windows assumes for calls of `char` based API functions from your process,

&hellip; UTF-8 encoded filesystem paths now Just Work&trade; with the classic C++03 file handling.

For example, the declaration `ifstream f( "æøå-poem.txt" );` now works.

But one cost, a price paid for that, is that without (5) the C++17 declaration `ifstream f( fs::path( "æøå-poem.txt" ) );`, where `fs` is an alias for `std::filesystem`, no longer can work&hellip; Because with just (1) through (4) `fs::path` incorrectly expects the `char`-based text to be encoded with the global Windows ANSI encoding. And depending on the `fs::path` implementation that may still happen with (5) in play.

A small [test program](apps/report_encodings/report_encodings.cpp) with all of the measures (1) through (5) in place, reported:

| Compiler: | `fs::path` assumes that a `char` string is encoded with: | Effectively: |
|:----------|:---------------------------------------------------------|:-------------|
| Visual C++ version 19.35.32215 for x64 | the process ANSI codepage, from `GetACP` | UTF-8 |
| MinGW g++ version 11.2.0 for x64 | the system ANSI codepage e.g. from `GetLocaleInfo` | E.g. cp 1252 |

Which means that with MinGW g++ 11.2.0 `fs::path` garbles a `char`-based path specification with non-ASCII characters.

***TANSTAAFL***: *There Ain’t No Such Thing As A Free Lunch*.

