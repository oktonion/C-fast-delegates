// doctest.h - the lightest feature-rich C++ single-header testing framework for unit tests and TDD
//
// Copyright (c) 2016-2018 Viktor Kirilov
// NotSoCopyright (nsc) 2019-... Gubanov Aleksandr
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
// The documentation can be found at the library's page:
// https://github.com/onqtam/doctest/blob/master/doc/markdown/readme.md
//
// =================================================================================================
// =================================================================================================
// =================================================================================================
//
// The library is heavily influenced by Catch - https://github.com/philsquared/Catch
// which uses the Boost Software License - Version 1.0
// see here - https://github.com/philsquared/Catch/blob/master/LICENSE.txt
//
// The concept of subcases (sections in Catch) and expression decomposition are from there.
// Some parts of the code are taken directly:
// - stringification - the detection of "ostream& operator<<(ostream&, const T&)" and StringMaker<>
// - the Approx() helper class for floating point comparison
// - colors in the console
// - breaking into a debugger
// - signal / SEH handling
// - timer
//
// The expression decomposing templates are taken from lest - https://github.com/martinmoene/lest
// which uses the Boost Software License - Version 1.0
// see here - https://github.com/martinmoene/lest/blob/master/LICENSE.txt
//
// The type list and the foreach algorithm on it for C++98 are taken from Loki
// - http://loki-lib.sourceforge.net/
// - https://en.wikipedia.org/wiki/Loki_%28C%2B%2B%29
// - https://github.com/snaewe/loki-lib
// which uses the MIT Software License
//
// =================================================================================================
// =================================================================================================
// =================================================================================================

#ifndef DOCTEST_LIBRARY_INCLUDED
#define DOCTEST_LIBRARY_INCLUDED

// =================================================================================================
// == VERSION ======================================================================================
// =================================================================================================

#define DOCTEST_VERSION_MAJOR 1
#define DOCTEST_VERSION_MINOR 3
#define DOCTEST_VERSION_PATCH 0
#define DOCTEST_VERSION_STR "1.3.0"

#define DOCTEST_VERSION                                                                            \
    (DOCTEST_VERSION_MAJOR * 10000 + DOCTEST_VERSION_MINOR * 100 + DOCTEST_VERSION_PATCH)

// =================================================================================================
// == COMPILER VERSION =============================================================================
// =================================================================================================

// ideas for the version stuff are taken from here: https://github.com/cxxstuff/cxx_detect

#define DOCTEST_COMPILER(MAJOR, MINOR, PATCH) ((MAJOR)*10000000 + (MINOR)*100000 + (PATCH))

#if defined(_MSC_VER) && defined(_MSC_FULL_VER)
#if _MSC_VER == _MSC_FULL_VER / 10000
#define DOCTEST_MSVC DOCTEST_COMPILER(_MSC_VER / 100, _MSC_VER % 100, _MSC_FULL_VER % 10000)
#else
#define DOCTEST_MSVC                                                                               \
    DOCTEST_COMPILER(_MSC_VER / 100, (_MSC_FULL_VER / 100000) % 100, _MSC_FULL_VER % 100000)
#endif
#elif defined(__clang__) && defined(__clang_minor__)
#define DOCTEST_CLANG DOCTEST_COMPILER(__clang_major__, __clang_minor__, __clang_patchlevel__)
#elif defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__) &&              \
        !defined(__INTEL_COMPILER)
#define DOCTEST_GCC DOCTEST_COMPILER(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#endif

#ifndef DOCTEST_MSVC
#define DOCTEST_MSVC 0
#endif // DOCTEST_MSVC
#ifndef DOCTEST_CLANG
#define DOCTEST_CLANG 0
#endif // DOCTEST_CLANG
#ifndef DOCTEST_GCC
#define DOCTEST_GCC 0
#endif // DOCTEST_GCC

// =================================================================================================
// == COMPILER WARNINGS HELPERS ====================================================================
// =================================================================================================

#if DOCTEST_CLANG
#ifdef __has_warning
#define DOCTEST_CLANG_HAS_WARNING(x) __has_warning(x)
#endif // __has_warning
#ifdef __has_feature
#define DOCTEST_CLANG_HAS_FEATURE(x) __has_feature(x)
#endif // __has_feature
#define DOCTEST_PRAGMA_TO_STR(x) _Pragma(#x)
#define DOCTEST_CLANG_SUPPRESS_WARNING_PUSH _Pragma("clang diagnostic push")
#define DOCTEST_MSVC_SUPPRESS_WARNING_PUSH
#define DOCTEST_GCC_SUPPRESS_WARNING_PUSH
#define DOCTEST_CLANG_SUPPRESS_WARNING(w) DOCTEST_PRAGMA_TO_STR(clang diagnostic ignored w)
#define DOCTEST_MSVC_SUPPRESS_WARNING(w)
#define DOCTEST_GCC_SUPPRESS_WARNING(w)
#define DOCTEST_CLANG_SUPPRESS_WARNING_POP _Pragma("clang diagnostic pop")
#define DOCTEST_MSVC_SUPPRESS_WARNING_POP
#define DOCTEST_GCC_SUPPRESS_WARNING_POP
#define DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)                                                \
    DOCTEST_CLANG_SUPPRESS_WARNING_PUSH DOCTEST_CLANG_SUPPRESS_WARNING(w)
#define DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)
#define DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH(w)
#elif DOCTEST_GCC
#define DOCTEST_PRAGMA_TO_STR(x) _Pragma(#x)
#define DOCTEST_CLANG_SUPPRESS_WARNING_PUSH
#define DOCTEST_MSVC_SUPPRESS_WARNING_PUSH
#if DOCTEST_GCC >= DOCTEST_COMPILER(4, 7, 0)
#define DOCTEST_GCC_SUPPRESS_WARNING_PUSH _Pragma("GCC diagnostic push")
#else // GCC 4.7+
#define DOCTEST_GCC_SUPPRESS_WARNING_PUSH
#endif // GCC 4.7+
#define DOCTEST_CLANG_SUPPRESS_WARNING(w)
#define DOCTEST_MSVC_SUPPRESS_WARNING(w)
#define DOCTEST_GCC_SUPPRESS_WARNING(w) DOCTEST_PRAGMA_TO_STR(GCC diagnostic ignored w)
#define DOCTEST_CLANG_SUPPRESS_WARNING_POP
#define DOCTEST_MSVC_SUPPRESS_WARNING_POP
#if DOCTEST_GCC >= DOCTEST_COMPILER(4, 7, 0)
#define DOCTEST_GCC_SUPPRESS_WARNING_POP _Pragma("GCC diagnostic pop")
#else // GCC 4.7+
#define DOCTEST_GCC_SUPPRESS_WARNING_POP
#endif // GCC 4.7+
#define DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)
#define DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)
#define DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH(w)                                                  \
    DOCTEST_GCC_SUPPRESS_WARNING_PUSH DOCTEST_GCC_SUPPRESS_WARNING(w)
#elif DOCTEST_MSVC
#define DOCTEST_PRAGMA_TO_STR(x)
#define DOCTEST_CLANG_SUPPRESS_WARNING_PUSH
#define DOCTEST_MSVC_SUPPRESS_WARNING_PUSH __pragma(warning(push))
#define DOCTEST_GCC_SUPPRESS_WARNING_PUSH
#define DOCTEST_CLANG_SUPPRESS_WARNING(w)
#define DOCTEST_MSVC_SUPPRESS_WARNING(w) __pragma(warning(disable : w))
#define DOCTEST_GCC_SUPPRESS_WARNING(w)
#define DOCTEST_CLANG_SUPPRESS_WARNING_POP
#define DOCTEST_MSVC_SUPPRESS_WARNING_POP __pragma(warning(pop))
#define DOCTEST_GCC_SUPPRESS_WARNING_POP
#define DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)
#define DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)                                                 \
    DOCTEST_MSVC_SUPPRESS_WARNING_PUSH DOCTEST_MSVC_SUPPRESS_WARNING(w)
#define DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH(w)
#else
#define DOCTEST_PRAGMA_TO_STR(x)
#define DOCTEST_CLANG_SUPPRESS_WARNING_PUSH
#define DOCTEST_MSVC_SUPPRESS_WARNING_PUSH 
#define DOCTEST_GCC_SUPPRESS_WARNING_PUSH
#define DOCTEST_CLANG_SUPPRESS_WARNING(w)
#define DOCTEST_MSVC_SUPPRESS_WARNING(w) 
#define DOCTEST_GCC_SUPPRESS_WARNING(w)
#define DOCTEST_CLANG_SUPPRESS_WARNING_POP
#define DOCTEST_MSVC_SUPPRESS_WARNING_POP 
#define DOCTEST_GCC_SUPPRESS_WARNING_POP
#define DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)
#define DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)
#define DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // different compilers - warning suppression macros


#ifndef DOCTEST_CLANG_HAS_WARNING
#define DOCTEST_CLANG_HAS_WARNING(x) 1
#endif // DOCTEST_CLANG_HAS_WARNING

#ifndef DOCTEST_CLANG_HAS_FEATURE
#define DOCTEST_CLANG_HAS_FEATURE(x) 0
#endif // DOCTEST_CLANG_HAS_FEATURE

// =================================================================================================
// == COMPILER WARNINGS ============================================================================
// =================================================================================================

DOCTEST_CLANG_SUPPRESS_WARNING_PUSH
DOCTEST_CLANG_SUPPRESS_WARNING("-Wunknown-pragmas")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wnon-virtual-dtor")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wweak-vtables")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wpadded")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wdeprecated")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wmissing-prototypes")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wunused-local-typedef")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wc++11-long-long")
#if DOCTEST_CLANG && DOCTEST_CLANG_HAS_WARNING("-Wzero-as-null-pointer-constant")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wzero-as-null-pointer-constant")
#endif // clang - 0 as null

DOCTEST_GCC_SUPPRESS_WARNING_PUSH
DOCTEST_GCC_SUPPRESS_WARNING("-Wunknown-pragmas")
DOCTEST_GCC_SUPPRESS_WARNING("-Wpragmas")
DOCTEST_GCC_SUPPRESS_WARNING("-Weffc++")
DOCTEST_GCC_SUPPRESS_WARNING("-Wstrict-overflow")
DOCTEST_GCC_SUPPRESS_WARNING("-Wstrict-aliasing")
DOCTEST_GCC_SUPPRESS_WARNING("-Wctor-dtor-privacy")
DOCTEST_GCC_SUPPRESS_WARNING("-Wmissing-declarations")
DOCTEST_GCC_SUPPRESS_WARNING("-Wnon-virtual-dtor")
DOCTEST_GCC_SUPPRESS_WARNING("-Winline")
DOCTEST_GCC_SUPPRESS_WARNING("-Wlong-long")
DOCTEST_GCC_SUPPRESS_WARNING("-Wzero-as-null-pointer-constant")
DOCTEST_GCC_SUPPRESS_WARNING("-Wunused-local-typedefs")
DOCTEST_GCC_SUPPRESS_WARNING("-Wuseless-cast")

DOCTEST_MSVC_SUPPRESS_WARNING_PUSH
DOCTEST_MSVC_SUPPRESS_WARNING(4616) // invalid compiler warning
DOCTEST_MSVC_SUPPRESS_WARNING(4619) // invalid compiler warning
DOCTEST_MSVC_SUPPRESS_WARNING(4996) // The compiler encountered a deprecated declaration
DOCTEST_MSVC_SUPPRESS_WARNING(4706) // assignment within conditional expression
DOCTEST_MSVC_SUPPRESS_WARNING(4512) // 'class' : assignment operator could not be generated
DOCTEST_MSVC_SUPPRESS_WARNING(4127) // conditional expression is constant
DOCTEST_MSVC_SUPPRESS_WARNING(4820) // padding
DOCTEST_MSVC_SUPPRESS_WARNING(4625) // copy constructor was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(4626) // assignment operator was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(5027) // move assignment operator was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(5026) // move constructor was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(4623) // default constructor was implicitly defined as deleted
DOCTEST_MSVC_SUPPRESS_WARNING(4640) // construction of local static object is not thread-safe

// C4548 - expression before comma has no effect; expected expression with side - effect
// C4986 - exception specification does not match previous declaration
// C4350 - behavior change: 'member1' called instead of 'member2'
// C4668 - 'x' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
// C4365 - conversion from 'int' to 'unsigned long', signed/unsigned mismatch
// C4774 - format string expected in argument 'x' is not a string literal
// C4820 - padding in structs

// only 4 should be disabled globally:
// - C4514 # unreferenced inline function has been removed
// - C4571 # SEH related
// - C4710 # function not inlined
// - C4711 # function 'x' selected for automatic inline expansion

#define DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN                                 \
    DOCTEST_MSVC_SUPPRESS_WARNING_PUSH                                                             \
    DOCTEST_MSVC_SUPPRESS_WARNING(4548)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4986)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4350)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4668)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4365)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4774)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4820)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4625)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4626)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(5027)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(5026)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(4623)                                                            \
    DOCTEST_MSVC_SUPPRESS_WARNING(5039)

#define DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END DOCTEST_MSVC_SUPPRESS_WARNING_POP

// =================================================================================================
// == FEATURE DETECTION ============================================================================
// =================================================================================================

#if __cplusplus >= 201103L
#ifndef DOCTEST_CONFIG_WITH_DELETED_FUNCTIONS
#define DOCTEST_CONFIG_WITH_DELETED_FUNCTIONS
#endif // DOCTEST_CONFIG_WITH_DELETED_FUNCTIONS
#ifndef DOCTEST_CONFIG_WITH_RVALUE_REFERENCES
#define DOCTEST_CONFIG_WITH_RVALUE_REFERENCES
#endif // DOCTEST_CONFIG_WITH_RVALUE_REFERENCES
#ifndef DOCTEST_CONFIG_WITH_NULLPTR
#define DOCTEST_CONFIG_WITH_NULLPTR
#endif // DOCTEST_CONFIG_WITH_NULLPTR
#ifndef DOCTEST_CONFIG_WITH_LONG_LONG
#define DOCTEST_CONFIG_WITH_LONG_LONG
#endif // DOCTEST_CONFIG_WITH_LONG_LONG
#ifndef DOCTEST_CONFIG_WITH_STATIC_ASSERT
#define DOCTEST_CONFIG_WITH_STATIC_ASSERT
#endif // DOCTEST_CONFIG_WITH_STATIC_ASSERT
#ifndef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#endif // __cplusplus >= 201103L

// MSVC C++11 feature support table: https://msdn.microsoft.com/en-us/library/hh567368.aspx
// GCC C++11 feature support table: https://gcc.gnu.org/projects/cxx-status.html
// MSVC version table:
// MSVC++ 15.0 _MSC_VER == 1910 (Visual Studio 2017)
// MSVC++ 14.0 _MSC_VER == 1900 (Visual Studio 2015)
// MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
// MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
// MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
// MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
// MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)

// deleted functions

#ifndef DOCTEST_CONFIG_WITH_DELETED_FUNCTIONS
#if DOCTEST_MSVC >= DOCTEST_COMPILER(18, 0, 0)
#define DOCTEST_CONFIG_WITH_DELETED_FUNCTIONS
#endif // MSVC
#if DOCTEST_CLANG && DOCTEST_CLANG_HAS_FEATURE(cxx_deleted_functions)
#define DOCTEST_CONFIG_WITH_DELETED_FUNCTIONS
#endif // clang
#if DOCTEST_GCC >= DOCTEST_COMPILER(4, 5, 0) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define DOCTEST_CONFIG_WITH_DELETED_FUNCTIONS
#endif // GCC
#endif // DOCTEST_CONFIG_WITH_DELETED_FUNCTIONS

#if defined(DOCTEST_CONFIG_NO_DELETED_FUNCTIONS) && defined(DOCTEST_CONFIG_WITH_DELETED_FUNCTIONS)
#undef DOCTEST_CONFIG_WITH_DELETED_FUNCTIONS
#endif // DOCTEST_CONFIG_NO_DELETED_FUNCTIONS

// rvalue references

#ifndef DOCTEST_CONFIG_WITH_RVALUE_REFERENCES
#if DOCTEST_MSVC >= DOCTEST_COMPILER(16, 0, 0)
#define DOCTEST_CONFIG_WITH_RVALUE_REFERENCES
#endif // MSVC
#if DOCTEST_CLANG && DOCTEST_CLANG_HAS_FEATURE(cxx_rvalue_references)
#define DOCTEST_CONFIG_WITH_RVALUE_REFERENCES
#endif // clang
#if DOCTEST_GCC >= DOCTEST_COMPILER(4, 3, 0) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define DOCTEST_CONFIG_WITH_RVALUE_REFERENCES
#endif // GCC
#endif // DOCTEST_CONFIG_WITH_RVALUE_REFERENCES

#if defined(DOCTEST_CONFIG_NO_RVALUE_REFERENCES) && defined(DOCTEST_CONFIG_WITH_RVALUE_REFERENCES)
#undef DOCTEST_CONFIG_WITH_RVALUE_REFERENCES
#endif // DOCTEST_CONFIG_NO_RVALUE_REFERENCES

// nullptr

#ifndef DOCTEST_CONFIG_WITH_NULLPTR
#if DOCTEST_CLANG && DOCTEST_CLANG_HAS_FEATURE(cxx_nullptr)
#define DOCTEST_CONFIG_WITH_NULLPTR
#endif // clang
#if DOCTEST_GCC >= DOCTEST_COMPILER(4, 6, 0) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define DOCTEST_CONFIG_WITH_NULLPTR
#endif // GCC
#if DOCTEST_MSVC >= DOCTEST_COMPILER(16, 0, 0)
#define DOCTEST_CONFIG_WITH_NULLPTR
#endif // MSVC
#endif // DOCTEST_CONFIG_WITH_NULLPTR

#if defined(DOCTEST_CONFIG_NO_NULLPTR) && defined(DOCTEST_CONFIG_WITH_NULLPTR)
#undef DOCTEST_CONFIG_WITH_NULLPTR
#endif // DOCTEST_CONFIG_NO_NULLPTR

// variadic macros

#ifndef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#if DOCTEST_MSVC >= DOCTEST_COMPILER(14, 0, 0) && !defined(__EDGE__)
#define DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#endif // MSVC
#if(DOCTEST_CLANG || DOCTEST_GCC >= DOCTEST_COMPILER(4, 1, 0)) &&                                  \
        defined(__GXX_EXPERIMENTAL_CXX0X__)
#define DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#endif // GCC and clang
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

#if defined(DOCTEST_CONFIG_NO_VARIADIC_MACROS) && defined(DOCTEST_CONFIG_WITH_VARIADIC_MACROS)
#undef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#endif // DOCTEST_CONFIG_NO_VARIADIC_MACROS

// long long

#ifndef DOCTEST_CONFIG_WITH_LONG_LONG
#if DOCTEST_MSVC >= DOCTEST_COMPILER(14, 0, 0)
#define DOCTEST_CONFIG_WITH_LONG_LONG
#endif // MSVC
#if(DOCTEST_CLANG || DOCTEST_GCC >= DOCTEST_COMPILER(4, 5, 0)) &&                                  \
        defined(__GXX_EXPERIMENTAL_CXX0X__)
#define DOCTEST_CONFIG_WITH_LONG_LONG
#endif // GCC and clang
#endif // DOCTEST_CONFIG_WITH_LONG_LONG

#if defined(DOCTEST_CONFIG_NO_LONG_LONG) && defined(DOCTEST_CONFIG_WITH_LONG_LONG)
#undef DOCTEST_CONFIG_WITH_LONG_LONG
#endif // DOCTEST_CONFIG_NO_LONG_LONG

// static_assert

#ifndef DOCTEST_CONFIG_WITH_STATIC_ASSERT
#if DOCTEST_CLANG && DOCTEST_CLANG_HAS_FEATURE(cxx_static_assert)
#define DOCTEST_CONFIG_WITH_STATIC_ASSERT
#endif // clang
#if DOCTEST_GCC >= DOCTEST_COMPILER(4, 3, 0) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define DOCTEST_CONFIG_WITH_STATIC_ASSERT
#endif // GCC
#if DOCTEST_MSVC >= DOCTEST_COMPILER(16, 0, 0)
#define DOCTEST_CONFIG_WITH_STATIC_ASSERT
#endif // MSVC
#endif // DOCTEST_CONFIG_WITH_STATIC_ASSERT

#if defined(DOCTEST_CONFIG_NO_STATIC_ASSERT) && defined(DOCTEST_CONFIG_WITH_STATIC_ASSERT)
#undef DOCTEST_CONFIG_WITH_STATIC_ASSERT
#endif // DOCTEST_CONFIG_NO_STATIC_ASSERT

// other stuff...

#if defined(DOCTEST_CONFIG_WITH_RVALUE_REFERENCES) || defined(DOCTEST_CONFIG_WITH_LONG_LONG) ||    \
        defined(DOCTEST_CONFIG_WITH_DELETED_FUNCTIONS) || defined(DOCTEST_CONFIG_WITH_NULLPTR) ||  \
        defined(DOCTEST_CONFIG_WITH_VARIADIC_MACROS) || defined(DOCTEST_CONFIG_WITH_STATIC_ASSERT)
#define DOCTEST_NO_CPP11_COMPAT
#endif // c++11 stuff

#if defined(DOCTEST_NO_CPP11_COMPAT)
DOCTEST_CLANG_SUPPRESS_WARNING("-Wc++98-compat")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wc++98-compat-pedantic")
#endif // DOCTEST_NO_CPP11_COMPAT

#if DOCTEST_MSVC && !defined(DOCTEST_CONFIG_WINDOWS_SEH)
#define DOCTEST_CONFIG_WINDOWS_SEH
#endif // MSVC
#if defined(DOCTEST_CONFIG_NO_WINDOWS_SEH) && defined(DOCTEST_CONFIG_WINDOWS_SEH)
#undef DOCTEST_CONFIG_WINDOWS_SEH
#endif // DOCTEST_CONFIG_NO_WINDOWS_SEH

#if !defined(_WIN32) && !defined(__QNX__) && !defined(DOCTEST_CONFIG_POSIX_SIGNALS)
#define DOCTEST_CONFIG_POSIX_SIGNALS
#endif // _WIN32
#if defined(DOCTEST_CONFIG_NO_POSIX_SIGNALS) && defined(DOCTEST_CONFIG_POSIX_SIGNALS)
#undef DOCTEST_CONFIG_POSIX_SIGNALS
#endif // DOCTEST_CONFIG_NO_POSIX_SIGNALS

#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
#if(DOCTEST_GCC || DOCTEST_CLANG) && !defined(__EXCEPTIONS)
#define DOCTEST_CONFIG_NO_EXCEPTIONS
#endif // clang and gcc
// in MSVC _HAS_EXCEPTIONS is defined in a header instead of as a project define
// so we can't do the automatic detection for MSVC without including some header
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS

#ifdef DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
#define DOCTEST_CONFIG_NO_EXCEPTIONS
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#if defined(DOCTEST_CONFIG_NO_EXCEPTIONS) && !defined(DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS)
#define DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS && !DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS

#if defined(DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN) && !defined(DOCTEST_CONFIG_IMPLEMENT)
#define DOCTEST_CONFIG_IMPLEMENT
#endif // DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#if defined _WIN32 || defined __CYGWIN__
#if DOCTEST_MSVC
#define DOCTEST_SYMBOL_EXPORT __declspec(dllexport)
#define DOCTEST_SYMBOL_IMPORT __declspec(dllimport)
#else // MSVC
#define DOCTEST_SYMBOL_EXPORT __attribute__((dllexport))
#define DOCTEST_SYMBOL_IMPORT __attribute__((dllimport))
#endif // MSVC
#else  // _WIN32
#define DOCTEST_SYMBOL_EXPORT __attribute__((visibility("default")))
#define DOCTEST_SYMBOL_IMPORT
#endif // _WIN32

#ifdef DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#ifdef DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_INTERFACE DOCTEST_SYMBOL_EXPORT
#else // DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_INTERFACE DOCTEST_SYMBOL_IMPORT
#endif // DOCTEST_CONFIG_IMPLEMENT
#else  // DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#define DOCTEST_INTERFACE
#endif // DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL

#if DOCTEST_MSVC
#define DOCTEST_NOINLINE __declspec(noinline)
#define DOCTEST_UNUSED
#define DOCTEST_ALIGNMENT(x)
#elif ( DOCTEST_CLANG || DOCTEST_GCC )
#define DOCTEST_NOINLINE __attribute__((noinline))
#define DOCTEST_UNUSED __attribute__((unused))
#define DOCTEST_ALIGNMENT(x) __attribute__((aligned(x)))
#elif defined(__BORLANDC__)
#define DOCTEST_NOINLINE 
#define DOCTEST_UNUSED 
#define DOCTEST_ALIGNMENT(x)
#define DOCTEST_CONFIG_USE_IOSFWD
#else
#error ""
#endif // MSVC

#ifndef DOCTEST_CONFIG_NUM_CAPTURES_ON_STACK
#define DOCTEST_CONFIG_NUM_CAPTURES_ON_STACK 5
#endif // DOCTEST_CONFIG_NUM_CAPTURES_ON_STACK

// =================================================================================================
// == FEATURE DETECTION END ========================================================================
// =================================================================================================

// internal macros for string concatenation and anonymous variable name generation
#define DOCTEST_CAT_IMPL(s1, s2) s1##s2
#define DOCTEST_CAT(s1, s2) DOCTEST_CAT_IMPL(s1, s2)
#ifdef __COUNTER__ // not standard and may be missing for some compilers
#define DOCTEST_ANONYMOUS(x) DOCTEST_CAT(x, __COUNTER__)
#else // __COUNTER__
#define DOCTEST_ANONYMOUS(x) DOCTEST_CAT(x, __LINE__)
#endif // __COUNTER__

// macro for making a string out of an identifier
#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_TOSTR_IMPL(...) #__VA_ARGS__
#define DOCTEST_TOSTR(...) DOCTEST_TOSTR_IMPL(__VA_ARGS__)
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_TOSTR_IMPL(x) #x
#define DOCTEST_TOSTR(x) DOCTEST_TOSTR_IMPL(x)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

// counts the number of elements in a C string
#define DOCTEST_COUNTOF(x) (sizeof(x) / sizeof(x[0]))

#ifndef DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE
#define DOCTEST_REF_WRAP(x) x&
#else // DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE
#define DOCTEST_REF_WRAP(x) x
#endif // DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE

// not using __APPLE__ because... this is how Catch does it
#if defined(__MAC_OS_X_VERSION_MIN_REQUIRED)
#define DOCTEST_PLATFORM_MAC
#elif defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#define DOCTEST_PLATFORM_IPHONE
#elif defined(_WIN32)
#define DOCTEST_PLATFORM_WINDOWS
#else
#define DOCTEST_PLATFORM_LINUX
#endif

#define DOCTEST_GLOBAL_NO_WARNINGS(var)                                                            \
    DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wglobal-constructors") static int var DOCTEST_UNUSED
#define DOCTEST_GLOBAL_NO_WARNINGS_END() DOCTEST_CLANG_SUPPRESS_WARNING_POP

// should probably take a look at https://github.com/scottt/debugbreak
#ifdef DOCTEST_PLATFORM_MAC
#define DOCTEST_BREAK_INTO_DEBUGGER() __asm__("int $3\n" : :)
#elif DOCTEST_MSVC
#define DOCTEST_BREAK_INTO_DEBUGGER() __debugbreak()
#elif defined(__MINGW32__)
extern "C" __declspec(dllimport) void __stdcall DebugBreak();
#define DOCTEST_BREAK_INTO_DEBUGGER() ::DebugBreak()
#else // linux
#define DOCTEST_BREAK_INTO_DEBUGGER() ((void)0)
#endif // linux

#if DOCTEST_CLANG
// to detect if libc++ is being used with clang (the _LIBCPP_VERSION identifier)
#include <ciso646>
#endif // clang

#ifdef _LIBCPP_VERSION
// not forward declaring ostream for libc++ because I had some problems (inline namespaces vs c++98)
// so the <iosfwd> header is used - also it is very light and doesn't drag a ton of stuff
#include <iosfwd>
#else // _LIBCPP_VERSION
#ifndef DOCTEST_CONFIG_USE_IOSFWD
namespace std
{
template <class charT>
struct char_traits;
template <>
struct char_traits<char>;
template <class charT, class traits>
class basic_ostream;
typedef basic_ostream<char, char_traits<char> > ostream;
} // namespace std
#else // DOCTEST_CONFIG_USE_IOSFWD
#include <iosfwd>
#endif // DOCTEST_CONFIG_USE_IOSFWD
#endif // _LIBCPP_VERSION

// static assert macro - because of the c++98 support requires that the message is an
// identifier (no spaces and not a C string) - example without quotes: I_am_a_message
// taken from here: http://stackoverflow.com/a/1980156/3162383
#ifdef DOCTEST_CONFIG_WITH_STATIC_ASSERT
#define DOCTEST_STATIC_ASSERT(expression, message) static_assert(expression, #message)
#else // DOCTEST_CONFIG_WITH_STATIC_ASSERT
#define DOCTEST_STATIC_ASSERT(expression, message)                                                 \
    struct DOCTEST_CAT(__static_assertion_at_line_, __LINE__)                                      \
    {                                                                                              \
        doctest::detail::static_assert_impl::StaticAssertion<static_cast<bool>((expression))>      \
                DOCTEST_CAT(DOCTEST_CAT(DOCTEST_CAT(STATIC_ASSERTION_FAILED_AT_LINE_, __LINE__),   \
                                        _),                                                        \
                            message);                                                              \
    };                                                                                             \
    typedef doctest::detail::static_assert_impl::StaticAssertionTest<static_cast<int>(             \
            sizeof(DOCTEST_CAT(__static_assertion_at_line_, __LINE__)))>                           \
            DOCTEST_CAT(__static_assertion_test_at_line_, __LINE__)
#endif // DOCTEST_CONFIG_WITH_STATIC_ASSERT

#ifdef DOCTEST_CONFIG_WITH_NULLPTR
#ifdef _LIBCPP_VERSION
#include <cstddef>
#else  // _LIBCPP_VERSION
namespace std
{
typedef decltype(nullptr) nullptr_t;
}
#endif // _LIBCPP_VERSION
#endif // DOCTEST_CONFIG_WITH_NULLPTR

#ifndef DOCTEST_CONFIG_DISABLE

#ifdef DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS
#include <type_traits>
#endif // DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS

namespace doctest
{
namespace detail
{
    struct TestSuite
    {
        const char* m_test_suite;
        const char* m_description;
        bool        m_skip;
        bool        m_may_fail;
        bool        m_should_fail;
        int         m_expected_failures;
        double      m_timeout;

        TestSuite& operator*(const char* in) {
            m_test_suite = in;
            // clear state
            m_description       = 0;
            m_skip              = false;
            m_may_fail          = false;
            m_should_fail       = false;
            m_expected_failures = 0;
            m_timeout           = 0;
            return *this;
        }

        template <typename T>
        TestSuite& operator*(const T& in) {
            in.fill(*this);
            return *this;
        }
    };
} // namespace detail
} // namespace doctest

// in a separate namespace outside of doctest because the DOCTEST_TEST_SUITE macro
// introduces an anonymous namespace in which getCurrentTestSuite gets overridden
namespace doctest_detail_test_suite_ns
{
DOCTEST_INTERFACE doctest::detail::TestSuite& getCurrentTestSuite();
} // namespace doctest_detail_test_suite_ns

#endif // DOCTEST_CONFIG_DISABLE

namespace doctest
{
// A 24 byte string class (can be as small as 17 for x64 and 13 for x86) that can hold strings with length
// of up to 23 chars on the stack before going on the heap - the last byte of the buffer is used for:
// - "is small" bit - the highest bit - if "0" then it is small - otherwise its "1" (128)
// - if small - capacity left before going on the heap - using the lowest 5 bits
// - if small - 2 bits are left unused - the second and third highest ones
// - if small - acts as a null terminator if strlen() is 23 (24 including the null terminator)
//              and the "is small" bit remains "0" ("as well as the capacity left") so its OK
// Idea taken from this lecture about the string implementation of facebook/folly - fbstring
// https://www.youtube.com/watch?v=kPR8h4-qZdk
// TODO:
// - optimizations - like not deleting memory unnecessarily in operator= and etc.
// - resize/reserve/clear
// - substr
// - replace
// - back/front
// - iterator stuff
// - find & friends
// - push_back/pop_back
// - assign/insert/erase
// - relational operators as free functions - taking const char* as one of the params
class DOCTEST_INTERFACE String
{

    struct view // len should be more than sizeof(view) - because of the final byte for flags
    {
        char*    ptr;
        unsigned size;
        unsigned capacity;
    };

    static const unsigned len  = sizeof(view) > 24 ? sizeof(view) : 24;      //!OCLINT avoid private static members
    static const unsigned last = len - 1; //!OCLINT avoid private static members

    union
    {
        char buf[sizeof(view) > 24 ? sizeof(view) : 24];
        view data;
    };

    void copy(const String& other);

    void setOnHeap() { *reinterpret_cast<unsigned char*>(&buf[last]) = 128; }
    void setLast(unsigned in = last) { buf[last] = char(in); }

public:
    String() {
        buf[0] = '\0';
        setLast();
    }

    String(const char* in);

    String(const String& other) { copy(other); }

    ~String() {
        if(!isOnStack())
            delete[] data.ptr;
    }

    #if defined(__BORLANDC__)
#pragma option push
#pragma option -vi-
#endif
    // GCC 4.9/5/6 report Wstrict-overflow when optimizations are ON and it got inlined in the vector class somewhere...
    // see commit 574ef95f0cd379118be5011704664e4b5351f1e0 and build https://travis-ci.org/onqtam/doctest/builds/230671611
    DOCTEST_NOINLINE String& operator=(const String& other) {
        if(this != &other) {
            if(!isOnStack())
                delete[] data.ptr;

            copy(other);
        }

        return *this;
    }
    #if defined(__BORLANDC__)
#pragma option pop
#endif
    String& operator+=(const String& other);

    String operator+(const String& other) const { return String(*this) += other; }

#ifdef DOCTEST_CONFIG_WITH_RVALUE_REFERENCES
    String(String&& other);
    String& operator=(String&& other);
#endif // DOCTEST_CONFIG_WITH_RVALUE_REFERENCES

    bool isOnStack() const { return (buf[last] & 128) == 0; }

    char operator[](unsigned i) const { return const_cast<String*>(this)->operator[](i); } // NOLINT
    char& operator[](unsigned i) {
        if(isOnStack())
            return reinterpret_cast<char*>(buf)[i];
        return data.ptr[i];
    }

    const char* c_str() const { return const_cast<String*>(this)->c_str(); } // NOLINT
    char*       c_str() {
        if(isOnStack())
            return reinterpret_cast<char*>(buf);
        return data.ptr;
    }

    DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wmaybe-uninitialized")
    unsigned size() const {
        if(isOnStack())
            return last - (unsigned(buf[last]) & 31); // using "last" would work only if "len" is 32
        return data.size;
    }
    DOCTEST_GCC_SUPPRESS_WARNING_POP

    unsigned capacity() const {
        if(isOnStack())
            return len;
        return data.capacity;
    }

    int compare(const char* other, bool no_case = false) const;
    int compare(const String& other, bool no_case = false) const;
};

// clang-format off
inline bool operator==(const String& lhs, const String& rhs) { return lhs.compare(rhs) == 0; }
inline bool operator!=(const String& lhs, const String& rhs) { return lhs.compare(rhs) != 0; }
inline bool operator< (const String& lhs, const String& rhs) { return lhs.compare(rhs) < 0; }
inline bool operator> (const String& lhs, const String& rhs) { return lhs.compare(rhs) > 0; }
inline bool operator<=(const String& lhs, const String& rhs) { return (lhs != rhs) ? lhs.compare(rhs) < 0 : true; }
inline bool operator>=(const String& lhs, const String& rhs) { return (lhs != rhs) ? lhs.compare(rhs) > 0 : true; }
// clang-format on

DOCTEST_INTERFACE std::ostream& operator<<(std::ostream& s, const String& in);

namespace detail
{
#ifndef DOCTEST_CONFIG_WITH_STATIC_ASSERT
    namespace static_assert_impl
    {
        template <bool>
        struct StaticAssertion;

        template <>
        struct StaticAssertion<true>
        {};

        template <int i>
        struct StaticAssertionTest
        {};
    }  // namespace static_assert_impl
#endif // DOCTEST_CONFIG_WITH_STATIC_ASSERT

    template <bool CONDITION, typename TYPE = void>
    struct enable_if
    {};

    template <typename TYPE>
    struct enable_if<true, TYPE>
    { typedef TYPE type; };

    template <typename T>
    struct deferred_false
    // cppcheck-suppress unusedStructMember
    { static const bool value = false; };

    // to silence the warning "-Wzero-as-null-pointer-constant" only for gcc 5 for the Approx template ctor - pragmas don't work for it...
    inline void* getNull() { return 0; }

    namespace has_insertion_operator_impl
    {
        typedef char no;
        typedef char yes[2];

        struct any_t
        {
            template <typename T>
            // cppcheck-suppress noExplicitConstructor
            any_t(const DOCTEST_REF_WRAP(T));
        };

        yes& testStreamable(std::ostream&);
        no   testStreamable(no);

        no operator<<(const std::ostream&, const any_t&);

        template <typename T>
        struct has_insertion_operator
        {
            static std::ostream& s;
            static const DOCTEST_REF_WRAP(T) t;
            static const bool value = sizeof(testStreamable(s << t)) == sizeof(yes);
        };
    } // namespace has_insertion_operator_impl

    template <typename T>
    struct has_insertion_operator : has_insertion_operator_impl::has_insertion_operator<T>
    {};

    DOCTEST_INTERFACE void my_memcpy(void* dest, const void* src, unsigned num);

    DOCTEST_INTERFACE std::ostream* createStream();
    DOCTEST_INTERFACE String getStreamResult(std::ostream*);
    DOCTEST_INTERFACE void   freeStream(std::ostream*);

    template <bool C>
    struct StringMakerBase
    {
        template <typename T>
        static String convert(const DOCTEST_REF_WRAP(T)) {
            return "{?}";
        }
    };

    template <>
    struct StringMakerBase<true>
    {
        template <typename T>
        static String convert(const DOCTEST_REF_WRAP(T) in) {
            std::ostream* s = createStream();
            *s << in;
            String result = getStreamResult(s);
            freeStream(s);
            return result;
        }
    };

    DOCTEST_INTERFACE String rawMemoryToString(const void* object, unsigned size);

    template <typename T>
    String rawMemoryToString(const DOCTEST_REF_WRAP(T) object) {
        return rawMemoryToString(&object, sizeof(object));
    }

    class NullType
    {
    };

    template <class T, class U>
    struct Typelist
    {
        typedef T Head;
        typedef U Tail;
    };

    // type of recursive function
    template <class TList, class Callable>
    struct ForEachType;

    // Recursion rule
    template <class Head, class Tail, class Callable>
    struct ForEachType<Typelist<Head, Tail>, Callable> : public ForEachType<Tail, Callable>
    {
        enum
        {
            value = 1 + ForEachType<Tail, Callable>::value
        };

        explicit ForEachType(Callable& callable)
                : ForEachType<Tail, Callable>(callable) {
#if DOCTEST_MSVC && DOCTEST_MSVC < DOCTEST_COMPILER(19, 10, 0)
            callable.operator()<value, Head>();
#else  // MSVC
            callable.template operator()<value, Head>();
#endif // MSVC
        }
    };

    // Recursion end
    template <class Head, class Callable>
    struct ForEachType<Typelist<Head, NullType>, Callable>
    {
    public:
        enum
        {
            value = 0
        };

        explicit ForEachType(Callable& callable) {
#if DOCTEST_MSVC && DOCTEST_MSVC < DOCTEST_COMPILER(19, 10, 0)
            callable.operator()<value, Head>();
#else  // MSVC
            callable.template operator()<value, Head>();
#endif // MSVC
        }
    };

    template <typename T>
    const char* type_to_string() {
        return "<>";
    }
} // namespace detail

template <typename T1 = detail::NullType, typename T2 = detail::NullType,
          typename T3 = detail::NullType, typename T4 = detail::NullType,
          typename T5 = detail::NullType, typename T6 = detail::NullType,
          typename T7 = detail::NullType, typename T8 = detail::NullType,
          typename T9 = detail::NullType, typename T10 = detail::NullType,
          typename T11 = detail::NullType, typename T12 = detail::NullType,
          typename T13 = detail::NullType, typename T14 = detail::NullType,
          typename T15 = detail::NullType, typename T16 = detail::NullType,
          typename T17 = detail::NullType, typename T18 = detail::NullType,
          typename T19 = detail::NullType, typename T20 = detail::NullType,
          typename T21 = detail::NullType, typename T22 = detail::NullType,
          typename T23 = detail::NullType, typename T24 = detail::NullType,
          typename T25 = detail::NullType, typename T26 = detail::NullType,
          typename T27 = detail::NullType, typename T28 = detail::NullType,
          typename T29 = detail::NullType, typename T30 = detail::NullType,
          typename T31 = detail::NullType, typename T32 = detail::NullType,
          typename T33 = detail::NullType, typename T34 = detail::NullType,
          typename T35 = detail::NullType, typename T36 = detail::NullType,
          typename T37 = detail::NullType, typename T38 = detail::NullType,
          typename T39 = detail::NullType, typename T40 = detail::NullType,
          typename T41 = detail::NullType, typename T42 = detail::NullType,
          typename T43 = detail::NullType, typename T44 = detail::NullType,
          typename T45 = detail::NullType, typename T46 = detail::NullType,
          typename T47 = detail::NullType, typename T48 = detail::NullType,
          typename T49 = detail::NullType, typename T50 = detail::NullType,
          typename T51 = detail::NullType, typename T52 = detail::NullType,
          typename T53 = detail::NullType, typename T54 = detail::NullType,
          typename T55 = detail::NullType, typename T56 = detail::NullType,
          typename T57 = detail::NullType, typename T58 = detail::NullType,
          typename T59 = detail::NullType, typename T60 = detail::NullType>
struct Types
{
private:
    typedef typename Types<T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17,
                           T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31,
                           T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43, T44, T45,
                           T46, T47, T48, T49, T50, T51, T52, T53, T54, T55, T56, T57, T58, T59,
                           T60>::Result TailResult;

public:
    typedef detail::Typelist<T1, TailResult> Result;
};

template <>
struct Types<>
{ typedef detail::NullType Result; };

template <typename T>
struct StringMaker : detail::StringMakerBase<detail::has_insertion_operator<T>::value>
{};

template <typename T>
struct StringMaker<T*>
{
    template <typename U>
    static String convert(U* p) {
        if(p)
            return detail::rawMemoryToString(p);
        return "NULL";
    }
};

template <typename R, typename C>
struct StringMaker<R C::*>
{
    static String convert(R C::*p) {
        if(p)
            return detail::rawMemoryToString(p);
        return "NULL";
    }
};

template <typename T>
String toString(const DOCTEST_REF_WRAP(T) value) {
    return StringMaker<T>::convert(value);
}

#ifdef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
DOCTEST_INTERFACE String toString(char* in);
DOCTEST_INTERFACE String toString(const char* in);
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
DOCTEST_INTERFACE String toString(bool in);
DOCTEST_INTERFACE String toString(float in);
DOCTEST_INTERFACE String toString(double in);
DOCTEST_INTERFACE String toString(double long in);

DOCTEST_INTERFACE String toString(char in);
DOCTEST_INTERFACE String toString(char signed in);
DOCTEST_INTERFACE String toString(char unsigned in);
DOCTEST_INTERFACE String toString(int short in);
DOCTEST_INTERFACE String toString(int short unsigned in);
DOCTEST_INTERFACE String toString(int in);
DOCTEST_INTERFACE String toString(int unsigned in);
DOCTEST_INTERFACE String toString(int long in);
DOCTEST_INTERFACE String toString(int long unsigned in);

#ifdef DOCTEST_CONFIG_WITH_LONG_LONG
DOCTEST_INTERFACE String toString(int long long in);
DOCTEST_INTERFACE String toString(int long long unsigned in);
#endif // DOCTEST_CONFIG_WITH_LONG_LONG

#ifdef DOCTEST_CONFIG_WITH_NULLPTR
DOCTEST_INTERFACE String toString(std::nullptr_t in);
#endif // DOCTEST_CONFIG_WITH_NULLPTR

//DOCTEST_INTERFACE template<class T> String toString(T *in);

class DOCTEST_INTERFACE Approx
{
public:
    explicit Approx(double value);

    Approx operator()(double value) const {
        Approx approx(value);
        approx.epsilon(m_epsilon);
        approx.scale(m_scale);
        return approx;
    }

#ifdef DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS
    template <typename T>
    explicit Approx(const T& value,
                    typename detail::enable_if<std::is_constructible<double, T>::value>::type* =
                            static_cast<T*>(detail::getNull())) {
        *this = Approx(static_cast<double>(value));
    }
#endif // DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS

    // clang-format off
    // overloads for double - the first one is necessary as it is in the implementation part of doctest
    // as for the others - keeping them for potentially faster compile times
    DOCTEST_INTERFACE friend bool operator==(double lhs, Approx const& rhs);
    friend bool operator==(Approx const& lhs, double rhs) { return operator==(rhs, lhs); }
    friend bool operator!=(double lhs, Approx const& rhs) { return !operator==(lhs, rhs); }
    friend bool operator!=(Approx const& lhs, double rhs) { return !operator==(rhs, lhs); }
    friend bool operator<=(double lhs, Approx const& rhs) { return lhs < rhs.m_value || lhs == rhs; }
    friend bool operator<=(Approx const& lhs, double rhs) { return lhs.m_value < rhs || lhs == rhs; }
    friend bool operator>=(double lhs, Approx const& rhs) { return lhs > rhs.m_value || lhs == rhs; }
    friend bool operator>=(Approx const& lhs, double rhs) { return lhs.m_value > rhs || lhs == rhs; }
    friend bool operator< (double lhs, Approx const& rhs) { return lhs < rhs.m_value && lhs != rhs; }
    friend bool operator< (Approx const& lhs, double rhs) { return lhs.m_value < rhs && lhs != rhs; }
    friend bool operator> (double lhs, Approx const& rhs) { return lhs > rhs.m_value && lhs != rhs; }
    friend bool operator> (Approx const& lhs, double rhs) { return lhs.m_value > rhs && lhs != rhs; }

#ifdef DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS
#define DOCTEST_APPROX_PREFIX \
    template <typename T> friend typename detail::enable_if<std::is_constructible<double, T>::value, bool>::type

    DOCTEST_APPROX_PREFIX operator==(const T& lhs, const Approx& rhs) { return operator==(double(lhs), rhs); }
    DOCTEST_APPROX_PREFIX operator==(const Approx& lhs, const T& rhs) { return operator==(rhs, lhs); }
    DOCTEST_APPROX_PREFIX operator!=(const T& lhs, const Approx& rhs) { return !operator==(lhs, rhs); }
    DOCTEST_APPROX_PREFIX operator!=(const Approx& lhs, const T& rhs) { return !operator==(rhs, lhs); }
    DOCTEST_APPROX_PREFIX operator<=(const T& lhs, const Approx& rhs) { return double(lhs) < rhs.m_value || lhs == rhs; }
    DOCTEST_APPROX_PREFIX operator<=(const Approx& lhs, const T& rhs) { return lhs.m_value < double(rhs) || lhs == rhs; }
    DOCTEST_APPROX_PREFIX operator>=(const T& lhs, const Approx& rhs) { return double(lhs) > rhs.m_value || lhs == rhs; }
    DOCTEST_APPROX_PREFIX operator>=(const Approx& lhs, const T& rhs) { return lhs.m_value > double(rhs) || lhs == rhs; }
    DOCTEST_APPROX_PREFIX operator< (const T& lhs, const Approx& rhs) { return double(lhs) < rhs.m_value && lhs != rhs; }
    DOCTEST_APPROX_PREFIX operator< (const Approx& lhs, const T& rhs) { return lhs.m_value < double(rhs) && lhs != rhs; }
    DOCTEST_APPROX_PREFIX operator> (const T& lhs, const Approx& rhs) { return double(lhs) > rhs.m_value && lhs != rhs; }
    DOCTEST_APPROX_PREFIX operator> (const Approx& lhs, const T& rhs) { return lhs.m_value > double(rhs) && lhs != rhs; }
#undef DOCTEST_APPROX_PREFIX
#endif // DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS

    // clang-format on

    Approx& epsilon(double newEpsilon) {
        m_epsilon = newEpsilon;
        return *this;
    }

#ifdef DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS
    template <typename T>
    typename detail::enable_if<std::is_constructible<double, T>::value, Approx&>::type epsilon(
            const T& newEpsilon) {
        m_epsilon = static_cast<double>(newEpsilon);
        return *this;
    }
#endif //  DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS

    Approx& scale(double newScale) {
        m_scale = newScale;
        return *this;
    }

#ifdef DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS
    template <typename T>
    typename detail::enable_if<std::is_constructible<double, T>::value, Approx&>::type scale(
            const T& newScale) {
        m_scale = static_cast<double>(newScale);
        return *this;
    }
#endif // DOCTEST_CONFIG_INCLUDE_TYPE_TRAITS

    String toString() const;

private:
    double m_epsilon;
    double m_scale;
    double m_value;
};

template <>
inline String toString<Approx>(const DOCTEST_REF_WRAP(Approx) value) {
    return value.toString();
}

#if !defined(DOCTEST_CONFIG_DISABLE)

namespace detail
{
    // the function type this library works with
    typedef void (*funcType)();

    namespace assertType
    {
        enum Enum
        {
            // macro traits

            is_warn    = 1,
            is_check   = 2,
            is_require = 4,

            is_throws    = 8,
            is_throws_as = 16,
            is_nothrow   = 32,

            is_fast  = 64, // not checked anywhere - used just to distinguish the types
            is_false = 128,
            is_unary = 256,

            is_eq = 512,
            is_ne = 1024,

            is_lt = 2048,
            is_gt = 4096,

            is_ge = 8192,
            is_le = 16384,

            // macro types

            DT_WARN    = is_warn,
            DT_CHECK   = is_check,
            DT_REQUIRE = is_require,

            DT_WARN_FALSE    = is_false | is_warn,
            DT_CHECK_FALSE   = is_false | is_check,
            DT_REQUIRE_FALSE = is_false | is_require,

            DT_WARN_THROWS    = is_throws | is_warn,
            DT_CHECK_THROWS   = is_throws | is_check,
            DT_REQUIRE_THROWS = is_throws | is_require,

            DT_WARN_THROWS_AS    = is_throws_as | is_warn,
            DT_CHECK_THROWS_AS   = is_throws_as | is_check,
            DT_REQUIRE_THROWS_AS = is_throws_as | is_require,

            DT_WARN_NOTHROW    = is_nothrow | is_warn,
            DT_CHECK_NOTHROW   = is_nothrow | is_check,
            DT_REQUIRE_NOTHROW = is_nothrow | is_require,

            DT_WARN_EQ    = is_eq | is_warn,
            DT_CHECK_EQ   = is_eq | is_check,
            DT_REQUIRE_EQ = is_eq | is_require,

            DT_WARN_NE    = is_ne | is_warn,
            DT_CHECK_NE   = is_ne | is_check,
            DT_REQUIRE_NE = is_ne | is_require,

            DT_WARN_GT    = is_gt | is_warn,
            DT_CHECK_GT   = is_gt | is_check,
            DT_REQUIRE_GT = is_gt | is_require,

            DT_WARN_LT    = is_lt | is_warn,
            DT_CHECK_LT   = is_lt | is_check,
            DT_REQUIRE_LT = is_lt | is_require,

            DT_WARN_GE    = is_ge | is_warn,
            DT_CHECK_GE   = is_ge | is_check,
            DT_REQUIRE_GE = is_ge | is_require,

            DT_WARN_LE    = is_le | is_warn,
            DT_CHECK_LE   = is_le | is_check,
            DT_REQUIRE_LE = is_le | is_require,

            DT_WARN_UNARY    = is_unary | is_warn,
            DT_CHECK_UNARY   = is_unary | is_check,
            DT_REQUIRE_UNARY = is_unary | is_require,

            DT_WARN_UNARY_FALSE    = is_false | is_unary | is_warn,
            DT_CHECK_UNARY_FALSE   = is_false | is_unary | is_check,
            DT_REQUIRE_UNARY_FALSE = is_false | is_unary | is_require,

            DT_FAST_WARN_EQ    = is_fast | is_eq | is_warn,
            DT_FAST_CHECK_EQ   = is_fast | is_eq | is_check,
            DT_FAST_REQUIRE_EQ = is_fast | is_eq | is_require,

            DT_FAST_WARN_NE    = is_fast | is_ne | is_warn,
            DT_FAST_CHECK_NE   = is_fast | is_ne | is_check,
            DT_FAST_REQUIRE_NE = is_fast | is_ne | is_require,

            DT_FAST_WARN_GT    = is_fast | is_gt | is_warn,
            DT_FAST_CHECK_GT   = is_fast | is_gt | is_check,
            DT_FAST_REQUIRE_GT = is_fast | is_gt | is_require,

            DT_FAST_WARN_LT    = is_fast | is_lt | is_warn,
            DT_FAST_CHECK_LT   = is_fast | is_lt | is_check,
            DT_FAST_REQUIRE_LT = is_fast | is_lt | is_require,

            DT_FAST_WARN_GE    = is_fast | is_ge | is_warn,
            DT_FAST_CHECK_GE   = is_fast | is_ge | is_check,
            DT_FAST_REQUIRE_GE = is_fast | is_ge | is_require,

            DT_FAST_WARN_LE    = is_fast | is_le | is_warn,
            DT_FAST_CHECK_LE   = is_fast | is_le | is_check,
            DT_FAST_REQUIRE_LE = is_fast | is_le | is_require,

            DT_FAST_WARN_UNARY    = is_fast | is_unary | is_warn,
            DT_FAST_CHECK_UNARY   = is_fast | is_unary | is_check,
            DT_FAST_REQUIRE_UNARY = is_fast | is_unary | is_require,

            DT_FAST_WARN_UNARY_FALSE    = is_fast | is_false | is_unary | is_warn,
            DT_FAST_CHECK_UNARY_FALSE   = is_fast | is_false | is_unary | is_check,
            DT_FAST_REQUIRE_UNARY_FALSE = is_fast | is_false | is_unary | is_require
        };
    } // namespace assertType

    DOCTEST_INTERFACE const char* assertString(assertType::Enum val);

    // clang-format off
    template<class T>               struct decay_array       { typedef T type; };
    template<class T, unsigned N>   struct decay_array<T[N]> { typedef T* type; };
    //template<class T>               struct decay_array<T[]>  { typedef T* type; };

    template<class T>   struct not_char_pointer              { enum { value = 1 }; };
    template<>          struct not_char_pointer<char*>       { enum { value = 0 }; };
    template<>          struct not_char_pointer<const char*> { enum { value = 0 }; };

    template<class T> struct can_use_op : not_char_pointer<typename decay_array<T>::type> {};
    // clang-format on

    struct TestFailureException
    {
    };

    DOCTEST_INTERFACE bool checkIfShouldThrow(assertType::Enum at);
    DOCTEST_INTERFACE void fastAssertThrowIfFlagSet(int flags);

    struct TestAccessibleContextState
    {
        bool no_throw; // to skip exceptions-related assertion macros
        bool success;  // include successful assertions in output
    };

    struct ContextState;

    DOCTEST_INTERFACE TestAccessibleContextState* getTestsContextState();

    struct DOCTEST_INTERFACE SubcaseSignature
    {
        const char* m_name;
        const char* m_file;
        int         m_line;

        SubcaseSignature(const char* name, const char* file, int line)
                : m_name(name)
                , m_file(file)
                , m_line(line) {}

        bool operator<(const SubcaseSignature& other) const;
    };

    // cppcheck-suppress copyCtorAndEqOperator
    struct DOCTEST_INTERFACE Subcase
    {
        SubcaseSignature m_signature;
        bool             m_entered;

        Subcase(const char* name, const char* file, int line);
        Subcase(const Subcase& other);
        ~Subcase();

        operator bool() const { return m_entered; }
    };

    template <typename L, typename R>
    String stringifyBinaryExpr(const DOCTEST_REF_WRAP(L) lhs, const char* op,
                               const DOCTEST_REF_WRAP(R) rhs) {
        return toString(lhs) + op + toString(rhs);
    }

    struct DOCTEST_INTERFACE Result
    {
        bool   m_passed;
        String m_decomposition;

        ~Result();
        #if defined(__BORLANDC__)
#pragma option push
#pragma option -vi-
#endif

        DOCTEST_NOINLINE Result(bool passed = false, const String& decomposition = String())
                : m_passed(passed)
                , m_decomposition(decomposition) {}

        DOCTEST_NOINLINE Result(const Result& other)
                : m_passed(other.m_passed)
                , m_decomposition(other.m_decomposition) {}

        #if defined(__BORLANDC__)
#pragma option pop
#endif

        Result& operator=(const Result& other);

        operator bool() { return !m_passed; }

        // clang-format off
        // forbidding some expressions based on this table: http://en.cppreference.com/w/cpp/language/operator_precedence
        template <typename R> Result& operator&  (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator^  (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator|  (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator&& (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator|| (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator== (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator!= (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator<  (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator>  (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator<= (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator>= (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator=  (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator+= (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator-= (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator*= (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator/= (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator%= (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator<<=(const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator>>=(const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator&= (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator^= (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        template <typename R> Result& operator|= (const R&) { DOCTEST_STATIC_ASSERT(deferred_false<R>::value, Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison); return *this; }
        // clang-format on
    };

#ifndef DOCTEST_CONFIG_NO_COMPARISON_WARNING_SUPPRESSION

    DOCTEST_CLANG_SUPPRESS_WARNING_PUSH
    DOCTEST_CLANG_SUPPRESS_WARNING("-Wsign-conversion")
    DOCTEST_CLANG_SUPPRESS_WARNING("-Wsign-compare")
    //DOCTEST_CLANG_SUPPRESS_WARNING("-Wdouble-promotion")
    //DOCTEST_CLANG_SUPPRESS_WARNING("-Wconversion")
    //DOCTEST_CLANG_SUPPRESS_WARNING("-Wfloat-equal")

    DOCTEST_GCC_SUPPRESS_WARNING_PUSH
    DOCTEST_GCC_SUPPRESS_WARNING("-Wsign-conversion")
    DOCTEST_GCC_SUPPRESS_WARNING("-Wsign-compare")
    //DOCTEST_GCC_SUPPRESS_WARNING("-Wdouble-promotion")
    //DOCTEST_GCC_SUPPRESS_WARNING("-Wconversion")
    //DOCTEST_GCC_SUPPRESS_WARNING("-Wfloat-equal")

    DOCTEST_MSVC_SUPPRESS_WARNING_PUSH
    // http://stackoverflow.com/questions/39479163 what's the difference between C4018 and C4389
    DOCTEST_MSVC_SUPPRESS_WARNING(4388) // signed/unsigned mismatch
    DOCTEST_MSVC_SUPPRESS_WARNING(4389) // 'operator' : signed/unsigned mismatch
    DOCTEST_MSVC_SUPPRESS_WARNING(4018) // 'expression' : signed/unsigned mismatch
    //DOCTEST_MSVC_SUPPRESS_WARNING(4805) // 'operation' : unsafe mix of type 'type' and type 'type' in operation

#endif // DOCTEST_CONFIG_NO_COMPARISON_WARNING_SUPPRESSION

// clang-format off
#ifndef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#define DOCTEST_COMPARISON_RETURN_TYPE bool
#else // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#define DOCTEST_COMPARISON_RETURN_TYPE typename enable_if<can_use_op<L>::value || can_use_op<R>::value, bool>::type
    inline bool eq(const char* lhs, const char* rhs) { return String(lhs) == String(rhs); }
    inline bool ne(const char* lhs, const char* rhs) { return String(lhs) != String(rhs); }
    inline bool lt(const char* lhs, const char* rhs) { return String(lhs) <  String(rhs); }
    inline bool gt(const char* lhs, const char* rhs) { return String(lhs) >  String(rhs); }
    inline bool le(const char* lhs, const char* rhs) { return String(lhs) <= String(rhs); }
    inline bool ge(const char* lhs, const char* rhs) { return String(lhs) >= String(rhs); }
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING

    template <typename L, typename R> DOCTEST_COMPARISON_RETURN_TYPE eq(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) { return lhs == rhs; }
    template <typename L, typename R> DOCTEST_COMPARISON_RETURN_TYPE ne(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) { return lhs != rhs; }
    template <typename L, typename R> DOCTEST_COMPARISON_RETURN_TYPE lt(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) { return lhs <  rhs; }
    template <typename L, typename R> DOCTEST_COMPARISON_RETURN_TYPE gt(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) { return lhs >  rhs; }
    template <typename L, typename R> DOCTEST_COMPARISON_RETURN_TYPE le(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) { return lhs <= rhs; }
    template <typename L, typename R> DOCTEST_COMPARISON_RETURN_TYPE ge(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) { return lhs >= rhs; }
    // clang-format on

#ifndef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#define DOCTEST_CMP_EQ(l, r) l == r
#define DOCTEST_CMP_NE(l, r) l != r
#define DOCTEST_CMP_GT(l, r) l > r
#define DOCTEST_CMP_LT(l, r) l < r
#define DOCTEST_CMP_GE(l, r) l >= r
#define DOCTEST_CMP_LE(l, r) l <= r
#else // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#define DOCTEST_CMP_EQ(l, r) eq(l, r)
#define DOCTEST_CMP_NE(l, r) ne(l, r)
#define DOCTEST_CMP_GT(l, r) gt(l, r)
#define DOCTEST_CMP_LT(l, r) lt(l, r)
#define DOCTEST_CMP_GE(l, r) ge(l, r)
#define DOCTEST_CMP_LE(l, r) le(l, r)
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING

#define DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(op, op_str, op_macro)                              \
    template <typename R>                                                                          \
    DOCTEST_NOINLINE Result operator op(const DOCTEST_REF_WRAP(R) rhs) {                           \
        bool res = op_macro(lhs, rhs);                                                             \
        if(m_assert_type & assertType::is_false)                                                   \
            res = !res;                                                                            \
        if(!res || doctest::detail::getTestsContextState()->success)                               \
            return Result(res, stringifyBinaryExpr(lhs, op_str, rhs));                             \
        return Result(res);                                                                        \
    }                                                                                              

#define DOCTEST_FORBIT_EXPRESSION(op)                                                              \
    template <typename R>                                                                          \
    Expression_lhs& operator op(const R&) {                                                        \
        DOCTEST_STATIC_ASSERT(deferred_false<R>::value,                                            \
                              Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison);         \
        return *this;                                                                              \
    }

    template <typename L>
    // cppcheck-suppress copyCtorAndEqOperator
    struct Expression_lhs
    {
        L                lhs;
        assertType::Enum m_assert_type;

        explicit Expression_lhs(L in, assertType::Enum at)
                : lhs(in)
                , m_assert_type(at) {}

        #if defined(__BORLANDC__)
#pragma option push
#pragma option -vi-
#endif

        DOCTEST_NOINLINE operator Result() {
            bool res = !!lhs;
            if(m_assert_type & assertType::is_false) //!OCLINT bitwise operator in conditional
                res = !res;

            if(!res || getTestsContextState()->success)
            {
                String str = doctest::toString(lhs);
                return Result(res, str);
            }
            return Result(res);
        }

        #if defined(__BORLANDC__)
#pragma option pop
#endif

        // clang-format off
#if defined(__BORLANDC__)
#pragma option push
#pragma option -vi-
#endif
        DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(==, " == ", DOCTEST_CMP_EQ) //!OCLINT bitwise operator in conditional
        DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(!=, " != ", DOCTEST_CMP_NE) //!OCLINT bitwise operator in conditional
        DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(>,  " >  ", DOCTEST_CMP_GT) //!OCLINT bitwise operator in conditional
        DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(<,  " <  ", DOCTEST_CMP_LT) //!OCLINT bitwise operator in conditional
        DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(>=, " >= ", DOCTEST_CMP_GE) //!OCLINT bitwise operator in conditional
        DOCTEST_DO_BINARY_EXPRESSION_COMPARISON(<=, " <= ", DOCTEST_CMP_LE) //!OCLINT bitwise operator in conditional

#if defined(__BORLANDC__)
#pragma option pop
#endif
        // clang-format on

        // forbidding some expressions based on this table: http://en.cppreference.com/w/cpp/language/operator_precedence
        DOCTEST_FORBIT_EXPRESSION(&)
        DOCTEST_FORBIT_EXPRESSION(^)
        DOCTEST_FORBIT_EXPRESSION(|)
        DOCTEST_FORBIT_EXPRESSION(&&)
        DOCTEST_FORBIT_EXPRESSION(||)
        DOCTEST_FORBIT_EXPRESSION(=)
        DOCTEST_FORBIT_EXPRESSION(+=)
        DOCTEST_FORBIT_EXPRESSION(-=)
        DOCTEST_FORBIT_EXPRESSION(*=)
        DOCTEST_FORBIT_EXPRESSION(/=)
        DOCTEST_FORBIT_EXPRESSION(%=)
        DOCTEST_FORBIT_EXPRESSION(<<=)
        DOCTEST_FORBIT_EXPRESSION(>>=)
        DOCTEST_FORBIT_EXPRESSION(&=)
        DOCTEST_FORBIT_EXPRESSION(^=)
        DOCTEST_FORBIT_EXPRESSION(|=)
        // these 2 are unfortunate because they should be allowed - they have higher precedence over the comparisons, but the
        // ExpressionDecomposer class uses the left shift operator to capture the left operand of the binary expression...
        DOCTEST_FORBIT_EXPRESSION(<<)
        DOCTEST_FORBIT_EXPRESSION(>>)
    };

#ifndef DOCTEST_CONFIG_NO_COMPARISON_WARNING_SUPPRESSION

    DOCTEST_CLANG_SUPPRESS_WARNING_POP
    DOCTEST_MSVC_SUPPRESS_WARNING_POP
    DOCTEST_GCC_SUPPRESS_WARNING_POP

#endif // DOCTEST_CONFIG_NO_COMPARISON_WARNING_SUPPRESSION

    struct ExpressionDecomposer
    {
        assertType::Enum m_assert_type;

        ExpressionDecomposer(assertType::Enum at)
                : m_assert_type(at) {}

        // The right operator for capturing expressions is "<=" instead of "<<" (based on the operator precedence table)
        // but then there will be warnings from GCC about "-Wparentheses" and since "_Pragma()" is problematic this will stay for now...
        // https://github.com/philsquared/Catch/issues/870
        // https://github.com/philsquared/Catch/issues/565
        template <typename L>
        Expression_lhs<const DOCTEST_REF_WRAP(L)> operator<<(const DOCTEST_REF_WRAP(L) operand) {
            return Expression_lhs<const DOCTEST_REF_WRAP(L)>(operand, m_assert_type);
        }
    };

    struct DOCTEST_INTERFACE TestCase
    {
        // not used for determining uniqueness
        funcType m_test;    // a function pointer to the test case
        String m_full_name; // contains the name (only for templated test cases!) + the template type
        const char* m_name;       // name of the test case
        const char* m_type;       // for templated test cases - gets appended to the real name
        const char* m_test_suite; // the test suite in which the test was added
        const char* m_description;
        bool        m_skip;
        bool        m_may_fail;
        bool        m_should_fail;
        int         m_expected_failures;
        double      m_timeout;

        // fields by which uniqueness of test cases shall be determined
        const char* m_file; // the file in which the test was registered
        unsigned    m_line; // the line where the test was registered
        int m_template_id; // an ID used to distinguish between the different versions of a templated test case

        TestCase(funcType test, const char* file, unsigned line, const TestSuite& test_suite,
                 const char* type = "", int template_id = -1);

        #if defined(__BORLANDC__)
#pragma option push
#pragma option -vi-
#endif

        // for gcc 4.7
        DOCTEST_NOINLINE ~TestCase() {}

        #if defined(__BORLANDC__)
#pragma option pop
#endif

        TestCase& operator*(const char* in);

        template <typename T>
        TestCase& operator*(const T& in) {
            in.fill(*this);
            return *this;
        }

        TestCase(const TestCase& other) { *this = other; }

        TestCase& operator=(const TestCase& other);

        bool operator<(const TestCase& other) const;
    };

    // forward declarations of functions used by the macros
    DOCTEST_INTERFACE int regTest(const TestCase& tc);
    DOCTEST_INTERFACE int setTestSuite(const TestSuite& ts);

    namespace binaryAssertComparison
    {
        enum Enum
        {
            eq = 0,
            ne,
            gt,
            lt,
            ge,
            le
        };
    } // namespace binaryAssertComparison

    // clang-format off
    template <int, class L, class R> struct RelationalComparator     { bool operator()(const DOCTEST_REF_WRAP(L),     const DOCTEST_REF_WRAP(R)    ) const { return false;        } };
    template <class L, class R> struct RelationalComparator<0, L, R> { bool operator()(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) const { return eq(lhs, rhs); } };
    template <class L, class R> struct RelationalComparator<1, L, R> { bool operator()(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) const { return ne(lhs, rhs); } };
    template <class L, class R> struct RelationalComparator<2, L, R> { bool operator()(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) const { return gt(lhs, rhs); } };
    template <class L, class R> struct RelationalComparator<3, L, R> { bool operator()(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) const { return lt(lhs, rhs); } };
    template <class L, class R> struct RelationalComparator<4, L, R> { bool operator()(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) const { return ge(lhs, rhs); } };
    template <class L, class R> struct RelationalComparator<5, L, R> { bool operator()(const DOCTEST_REF_WRAP(L) lhs, const DOCTEST_REF_WRAP(R) rhs) const { return le(lhs, rhs); } };
    // clang-format on

    struct DOCTEST_INTERFACE ResultBuilder
    {
        assertType::Enum m_assert_type;
        const char*      m_file;
        int              m_line;
        const char*      m_expr;
        const char*      m_exception_type;

        Result m_result;
        bool   m_threw;
        bool   m_threw_as;
        bool   m_failed;
        String m_exception;

        ResultBuilder(assertType::Enum at, const char* file, int line, const char* expr,
                      const char* exception_type = "");

        ~ResultBuilder();

        void setResult(const Result& res) { m_result = res; }

        #if defined(__BORLANDC__)
#pragma option push
#pragma option -vi-
#endif

        template <int comparison, typename L, typename R>
        DOCTEST_NOINLINE void binary_assert(const DOCTEST_REF_WRAP(L) lhs,
                                            const DOCTEST_REF_WRAP(R) rhs) {
            m_result.m_passed = RelationalComparator<comparison, L, R>()(lhs, rhs);
            if(!m_result.m_passed || getTestsContextState()->success)
                m_result.m_decomposition = stringifyBinaryExpr(lhs, ", ", rhs);
        }

        template <typename L>
        DOCTEST_NOINLINE void unary_assert(const DOCTEST_REF_WRAP(L) val) {
            m_result.m_passed = !!val;

            if(m_assert_type & assertType::is_false) //!OCLINT bitwise operator in conditional
                m_result.m_passed = !m_result.m_passed;

            if(!m_result.m_passed || getTestsContextState()->success)
                m_result.m_decomposition = toString(val);
        }

        #if defined(__BORLANDC__)
#pragma option pop
#endif

        void unexpectedExceptionOccurred();

        bool log();
        void react() const;
    };

    namespace assertAction
    {
        enum Enum
        {
            nothing     = 0,
            dbgbreak    = 1,
            shouldthrow = 2
        };
    } // namespace assertAction

    #if defined(__BORLANDC__)
#pragma option push
#pragma option -vi-
#endif

    template <int comparison, typename L, typename R>
    DOCTEST_NOINLINE int fast_binary_assert(assertType::Enum at, const char* file, int line,
                                            const char* expr, const DOCTEST_REF_WRAP(L) lhs,
                                            const DOCTEST_REF_WRAP(R) rhs) {
        ResultBuilder rb(at, file, line, expr);

        rb.m_result.m_passed = RelationalComparator<comparison, L, R>()(lhs, rhs);

        if(!rb.m_result.m_passed || getTestsContextState()->success)
            rb.m_result.m_decomposition = stringifyBinaryExpr(lhs, ", ", rhs);

        int res = 0;

        if(rb.log())
            res |= assertAction::dbgbreak;

        if(rb.m_failed && checkIfShouldThrow(at))
            res |= assertAction::shouldthrow;

#ifdef DOCTEST_CONFIG_SUPER_FAST_ASSERTS
        // #########################################################################################
        // IF THE DEBUGGER BREAKS HERE - GO 1 LEVEL UP IN THE CALLSTACK TO SEE THE FAILING ASSERTION
        // THIS IS THE EFFECT OF HAVING 'DOCTEST_CONFIG_SUPER_FAST_ASSERTS' DEFINED
        // #########################################################################################
        if(res & assertAction::dbgbreak)
            DOCTEST_BREAK_INTO_DEBUGGER();
        fastAssertThrowIfFlagSet(res);
#endif // DOCTEST_CONFIG_SUPER_FAST_ASSERTS

        return res;
    }

    template <typename L>
    DOCTEST_NOINLINE int fast_unary_assert(assertType::Enum at, const char* file, int line,
                                           const char* val_str, const DOCTEST_REF_WRAP(L) val) {
        ResultBuilder rb(at, file, line, val_str);

        rb.m_result.m_passed = !!val;

        if(at & assertType::is_false) //!OCLINT bitwise operator in conditional
            rb.m_result.m_passed = !rb.m_result.m_passed;

        if(!rb.m_result.m_passed || getTestsContextState()->success)
            rb.m_result.m_decomposition = toString(val);

        int res = 0;

        if(rb.log())
            res |= assertAction::dbgbreak;

        if(rb.m_failed && checkIfShouldThrow(at))
            res |= assertAction::shouldthrow;

#ifdef DOCTEST_CONFIG_SUPER_FAST_ASSERTS
        // #########################################################################################
        // IF THE DEBUGGER BREAKS HERE - GO 1 LEVEL UP IN THE CALLSTACK TO SEE THE FAILING ASSERTION
        // THIS IS THE EFFECT OF HAVING 'DOCTEST_CONFIG_SUPER_FAST_ASSERTS' DEFINED
        // #########################################################################################
        if(res & assertAction::dbgbreak)
            DOCTEST_BREAK_INTO_DEBUGGER();
        fastAssertThrowIfFlagSet(res);
#endif // DOCTEST_CONFIG_SUPER_FAST_ASSERTS

        return res;
    }
    #if defined(__BORLANDC__)
#pragma option pop
#endif

    struct DOCTEST_INTERFACE IExceptionTranslator
    {
        virtual ~IExceptionTranslator() {}
        virtual bool translate(String&) const = 0;
    };

    template <typename T>
    class ExceptionTranslator : public IExceptionTranslator //!OCLINT destructor of virtual class
    {
    public:
        explicit ExceptionTranslator(String (*translateFunction)(T))
                : m_translateFunction(translateFunction) {}

        bool translate(String& res) const {
#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
            try {
                throw;
                // cppcheck-suppress catchExceptionByValue
            } catch(T ex) {                    // NOLINT
                res = m_translateFunction(ex); //!OCLINT parameter reassignment
                return true;
            } catch(...) {} //!OCLINT -  empty catch statement
#endif                      // DOCTEST_CONFIG_NO_EXCEPTIONS
            ((void)res);    // to silence -Wunused-parameter
            return false;
        }

    protected:
        String (*m_translateFunction)(T);
    };

    DOCTEST_INTERFACE void registerExceptionTranslatorImpl(
            const IExceptionTranslator* translateFunction);

    // FIX FOR VISUAL STUDIO VERSIONS PRIOR TO 2015 - they failed to compile the call to operator<< with
    // std::ostream passed as a reference noting that there is a use of an undefined type (which there isn't)
    DOCTEST_INTERFACE void writeStringToStream(std::ostream* s, const String& str);

    template <bool C>
    struct StringStreamBase
    {
        template <typename T>
        static void convert(std::ostream* s, const T& in) {
            writeStringToStream(s, toString(in));
        }

        // always treat char* as a string in this context - no matter
        // if DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING is defined
        static void convert(std::ostream* s, const char* in) { writeStringToStream(s, String(in)); }
    };

    template <>
    struct StringStreamBase<true>
    {
        template <typename T>
        static void convert(std::ostream* s, const T& in) {
            *s << in;
        }
    };

    template <typename T>
    struct StringStream : StringStreamBase<has_insertion_operator<T>::value>
    {};

    template <typename T>
    void toStream(std::ostream* s, const T& value) {
        StringStream<T>::convert(s, value);
    }

#ifdef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
    DOCTEST_INTERFACE void toStream(std::ostream* s, char* in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, const char* in);
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
    DOCTEST_INTERFACE void toStream(std::ostream* s, bool in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, float in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, double in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, double long in);

    DOCTEST_INTERFACE void toStream(std::ostream* s, char in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, char signed in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, char unsigned in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int short in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int short unsigned in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int unsigned in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int long in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int long unsigned in);

#ifdef DOCTEST_CONFIG_WITH_LONG_LONG
    DOCTEST_INTERFACE void toStream(std::ostream* s, int long long in);
    DOCTEST_INTERFACE void toStream(std::ostream* s, int long long unsigned in);
#endif // DOCTEST_CONFIG_WITH_LONG_LONG

    struct IContextScope
    {
        virtual ~IContextScope() {}
        virtual void build(std::ostream*) = 0;
    };

    DOCTEST_INTERFACE void addToContexts(IContextScope* ptr);
    DOCTEST_INTERFACE void popFromContexts();
    DOCTEST_INTERFACE void useContextIfExceptionOccurred(IContextScope* ptr);

    // cppcheck-suppress copyCtorAndEqOperator
    class ContextBuilder
    {
        friend class ContextScope;

        struct ICapture
        {
            virtual ~ICapture() {}
            virtual void toStream(std::ostream*) const = 0;
        };

        template <typename T>
        struct Capture : ICapture //!OCLINT destructor of virtual class
        {
            const T* capture;

            explicit Capture(const T* in)
                    : capture(in) {}
            virtual void toStream(std::ostream* s) const { // override
                detail::toStream(s, *capture);
            }
        };

        struct Chunk
        {
            char buf[sizeof(Capture<char>)] DOCTEST_ALIGNMENT(
                    2 * sizeof(void*)); // place to construct a Capture<T>
        };

        struct Node
        {
            Chunk chunk;
            Node* next;
        };

        Chunk stackChunks[DOCTEST_CONFIG_NUM_CAPTURES_ON_STACK];
        int   numCaptures;
        Node* head;
        Node* tail;

        DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wcast-align")
        void build(std::ostream* s) const {
            int curr = 0;
            // iterate over small buffer
            while(curr < numCaptures && curr < DOCTEST_CONFIG_NUM_CAPTURES_ON_STACK)
                reinterpret_cast<const ICapture*>(stackChunks[curr++].buf)->toStream(s);
            // iterate over list
            Node* curr_elem = head;
            while(curr < numCaptures) {
                reinterpret_cast<const ICapture*>(curr_elem->chunk.buf)->toStream(s);
                curr_elem = curr_elem->next;
                ++curr;
            }
        }
        DOCTEST_GCC_SUPPRESS_WARNING_POP

        #if defined(__BORLANDC__)
#pragma option push
#pragma option -vi-
#endif

        // steal the contents of the other - acting as a move constructor...
        DOCTEST_NOINLINE ContextBuilder(ContextBuilder& other)
                : numCaptures(other.numCaptures)
                , head(other.head)
                , tail(other.tail) {
            other.numCaptures = 0;
            other.head        = 0;
            other.tail        = 0;
            my_memcpy(stackChunks, other.stackChunks,
                      unsigned(int(sizeof(Chunk)) * DOCTEST_CONFIG_NUM_CAPTURES_ON_STACK));
        }
        #if defined(__BORLANDC__)
#pragma option pop
#endif

        ContextBuilder& operator=(const ContextBuilder&); // NOLINT

    public:

        #if defined(__BORLANDC__)
#pragma option push
#pragma option -vi-
#endif

        // cppcheck-suppress uninitMemberVar
        DOCTEST_NOINLINE ContextBuilder() // NOLINT
                : numCaptures(0)
                , head(0)
                , tail(0) {}

        template <typename T>
        DOCTEST_NOINLINE ContextBuilder& operator<<(T& in) {
            Capture<T> temp(&in);

            // construct either on stack or on heap
            // copy the bytes for the whole object - including the vtable because we cant construct
            // the object directly in the buffer using placement new - need the <new> header...
            if(numCaptures < DOCTEST_CONFIG_NUM_CAPTURES_ON_STACK) {
                my_memcpy(stackChunks[numCaptures].buf, &temp, sizeof(Chunk));
            } else {
                Node* curr = new Node;
                curr->next = 0;
                if(tail) {
                    tail->next = curr;
                    tail       = curr;
                } else {
                    head = tail = curr;
                }

                my_memcpy(tail->chunk.buf, &temp, sizeof(Chunk));
            }
            ++numCaptures;
            return *this;
        }

        DOCTEST_NOINLINE ~ContextBuilder() {
            // free the linked list - the ones on the stack are left as-is
            // no destructors are called at all - there is no need
            while(head) {
                Node* next = head->next;
                delete head;
                head = next;
            }
        }

        #if defined(__BORLANDC__)
#pragma option pop
#endif

#ifdef DOCTEST_CONFIG_WITH_RVALUE_REFERENCES
        template <typename T>
        ContextBuilder& operator<<(const T&&) {
            DOCTEST_STATIC_ASSERT(
                    deferred_false<T>::value,
                    Cannot_pass_temporaries_or_rvalues_to_the_streaming_operator_because_it_caches_pointers_to_the_passed_objects_for_lazy_evaluation);
            return *this;
        }
#endif // DOCTEST_CONFIG_WITH_RVALUE_REFERENCES
    };

    class ContextScope : public IContextScope
    {
        ContextBuilder contextBuilder;

    public:
        #if defined(__BORLANDC__)
#pragma option push
#pragma option -vi-
#endif

        DOCTEST_NOINLINE explicit ContextScope(ContextBuilder& temp)
                : contextBuilder(temp) {
            addToContexts(this);
        }

        DOCTEST_NOINLINE ~ContextScope() {
            useContextIfExceptionOccurred(this);
            popFromContexts();
        }

        #if defined(__BORLANDC__)
#pragma option pop
#endif

        void build(std::ostream* s) { contextBuilder.build(s); }
    };

    class DOCTEST_INTERFACE MessageBuilder
    {
        std::ostream*    m_stream;
        const char*      m_file;
        int              m_line;
        assertType::Enum m_severity;

    public:
        MessageBuilder(const char* file, int line, assertType::Enum severity);
        ~MessageBuilder();

        template <typename T>
        MessageBuilder& operator<<(const T& in) {
            toStream(m_stream, in);
            return *this;
        }

        void log(std::ostream&);
        bool log();
        void react();
    };
} // namespace detail

struct test_suite
{
    const char* data;
    test_suite(const char* in)
            : data(in) {}
    void fill(detail::TestCase& state) const { state.m_test_suite = data; }
    void fill(detail::TestSuite& state) const { state.m_test_suite = data; }
};

struct description
{
    const char* data;
    description(const char* in)
            : data(in) {}
    void fill(detail::TestCase& state) const { state.m_description = data; }
    void fill(detail::TestSuite& state) const { state.m_description = data; }
};

struct skip
{
    bool data;
    skip(bool in = true)
            : data(in) {}
    void fill(detail::TestCase& state) const { state.m_skip = data; }
    void fill(detail::TestSuite& state) const { state.m_skip = data; }
};

struct timeout
{
    double data;
    timeout(double in)
            : data(in) {}
    void fill(detail::TestCase& state) const { state.m_timeout = data; }
    void fill(detail::TestSuite& state) const { state.m_timeout = data; }
};

struct may_fail
{
    bool data;
    may_fail(bool in = true)
            : data(in) {}
    void fill(detail::TestCase& state) const { state.m_may_fail = data; }
    void fill(detail::TestSuite& state) const { state.m_may_fail = data; }
};

struct should_fail
{
    bool data;
    should_fail(bool in = true)
            : data(in) {}
    void fill(detail::TestCase& state) const { state.m_should_fail = data; }
    void fill(detail::TestSuite& state) const { state.m_should_fail = data; }
};

struct expected_failures
{
    int data;
    expected_failures(int in)
            : data(in) {}
    void fill(detail::TestCase& state) const { state.m_expected_failures = data; }
    void fill(detail::TestSuite& state) const { state.m_expected_failures = data; }
};

#endif // DOCTEST_CONFIG_DISABLE

#ifndef DOCTEST_CONFIG_DISABLE
template <typename T>
int registerExceptionTranslator(String (*translateFunction)(T)) {
    DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wexit-time-destructors")
    static detail::ExceptionTranslator<T> exceptionTranslator(translateFunction);
    DOCTEST_CLANG_SUPPRESS_WARNING_POP
    detail::registerExceptionTranslatorImpl(&exceptionTranslator);
    return 0;
}

#else  // DOCTEST_CONFIG_DISABLE
template <typename T>
int registerExceptionTranslator(String (*)(T)) {
    return 0;
}
#endif // DOCTEST_CONFIG_DISABLE

DOCTEST_INTERFACE bool isRunningInTest();

// cppcheck-suppress noCopyConstructor
class DOCTEST_INTERFACE Context
{
#if !defined(DOCTEST_CONFIG_DISABLE)
    detail::ContextState* p;

    void parseArgs(int argc, const char* const* argv, bool withDefaults = false);

#endif // DOCTEST_CONFIG_DISABLE

public:
    explicit Context(int argc = 0, const char* const* argv = 0);

    ~Context();

    void applyCommandLine(int argc, const char* const* argv);

    void addFilter(const char* filter, const char* value);
    void clearFilters();
    void setOption(const char* option, int value);
    void setOption(const char* option, const char* value);

    bool shouldExit();

    int run();
};

} // namespace doctest


// if registering is not disabled
#if !defined(DOCTEST_CONFIG_DISABLE)

#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_EXPAND_VA_ARGS(...) __VA_ARGS__
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_EXPAND_VA_ARGS
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

#define DOCTEST_STRIP_PARENS(x) x
#define DOCTEST_HANDLE_BRACED_VA_ARGS(expr) DOCTEST_STRIP_PARENS(DOCTEST_EXPAND_VA_ARGS expr)

// registers the test by initializing a dummy var with a function
#define DOCTEST_REGISTER_FUNCTION(f, decorators)                                                   \
    DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_VAR_)) = doctest::detail::regTest(  \
            doctest::detail::TestCase(f, __FILE__, __LINE__,                                       \
                                      doctest_detail_test_suite_ns::getCurrentTestSuite()) *       \
            decorators);                                                                           \
    DOCTEST_GLOBAL_NO_WARNINGS_END()

#define DOCTEST_IMPLEMENT_FIXTURE(der, base, func, decorators)                                     \
    namespace                                                                                      \
    {                                                                                              \
        struct der : base                                                                          \
        {                                                                                          \
            void f();                                                                              \
        };                                                                                         \
        static void func() {                                                                       \
            der v;                                                                                 \
            v.f();                                                                                 \
        }                                                                                          \
        DOCTEST_REGISTER_FUNCTION(func, decorators)                                                \
    }                                                                                              \                                                                   \
    inline DOCTEST_NOINLINE void der::f()                                                          

#define DOCTEST_CREATE_AND_REGISTER_FUNCTION(f, decorators)                                        \
    static void f();                                                                               \
    DOCTEST_REGISTER_FUNCTION(f, decorators)                                                       \
    static void f()

// for registering tests
#define DOCTEST_TEST_CASE(decorators)                                                              \
    DOCTEST_CREATE_AND_REGISTER_FUNCTION(DOCTEST_ANONYMOUS(_DOCTEST_ANON_FUNC_), decorators)

// for registering tests with a fixture
#define DOCTEST_TEST_CASE_FIXTURE(c, decorators)                                                   \
    DOCTEST_IMPLEMENT_FIXTURE(DOCTEST_ANONYMOUS(_DOCTEST_ANON_CLASS_), c,                          \
                              DOCTEST_ANONYMOUS(_DOCTEST_ANON_FUNC_), decorators)

// for converting types to strings without the <typeinfo> header and demangling
#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_TYPE_TO_STRING_IMPL(...)                                                           \
    template <>                                                                                    \
    inline const char* type_to_string<__VA_ARGS__>() {                                             \
        return "<" #__VA_ARGS__ ">";                                                               \
    }
#define DOCTEST_TYPE_TO_STRING(...)                                                                \
    namespace doctest                                                                              \
    {                                                                                              \
        namespace detail                                                                           \
        {                                                                                          \
            DOCTEST_TYPE_TO_STRING_IMPL(__VA_ARGS__)                                               \
        }                                                                                          \
    }                                                                                              \
    typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_TYPE_TO_STRING_IMPL(x)                                                             \
    template <>                                                                                    \
    inline const char* type_to_string<x>() {                                                       \
        return "<" #x ">";                                                                         \
    }
#define DOCTEST_TYPE_TO_STRING(x)                                                                  \
    namespace doctest                                                                              \
    {                                                                                              \
        namespace detail                                                                           \
        {                                                                                          \
            DOCTEST_TYPE_TO_STRING_IMPL(x)                                                         \
        }                                                                                          \
    }                                                                                              \
    typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

// for typed tests
#define DOCTEST_TEST_CASE_TEMPLATE_IMPL(decorators, T, types, anon)                                \
    template <typename T>                                                                          \
    inline void anon();                                                                            \
    struct DOCTEST_CAT(anon, FUNCTOR)                                                              \
    {                                                                                              \
        template <int Index, typename Type>                                                        \
        void operator()() {                                                                        \
            doctest::detail::regTest(                                                              \
                    doctest::detail::TestCase(anon<Type>, __FILE__, __LINE__,                      \
                                              doctest_detail_test_suite_ns::getCurrentTestSuite(), \
                                              doctest::detail::type_to_string<Type>(), Index) *    \
                    decorators);                                                                   \
        }                                                                                          \
    };                                                                                             \
    inline int DOCTEST_CAT(anon, REG_FUNC)() {                                                     \
        DOCTEST_CAT(anon, FUNCTOR) registrar;                                                      \
        doctest::detail::ForEachType<DOCTEST_HANDLE_BRACED_VA_ARGS(types)::Result,                 \
                                     DOCTEST_CAT(anon, FUNCTOR)>                                   \
                doIt(registrar);                                                                   \
        return 0;                                                                                  \
    }                                                                                              \
    DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_CAT(anon, DUMMY)) = DOCTEST_CAT(anon, REG_FUNC)();          \
    DOCTEST_GLOBAL_NO_WARNINGS_END()                                                               \
    template <typename T>                                                                          \
    inline void anon()

#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_TEST_CASE_TEMPLATE(decorators, T, ...)                                             \
    DOCTEST_TEST_CASE_TEMPLATE_IMPL(decorators, T, (__VA_ARGS__),                                  \
                                    DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_))
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_TEST_CASE_TEMPLATE(decorators, T, types)                                           \
    DOCTEST_TEST_CASE_TEMPLATE_IMPL(decorators, T, types, DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_))
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

#define DOCTEST_TEST_CASE_TEMPLATE_DEFINE_IMPL(decorators, T, id, anon)                            \
    template <typename T>                                                                          \
    inline void anon();                                                                            \
    struct DOCTEST_CAT(id, _FUNCTOR)                                                               \
    {                                                                                              \
        int m_line;                                                                                \
        DOCTEST_CAT(id, _FUNCTOR)                                                                  \
        (int line)                                                                                 \
                : m_line(line) {}                                                                  \
        template <int Index, typename Type>                                                        \
        void operator()() {                                                                        \
            doctest::detail::regTest(                                                              \
                    doctest::detail::TestCase(anon<Type>, __FILE__, __LINE__,                      \
                                              doctest_detail_test_suite_ns::getCurrentTestSuite(), \
                                              doctest::detail::type_to_string<Type>(),             \
                                              m_line * 1000 + Index) *                             \
                    decorators);                                                                   \
        }                                                                                          \
    };                                                                                             \
    template <typename T>                                                                          \
    inline void anon()

#define DOCTEST_TEST_CASE_TEMPLATE_DEFINE(decorators, T, id)                                       \
    DOCTEST_TEST_CASE_TEMPLATE_DEFINE_IMPL(decorators, T, id, DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_))

#define DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE_IMPL(id, types, anon)                               \
    static int DOCTEST_CAT(anon, REG_FUNC)() {                                                     \
        DOCTEST_CAT(id, _FUNCTOR) registrar(__LINE__);                                             \
        doctest::detail::ForEachType<DOCTEST_HANDLE_BRACED_VA_ARGS(types)::Result,                 \
                                     DOCTEST_CAT(id, _FUNCTOR)>                                    \
                doIt(registrar);                                                                   \
        return 0;                                                                                  \
    }                                                                                              \
    DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_CAT(anon, DUMMY)) = DOCTEST_CAT(anon, REG_FUNC)();          \
    DOCTEST_GLOBAL_NO_WARNINGS_END() typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE(id, ...)                                            \
    DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE_IMPL(id, (__VA_ARGS__),                                 \
                                                DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_))
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE(id, types)                                          \
    DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE_IMPL(id, types, DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_))
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

// for subcases
#define DOCTEST_SUBCASE(name)                                                                      \
    if(const doctest::detail::Subcase & DOCTEST_ANONYMOUS(_DOCTEST_ANON_SUBCASE_) DOCTEST_UNUSED = \
               doctest::detail::Subcase(name, __FILE__, __LINE__))

// for grouping tests in test suites by using code blocks
#define DOCTEST_TEST_SUITE_IMPL(decorators, ns_name)                                               \
    namespace ns_name                                                                              \
    {                                                                                              \
        namespace doctest_detail_test_suite_ns                                                     \
        {                                                                                          \                                                                \
            static DOCTEST_NOINLINE doctest::detail::TestSuite& getCurrentTestSuite() {            \
                static doctest::detail::TestSuite data;                                            \
                static bool                       inited = false;                                  \
                if(!inited) {                                                                      \
                    data* decorators;                                                              \
                    inited = true;                                                                 \
                }                                                                                  \
                return data;                                                                       \
            }                                                                                      \                                                                  \
        }                                                                                          \
    }                                                                                              \
    namespace ns_name

#define DOCTEST_TEST_SUITE(decorators)                                                             \
    DOCTEST_TEST_SUITE_IMPL(decorators, DOCTEST_ANONYMOUS(_DOCTEST_ANON_SUITE_))

// for starting a testsuite block
#define DOCTEST_TEST_SUITE_BEGIN(decorators)                                                       \
    DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_VAR_)) =                            \
            doctest::detail::setTestSuite(doctest::detail::TestSuite() * decorators);              \
    DOCTEST_GLOBAL_NO_WARNINGS_END()                                                               \
    typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

// for ending a testsuite block
#define DOCTEST_TEST_SUITE_END                                                                     \
    DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_VAR_)) =                            \
            doctest::detail::setTestSuite(doctest::detail::TestSuite() * "");                      \
    DOCTEST_GLOBAL_NO_WARNINGS_END()                                                               \
    typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

// for registering exception translators
#define DOCTEST_REGISTER_EXCEPTION_TRANSLATOR_IMPL(translatorName, signature)                      \
    inline doctest::String translatorName(signature);                                              \
    DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_TRANSLATOR_)) =                     \
            doctest::registerExceptionTranslator(translatorName);                                  \
    DOCTEST_GLOBAL_NO_WARNINGS_END()                                                               \
    doctest::String translatorName(signature)

#define DOCTEST_REGISTER_EXCEPTION_TRANSLATOR(signature)                                           \
    DOCTEST_REGISTER_EXCEPTION_TRANSLATOR_IMPL(DOCTEST_ANONYMOUS(_DOCTEST_ANON_TRANSLATOR_),       \
                                               signature)

// for logging
#define DOCTEST_INFO(x)                                                                            \
    doctest::detail::ContextScope DOCTEST_ANONYMOUS(_DOCTEST_CAPTURE_)(                            \
            doctest::detail::ContextBuilder() << x)
#define DOCTEST_CAPTURE(x) DOCTEST_INFO(#x " := " << x)

#define DOCTEST_ADD_AT_IMPL(type, file, line, mb, x)                                               \
    do {                                                                                           \
        doctest::detail::MessageBuilder mb(file, line, doctest::detail::assertType::type);         \
        mb << x;                                                                                   \
        if(mb.log())                                                                               \
            DOCTEST_BREAK_INTO_DEBUGGER();                                                         \
        mb.react();                                                                                \
    } while((void)0, 0)

// clang-format off
#define DOCTEST_ADD_MESSAGE_AT(file, line, x) DOCTEST_ADD_AT_IMPL(is_warn, file, line, DOCTEST_ANONYMOUS(_DOCTEST_MESSAGE_), x)
#define DOCTEST_ADD_FAIL_CHECK_AT(file, line, x) DOCTEST_ADD_AT_IMPL(is_check, file, line, DOCTEST_ANONYMOUS(_DOCTEST_MESSAGE_), x)
#define DOCTEST_ADD_FAIL_AT(file, line, x) DOCTEST_ADD_AT_IMPL(is_require, file, line, DOCTEST_ANONYMOUS(_DOCTEST_MESSAGE_), x)
// clang-format on

#define DOCTEST_MESSAGE(x) DOCTEST_ADD_MESSAGE_AT(__FILE__, __LINE__, x)
#define DOCTEST_FAIL_CHECK(x) DOCTEST_ADD_FAIL_CHECK_AT(__FILE__, __LINE__, x)
#define DOCTEST_FAIL(x) DOCTEST_ADD_FAIL_AT(__FILE__, __LINE__, x)

#if __cplusplus >= 201402L || (DOCTEST_MSVC >= DOCTEST_COMPILER(19, 10, 0))
template <class T, T x>
constexpr T to_lvalue = x;
#define DOCTEST_TO_LVALUE(...) to_lvalue<decltype(__VA_ARGS__), __VA_ARGS__>
#else
#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_TO_LVALUE(...) TO_LVALUE_CAN_BE_USED_ONLY_IN_CPP14_MODE_OR_WITH_VS_2017_OR_NEWER
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_TO_LVALUE(x) TO_LVALUE_CAN_BE_USED_ONLY_IN_CPP14_MODE_OR_WITH_VS_2017_OR_NEWER
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#endif // TO_LVALUE hack for logging macros like INFO()

// common code in asserts - for convenience
#define DOCTEST_ASSERT_LOG_AND_REACT(rb)                                                           \
    if(rb.log())                                                                                   \
        DOCTEST_BREAK_INTO_DEBUGGER();                                                             \
    rb.react()

#ifdef DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS
#define DOCTEST_WRAP_IN_TRY(x) x;
#else // DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS
#define DOCTEST_WRAP_IN_TRY(x)                                                                     \
    try {                                                                                          \
        x;                                                                                         \
    } catch(...) { _DOCTEST_RB.unexpectedExceptionOccurred(); }
#endif // DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS

#define DOCTEST_ASSERT_IMPLEMENT_2(expr, assert_type)                                              \
    DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Woverloaded-shift-op-parentheses")                  \
    doctest::detail::ResultBuilder _DOCTEST_RB(                                                    \
            doctest::detail::assertType::assert_type, __FILE__, __LINE__,                          \
            DOCTEST_TOSTR(DOCTEST_HANDLE_BRACED_VA_ARGS(expr)));                                   \
    DOCTEST_WRAP_IN_TRY(_DOCTEST_RB.setResult(                                                     \
            doctest::detail::ExpressionDecomposer(doctest::detail::assertType::assert_type)        \
            << DOCTEST_HANDLE_BRACED_VA_ARGS(expr)))                                               \
    DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB)                                                      \
    DOCTEST_CLANG_SUPPRESS_WARNING_POP

#define DOCTEST_ASSERT_IMPLEMENT_1(expr, assert_type)                                              \
    do {                                                                                           \
        DOCTEST_ASSERT_IMPLEMENT_2(expr, assert_type);                                             \
    } while((void)0, 0)

#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN(...) DOCTEST_ASSERT_IMPLEMENT_1((__VA_ARGS__), DT_WARN)
#define DOCTEST_CHECK(...) DOCTEST_ASSERT_IMPLEMENT_1((__VA_ARGS__), DT_CHECK)
#define DOCTEST_REQUIRE(...) DOCTEST_ASSERT_IMPLEMENT_1((__VA_ARGS__), DT_REQUIRE)
#define DOCTEST_WARN_FALSE(...) DOCTEST_ASSERT_IMPLEMENT_1((__VA_ARGS__), DT_WARN_FALSE)
#define DOCTEST_CHECK_FALSE(...) DOCTEST_ASSERT_IMPLEMENT_1((__VA_ARGS__), DT_CHECK_FALSE)
#define DOCTEST_REQUIRE_FALSE(...) DOCTEST_ASSERT_IMPLEMENT_1((__VA_ARGS__), DT_REQUIRE_FALSE)
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN(expr) DOCTEST_ASSERT_IMPLEMENT_1(expr, DT_WARN)
#define DOCTEST_CHECK(expr) DOCTEST_ASSERT_IMPLEMENT_1(expr, DT_CHECK)
#define DOCTEST_REQUIRE(expr) DOCTEST_ASSERT_IMPLEMENT_1(expr, DT_REQUIRE)
#define DOCTEST_WARN_FALSE(expr) DOCTEST_ASSERT_IMPLEMENT_1(expr, DT_WARN_FALSE)
#define DOCTEST_CHECK_FALSE(expr) DOCTEST_ASSERT_IMPLEMENT_1(expr, DT_CHECK_FALSE)
#define DOCTEST_REQUIRE_FALSE(expr) DOCTEST_ASSERT_IMPLEMENT_1(expr, DT_REQUIRE_FALSE)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

// clang-format off
#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2((cond), DT_WARN); } while((void)0, 0)
#define DOCTEST_CHECK_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2((cond), DT_CHECK); } while((void)0, 0)
#define DOCTEST_REQUIRE_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2((cond), DT_REQUIRE); } while((void)0, 0)
#define DOCTEST_WARN_FALSE_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2((cond), DT_WARN_FALSE); } while((void)0, 0)
#define DOCTEST_CHECK_FALSE_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2((cond), DT_CHECK_FALSE); } while((void)0, 0)
#define DOCTEST_REQUIRE_FALSE_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2((cond), DT_REQUIRE_FALSE); } while((void)0, 0)
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2(cond, DT_WARN); } while((void)0, 0)
#define DOCTEST_CHECK_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2(cond, DT_CHECK); } while((void)0, 0)
#define DOCTEST_REQUIRE_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2(cond, DT_REQUIRE); } while((void)0, 0)
#define DOCTEST_WARN_FALSE_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2(cond, DT_WARN_FALSE); } while((void)0, 0)
#define DOCTEST_CHECK_FALSE_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2(cond, DT_CHECK_FALSE); } while((void)0, 0)
#define DOCTEST_REQUIRE_FALSE_MESSAGE(cond, msg) do { DOCTEST_INFO(msg); DOCTEST_ASSERT_IMPLEMENT_2(cond, DT_REQUIRE_FALSE); } while((void)0, 0)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
// clang-format on

#define DOCTEST_ASSERT_THROWS(expr, assert_type)                                                   \
    do {                                                                                           \
        if(!doctest::detail::getTestsContextState()->no_throw) {                                   \
            doctest::detail::ResultBuilder _DOCTEST_RB(doctest::detail::assertType::assert_type,   \
                                                       __FILE__, __LINE__, #expr);                 \
            try {                                                                                  \
                expr;                                                                              \
            } catch(...) { _DOCTEST_RB.m_threw = true; }                                           \
            DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB);                                             \
        }                                                                                          \
    } while((void)0, 0)

#define DOCTEST_ASSERT_THROWS_AS(expr, as, assert_type)                                            \
    do {                                                                                           \
        if(!doctest::detail::getTestsContextState()->no_throw) {                                   \
            doctest::detail::ResultBuilder _DOCTEST_RB(                                            \
                    doctest::detail::assertType::assert_type, __FILE__, __LINE__, #expr,           \
                    DOCTEST_TOSTR(DOCTEST_HANDLE_BRACED_VA_ARGS(as)));                             \
            try {                                                                                  \
                expr;                                                                              \
            } catch(const DOCTEST_HANDLE_BRACED_VA_ARGS(as)&) {                                    \
                _DOCTEST_RB.m_threw    = true;                                                     \
                _DOCTEST_RB.m_threw_as = true;                                                     \
            } catch(...) { _DOCTEST_RB.unexpectedExceptionOccurred(); }                            \
            DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB);                                             \
        }                                                                                          \
    } while((void)0, 0)

#define DOCTEST_ASSERT_NOTHROW(expr, assert_type)                                                  \
    do {                                                                                           \
        if(!doctest::detail::getTestsContextState()->no_throw) {                                   \
            doctest::detail::ResultBuilder _DOCTEST_RB(doctest::detail::assertType::assert_type,   \
                                                       __FILE__, __LINE__, #expr);                 \
            try {                                                                                  \
                expr;                                                                              \
            } catch(...) { _DOCTEST_RB.unexpectedExceptionOccurred(); }                            \
            DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB);                                             \
        }                                                                                          \
    } while((void)0, 0)

#define DOCTEST_WARN_THROWS(expr) DOCTEST_ASSERT_THROWS(expr, DT_WARN_THROWS)
#define DOCTEST_CHECK_THROWS(expr) DOCTEST_ASSERT_THROWS(expr, DT_CHECK_THROWS)
#define DOCTEST_REQUIRE_THROWS(expr) DOCTEST_ASSERT_THROWS(expr, DT_REQUIRE_THROWS)

// clang-format off
#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN_THROWS_AS(expr, ...) DOCTEST_ASSERT_THROWS_AS(expr, (__VA_ARGS__), DT_WARN_THROWS_AS)
#define DOCTEST_CHECK_THROWS_AS(expr, ...) DOCTEST_ASSERT_THROWS_AS(expr, (__VA_ARGS__), DT_CHECK_THROWS_AS)
#define DOCTEST_REQUIRE_THROWS_AS(expr, ...) DOCTEST_ASSERT_THROWS_AS(expr, (__VA_ARGS__), DT_REQUIRE_THROWS_AS)
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN_THROWS_AS(expr, ex) DOCTEST_ASSERT_THROWS_AS(expr, ex, DT_WARN_THROWS_AS)
#define DOCTEST_CHECK_THROWS_AS(expr, ex) DOCTEST_ASSERT_THROWS_AS(expr, ex, DT_CHECK_THROWS_AS)
#define DOCTEST_REQUIRE_THROWS_AS(expr, ex) DOCTEST_ASSERT_THROWS_AS(expr, ex, DT_REQUIRE_THROWS_AS)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
// clang-format on

#define DOCTEST_WARN_NOTHROW(expr) DOCTEST_ASSERT_NOTHROW(expr, DT_WARN_NOTHROW)
#define DOCTEST_CHECK_NOTHROW(expr) DOCTEST_ASSERT_NOTHROW(expr, DT_CHECK_NOTHROW)
#define DOCTEST_REQUIRE_NOTHROW(expr) DOCTEST_ASSERT_NOTHROW(expr, DT_REQUIRE_NOTHROW)

// clang-format off
#define DOCTEST_WARN_THROWS_MESSAGE(expr, msg) do { DOCTEST_INFO(msg); DOCTEST_WARN_THROWS(expr); } while((void)0, 0)
#define DOCTEST_CHECK_THROWS_MESSAGE(expr, msg) do { DOCTEST_INFO(msg); DOCTEST_CHECK_THROWS(expr); } while((void)0, 0)
#define DOCTEST_REQUIRE_THROWS_MESSAGE(expr, msg) do { DOCTEST_INFO(msg); DOCTEST_REQUIRE_THROWS(expr); } while((void)0, 0)
#define DOCTEST_WARN_THROWS_AS_MESSAGE(expr, ex, msg) do { DOCTEST_INFO(msg); DOCTEST_WARN_THROWS_AS(expr, ex); } while((void)0, 0)
#define DOCTEST_CHECK_THROWS_AS_MESSAGE(expr, ex, msg) do { DOCTEST_INFO(msg); DOCTEST_CHECK_THROWS_AS(expr, ex); } while((void)0, 0)
#define DOCTEST_REQUIRE_THROWS_AS_MESSAGE(expr, ex, msg) do { DOCTEST_INFO(msg); DOCTEST_REQUIRE_THROWS_AS(expr, ex); } while((void)0, 0)
#define DOCTEST_WARN_NOTHROW_MESSAGE(expr, msg) do { DOCTEST_INFO(msg); DOCTEST_WARN_NOTHROW(expr); } while((void)0, 0)
#define DOCTEST_CHECK_NOTHROW_MESSAGE(expr, msg) do { DOCTEST_INFO(msg); DOCTEST_CHECK_NOTHROW(expr); } while((void)0, 0)
#define DOCTEST_REQUIRE_NOTHROW_MESSAGE(expr, msg) do { DOCTEST_INFO(msg); DOCTEST_REQUIRE_NOTHROW(expr); } while((void)0, 0)
// clang-format on

#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_BINARY_ASSERT(assert_type, expr, comp)                                             \
    do {                                                                                           \
        doctest::detail::ResultBuilder _DOCTEST_RB(                                                \
                doctest::detail::assertType::assert_type, __FILE__, __LINE__,                      \
                DOCTEST_TOSTR(DOCTEST_HANDLE_BRACED_VA_ARGS(expr)));                               \
        DOCTEST_WRAP_IN_TRY(                                                                       \
                _DOCTEST_RB.binary_assert<doctest::detail::binaryAssertComparison::comp>(          \
                        DOCTEST_HANDLE_BRACED_VA_ARGS(expr)))                                      \
        DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB);                                                 \
    } while((void)0, 0)
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_BINARY_ASSERT(assert_type, lhs, rhs, comp)                                         \
    do {                                                                                           \
        doctest::detail::ResultBuilder _DOCTEST_RB(doctest::detail::assertType::assert_type,       \
                                                   __FILE__, __LINE__, #lhs ", " #rhs);            \
        DOCTEST_WRAP_IN_TRY(                                                                       \
                _DOCTEST_RB.binary_assert<doctest::detail::binaryAssertComparison::comp>(lhs,      \
                                                                                         rhs))     \
        DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB);                                                 \
    } while((void)0, 0)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

#define DOCTEST_UNARY_ASSERT(assert_type, expr)                                                    \
    do {                                                                                           \
        doctest::detail::ResultBuilder _DOCTEST_RB(                                                \
                doctest::detail::assertType::assert_type, __FILE__, __LINE__,                      \
                DOCTEST_TOSTR(DOCTEST_HANDLE_BRACED_VA_ARGS(expr)));                               \
        DOCTEST_WRAP_IN_TRY(_DOCTEST_RB.unary_assert(DOCTEST_HANDLE_BRACED_VA_ARGS(expr)))         \
        DOCTEST_ASSERT_LOG_AND_REACT(_DOCTEST_RB);                                                 \
    } while((void)0, 0)

#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN_EQ(...) DOCTEST_BINARY_ASSERT(DT_WARN_EQ, (__VA_ARGS__), eq)
#define DOCTEST_CHECK_EQ(...) DOCTEST_BINARY_ASSERT(DT_CHECK_EQ, (__VA_ARGS__), eq)
#define DOCTEST_REQUIRE_EQ(...) DOCTEST_BINARY_ASSERT(DT_REQUIRE_EQ, (__VA_ARGS__), eq)
#define DOCTEST_WARN_NE(...) DOCTEST_BINARY_ASSERT(DT_WARN_NE, (__VA_ARGS__), ne)
#define DOCTEST_CHECK_NE(...) DOCTEST_BINARY_ASSERT(DT_CHECK_NE, (__VA_ARGS__), ne)
#define DOCTEST_REQUIRE_NE(...) DOCTEST_BINARY_ASSERT(DT_REQUIRE_NE, (__VA_ARGS__), ne)
#define DOCTEST_WARN_GT(...) DOCTEST_BINARY_ASSERT(DT_WARN_GT, (__VA_ARGS__), gt)
#define DOCTEST_CHECK_GT(...) DOCTEST_BINARY_ASSERT(DT_CHECK_GT, (__VA_ARGS__), gt)
#define DOCTEST_REQUIRE_GT(...) DOCTEST_BINARY_ASSERT(DT_REQUIRE_GT, (__VA_ARGS__), gt)
#define DOCTEST_WARN_LT(...) DOCTEST_BINARY_ASSERT(DT_WARN_LT, (__VA_ARGS__), lt)
#define DOCTEST_CHECK_LT(...) DOCTEST_BINARY_ASSERT(DT_CHECK_LT, (__VA_ARGS__), lt)
#define DOCTEST_REQUIRE_LT(...) DOCTEST_BINARY_ASSERT(DT_REQUIRE_LT, (__VA_ARGS__), lt)
#define DOCTEST_WARN_GE(...) DOCTEST_BINARY_ASSERT(DT_WARN_GE, (__VA_ARGS__), ge)
#define DOCTEST_CHECK_GE(...) DOCTEST_BINARY_ASSERT(DT_CHECK_GE, (__VA_ARGS__), ge)
#define DOCTEST_REQUIRE_GE(...) DOCTEST_BINARY_ASSERT(DT_REQUIRE_GE, (__VA_ARGS__), ge)
#define DOCTEST_WARN_LE(...) DOCTEST_BINARY_ASSERT(DT_WARN_LE, (__VA_ARGS__), le)
#define DOCTEST_CHECK_LE(...) DOCTEST_BINARY_ASSERT(DT_CHECK_LE, (__VA_ARGS__), le)
#define DOCTEST_REQUIRE_LE(...) DOCTEST_BINARY_ASSERT(DT_REQUIRE_LE, (__VA_ARGS__), le)

#define DOCTEST_WARN_UNARY(...) DOCTEST_UNARY_ASSERT(DT_WARN_UNARY, (__VA_ARGS__))
#define DOCTEST_CHECK_UNARY(...) DOCTEST_UNARY_ASSERT(DT_CHECK_UNARY, (__VA_ARGS__))
#define DOCTEST_REQUIRE_UNARY(...) DOCTEST_UNARY_ASSERT(DT_REQUIRE_UNARY, (__VA_ARGS__))
#define DOCTEST_WARN_UNARY_FALSE(...) DOCTEST_UNARY_ASSERT(DT_WARN_UNARY_FALSE, (__VA_ARGS__))
#define DOCTEST_CHECK_UNARY_FALSE(...) DOCTEST_UNARY_ASSERT(DT_CHECK_UNARY_FALSE, (__VA_ARGS__))
#define DOCTEST_REQUIRE_UNARY_FALSE(...) DOCTEST_UNARY_ASSERT(DT_REQUIRE_UNARY_FALSE, (__VA_ARGS__))
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN_EQ(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_WARN_EQ, lhs, rhs, eq)
#define DOCTEST_CHECK_EQ(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_CHECK_EQ, lhs, rhs, eq)
#define DOCTEST_REQUIRE_EQ(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_REQUIRE_EQ, lhs, rhs, eq)
#define DOCTEST_WARN_NE(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_WARN_NE, lhs, rhs, ne)
#define DOCTEST_CHECK_NE(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_CHECK_NE, lhs, rhs, ne)
#define DOCTEST_REQUIRE_NE(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_REQUIRE_NE, lhs, rhs, ne)
#define DOCTEST_WARN_GT(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_WARN_GT, lhs, rhs, gt)
#define DOCTEST_CHECK_GT(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_CHECK_GT, lhs, rhs, gt)
#define DOCTEST_REQUIRE_GT(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_REQUIRE_GT, lhs, rhs, gt)
#define DOCTEST_WARN_LT(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_WARN_LT, lhs, rhs, lt)
#define DOCTEST_CHECK_LT(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_CHECK_LT, lhs, rhs, lt)
#define DOCTEST_REQUIRE_LT(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_REQUIRE_LT, lhs, rhs, lt)
#define DOCTEST_WARN_GE(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_WARN_GE, lhs, rhs, ge)
#define DOCTEST_CHECK_GE(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_CHECK_GE, lhs, rhs, ge)
#define DOCTEST_REQUIRE_GE(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_REQUIRE_GE, lhs, rhs, ge)
#define DOCTEST_WARN_LE(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_WARN_LE, lhs, rhs, le)
#define DOCTEST_CHECK_LE(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_CHECK_LE, lhs, rhs, le)
#define DOCTEST_REQUIRE_LE(lhs, rhs) DOCTEST_BINARY_ASSERT(DT_REQUIRE_LE, lhs, rhs, le)

#define DOCTEST_WARN_UNARY(v) DOCTEST_UNARY_ASSERT(DT_WARN_UNARY, v)
#define DOCTEST_CHECK_UNARY(v) DOCTEST_UNARY_ASSERT(DT_CHECK_UNARY, v)
#define DOCTEST_REQUIRE_UNARY(v) DOCTEST_UNARY_ASSERT(DT_REQUIRE_UNARY, v)
#define DOCTEST_WARN_UNARY_FALSE(v) DOCTEST_UNARY_ASSERT(DT_WARN_UNARY_FALSE, v)
#define DOCTEST_CHECK_UNARY_FALSE(v) DOCTEST_UNARY_ASSERT(DT_CHECK_UNARY_FALSE, v)
#define DOCTEST_REQUIRE_UNARY_FALSE(v) DOCTEST_UNARY_ASSERT(DT_REQUIRE_UNARY_FALSE, v)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

#ifndef DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_FAST_BINARY_ASSERT(assert_type, expr, comparison)                                  \
    do {                                                                                           \
        int _DOCTEST_FAST_RES = doctest::detail::fast_binary_assert<                               \
                doctest::detail::binaryAssertComparison::comparison>(                              \
                doctest::detail::assertType::assert_type, __FILE__, __LINE__,                      \
                DOCTEST_TOSTR(DOCTEST_HANDLE_BRACED_VA_ARGS(expr)),                                \
                DOCTEST_HANDLE_BRACED_VA_ARGS(expr));                                              \
        if(_DOCTEST_FAST_RES & doctest::detail::assertAction::dbgbreak)                            \
            DOCTEST_BREAK_INTO_DEBUGGER();                                                         \
        doctest::detail::fastAssertThrowIfFlagSet(_DOCTEST_FAST_RES);                              \
    } while((void)0, 0)
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_FAST_BINARY_ASSERT(assert_type, lhs, rhs, comparison)                              \
    do {                                                                                           \
        int _DOCTEST_FAST_RES = doctest::detail::fast_binary_assert<                               \
                doctest::detail::binaryAssertComparison::comparison>(                              \
                doctest::detail::assertType::assert_type, __FILE__, __LINE__, #lhs ", " #rhs, lhs, \
                rhs);                                                                              \
        if(_DOCTEST_FAST_RES & doctest::detail::assertAction::dbgbreak)                            \
            DOCTEST_BREAK_INTO_DEBUGGER();                                                         \
        doctest::detail::fastAssertThrowIfFlagSet(_DOCTEST_FAST_RES);                              \
    } while((void)0, 0)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

#define DOCTEST_FAST_UNARY_ASSERT(assert_type, expr)                                               \
    do {                                                                                           \
        int _DOCTEST_FAST_RES = doctest::detail::fast_unary_assert(                                \
                doctest::detail::assertType::assert_type, __FILE__, __LINE__,                      \
                DOCTEST_TOSTR(DOCTEST_HANDLE_BRACED_VA_ARGS(expr)),                                \
                DOCTEST_HANDLE_BRACED_VA_ARGS(expr));                                              \
        if(_DOCTEST_FAST_RES & doctest::detail::assertAction::dbgbreak)                            \
            DOCTEST_BREAK_INTO_DEBUGGER();                                                         \
        doctest::detail::fastAssertThrowIfFlagSet(_DOCTEST_FAST_RES);                              \
    } while((void)0, 0)

#else // DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_FAST_BINARY_ASSERT(assert_type, expr, comparison)                                  \
    doctest::detail::fast_binary_assert<doctest::detail::binaryAssertComparison::comparison>(      \
            doctest::detail::assertType::assert_type, __FILE__, __LINE__,                          \
            DOCTEST_TOSTR(DOCTEST_HANDLE_BRACED_VA_ARGS(expr)),                                    \
            DOCTEST_HANDLE_BRACED_VA_ARGS(expr))
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_FAST_BINARY_ASSERT(assert_type, lhs, rhs, comparison)                              \
    doctest::detail::fast_binary_assert<doctest::detail::binaryAssertComparison::comparison>(      \
            doctest::detail::assertType::assert_type, __FILE__, __LINE__, #lhs ", " #rhs, lhs,     \
            rhs)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

#define DOCTEST_FAST_UNARY_ASSERT(assert_type, expr)                                               \
    doctest::detail::fast_unary_assert(doctest::detail::assertType::assert_type, __FILE__,         \
                                       __LINE__,                                                   \
                                       DOCTEST_TOSTR(DOCTEST_HANDLE_BRACED_VA_ARGS(expr)),         \
                                       DOCTEST_HANDLE_BRACED_VA_ARGS(expr))

#endif // DOCTEST_CONFIG_SUPER_FAST_ASSERTS

// clang-format off
#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_FAST_WARN_EQ(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_WARN_EQ, (__VA_ARGS__), eq)
#define DOCTEST_FAST_CHECK_EQ(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_CHECK_EQ, (__VA_ARGS__), eq)
#define DOCTEST_FAST_REQUIRE_EQ(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_REQUIRE_EQ, (__VA_ARGS__), eq)
#define DOCTEST_FAST_WARN_NE(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_WARN_NE, (__VA_ARGS__), ne)
#define DOCTEST_FAST_CHECK_NE(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_CHECK_NE, (__VA_ARGS__), ne)
#define DOCTEST_FAST_REQUIRE_NE(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_REQUIRE_NE, (__VA_ARGS__), ne)
#define DOCTEST_FAST_WARN_GT(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_WARN_GT, (__VA_ARGS__), gt)
#define DOCTEST_FAST_CHECK_GT(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_CHECK_GT, (__VA_ARGS__), gt)
#define DOCTEST_FAST_REQUIRE_GT(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_REQUIRE_GT, (__VA_ARGS__), gt)
#define DOCTEST_FAST_WARN_LT(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_WARN_LT, (__VA_ARGS__), lt)
#define DOCTEST_FAST_CHECK_LT(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_CHECK_LT, (__VA_ARGS__), lt)
#define DOCTEST_FAST_REQUIRE_LT(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_REQUIRE_LT, (__VA_ARGS__), lt)
#define DOCTEST_FAST_WARN_GE(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_WARN_GE, (__VA_ARGS__), ge)
#define DOCTEST_FAST_CHECK_GE(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_CHECK_GE, (__VA_ARGS__), ge)
#define DOCTEST_FAST_REQUIRE_GE(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_REQUIRE_GE, (__VA_ARGS__), ge)
#define DOCTEST_FAST_WARN_LE(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_WARN_LE, (__VA_ARGS__), le)
#define DOCTEST_FAST_CHECK_LE(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_CHECK_LE, (__VA_ARGS__), le)
#define DOCTEST_FAST_REQUIRE_LE(...) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_REQUIRE_LE, (__VA_ARGS__), le)

#define DOCTEST_FAST_WARN_UNARY(...) DOCTEST_FAST_UNARY_ASSERT(DT_FAST_WARN_UNARY, (__VA_ARGS__))
#define DOCTEST_FAST_CHECK_UNARY(...) DOCTEST_FAST_UNARY_ASSERT(DT_FAST_CHECK_UNARY, (__VA_ARGS__))
#define DOCTEST_FAST_REQUIRE_UNARY(...) DOCTEST_FAST_UNARY_ASSERT(DT_FAST_REQUIRE_UNARY, (__VA_ARGS__))
#define DOCTEST_FAST_WARN_UNARY_FALSE(...) DOCTEST_FAST_UNARY_ASSERT(DT_FAST_WARN_UNARY_FALSE, (__VA_ARGS__))
#define DOCTEST_FAST_CHECK_UNARY_FALSE(...) DOCTEST_FAST_UNARY_ASSERT(DT_FAST_CHECK_UNARY_FALSE, (__VA_ARGS__))
#define DOCTEST_FAST_REQUIRE_UNARY_FALSE(...) DOCTEST_FAST_UNARY_ASSERT(DT_FAST_REQUIRE_UNARY_FALSE, (__VA_ARGS__))
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_FAST_WARN_EQ(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_WARN_EQ, l, r, eq)
#define DOCTEST_FAST_CHECK_EQ(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_CHECK_EQ, l, r, eq)
#define DOCTEST_FAST_REQUIRE_EQ(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_REQUIRE_EQ, l, r, eq)
#define DOCTEST_FAST_WARN_NE(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_WARN_NE, l, r, ne)
#define DOCTEST_FAST_CHECK_NE(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_CHECK_NE, l, r, ne)
#define DOCTEST_FAST_REQUIRE_NE(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_REQUIRE_NE, l, r, ne)
#define DOCTEST_FAST_WARN_GT(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_WARN_GT, l, r, gt)
#define DOCTEST_FAST_CHECK_GT(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_CHECK_GT, l, r, gt)
#define DOCTEST_FAST_REQUIRE_GT(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_REQUIRE_GT, l, r, gt)
#define DOCTEST_FAST_WARN_LT(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_WARN_LT, l, r, lt)
#define DOCTEST_FAST_CHECK_LT(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_CHECK_LT, l, r, lt)
#define DOCTEST_FAST_REQUIRE_LT(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_REQUIRE_LT, l, r, lt)
#define DOCTEST_FAST_WARN_GE(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_WARN_GE, l, r, ge)
#define DOCTEST_FAST_CHECK_GE(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_CHECK_GE, l, r, ge)
#define DOCTEST_FAST_REQUIRE_GE(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_REQUIRE_GE, l, r, ge)
#define DOCTEST_FAST_WARN_LE(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_WARN_LE, l, r, le)
#define DOCTEST_FAST_CHECK_LE(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_CHECK_LE, l, r, le)
#define DOCTEST_FAST_REQUIRE_LE(l, r) DOCTEST_FAST_BINARY_ASSERT(DT_FAST_REQUIRE_LE, l, r, le)

#define DOCTEST_FAST_WARN_UNARY(v) DOCTEST_FAST_UNARY_ASSERT(DT_FAST_WARN_UNARY, v)
#define DOCTEST_FAST_CHECK_UNARY(v) DOCTEST_FAST_UNARY_ASSERT(DT_FAST_CHECK_UNARY, v)
#define DOCTEST_FAST_REQUIRE_UNARY(v) DOCTEST_FAST_UNARY_ASSERT(DT_FAST_REQUIRE_UNARY, v)
#define DOCTEST_FAST_WARN_UNARY_FALSE(v) DOCTEST_FAST_UNARY_ASSERT(DT_FAST_WARN_UNARY_FALSE, v)
#define DOCTEST_FAST_CHECK_UNARY_FALSE(v) DOCTEST_FAST_UNARY_ASSERT(DT_FAST_CHECK_UNARY_FALSE, v)
#define DOCTEST_FAST_REQUIRE_UNARY_FALSE(v) DOCTEST_FAST_UNARY_ASSERT(DT_FAST_REQUIRE_UNARY_FALSE, v)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
// clang-format on

#ifdef DOCTEST_CONFIG_NO_EXCEPTIONS

#undef DOCTEST_WARN_THROWS
#undef DOCTEST_CHECK_THROWS
#undef DOCTEST_REQUIRE_THROWS
#undef DOCTEST_WARN_THROWS_AS
#undef DOCTEST_CHECK_THROWS_AS
#undef DOCTEST_REQUIRE_THROWS_AS
#undef DOCTEST_WARN_NOTHROW
#undef DOCTEST_CHECK_NOTHROW
#undef DOCTEST_REQUIRE_NOTHROW

#undef DOCTEST_WARN_THROWS_MESSAGE
#undef DOCTEST_CHECK_THROWS_MESSAGE
#undef DOCTEST_REQUIRE_THROWS_MESSAGE
#undef DOCTEST_WARN_THROWS_AS_MESSAGE
#undef DOCTEST_CHECK_THROWS_AS_MESSAGE
#undef DOCTEST_REQUIRE_THROWS_AS_MESSAGE
#undef DOCTEST_WARN_NOTHROW_MESSAGE
#undef DOCTEST_CHECK_NOTHROW_MESSAGE
#undef DOCTEST_REQUIRE_NOTHROW_MESSAGE

#ifdef DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#define DOCTEST_WARN_THROWS(expr) ((void)0)
#define DOCTEST_CHECK_THROWS(expr) ((void)0)
#define DOCTEST_REQUIRE_THROWS(expr) ((void)0)
#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN_THROWS_AS(expr, ...) ((void)0)
#define DOCTEST_CHECK_THROWS_AS(expr, ...) ((void)0)
#define DOCTEST_REQUIRE_THROWS_AS(expr, ...) ((void)0)
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN_THROWS_AS(expr, ex) ((void)0)
#define DOCTEST_CHECK_THROWS_AS(expr, ex) ((void)0)
#define DOCTEST_REQUIRE_THROWS_AS(expr, ex) ((void)0)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN_NOTHROW(expr) ((void)0)
#define DOCTEST_CHECK_NOTHROW(expr) ((void)0)
#define DOCTEST_REQUIRE_NOTHROW(expr) ((void)0)

#define DOCTEST_WARN_THROWS_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_CHECK_THROWS_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_REQUIRE_THROWS_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_WARN_THROWS_AS_MESSAGE(expr, ex, msg) ((void)0)
#define DOCTEST_CHECK_THROWS_AS_MESSAGE(expr, ex, msg) ((void)0)
#define DOCTEST_REQUIRE_THROWS_AS_MESSAGE(expr, ex, msg) ((void)0)
#define DOCTEST_WARN_NOTHROW_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_CHECK_NOTHROW_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_REQUIRE_NOTHROW_MESSAGE(expr, msg) ((void)0)

#else // DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#undef DOCTEST_REQUIRE
#undef DOCTEST_REQUIRE_FALSE
#undef DOCTEST_REQUIRE_MESSAGE
#undef DOCTEST_REQUIRE_FALSE_MESSAGE
#undef DOCTEST_REQUIRE_EQ
#undef DOCTEST_REQUIRE_NE
#undef DOCTEST_REQUIRE_GT
#undef DOCTEST_REQUIRE_LT
#undef DOCTEST_REQUIRE_GE
#undef DOCTEST_REQUIRE_LE
#undef DOCTEST_REQUIRE_UNARY
#undef DOCTEST_REQUIRE_UNARY_FALSE
#undef DOCTEST_FAST_REQUIRE_EQ
#undef DOCTEST_FAST_REQUIRE_NE
#undef DOCTEST_FAST_REQUIRE_GT
#undef DOCTEST_FAST_REQUIRE_LT
#undef DOCTEST_FAST_REQUIRE_GE
#undef DOCTEST_FAST_REQUIRE_LE
#undef DOCTEST_FAST_REQUIRE_UNARY
#undef DOCTEST_FAST_REQUIRE_UNARY_FALSE

#endif // DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#endif // DOCTEST_CONFIG_NO_EXCEPTIONS

// =================================================================================================
// == WHAT FOLLOWS IS VERSIONS OF THE MACROS THAT DO NOT DO ANY REGISTERING!                      ==
// == THIS CAN BE ENABLED BY DEFINING DOCTEST_CONFIG_DISABLE GLOBALLY!                            ==
// =================================================================================================
#else // DOCTEST_CONFIG_DISABLE

#define DOCTEST_IMPLEMENT_FIXTURE(der, base, func, name)                                           \
    namespace                                                                                      \
    {                                                                                              \
        template <typename DOCTEST_UNUSED_TEMPLATE_TYPE>                                           \
        struct der : base                                                                          \
        { void f(); };                                                                             \
    }                                                                                              \
    template <typename DOCTEST_UNUSED_TEMPLATE_TYPE>                                               \
    inline void der<DOCTEST_UNUSED_TEMPLATE_TYPE>::f()

#define DOCTEST_CREATE_AND_REGISTER_FUNCTION(f, name)                                              \
    template <typename DOCTEST_UNUSED_TEMPLATE_TYPE>                                               \
    static inline void f()

// for registering tests
#define DOCTEST_TEST_CASE(name)                                                                    \
    DOCTEST_CREATE_AND_REGISTER_FUNCTION(DOCTEST_ANONYMOUS(_DOCTEST_ANON_FUNC_), name)

// for registering tests with a fixture
#define DOCTEST_TEST_CASE_FIXTURE(x, name)                                                         \
    DOCTEST_IMPLEMENT_FIXTURE(DOCTEST_ANONYMOUS(_DOCTEST_ANON_CLASS_), x,                          \
                              DOCTEST_ANONYMOUS(_DOCTEST_ANON_FUNC_), name)

// for converting types to strings without the <typeinfo> header and demangling
#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_TYPE_TO_STRING(...) typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)
#define DOCTEST_TYPE_TO_STRING_IMPL(...)
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_TYPE_TO_STRING(x) typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)
#define DOCTEST_TYPE_TO_STRING_IMPL(x)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

// for typed tests
#define DOCTEST_TEST_CASE_TEMPLATE(name, type, types)                                              \
    template <typename type>                                                                       \
    inline void DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_)()

#define DOCTEST_TEST_CASE_TEMPLATE_DEFINE(name, type, id)                                          \
    template <typename type>                                                                       \
    inline void DOCTEST_ANONYMOUS(_DOCTEST_ANON_TMP_)()

#define DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE(id, types)                                          \
    typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

// for subcases
#define DOCTEST_SUBCASE(name)

// for a testsuite block
#define DOCTEST_TEST_SUITE(name) namespace

// for starting a testsuite block
#define DOCTEST_TEST_SUITE_BEGIN(name) typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

// for ending a testsuite block
#define DOCTEST_TEST_SUITE_END typedef int DOCTEST_ANONYMOUS(_DOCTEST_ANON_FOR_SEMICOLON_)

#define DOCTEST_REGISTER_EXCEPTION_TRANSLATOR(signature)                                           \
    template <typename DOCTEST_UNUSED_TEMPLATE_TYPE>                                               \
    static inline doctest::String DOCTEST_ANONYMOUS(_DOCTEST_ANON_TRANSLATOR_)(signature)

#define DOCTEST_INFO(x) ((void)0)
#define DOCTEST_CAPTURE(x) ((void)0)
#define DOCTEST_ADD_MESSAGE_AT(file, line, x) ((void)0)
#define DOCTEST_ADD_FAIL_CHECK_AT(file, line, x) ((void)0)
#define DOCTEST_ADD_FAIL_AT(file, line, x) ((void)0)
#define DOCTEST_MESSAGE(x) ((void)0)
#define DOCTEST_FAIL_CHECK(x) ((void)0)
#define DOCTEST_FAIL(x) ((void)0)

#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN(...) ((void)0)
#define DOCTEST_CHECK(...) ((void)0)
#define DOCTEST_REQUIRE(...) ((void)0)
#define DOCTEST_WARN_FALSE(...) ((void)0)
#define DOCTEST_CHECK_FALSE(...) ((void)0)
#define DOCTEST_REQUIRE_FALSE(...) ((void)0)
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN(expr) ((void)0)
#define DOCTEST_CHECK(expr) ((void)0)
#define DOCTEST_REQUIRE(expr) ((void)0)
#define DOCTEST_WARN_FALSE(expr) ((void)0)
#define DOCTEST_CHECK_FALSE(expr) ((void)0)
#define DOCTEST_REQUIRE_FALSE(expr) ((void)0)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

#define DOCTEST_WARN_MESSAGE(cond, msg) ((void)0)
#define DOCTEST_CHECK_MESSAGE(cond, msg) ((void)0)
#define DOCTEST_REQUIRE_MESSAGE(cond, msg) ((void)0)
#define DOCTEST_WARN_FALSE_MESSAGE(cond, msg) ((void)0)
#define DOCTEST_CHECK_FALSE_MESSAGE(cond, msg) ((void)0)
#define DOCTEST_REQUIRE_FALSE_MESSAGE(cond, msg) ((void)0)

#define DOCTEST_WARN_THROWS(expr) ((void)0)
#define DOCTEST_CHECK_THROWS(expr) ((void)0)
#define DOCTEST_REQUIRE_THROWS(expr) ((void)0)
#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN_THROWS_AS(expr, ...) ((void)0)
#define DOCTEST_CHECK_THROWS_AS(expr, ...) ((void)0)
#define DOCTEST_REQUIRE_THROWS_AS(expr, ...) ((void)0)
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN_THROWS_AS(expr, ex) ((void)0)
#define DOCTEST_CHECK_THROWS_AS(expr, ex) ((void)0)
#define DOCTEST_REQUIRE_THROWS_AS(expr, ex) ((void)0)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_WARN_NOTHROW(expr) ((void)0)
#define DOCTEST_CHECK_NOTHROW(expr) ((void)0)
#define DOCTEST_REQUIRE_NOTHROW(expr) ((void)0)

#define DOCTEST_WARN_THROWS_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_CHECK_THROWS_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_REQUIRE_THROWS_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_WARN_THROWS_AS_MESSAGE(expr, ex, msg) ((void)0)
#define DOCTEST_CHECK_THROWS_AS_MESSAGE(expr, ex, msg) ((void)0)
#define DOCTEST_REQUIRE_THROWS_AS_MESSAGE(expr, ex, msg) ((void)0)
#define DOCTEST_WARN_NOTHROW_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_CHECK_NOTHROW_MESSAGE(expr, msg) ((void)0)
#define DOCTEST_REQUIRE_NOTHROW_MESSAGE(expr, msg) ((void)0)

#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS

#define DOCTEST_WARN_EQ(...) ((void)0)
#define DOCTEST_CHECK_EQ(...) ((void)0)
#define DOCTEST_REQUIRE_EQ(...) ((void)0)
#define DOCTEST_WARN_NE(...) ((void)0)
#define DOCTEST_CHECK_NE(...) ((void)0)
#define DOCTEST_REQUIRE_NE(...) ((void)0)
#define DOCTEST_WARN_GT(...) ((void)0)
#define DOCTEST_CHECK_GT(...) ((void)0)
#define DOCTEST_REQUIRE_GT(...) ((void)0)
#define DOCTEST_WARN_LT(...) ((void)0)
#define DOCTEST_CHECK_LT(...) ((void)0)
#define DOCTEST_REQUIRE_LT(...) ((void)0)
#define DOCTEST_WARN_GE(...) ((void)0)
#define DOCTEST_CHECK_GE(...) ((void)0)
#define DOCTEST_REQUIRE_GE(...) ((void)0)
#define DOCTEST_WARN_LE(...) ((void)0)
#define DOCTEST_CHECK_LE(...) ((void)0)
#define DOCTEST_REQUIRE_LE(...) ((void)0)

#define DOCTEST_WARN_UNARY(...) ((void)0)
#define DOCTEST_CHECK_UNARY(...) ((void)0)
#define DOCTEST_REQUIRE_UNARY(...) ((void)0)
#define DOCTEST_WARN_UNARY_FALSE(...) ((void)0)
#define DOCTEST_CHECK_UNARY_FALSE(...) ((void)0)
#define DOCTEST_REQUIRE_UNARY_FALSE(...) ((void)0)

#define DOCTEST_FAST_WARN_EQ(...) ((void)0)
#define DOCTEST_FAST_CHECK_EQ(...) ((void)0)
#define DOCTEST_FAST_REQUIRE_EQ(...) ((void)0)
#define DOCTEST_FAST_WARN_NE(...) ((void)0)
#define DOCTEST_FAST_CHECK_NE(...) ((void)0)
#define DOCTEST_FAST_REQUIRE_NE(...) ((void)0)
#define DOCTEST_FAST_WARN_GT(...) ((void)0)
#define DOCTEST_FAST_CHECK_GT(...) ((void)0)
#define DOCTEST_FAST_REQUIRE_GT(...) ((void)0)
#define DOCTEST_FAST_WARN_LT(...) ((void)0)
#define DOCTEST_FAST_CHECK_LT(...) ((void)0)
#define DOCTEST_FAST_REQUIRE_LT(...) ((void)0)
#define DOCTEST_FAST_WARN_GE(...) ((void)0)
#define DOCTEST_FAST_CHECK_GE(...) ((void)0)
#define DOCTEST_FAST_REQUIRE_GE(...) ((void)0)
#define DOCTEST_FAST_WARN_LE(...) ((void)0)
#define DOCTEST_FAST_CHECK_LE(...) ((void)0)
#define DOCTEST_FAST_REQUIRE_LE(...) ((void)0)

#define DOCTEST_FAST_WARN_UNARY(...) ((void)0)
#define DOCTEST_FAST_CHECK_UNARY(...) ((void)0)
#define DOCTEST_FAST_REQUIRE_UNARY(...) ((void)0)
#define DOCTEST_FAST_WARN_UNARY_FALSE(...) ((void)0)
#define DOCTEST_FAST_CHECK_UNARY_FALSE(...) ((void)0)
#define DOCTEST_FAST_REQUIRE_UNARY_FALSE(...) ((void)0)

#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

#define DOCTEST_WARN_EQ(lhs, rhs) ((void)0)
#define DOCTEST_CHECK_EQ(lhs, rhs) ((void)0)
#define DOCTEST_REQUIRE_EQ(lhs, rhs) ((void)0)
#define DOCTEST_WARN_NE(lhs, rhs) ((void)0)
#define DOCTEST_CHECK_NE(lhs, rhs) ((void)0)
#define DOCTEST_REQUIRE_NE(lhs, rhs) ((void)0)
#define DOCTEST_WARN_GT(lhs, rhs) ((void)0)
#define DOCTEST_CHECK_GT(lhs, rhs) ((void)0)
#define DOCTEST_REQUIRE_GT(lhs, rhs) ((void)0)
#define DOCTEST_WARN_LT(lhs, rhs) ((void)0)
#define DOCTEST_CHECK_LT(lhs, rhs) ((void)0)
#define DOCTEST_REQUIRE_LT(lhs, rhs) ((void)0)
#define DOCTEST_WARN_GE(lhs, rhs) ((void)0)
#define DOCTEST_CHECK_GE(lhs, rhs) ((void)0)
#define DOCTEST_REQUIRE_GE(lhs, rhs) ((void)0)
#define DOCTEST_WARN_LE(lhs, rhs) ((void)0)
#define DOCTEST_CHECK_LE(lhs, rhs) ((void)0)
#define DOCTEST_REQUIRE_LE(lhs, rhs) ((void)0)

#define DOCTEST_WARN_UNARY(val) ((void)0)
#define DOCTEST_CHECK_UNARY(val) ((void)0)
#define DOCTEST_REQUIRE_UNARY(val) ((void)0)
#define DOCTEST_WARN_UNARY_FALSE(val) ((void)0)
#define DOCTEST_CHECK_UNARY_FALSE(val) ((void)0)
#define DOCTEST_REQUIRE_UNARY_FALSE(val) ((void)0)

#define DOCTEST_FAST_WARN_EQ(lhs, rhs) ((void)0)
#define DOCTEST_FAST_CHECK_EQ(lhs, rhs) ((void)0)
#define DOCTEST_FAST_REQUIRE_EQ(lhs, rhs) ((void)0)
#define DOCTEST_FAST_WARN_NE(lhs, rhs) ((void)0)
#define DOCTEST_FAST_CHECK_NE(lhs, rhs) ((void)0)
#define DOCTEST_FAST_REQUIRE_NE(lhs, rhs) ((void)0)
#define DOCTEST_FAST_WARN_GT(lhs, rhs) ((void)0)
#define DOCTEST_FAST_CHECK_GT(lhs, rhs) ((void)0)
#define DOCTEST_FAST_REQUIRE_GT(lhs, rhs) ((void)0)
#define DOCTEST_FAST_WARN_LT(lhs, rhs) ((void)0)
#define DOCTEST_FAST_CHECK_LT(lhs, rhs) ((void)0)
#define DOCTEST_FAST_REQUIRE_LT(lhs, rhs) ((void)0)
#define DOCTEST_FAST_WARN_GE(lhs, rhs) ((void)0)
#define DOCTEST_FAST_CHECK_GE(lhs, rhs) ((void)0)
#define DOCTEST_FAST_REQUIRE_GE(lhs, rhs) ((void)0)
#define DOCTEST_FAST_WARN_LE(lhs, rhs) ((void)0)
#define DOCTEST_FAST_CHECK_LE(lhs, rhs) ((void)0)
#define DOCTEST_FAST_REQUIRE_LE(lhs, rhs) ((void)0)

#define DOCTEST_FAST_WARN_UNARY(val) ((void)0)
#define DOCTEST_FAST_CHECK_UNARY(val) ((void)0)
#define DOCTEST_FAST_REQUIRE_UNARY(val) ((void)0)
#define DOCTEST_FAST_WARN_UNARY_FALSE(val) ((void)0)
#define DOCTEST_FAST_CHECK_UNARY_FALSE(val) ((void)0)
#define DOCTEST_FAST_REQUIRE_UNARY_FALSE(val) ((void)0)

#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS

#endif // DOCTEST_CONFIG_DISABLE

// BDD style macros
// clang-format off
#define DOCTEST_SCENARIO(name)  TEST_CASE("  Scenario: " name)
#ifdef DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_SCENARIO_TEMPLATE(name, T, ...)  TEST_CASE_TEMPLATE("  Scenario: " name, T, __VA_ARGS__)
#else // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_SCENARIO_TEMPLATE(name, T, types) TEST_CASE_TEMPLATE("  Scenario: " name, T, types)
#endif // DOCTEST_CONFIG_WITH_VARIADIC_MACROS
#define DOCTEST_SCENARIO_TEMPLATE_DEFINE(name, T, id) DOCTEST_TEST_CASE_TEMPLATE_DEFINE("  Scenario: " name, T, id)

#define DOCTEST_GIVEN(name)     SUBCASE("   Given: " name)
#define DOCTEST_WHEN(name)      SUBCASE("    When: " name)
#define DOCTEST_AND_WHEN(name)  SUBCASE("And when: " name)
#define DOCTEST_THEN(name)      SUBCASE("    Then: " name)
#define DOCTEST_AND_THEN(name)  SUBCASE("     And: " name)
// clang-format on

// == SHORT VERSIONS OF THE MACROS
#if !defined(DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES)

#define TEST_CASE DOCTEST_TEST_CASE
#define TEST_CASE_FIXTURE DOCTEST_TEST_CASE_FIXTURE
#define TYPE_TO_STRING DOCTEST_TYPE_TO_STRING
#define TEST_CASE_TEMPLATE DOCTEST_TEST_CASE_TEMPLATE
#define TEST_CASE_TEMPLATE_DEFINE DOCTEST_TEST_CASE_TEMPLATE_DEFINE
#define TEST_CASE_TEMPLATE_INSTANTIATE DOCTEST_TEST_CASE_TEMPLATE_INSTANTIATE
#define SUBCASE DOCTEST_SUBCASE
#define TEST_SUITE DOCTEST_TEST_SUITE
#define TEST_SUITE_BEGIN DOCTEST_TEST_SUITE_BEGIN
#define TEST_SUITE_END DOCTEST_TEST_SUITE_END
#define REGISTER_EXCEPTION_TRANSLATOR DOCTEST_REGISTER_EXCEPTION_TRANSLATOR
#define INFO DOCTEST_INFO
#define CAPTURE DOCTEST_CAPTURE
#define ADD_MESSAGE_AT DOCTEST_ADD_MESSAGE_AT
#define ADD_FAIL_CHECK_AT DOCTEST_ADD_FAIL_CHECK_AT
#define ADD_FAIL_AT DOCTEST_ADD_FAIL_AT
#define MESSAGE DOCTEST_MESSAGE
#define FAIL_CHECK DOCTEST_FAIL_CHECK
#define FAIL DOCTEST_FAIL
#define TO_LVALUE DOCTEST_TO_LVALUE

#define WARN DOCTEST_WARN
#define WARN_FALSE DOCTEST_WARN_FALSE
#define WARN_THROWS DOCTEST_WARN_THROWS
#define WARN_THROWS_AS DOCTEST_WARN_THROWS_AS
#define WARN_NOTHROW DOCTEST_WARN_NOTHROW
#define CHECK DOCTEST_CHECK
#define CHECK_FALSE DOCTEST_CHECK_FALSE
#define CHECK_THROWS DOCTEST_CHECK_THROWS
#define CHECK_THROWS_AS DOCTEST_CHECK_THROWS_AS
#define CHECK_NOTHROW DOCTEST_CHECK_NOTHROW
#define REQUIRE DOCTEST_REQUIRE
#define REQUIRE_FALSE DOCTEST_REQUIRE_FALSE
#define REQUIRE_THROWS DOCTEST_REQUIRE_THROWS
#define REQUIRE_THROWS_AS DOCTEST_REQUIRE_THROWS_AS
#define REQUIRE_NOTHROW DOCTEST_REQUIRE_NOTHROW

#define WARN_MESSAGE DOCTEST_WARN_MESSAGE
#define WARN_FALSE_MESSAGE DOCTEST_WARN_FALSE_MESSAGE
#define WARN_THROWS_MESSAGE DOCTEST_WARN_THROWS_MESSAGE
#define WARN_THROWS_AS_MESSAGE DOCTEST_WARN_THROWS_AS_MESSAGE
#define WARN_NOTHROW_MESSAGE DOCTEST_WARN_NOTHROW_MESSAGE
#define CHECK_MESSAGE DOCTEST_CHECK_MESSAGE
#define CHECK_FALSE_MESSAGE DOCTEST_CHECK_FALSE_MESSAGE
#define CHECK_THROWS_MESSAGE DOCTEST_CHECK_THROWS_MESSAGE
#define CHECK_THROWS_AS_MESSAGE DOCTEST_CHECK_THROWS_AS_MESSAGE
#define CHECK_NOTHROW_MESSAGE DOCTEST_CHECK_NOTHROW_MESSAGE
#define REQUIRE_MESSAGE DOCTEST_REQUIRE_MESSAGE
#define REQUIRE_FALSE_MESSAGE DOCTEST_REQUIRE_FALSE_MESSAGE
#define REQUIRE_THROWS_MESSAGE DOCTEST_REQUIRE_THROWS_MESSAGE
#define REQUIRE_THROWS_AS_MESSAGE DOCTEST_REQUIRE_THROWS_AS_MESSAGE
#define REQUIRE_NOTHROW_MESSAGE DOCTEST_REQUIRE_NOTHROW_MESSAGE

#define SCENARIO DOCTEST_SCENARIO
#define SCENARIO_TEMPLATE DOCTEST_SCENARIO_TEMPLATE
#define SCENARIO_TEMPLATE_DEFINE DOCTEST_SCENARIO_TEMPLATE_DEFINE
#define GIVEN DOCTEST_GIVEN
#define WHEN DOCTEST_WHEN
#define AND_WHEN DOCTEST_AND_WHEN
#define THEN DOCTEST_THEN
#define AND_THEN DOCTEST_AND_THEN

#define WARN_EQ DOCTEST_WARN_EQ
#define CHECK_EQ DOCTEST_CHECK_EQ
#define REQUIRE_EQ DOCTEST_REQUIRE_EQ
#define WARN_NE DOCTEST_WARN_NE
#define CHECK_NE DOCTEST_CHECK_NE
#define REQUIRE_NE DOCTEST_REQUIRE_NE
#define WARN_GT DOCTEST_WARN_GT
#define CHECK_GT DOCTEST_CHECK_GT
#define REQUIRE_GT DOCTEST_REQUIRE_GT
#define WARN_LT DOCTEST_WARN_LT
#define CHECK_LT DOCTEST_CHECK_LT
#define REQUIRE_LT DOCTEST_REQUIRE_LT
#define WARN_GE DOCTEST_WARN_GE
#define CHECK_GE DOCTEST_CHECK_GE
#define REQUIRE_GE DOCTEST_REQUIRE_GE
#define WARN_LE DOCTEST_WARN_LE
#define CHECK_LE DOCTEST_CHECK_LE
#define REQUIRE_LE DOCTEST_REQUIRE_LE
#define WARN_UNARY DOCTEST_WARN_UNARY
#define CHECK_UNARY DOCTEST_CHECK_UNARY
#define REQUIRE_UNARY DOCTEST_REQUIRE_UNARY
#define WARN_UNARY_FALSE DOCTEST_WARN_UNARY_FALSE
#define CHECK_UNARY_FALSE DOCTEST_CHECK_UNARY_FALSE
#define REQUIRE_UNARY_FALSE DOCTEST_REQUIRE_UNARY_FALSE

#define FAST_WARN_EQ DOCTEST_FAST_WARN_EQ
#define FAST_CHECK_EQ DOCTEST_FAST_CHECK_EQ
#define FAST_REQUIRE_EQ DOCTEST_FAST_REQUIRE_EQ
#define FAST_WARN_NE DOCTEST_FAST_WARN_NE
#define FAST_CHECK_NE DOCTEST_FAST_CHECK_NE
#define FAST_REQUIRE_NE DOCTEST_FAST_REQUIRE_NE
#define FAST_WARN_GT DOCTEST_FAST_WARN_GT
#define FAST_CHECK_GT DOCTEST_FAST_CHECK_GT
#define FAST_REQUIRE_GT DOCTEST_FAST_REQUIRE_GT
#define FAST_WARN_LT DOCTEST_FAST_WARN_LT
#define FAST_CHECK_LT DOCTEST_FAST_CHECK_LT
#define FAST_REQUIRE_LT DOCTEST_FAST_REQUIRE_LT
#define FAST_WARN_GE DOCTEST_FAST_WARN_GE
#define FAST_CHECK_GE DOCTEST_FAST_CHECK_GE
#define FAST_REQUIRE_GE DOCTEST_FAST_REQUIRE_GE
#define FAST_WARN_LE DOCTEST_FAST_WARN_LE
#define FAST_CHECK_LE DOCTEST_FAST_CHECK_LE
#define FAST_REQUIRE_LE DOCTEST_FAST_REQUIRE_LE
#define FAST_WARN_UNARY DOCTEST_FAST_WARN_UNARY
#define FAST_CHECK_UNARY DOCTEST_FAST_CHECK_UNARY
#define FAST_REQUIRE_UNARY DOCTEST_FAST_REQUIRE_UNARY
#define FAST_WARN_UNARY_FALSE DOCTEST_FAST_WARN_UNARY_FALSE
#define FAST_CHECK_UNARY_FALSE DOCTEST_FAST_CHECK_UNARY_FALSE
#define FAST_REQUIRE_UNARY_FALSE DOCTEST_FAST_REQUIRE_UNARY_FALSE

#endif // DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES

// this is here to clear the 'current test suite' for the current translation unit - at the top
DOCTEST_TEST_SUITE_END();

// add stringification for primitive/fundamental types
namespace doctest
{
namespace detail
{
    DOCTEST_TYPE_TO_STRING_IMPL(bool)
    DOCTEST_TYPE_TO_STRING_IMPL(float)
    DOCTEST_TYPE_TO_STRING_IMPL(double)
    DOCTEST_TYPE_TO_STRING_IMPL(long double)
    DOCTEST_TYPE_TO_STRING_IMPL(char)
    DOCTEST_TYPE_TO_STRING_IMPL(signed char)
    DOCTEST_TYPE_TO_STRING_IMPL(unsigned char)
    DOCTEST_TYPE_TO_STRING_IMPL(wchar_t)
    DOCTEST_TYPE_TO_STRING_IMPL(short int)
    DOCTEST_TYPE_TO_STRING_IMPL(unsigned short int)
    DOCTEST_TYPE_TO_STRING_IMPL(int)
    DOCTEST_TYPE_TO_STRING_IMPL(unsigned int)
    DOCTEST_TYPE_TO_STRING_IMPL(long int)
    DOCTEST_TYPE_TO_STRING_IMPL(unsigned long int)
#ifdef DOCTEST_CONFIG_WITH_LONG_LONG
    DOCTEST_TYPE_TO_STRING_IMPL(long long int)
    DOCTEST_TYPE_TO_STRING_IMPL(unsigned long long int)
#endif // DOCTEST_CONFIG_WITH_LONG_LONG
} // namespace detail
} // namespace doctest

DOCTEST_CLANG_SUPPRESS_WARNING_POP
DOCTEST_MSVC_SUPPRESS_WARNING_POP
DOCTEST_GCC_SUPPRESS_WARNING_POP

#endif // DOCTEST_LIBRARY_INCLUDED

#ifndef DOCTEST_SINGLE_HEADER
#define DOCTEST_SINGLE_HEADER
#endif // DOCTEST_SINGLE_HEADER

#if defined(DOCTEST_CONFIG_IMPLEMENT) || !defined(DOCTEST_SINGLE_HEADER)
#ifndef DOCTEST_LIBRARY_IMPLEMENTATION
#define DOCTEST_LIBRARY_IMPLEMENTATION

#ifndef DOCTEST_SINGLE_HEADER
#include "doctest_fwd.h"
#endif // DOCTEST_SINGLE_HEADER

DOCTEST_CLANG_SUPPRESS_WARNING_PUSH
DOCTEST_CLANG_SUPPRESS_WARNING("-Wunknown-pragmas")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wpadded")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wmissing-prototypes")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wsign-conversion")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wshorten-64-to-32")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wmissing-variable-declarations")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wswitch")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wswitch-enum")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wcovered-switch-default")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wmissing-noreturn")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wunused-local-typedef")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wdisabled-macro-expansion")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wmissing-braces")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wmissing-field-initializers")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wc++11-long-long")
#if DOCTEST_CLANG && DOCTEST_CLANG_HAS_WARNING("-Wzero-as-null-pointer-constant")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wzero-as-null-pointer-constant")
#endif // clang - 0 as null

DOCTEST_GCC_SUPPRESS_WARNING_PUSH
DOCTEST_GCC_SUPPRESS_WARNING("-Wunknown-pragmas")
DOCTEST_GCC_SUPPRESS_WARNING("-Wpragmas")
DOCTEST_GCC_SUPPRESS_WARNING("-Wconversion")
DOCTEST_GCC_SUPPRESS_WARNING("-Weffc++")
DOCTEST_GCC_SUPPRESS_WARNING("-Wsign-conversion")
DOCTEST_GCC_SUPPRESS_WARNING("-Wstrict-overflow")
DOCTEST_GCC_SUPPRESS_WARNING("-Wmissing-field-initializers")
DOCTEST_GCC_SUPPRESS_WARNING("-Wmissing-braces")
DOCTEST_GCC_SUPPRESS_WARNING("-Wmissing-declarations")
DOCTEST_GCC_SUPPRESS_WARNING("-Winline")
DOCTEST_GCC_SUPPRESS_WARNING("-Wswitch")
DOCTEST_GCC_SUPPRESS_WARNING("-Wswitch-enum")
DOCTEST_GCC_SUPPRESS_WARNING("-Wswitch-default")
DOCTEST_GCC_SUPPRESS_WARNING("-Wunsafe-loop-optimizations")
DOCTEST_GCC_SUPPRESS_WARNING("-Wlong-long")
DOCTEST_GCC_SUPPRESS_WARNING("-Wold-style-cast")
DOCTEST_GCC_SUPPRESS_WARNING("-Wzero-as-null-pointer-constant")
DOCTEST_GCC_SUPPRESS_WARNING("-Wunused-local-typedefs")
DOCTEST_GCC_SUPPRESS_WARNING("-Wuseless-cast")

DOCTEST_MSVC_SUPPRESS_WARNING_PUSH
DOCTEST_MSVC_SUPPRESS_WARNING(4616) // invalid compiler warning
DOCTEST_MSVC_SUPPRESS_WARNING(4619) // invalid compiler warning
DOCTEST_MSVC_SUPPRESS_WARNING(4996) // The compiler encountered a deprecated declaration
DOCTEST_MSVC_SUPPRESS_WARNING(4267) // 'var' : conversion from 'x' to 'y', possible loss of data
DOCTEST_MSVC_SUPPRESS_WARNING(4706) // assignment within conditional expression
DOCTEST_MSVC_SUPPRESS_WARNING(4512) // 'class' : assignment operator could not be generated
DOCTEST_MSVC_SUPPRESS_WARNING(4127) // conditional expression is constant
DOCTEST_MSVC_SUPPRESS_WARNING(4530) // C++ exception handler used, but unwind semantics not enabled
DOCTEST_MSVC_SUPPRESS_WARNING(4577) // 'noexcept' used with no exception handling mode specified
DOCTEST_MSVC_SUPPRESS_WARNING(4774) // format string expected in argument is not a string literal
DOCTEST_MSVC_SUPPRESS_WARNING(4365) // conversion from 'int' to 'unsigned', signed/unsigned mismatch
DOCTEST_MSVC_SUPPRESS_WARNING(4820) // padding in structs
DOCTEST_MSVC_SUPPRESS_WARNING(4640) // construction of local static object is not thread-safe
DOCTEST_MSVC_SUPPRESS_WARNING(5039) // pointer to potentially throwing function passed to extern C
DOCTEST_MSVC_SUPPRESS_WARNING(5045) // Spectre mitigation stuff

#if defined(DOCTEST_NO_CPP11_COMPAT)
DOCTEST_CLANG_SUPPRESS_WARNING("-Wc++98-compat")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wc++98-compat-pedantic")
#endif // DOCTEST_NO_CPP11_COMPAT

#define DOCTEST_LOG_START(s)                                                                       \
    do {                                                                                           \
        if(!contextState->hasLoggedCurrentTestStart) {                                             \
            logTestStart(s, *contextState->currentTest);                                           \
            DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_BEGIN;                                           \
            logTestStart(oss, *contextState->currentTest);                                         \
            DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_END;                                             \
            contextState->hasLoggedCurrentTestStart = true;                                        \
        }                                                                                          \
    } while(false)

#define DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_BEGIN                                                \
    if(isDebuggerActive()) {                                                                       \
        ContextState* p_cs     = contextState;                                                     \
        bool          with_col = p_cs->no_colors;                                                  \
        p_cs->no_colors        = false;                                                            \
    std::ostringstream oss

#define DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_END                                                  \
    printToDebugConsole(oss.str().c_str());                                                        \
    p_cs->no_colors = with_col;                                                                    \
    }                                                                                              \
    ((void)0)

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN

// required includes - will go only in one translation unit!
#include <ctime>
#include <cmath>
// borland (Embarcadero) compiler requires math.h and not cmath - https://github.com/onqtam/doctest/pull/37
#ifdef __BORLANDC__
#include <math.h>
#endif // __BORLANDC__
#include <new>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <set>
#include <exception>
#include <stdexcept>
#include <csignal>
#include <cfloat>
#include <cctype>
#if !DOCTEST_MSVC
#include <stdint.h>
#endif // !MSVC

#ifdef __BORLANDC__
namespace std
{
 using ::strcmp;
}
#endif // __BORLANDC__

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

namespace doctest
{
namespace detail
{
    // case insensitive strcmp
    int stricmp(char const* a, char const* b) {
        for(;; a++, b++) {
            using namespace std;
            const int d = tolower(*a) - tolower(*b);
            if(d != 0 || !*a)
                return d;
        }
    }

    void my_memcpy(void* dest, const void* src, unsigned num) { using namespace std; memcpy(dest, src, num); }

    template <typename T>
    String fpToString(T value, int precision) {
        using namespace std;
        std::ostringstream oss;
        oss << std::setprecision(precision) << std::fixed << value;
        std::string d = oss.str();
        size_t      i = d.find_last_not_of('0');
        if(i != std::string::npos && i != d.size() - 1) {
            if(d[i] == '.')
                i++;
            d = d.substr(0, i + 1);
        }
        return d.c_str();
    }

    struct Endianness
    {
        enum Arch
        {
            Big,
            Little
        };

        static Arch which() {
            union _
            {
                int  asInt;
                char asChar[sizeof(int)];
            } u;

            u.asInt = 1;                                            // NOLINT
            return (u.asChar[sizeof(int) - 1] == 1) ? Big : Little; // NOLINT
        }
    };

    String rawMemoryToString(const void* object, unsigned size) {
        // Reverse order for little endian architectures
        int i = 0, end = static_cast<int>(size), inc = 1;
        if(Endianness::which() == Endianness::Little) {
            i   = end - 1;
            end = inc = -1;
        }

        unsigned char const* bytes = static_cast<unsigned char const*>(object);
        std::ostringstream   oss;
        oss << "0x" << std::setfill('0') << std::hex;
        for(; i != end; i += inc)
            oss << std::setw(2) << static_cast<unsigned>(bytes[i]);
        return oss.str().c_str();
    }

    std::ostream* createStream() { std::ostringstream *s = new std::ostringstream(); s->setf(std::ios_base::boolalpha); return s;}
    String        getStreamResult(std::ostream* s) {
        return static_cast<std::ostringstream*>(s)->str().c_str(); // NOLINT
    }
    void freeStream(std::ostream* s) { delete s; }

#ifndef DOCTEST_CONFIG_DISABLE

    // this holds both parameters for the command line and runtime data for tests
    struct ContextState : TestAccessibleContextState //!OCLINT too many fields
    {
        // == parameters from the command line

        std::vector<std::vector<String> > filters;

        String   order_by;  // how tests should be ordered
        unsigned rand_seed; // the seed for rand ordering

        unsigned first; // the first (matching) test to be executed
        unsigned last;  // the last (matching) test to be executed

        int  abort_after;           // stop tests after this many failed assertions
        int  subcase_filter_levels; // apply the subcase filters for the first N levels
        bool case_sensitive;        // if filtering should be case sensitive
        bool exit;          // if the program should be exited after the tests are ran/whatever
        bool duration;      // print the time duration of each test case
        bool no_exitcode;   // if the framework should return 0 as the exitcode
        bool no_run;        // to not run the tests at all (can be done with an "*" exclude)
        bool no_version;    // to not print the version of the framework
        bool no_colors;     // if output to the console should be colorized
        bool force_colors;  // forces the use of colors even when a tty cannot be detected
        bool no_breaks;     // to not break into the debugger
        bool no_skip;       // don't skip test cases which are marked to be skipped
        bool gnu_file_line; // if line numbers should be surrounded with :x: and not (x):
        bool no_path_in_filenames; // if the path to files should be removed from the output
        bool no_line_numbers;      // if source code line numbers should be omitted from the output
        bool no_skipped_summary;   // don't print "skipped" in the summary !!! UNDOCUMENTED !!!

        bool help;             // to print the help
        bool version;          // to print the version
        bool count;            // if only the count of matching tests is to be retreived
        bool list_test_cases;  // to list all tests matching the filters
        bool list_test_suites; // to list all suites matching the filters

        // == data for the tests being ran

        unsigned        numTestsPassingFilters;
        unsigned        numTestSuitesPassingFilters;
        unsigned        numFailed;
        const TestCase* currentTest;
        bool            hasLoggedCurrentTestStart;
        int             numAssertionsForCurrentTestcase;
        int             numAssertions;
        int             numFailedAssertionsForCurrentTestcase;
        int             numFailedAssertions;
        bool            hasCurrentTestFailed;

        std::vector<IContextScope*> contexts;            // for logging with INFO() and friends
        std::vector<std::string>    exceptionalContexts; // logging from INFO() due to an exception

        // stuff for subcases
        std::set<SubcaseSignature> subcasesPassed;
        std::set<int>              subcasesEnteredLevels;
        std::vector<Subcase>       subcasesStack;
        int                        subcasesCurrentLevel;
        bool                       subcasesHasSkipped;

        void resetRunData() {
            numTestsPassingFilters                = 0;
            numTestSuitesPassingFilters           = 0;
            numFailed                             = 0;
            numAssertions                         = 0;
            numFailedAssertions                   = 0;
            numFailedAssertionsForCurrentTestcase = 0;
        }

        // cppcheck-suppress uninitMemberVar
        ContextState()
                : filters(8) // 8 different filters total
        {
            resetRunData();
        }
    };

    ContextState* contextState = 0;
#endif // DOCTEST_CONFIG_DISABLE
} // namespace detail

void String::copy(const String& other) {
    using namespace std;
    if(other.isOnStack()) {
        memcpy(buf, other.buf, len);
    } else {
        setOnHeap();
        data.size     = other.data.size;
        data.capacity = data.size + 1;
        data.ptr      = new char[data.capacity];
        memcpy(data.ptr, other.data.ptr, data.size + 1);
    }
}

String::String(const char* in) {
    using namespace std;
    unsigned in_len = strlen(in);
    if(in_len <= last) {
        memcpy(buf, in, in_len + 1);
        setLast(last - in_len);
    } else {
        setOnHeap();
        data.size     = in_len;
        data.capacity = data.size + 1;
        data.ptr      = new char[data.capacity];
        memcpy(data.ptr, in, in_len + 1);
    }
}

String& String::operator+=(const String& other) {
    using namespace std;
    const unsigned my_old_size = size();
    const unsigned other_size  = other.size();
    const unsigned total_size  = my_old_size + other_size;
    if(isOnStack()) {
        if(total_size < len) {
            // append to the current stack space
            memcpy(buf + my_old_size, other.c_str(), other_size + 1);
            setLast(last - total_size);
        } else {
            // alloc new chunk
            char* temp = new char[total_size + 1];
            // copy current data to new location before writing in the union
            memcpy(temp, buf, my_old_size); // skip the +1 ('\0') for speed
            // update data in union
            setOnHeap();
            data.size     = total_size;
            data.capacity = data.size + 1;
            data.ptr      = temp;
            // transfer the rest of the data
            memcpy(data.ptr + my_old_size, other.c_str(), other_size + 1);
        }
    } else {
        if(data.capacity > total_size) {
            // append to the current heap block
            data.size = total_size;
            memcpy(data.ptr + my_old_size, other.c_str(), other_size + 1);
        } else {
            // resize
            data.capacity *= 2;
            if(data.capacity <= total_size)
                data.capacity = total_size + 1;
            // alloc new chunk
            char* temp = new char[data.capacity];
            // copy current data to new location before releasing it
            memcpy(temp, data.ptr, my_old_size); // skip the +1 ('\0') for speed
            // release old chunk
            delete[] data.ptr;
            // update the rest of the union members
            data.size = total_size;
            data.ptr  = temp;
            // transfer the rest of the data
            memcpy(data.ptr + my_old_size, other.c_str(), other_size + 1);
        }
    }

    return *this;
}

#ifdef DOCTEST_CONFIG_WITH_RVALUE_REFERENCES
String::String(String&& other) {
    memcpy(buf, other.buf, len);
    other.buf[0] = '\0';
    other.setLast();
}

String& String::operator=(String&& other) {
    using namespace std;
    if(this != &other) {
        if(!isOnStack())
            delete[] data.ptr;
        memcpy(buf, other.buf, len);
        other.buf[0] = '\0';
        other.setLast();
    }
    return *this;
}
#endif // DOCTEST_CONFIG_WITH_RVALUE_REFERENCES

int String::compare(const char* other, bool no_case) const {
    if(no_case)
        return detail::stricmp(c_str(), other);
    return std::strcmp(c_str(), other);
}

int String::compare(const String& other, bool no_case) const {
    return compare(other.c_str(), no_case);
}

std::ostream& operator<<(std::ostream& s, const String& in) { return s << in.c_str(); }

Approx::Approx(double value)
        : m_epsilon(static_cast<double>(std::numeric_limits<float>::epsilon()) * 100)
        , m_scale(1.0)
        , m_value(value) {}

bool operator==(double lhs, Approx const& rhs) {
    // Thanks to Richard Harris for his help refining this formula
    using namespace std;
    return fabs(lhs - rhs.m_value) <
           rhs.m_epsilon * (rhs.m_scale + std::max(fabs(lhs), fabs(rhs.m_value)));
}

String Approx::toString() const { return String("Approx( ") + doctest::toString(m_value) + " )"; }

#ifdef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
String toString(char* in) { return toString(static_cast<const char*>(in)); }
String toString(const char* in) { return String("\"") + (in ? in : "{null string}") + "\""; }
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
String toString(bool in) { return in ? "true" : "false"; }
String toString(float in) { return detail::fpToString(in, 5) + "f"; }
String toString(double in) { return detail::fpToString(in, 10); }
String toString(double long in) { return detail::fpToString(in, 15); }

String toString(char in) {
    char buf[64];
    std::sprintf(buf, "%d", in);
    return &buf[0];
}

String toString(char signed in) {
    char buf[64];
    std::sprintf(buf, "%d", in);
    return &buf[0];
}

String toString(char unsigned in) {
    char buf[64];
    std::sprintf(buf, "%ud", in);
    return &buf[0];
}

String toString(int short in) {
    char buf[64];
    std::sprintf(buf, "%d", in);
    return &buf[0];
}

String toString(int short unsigned in) {
    char buf[64];
    std::sprintf(buf, "%u", in);
    return &buf[0];
}

String toString(int in) {
    char buf[64];
    std::sprintf(buf, "%d", in);
    return &buf[0];
}

String toString(int unsigned in) {
    char buf[64];
    std::sprintf(buf, "%u", in);
    return &buf[0];
}

String toString(int long in) {
    char buf[64];
    std::sprintf(buf, "%ld", in);
    return &buf[0];
}

String toString(int long unsigned in) {
    char buf[64];
    std::sprintf(buf, "%lu", in);
    return &buf[0];
}

#ifdef DOCTEST_CONFIG_WITH_LONG_LONG
String toString(int long long in) {
    char buf[64];
    std::sprintf(buf, "%lld", in);
    return &buf[0];
}
String toString(int long long unsigned in) {
    char buf[64];
    std::sprintf(buf, "%llu", in);
    return &buf[0];
}
#endif // DOCTEST_CONFIG_WITH_LONG_LONG

#ifdef DOCTEST_CONFIG_WITH_NULLPTR
String toString(std::nullptr_t) { return "nullptr"; }
#endif // DOCTEST_CONFIG_WITH_NULLPTR

/*template<class T>
String toString(T *in)
{
    char buf[64];
    std::sprintf(buf, "%p", in);
    return &buf[0];
} */

} // namespace doctest

#ifdef DOCTEST_CONFIG_DISABLE
namespace doctest
{
bool isRunningInTest() { return false; }
Context::Context(int, const char* const*) {}
Context::~Context() {}
void Context::applyCommandLine(int, const char* const*) {}
void Context::addFilter(const char*, const char*) {}
void Context::clearFilters() {}
void Context::setOption(const char*, int) {}
void Context::setOption(const char*, const char*) {}
bool Context::shouldExit() { return false; }
int  Context::run() { return 0; }
} // namespace doctest
#else // DOCTEST_CONFIG_DISABLE

#if !defined(DOCTEST_CONFIG_COLORS_NONE)
#if !defined(DOCTEST_CONFIG_COLORS_WINDOWS) && !defined(DOCTEST_CONFIG_COLORS_ANSI)
#ifdef DOCTEST_PLATFORM_WINDOWS
#define DOCTEST_CONFIG_COLORS_WINDOWS
#else // linux
#define DOCTEST_CONFIG_COLORS_ANSI
#endif // platform
#endif // DOCTEST_CONFIG_COLORS_WINDOWS && DOCTEST_CONFIG_COLORS_ANSI
#endif // DOCTEST_CONFIG_COLORS_NONE

#if DOCTEST_MSVC || defined(__MINGW32__)
#if DOCTEST_MSVC >= DOCTEST_COMPILER(17, 0, 0)
#define DOCTEST_WINDOWS_SAL_IN_OPT _In_opt_
#else // MSVC
#define DOCTEST_WINDOWS_SAL_IN_OPT
#endif // MSVC
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA(
        DOCTEST_WINDOWS_SAL_IN_OPT const char*);
extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
#endif // MSVC || __MINGW32__

#ifdef DOCTEST_CONFIG_COLORS_ANSI
#include <unistd.h>
#endif // DOCTEST_CONFIG_COLORS_ANSI

#ifdef DOCTEST_PLATFORM_WINDOWS

// defines for a leaner windows.h
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef VC_EXTRA_LEAN
#define VC_EXTRA_LEAN
#endif // VC_EXTRA_LEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN

// not sure what AfxWin.h is for - here I do what Catch does
#ifdef __AFXDLL
#include <AfxWin.h>
#else
#include <windows.h>
#endif
#include <io.h>

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

#else // DOCTEST_PLATFORM_WINDOWS

#include <sys/time.h>

#endif // DOCTEST_PLATFORM_WINDOWS

namespace doctest_detail_test_suite_ns
{
// holds the current test suite
doctest::detail::TestSuite& getCurrentTestSuite() {
    static doctest::detail::TestSuite data;
    return data;
}
} // namespace doctest_detail_test_suite_ns

namespace doctest
{
namespace detail
{
    TestCase::TestCase(funcType test, const char* file, unsigned line, const TestSuite& test_suite,
                       const char* type, int template_id)
            : m_test(test)
            , m_name(0)
            , m_type(type)
            , m_test_suite(test_suite.m_test_suite)
            , m_description(test_suite.m_description)
            , m_skip(test_suite.m_skip)
            , m_may_fail(test_suite.m_may_fail)
            , m_should_fail(test_suite.m_should_fail)
            , m_expected_failures(test_suite.m_expected_failures)
            , m_timeout(test_suite.m_timeout)
            , m_file(file)
            , m_line(line)
            , m_template_id(template_id) {}

    TestCase& TestCase::operator*(const char* in) {
        m_name = in;
        // make a new name with an appended type for templated test case
        if(m_template_id != -1) {
            m_full_name = String(m_name) + m_type;
            // redirect the name to point to the newly constructed full name
            m_name = m_full_name.c_str();
        }
        return *this;
    }

    TestCase& TestCase::operator=(const TestCase& other) {
        m_test              = other.m_test;
        m_full_name         = other.m_full_name;
        m_name              = other.m_name;
        m_type              = other.m_type;
        m_test_suite        = other.m_test_suite;
        m_description       = other.m_description;
        m_skip              = other.m_skip;
        m_may_fail          = other.m_may_fail;
        m_should_fail       = other.m_should_fail;
        m_expected_failures = other.m_expected_failures;
        m_timeout           = other.m_timeout;
        m_file              = other.m_file;
        m_line              = other.m_line;
        m_template_id       = other.m_template_id;

        if(m_template_id != -1)
            m_name = m_full_name.c_str();
        return *this;
    }

    bool TestCase::operator<(const TestCase& other) const {
        if(m_line != other.m_line)
            return m_line < other.m_line;
        const int file_cmp = std::strcmp(m_file, other.m_file);
        if(file_cmp != 0)
            return file_cmp < 0;
        return m_template_id < other.m_template_id;
    }

    const char* assertString(assertType::Enum val) {
        DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(
                4062) // enumerator 'x' in switch of enum 'y' is not handled
        switch(val) { //!OCLINT missing default in switch statements
            // clang-format off
            case assertType::DT_WARN                    : return "WARN";
            case assertType::DT_CHECK                   : return "CHECK";
            case assertType::DT_REQUIRE                 : return "REQUIRE";

            case assertType::DT_WARN_FALSE              : return "WARN_FALSE";
            case assertType::DT_CHECK_FALSE             : return "CHECK_FALSE";
            case assertType::DT_REQUIRE_FALSE           : return "REQUIRE_FALSE";

            case assertType::DT_WARN_THROWS             : return "WARN_THROWS";
            case assertType::DT_CHECK_THROWS            : return "CHECK_THROWS";
            case assertType::DT_REQUIRE_THROWS          : return "REQUIRE_THROWS";

            case assertType::DT_WARN_THROWS_AS          : return "WARN_THROWS_AS";
            case assertType::DT_CHECK_THROWS_AS         : return "CHECK_THROWS_AS";
            case assertType::DT_REQUIRE_THROWS_AS       : return "REQUIRE_THROWS_AS";

            case assertType::DT_WARN_NOTHROW            : return "WARN_NOTHROW";
            case assertType::DT_CHECK_NOTHROW           : return "CHECK_NOTHROW";
            case assertType::DT_REQUIRE_NOTHROW         : return "REQUIRE_NOTHROW";

            case assertType::DT_WARN_EQ                 : return "WARN_EQ";
            case assertType::DT_CHECK_EQ                : return "CHECK_EQ";
            case assertType::DT_REQUIRE_EQ              : return "REQUIRE_EQ";
            case assertType::DT_WARN_NE                 : return "WARN_NE";
            case assertType::DT_CHECK_NE                : return "CHECK_NE";
            case assertType::DT_REQUIRE_NE              : return "REQUIRE_NE";
            case assertType::DT_WARN_GT                 : return "WARN_GT";
            case assertType::DT_CHECK_GT                : return "CHECK_GT";
            case assertType::DT_REQUIRE_GT              : return "REQUIRE_GT";
            case assertType::DT_WARN_LT                 : return "WARN_LT";
            case assertType::DT_CHECK_LT                : return "CHECK_LT";
            case assertType::DT_REQUIRE_LT              : return "REQUIRE_LT";
            case assertType::DT_WARN_GE                 : return "WARN_GE";
            case assertType::DT_CHECK_GE                : return "CHECK_GE";
            case assertType::DT_REQUIRE_GE              : return "REQUIRE_GE";
            case assertType::DT_WARN_LE                 : return "WARN_LE";
            case assertType::DT_CHECK_LE                : return "CHECK_LE";
            case assertType::DT_REQUIRE_LE              : return "REQUIRE_LE";

            case assertType::DT_WARN_UNARY              : return "WARN_UNARY";
            case assertType::DT_CHECK_UNARY             : return "CHECK_UNARY";
            case assertType::DT_REQUIRE_UNARY           : return "REQUIRE_UNARY";
            case assertType::DT_WARN_UNARY_FALSE        : return "WARN_UNARY_FALSE";
            case assertType::DT_CHECK_UNARY_FALSE       : return "CHECK_UNARY_FALSE";
            case assertType::DT_REQUIRE_UNARY_FALSE     : return "REQUIRE_UNARY_FALSE";

            case assertType::DT_FAST_WARN_EQ            : return "FAST_WARN_EQ";
            case assertType::DT_FAST_CHECK_EQ           : return "FAST_CHECK_EQ";
            case assertType::DT_FAST_REQUIRE_EQ         : return "FAST_REQUIRE_EQ";
            case assertType::DT_FAST_WARN_NE            : return "FAST_WARN_NE";
            case assertType::DT_FAST_CHECK_NE           : return "FAST_CHECK_NE";
            case assertType::DT_FAST_REQUIRE_NE         : return "FAST_REQUIRE_NE";
            case assertType::DT_FAST_WARN_GT            : return "FAST_WARN_GT";
            case assertType::DT_FAST_CHECK_GT           : return "FAST_CHECK_GT";
            case assertType::DT_FAST_REQUIRE_GT         : return "FAST_REQUIRE_GT";
            case assertType::DT_FAST_WARN_LT            : return "FAST_WARN_LT";
            case assertType::DT_FAST_CHECK_LT           : return "FAST_CHECK_LT";
            case assertType::DT_FAST_REQUIRE_LT         : return "FAST_REQUIRE_LT";
            case assertType::DT_FAST_WARN_GE            : return "FAST_WARN_GE";
            case assertType::DT_FAST_CHECK_GE           : return "FAST_CHECK_GE";
            case assertType::DT_FAST_REQUIRE_GE         : return "FAST_REQUIRE_GE";
            case assertType::DT_FAST_WARN_LE            : return "FAST_WARN_LE";
            case assertType::DT_FAST_CHECK_LE           : return "FAST_CHECK_LE";
            case assertType::DT_FAST_REQUIRE_LE         : return "FAST_REQUIRE_LE";

            case assertType::DT_FAST_WARN_UNARY         : return "FAST_WARN_UNARY";
            case assertType::DT_FAST_CHECK_UNARY        : return "FAST_CHECK_UNARY";
            case assertType::DT_FAST_REQUIRE_UNARY      : return "FAST_REQUIRE_UNARY";
            case assertType::DT_FAST_WARN_UNARY_FALSE   : return "FAST_WARN_UNARY_FALSE";
            case assertType::DT_FAST_CHECK_UNARY_FALSE  : return "FAST_CHECK_UNARY_FALSE";
            case assertType::DT_FAST_REQUIRE_UNARY_FALSE: return "FAST_REQUIRE_UNARY_FALSE";
                // clang-format on
        }
        DOCTEST_MSVC_SUPPRESS_WARNING_POP
        return "";
    }

    bool checkIfShouldThrow(assertType::Enum at) {
        if(at & assertType::is_require) //!OCLINT bitwise operator in conditional
            return true;

        if((at & assertType::is_check) //!OCLINT bitwise operator in conditional
           && contextState->abort_after > 0 &&
           contextState->numFailedAssertions >= contextState->abort_after)
            return true;

        return false;
    }
    void throwException() {
#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
        throw TestFailureException();
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS
    }
    void fastAssertThrowIfFlagSet(int flags) {
        if(flags & assertAction::shouldthrow) //!OCLINT bitwise operator in conditional
            throwException();
    }

    // matching of a string against a wildcard mask (case sensitivity configurable) taken from
    // https://www.codeproject.com/Articles/1088/Wildcard-string-compare-globbing
    int wildcmp(const char* str, const char* wild, bool caseSensitive) {
        const char* cp = 0;
        const char* mp = 0;
        using namespace std;
        while((*str) && (*wild != '*')) {
            if((caseSensitive ? (*wild != *str) : (tolower(*wild) != tolower(*str))) &&
               (*wild != '?')) {
                return 0;
            }
            wild++;
            str++;
        }

        while(*str) {
            if(*wild == '*') {
                if(!*++wild) {
                    return 1;
                }
                mp = wild;
                cp = str + 1;
            } else if((caseSensitive ? (*wild == *str) : (tolower(*wild) == tolower(*str))) ||
                      (*wild == '?')) {
                wild++;
                str++;
            } else {
                wild = mp;   //!OCLINT parameter reassignment
                str  = cp++; //!OCLINT parameter reassignment
            }
        }

        while(*wild == '*') {
            wild++;
        }
        return !*wild;
    }

    //// C string hash function (djb2) - taken from http://www.cse.yorku.ca/~oz/hash.html
    //unsigned hashStr(unsigned const char* str) {
    //    unsigned long hash = 5381;
    //    char          c;
    //    while((c = *str++))
    //        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    //    return hash;
    //}

    // checks if the name matches any of the filters (and can be configured what to do when empty)
    bool matchesAny(const char* name, const std::vector<String>& filters, int matchEmpty,
                    bool caseSensitive) {
        if(filters.empty() && matchEmpty)
            return true;
        for(unsigned i = 0; i < filters.size(); ++i)
            if(wildcmp(name, filters[i].c_str(), caseSensitive))
                return true;
        return false;
    }

#ifdef DOCTEST_PLATFORM_WINDOWS

    typedef unsigned long long UInt64;

    UInt64 getCurrentTicks() {
        static UInt64 hz = 0, hzo = 0;
        if(!hz) {
            QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&hz));
            QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&hzo));
        }
        UInt64 t;
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&t));
        return ((t - hzo) * 1000000) / hz;
    }
#else  // DOCTEST_PLATFORM_WINDOWS

    typedef uint64_t UInt64;

    UInt64 getCurrentTicks() {
        timeval t;
        gettimeofday(&t, 0);
        return static_cast<UInt64>(t.tv_sec) * 1000000 + static_cast<UInt64>(t.tv_usec);
    }
#endif // DOCTEST_PLATFORM_WINDOWS

    class Timer
    {
    public:
        Timer()
                : m_ticks(0) {}
        void         start() { m_ticks = getCurrentTicks(); }
        unsigned int getElapsedMicroseconds() const {
            return static_cast<unsigned int>(getCurrentTicks() - m_ticks);
        }
        unsigned int getElapsedMilliseconds() const {
            return static_cast<unsigned int>(getElapsedMicroseconds() / 1000);
        }
        double getElapsedSeconds() const { return getElapsedMicroseconds() / 1000000.0; }

    private:
        UInt64 m_ticks;
    };

    TestAccessibleContextState* getTestsContextState() { return contextState; }

    // TODO: remove this from here
    void logTestEnd();

    bool SubcaseSignature::operator<(const SubcaseSignature& other) const {
        if(m_line != other.m_line)
            return m_line < other.m_line;
        if(std::strcmp(m_file, other.m_file) != 0)
            return std::strcmp(m_file, other.m_file) < 0;
        return std::strcmp(m_name, other.m_name) < 0;
    }

    Subcase::Subcase(const char* name, const char* file, int line)
            : m_signature(name, file, line)
            , m_entered(false) {
        ContextState* s = contextState;

        // if we have already completed it
        if(s->subcasesPassed.count(m_signature) != 0)
            return;

        // check subcase filters
        if(s->subcasesCurrentLevel < s->subcase_filter_levels) {
            if(!matchesAny(m_signature.m_name, s->filters[6], 1, s->case_sensitive))
                return;
            if(matchesAny(m_signature.m_name, s->filters[7], 0, s->case_sensitive))
                return;
        }

        // if a Subcase on the same level has already been entered
        if(s->subcasesEnteredLevels.count(s->subcasesCurrentLevel) != 0) {
            s->subcasesHasSkipped = true;
            return;
        }

        s->subcasesStack.push_back(*this);
        if(s->hasLoggedCurrentTestStart)
            logTestEnd();
        s->hasLoggedCurrentTestStart = false;

        s->subcasesEnteredLevels.insert(s->subcasesCurrentLevel++);
        m_entered = true;
    }

    Subcase::Subcase(const Subcase& other)
            : m_signature(other.m_signature.m_name, other.m_signature.m_file,
                          other.m_signature.m_line)
            , m_entered(other.m_entered) {}

    Subcase::~Subcase() {
        if(m_entered) {
            ContextState* s = contextState;

            s->subcasesCurrentLevel--;
            // only mark the subcase as passed if no subcases have been skipped
            if(s->subcasesHasSkipped == false)
                s->subcasesPassed.insert(m_signature);

            if(!s->subcasesStack.empty())
                s->subcasesStack.pop_back();
            if(s->hasLoggedCurrentTestStart)
                logTestEnd();
            s->hasLoggedCurrentTestStart = false;
        }
    }

    Result::~Result() {}

    Result& Result::operator=(const Result& other) {
        m_passed        = other.m_passed;
        m_decomposition = other.m_decomposition;

        return *this;
    }

    // for sorting tests by file/line
    int fileOrderComparator(const void* a, const void* b) {
        const TestCase* lhs = *static_cast<TestCase* const*>(a);
        const TestCase* rhs = *static_cast<TestCase* const*>(b);
#if DOCTEST_MSVC
        // this is needed because MSVC gives different case for drive letters
        // for __FILE__ when evaluated in a header and a source file
        const int res = stricmp(lhs->m_file, rhs->m_file);
#else  // MSVC
        const int res = std::strcmp(lhs->m_file, rhs->m_file);
#endif // MSVC
        if(res != 0)
            return res;
        return static_cast<int>(lhs->m_line - rhs->m_line);
    }

    // for sorting tests by suite/file/line
    int suiteOrderComparator(const void* a, const void* b) {
        const TestCase* lhs = *static_cast<TestCase* const*>(a);
        const TestCase* rhs = *static_cast<TestCase* const*>(b);

        const int res = std::strcmp(lhs->m_test_suite, rhs->m_test_suite);
        if(res != 0)
            return res;
        return fileOrderComparator(a, b);
    }

    // for sorting tests by name/suite/file/line
    int nameOrderComparator(const void* a, const void* b) {
        const TestCase* lhs = *static_cast<TestCase* const*>(a);
        const TestCase* rhs = *static_cast<TestCase* const*>(b);

        const int res_name = std::strcmp(lhs->m_name, rhs->m_name);
        if(res_name != 0)
            return res_name;
        return suiteOrderComparator(a, b);
    }

    // sets the current test suite
    int setTestSuite(const TestSuite& ts) {
        doctest_detail_test_suite_ns::getCurrentTestSuite() = ts;
        return 0;
    }

    // all the registered tests
    std::set<TestCase>& getRegisteredTests() {
        static std::set<TestCase> data;
        return data;
    }

    // used by the macros for registering tests
    int regTest(const TestCase& tc) {
        getRegisteredTests().insert(tc);
        return 0;
    }

    namespace Color
    {
        enum Code
        {
            None = 0,
            White,
            Red,
            Green,
            Blue,
            Cyan,
            Yellow,
            Grey,

            Bright = 0x10,

            BrightRed   = Bright | Red,
            BrightGreen = Bright | Green,
            LightGrey   = Bright | Grey,
            BrightWhite = Bright | White
        };

#ifdef DOCTEST_CONFIG_COLORS_WINDOWS
        HANDLE g_stdoutHandle;
        WORD   g_originalForegroundAttributes;
        WORD   g_originalBackgroundAttributes;
        bool   g_attrsInitted = false;
#endif // DOCTEST_CONFIG_COLORS_WINDOWS

        void init() {
#ifdef DOCTEST_CONFIG_COLORS_WINDOWS
            if(!g_attrsInitted) {
                g_stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
                g_attrsInitted = true;
                CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
                GetConsoleScreenBufferInfo(g_stdoutHandle, &csbiInfo);
                g_originalForegroundAttributes =
                        csbiInfo.wAttributes & ~(BACKGROUND_GREEN | BACKGROUND_RED |
                                                 BACKGROUND_BLUE | BACKGROUND_INTENSITY);
                g_originalBackgroundAttributes =
                        csbiInfo.wAttributes & ~(FOREGROUND_GREEN | FOREGROUND_RED |
                                                 FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            }
#endif // DOCTEST_CONFIG_COLORS_WINDOWS
        }

        std::ostream& operator<<(std::ostream&            s, Color::Code
#ifndef DOCTEST_CONFIG_COLORS_NONE
                                                          code
#endif // DOCTEST_CONFIG_COLORS_NONE
        ) {
            const ContextState* p = contextState;
            if(p->no_colors)
                return s;
#ifdef DOCTEST_CONFIG_COLORS_ANSI
            if(isatty(STDOUT_FILENO) == false && p->force_colors == false)
                return s;

            const char* col = "";
            // clang-format off
            switch(code) { //!OCLINT missing break in switch statement / unnecessary default statement in covered switch statement
                case Color::Red:         col = "[0;31m"; break;
                case Color::Green:       col = "[0;32m"; break;
                case Color::Blue:        col = "[0;34m"; break;
                case Color::Cyan:        col = "[0;36m"; break;
                case Color::Yellow:      col = "[0;33m"; break;
                case Color::Grey:        col = "[1;30m"; break;
                case Color::LightGrey:   col = "[0;37m"; break;
                case Color::BrightRed:   col = "[1;31m"; break;
                case Color::BrightGreen: col = "[1;32m"; break;
                case Color::BrightWhite: col = "[1;37m"; break;
                case Color::Bright: // invalid
                case Color::None:
                case Color::White:
                default:                 col = "[0m";
            }
            // clang-format on
            s << "\033" << col;
#endif // DOCTEST_CONFIG_COLORS_ANSI

#ifdef DOCTEST_CONFIG_COLORS_WINDOWS
            if(isatty(fileno(stdout)) == false && p->force_colors == false)
                return s;

#define DOCTEST_SET_ATTR(x)                                                                        \
    SetConsoleTextAttribute(g_stdoutHandle, x | g_originalBackgroundAttributes)

            // clang-format off
            switch (code) {
                case Color::White:       DOCTEST_SET_ATTR(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE); break;
                case Color::Red:         DOCTEST_SET_ATTR(FOREGROUND_RED);                                      break;
                case Color::Green:       DOCTEST_SET_ATTR(FOREGROUND_GREEN);                                    break;
                case Color::Blue:        DOCTEST_SET_ATTR(FOREGROUND_BLUE);                                     break;
                case Color::Cyan:        DOCTEST_SET_ATTR(FOREGROUND_BLUE | FOREGROUND_GREEN);                  break;
                case Color::Yellow:      DOCTEST_SET_ATTR(FOREGROUND_RED | FOREGROUND_GREEN);                   break;
                case Color::Grey:        DOCTEST_SET_ATTR(0);                                                   break;
                case Color::LightGrey:   DOCTEST_SET_ATTR(FOREGROUND_INTENSITY);                                break;
                case Color::BrightRed:   DOCTEST_SET_ATTR(FOREGROUND_INTENSITY | FOREGROUND_RED);               break;
                case Color::BrightGreen: DOCTEST_SET_ATTR(FOREGROUND_INTENSITY | FOREGROUND_GREEN);             break;
                case Color::BrightWhite: DOCTEST_SET_ATTR(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE); break;
                case Color::None:
                case Color::Bright: // invalid
                default:                 DOCTEST_SET_ATTR(g_originalForegroundAttributes);
            }
                // clang-format on
#undef DOCTEST_SET_ATTR
#endif // DOCTEST_CONFIG_COLORS_WINDOWS
            return s;
        }
    } // namespace Color

    std::vector<const IExceptionTranslator*>& getExceptionTranslators() {
        static std::vector<const IExceptionTranslator*> data;
        return data;
    }

    void registerExceptionTranslatorImpl(const IExceptionTranslator* translateFunction) {
        if(std::find(getExceptionTranslators().begin(), getExceptionTranslators().end(),
                     translateFunction) == getExceptionTranslators().end())
            getExceptionTranslators().push_back(translateFunction);
    }

    String translateActiveException() {
#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
        String                                    res;
        std::vector<const IExceptionTranslator*>& translators = getExceptionTranslators();
        for(size_t i = 0; i < translators.size(); ++i)
            if(translators[i]->translate(res))
                return res;
        // clang-format off
        try {
            throw;
        } catch(std::exception& ex) {
            return ex.what();
        } catch(std::string& msg) {
            return msg.c_str();
        } catch(const char* msg) {
            return msg;
        } catch(...) {
            return "unknown exception";
        }
// clang-format on
#else  // DOCTEST_CONFIG_NO_EXCEPTIONS
        return "";
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS
    }

    void writeStringToStream(std::ostream* s, const String& str) { *s << str; }

#ifdef DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
    void toStream(std::ostream* s, char* in) { *s << in; }
    void toStream(std::ostream* s, const char* in) { *s << in; }
#endif // DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
    void toStream(std::ostream* s, bool in) { *s << std::boolalpha << in << std::noboolalpha; }
    void toStream(std::ostream* s, float in) { *s << in; }
    void toStream(std::ostream* s, double in) { *s << in; }
    void toStream(std::ostream* s, double long in) { *s << in; }

    void toStream(std::ostream* s, char in) { *s << in; }
    void toStream(std::ostream* s, char signed in) { *s << in; }
    void toStream(std::ostream* s, char unsigned in) { *s << in; }
    void toStream(std::ostream* s, int short in) { *s << in; }
    void toStream(std::ostream* s, int short unsigned in) { *s << in; }
    void toStream(std::ostream* s, int in) { *s << in; }
    void toStream(std::ostream* s, int unsigned in) { *s << in; }
    void toStream(std::ostream* s, int long in) { *s << in; }
    void toStream(std::ostream* s, int long unsigned in) { *s << in; }

#ifdef DOCTEST_CONFIG_WITH_LONG_LONG
    void toStream(std::ostream* s, int long long in) { *s << in; }
    void toStream(std::ostream* s, int long long unsigned in) { *s << in; }
#endif // DOCTEST_CONFIG_WITH_LONG_LONG

    void addToContexts(IContextScope* ptr) { contextState->contexts.push_back(ptr); }
    void popFromContexts() { contextState->contexts.pop_back(); }
    DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4996) // std::uncaught_exception is deprecated in C++17
    DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wdeprecated-declarations")
    void useContextIfExceptionOccurred(IContextScope* ptr) {
        if(std::uncaught_exception()) {
            std::ostringstream s;
            ptr->build(&s);
            contextState->exceptionalContexts.push_back(s.str());
        }
    }
    DOCTEST_GCC_SUPPRESS_WARNING_POP
    DOCTEST_MSVC_SUPPRESS_WARNING_POP

    void printSummary(std::ostream& s);

#if !defined(DOCTEST_CONFIG_POSIX_SIGNALS) && !defined(DOCTEST_CONFIG_WINDOWS_SEH)
    void reportFatal(const std::string&) {}
    struct FatalConditionHandler
    {
        void reset() {}
    };
#else // DOCTEST_CONFIG_POSIX_SIGNALS || DOCTEST_CONFIG_WINDOWS_SEH

    void reportFatal(const std::string&);

#ifdef DOCTEST_PLATFORM_WINDOWS

    struct SignalDefs
    {
        DWORD       id;
        const char* name;
    };
    // There is no 1-1 mapping between signals and windows exceptions.
    // Windows can easily distinguish between SO and SigSegV,
    // but SigInt, SigTerm, etc are handled differently.
    SignalDefs signalDefs[] = {
            {EXCEPTION_ILLEGAL_INSTRUCTION, "SIGILL - Illegal instruction signal"},
            {EXCEPTION_STACK_OVERFLOW, "SIGSEGV - Stack overflow"},
            {EXCEPTION_ACCESS_VIOLATION, "SIGSEGV - Segmentation violation signal"},
            {EXCEPTION_INT_DIVIDE_BY_ZERO, "Divide by zero error"},
    };

    struct FatalConditionHandler
    {
        static LONG CALLBACK handleVectoredException(PEXCEPTION_POINTERS ExceptionInfo) {
            for(size_t i = 0; i < DOCTEST_COUNTOF(signalDefs); ++i) {
                if(ExceptionInfo->ExceptionRecord->ExceptionCode == signalDefs[i].id) {
                    reportFatal(signalDefs[i].name);
                }
            }
            // If its not an exception we care about, pass it along.
            // This stops us from eating debugger breaks etc.
            return EXCEPTION_CONTINUE_SEARCH;
        }

        FatalConditionHandler() {
            isSet = true;
            // 32k seems enough for doctest to handle stack overflow,
            // but the value was found experimentally, so there is no strong guarantee
            guaranteeSize          = 32 * 1024;
            exceptionHandlerHandle = 0;
            // Register as first handler in current chain
            exceptionHandlerHandle = AddVectoredExceptionHandler(1, handleVectoredException);
            // Pass in guarantee size to be filled
            SetThreadStackGuarantee(&guaranteeSize);
        }

        static void reset() {
            if(isSet) {
                // Unregister handler and restore the old guarantee
                RemoveVectoredExceptionHandler(exceptionHandlerHandle);
                SetThreadStackGuarantee(&guaranteeSize);
                exceptionHandlerHandle = 0;
                isSet                  = false;
            }
        }

        ~FatalConditionHandler() { reset(); }

    private:
        static bool  isSet;
        static ULONG guaranteeSize;
        static PVOID exceptionHandlerHandle;
    };

    bool  FatalConditionHandler::isSet                  = false;
    ULONG FatalConditionHandler::guaranteeSize          = 0;
    PVOID FatalConditionHandler::exceptionHandlerHandle = 0;

#else // DOCTEST_PLATFORM_WINDOWS

    struct SignalDefs
    {
        int         id;
        const char* name;
    };
    SignalDefs signalDefs[] = {{SIGINT, "SIGINT - Terminal interrupt signal"},
                               {SIGILL, "SIGILL - Illegal instruction signal"},
                               {SIGFPE, "SIGFPE - Floating point error signal"},
                               {SIGSEGV, "SIGSEGV - Segmentation violation signal"},
                               {SIGTERM, "SIGTERM - Termination request signal"},
                               {SIGABRT, "SIGABRT - Abort (abnormal termination) signal"}};

    struct FatalConditionHandler
    {
        static bool             isSet;
        static struct sigaction oldSigActions[DOCTEST_COUNTOF(signalDefs)];
        static stack_t          oldSigStack;
        static char             altStackMem[4 * SIGSTKSZ];

        static void handleSignal(int sig) {
            std::string name = "<unknown signal>";
            for(std::size_t i = 0; i < DOCTEST_COUNTOF(signalDefs); ++i) {
                SignalDefs& def = signalDefs[i];
                if(sig == def.id) {
                    name = def.name;
                    break;
                }
            }
            reset();
            reportFatal(name);
            raise(sig);
        }

        FatalConditionHandler() {
            isSet = true;
            stack_t sigStack;
            sigStack.ss_sp    = altStackMem;
            sigStack.ss_size  = sizeof(altStackMem);
            sigStack.ss_flags = 0;
            sigaltstack(&sigStack, &oldSigStack);
            struct sigaction sa = {0};

            sa.sa_handler = handleSignal; // NOLINT
            sa.sa_flags   = SA_ONSTACK;
            for(std::size_t i = 0; i < DOCTEST_COUNTOF(signalDefs); ++i) {
                sigaction(signalDefs[i].id, &sa, &oldSigActions[i]);
            }
        }

        ~FatalConditionHandler() { reset(); }
        static void reset() {
            if(isSet) {
                // Set signals back to previous values -- hopefully nobody overwrote them in the meantime
                for(std::size_t i = 0; i < DOCTEST_COUNTOF(signalDefs); ++i) {
                    sigaction(signalDefs[i].id, &oldSigActions[i], 0);
                }
                // Return the old stack
                sigaltstack(&oldSigStack, 0);
                isSet = false;
            }
        }
    };

    bool             FatalConditionHandler::isSet                                      = false;
    struct sigaction FatalConditionHandler::oldSigActions[DOCTEST_COUNTOF(signalDefs)] = {};
    stack_t          FatalConditionHandler::oldSigStack                                = {};
    char             FatalConditionHandler::altStackMem[]                              = {};

#endif // DOCTEST_PLATFORM_WINDOWS
#endif // DOCTEST_CONFIG_POSIX_SIGNALS || DOCTEST_CONFIG_WINDOWS_SEH

    void separator_to_stream(std::ostream& s) {
        s << Color::Yellow
          << "===============================================================================\n";
    }

    // depending on the current options this will remove the path of filenames
    const char* fileForOutput(const char* file) {
        if(contextState->no_path_in_filenames) {
            const char* back    = std::strrchr(file, '\\');
            const char* forward = std::strrchr(file, '/');
            if(back || forward) {
                if(back > forward)
                    forward = back;
                return forward + 1;
            }
        }
        return file;
    }

    void file_line_to_stream(std::ostream& s, const char* file, int line, const char* tail = "") {
        s << Color::LightGrey << fileForOutput(file) << (contextState->gnu_file_line ? ":" : "(")
          << (contextState->no_line_numbers ? 0 : line) // 0 or the real num depending on the option
          << (contextState->gnu_file_line ? ":" : "):") << tail;
    }

    const char* getSuccessOrFailString(bool success, assertType::Enum at, const char* success_str) {
        if(success)
            return success_str;
        if(at & assertType::is_warn) //!OCLINT bitwise operator in conditional
            return "WARNING: ";
        if(at & assertType::is_check) //!OCLINT bitwise operator in conditional
            return "ERROR: ";
        if(at & assertType::is_require) //!OCLINT bitwise operator in conditional
            return "FATAL ERROR: ";
        return "";
    }

    Color::Code getSuccessOrFailColor(bool success, assertType::Enum at) {
        return success ? Color::BrightGreen :
                         (at & assertType::is_warn) ? Color::Yellow : Color::Red;
    }

    void successOrFailColoredStringToStream(std::ostream& s, bool success, assertType::Enum at,
                                            const char* success_str = "SUCCESS: ") {
        s << getSuccessOrFailColor(success, at) << getSuccessOrFailString(success, at, success_str);
    }

#ifdef DOCTEST_PLATFORM_MAC
#include <sys/types.h>
#include <unistd.h>
#include <sys/sysctl.h>
    // The following function is taken directly from the following technical note:
    // http://developer.apple.com/library/mac/#qa/qa2004/qa1361.html
    // Returns true if the current process is being debugged (either
    // running under the debugger or has a debugger attached post facto).
    bool isDebuggerActive() {
        int        mib[4];
        kinfo_proc info;
        size_t     size;
        // Initialize the flags so that, if sysctl fails for some bizarre
        // reason, we get a predictable result.
        info.kp_proc.p_flag = 0;
        // Initialize mib, which tells sysctl the info we want, in this case
        // we're looking for information about a specific process ID.
        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC;
        mib[2] = KERN_PROC_PID;
        mib[3] = getpid();
        // Call sysctl.
        size = sizeof(info);
        if(sysctl(mib, DOCTEST_COUNTOF(mib), &info, &size, 0, 0) != 0) {
            fprintf(stderr, "\n** Call to sysctl failed - unable to determine if debugger is "
                            "active **\n\n");
            return false;
        }
        // We're being debugged if the P_TRACED flag is set.
        return ((info.kp_proc.p_flag & P_TRACED) != 0);
    }
#elif DOCTEST_MSVC || defined(__MINGW32__)
    bool  isDebuggerActive() { return ::IsDebuggerPresent() != 0; }
#else
    bool isDebuggerActive() { return false; }
#endif // Platform

#ifdef DOCTEST_PLATFORM_WINDOWS
    void myOutputDebugString(const String& text) { ::OutputDebugStringA(text.c_str()); }
#else
    // TODO: integration with XCode and other IDEs
    void myOutputDebugString(const String&) {}
#endif // Platform

    void printToDebugConsole(const String& text) {
        if(isDebuggerActive())
            myOutputDebugString(text);
    }

    void addFailedAssert(assertType::Enum at) {
        if((at & assertType::is_warn) == 0) { //!OCLINT bitwise operator in conditional
            contextState->numFailedAssertions++;
            contextState->numFailedAssertionsForCurrentTestcase++;
            contextState->hasCurrentTestFailed = true;
        }
    }

    std::ostream& operator<<(std::ostream& s, const std::vector<IContextScope*>& contexts) {
        if(!contexts.empty())
            s << Color::None << "  logged: ";
        for(size_t i = 0; i < contexts.size(); ++i) {
            s << (i == 0 ? "" : "          ");
            contexts[i]->build(&s);
            s << "\n";
        }
        s << "\n";
        return s;
    }

    void logTestStart(std::ostream& s, const TestCase& tc) {
        using namespace std;
        separator_to_stream(s);
        file_line_to_stream(s, tc.m_file, tc.m_line, "\n");
        if(tc.m_description)
            s << Color::Yellow << "DESCRIPTION: " << Color::None << tc.m_description << "\n";
        if(tc.m_test_suite && tc.m_test_suite[0] != '\0')
            s << Color::Yellow << "TEST SUITE: " << Color::None << tc.m_test_suite << "\n";
        if(strncmp(tc.m_name, "  Scenario:", 11) != 0)
            s << Color::None << "TEST CASE:  ";
        s << Color::None << tc.m_name << "\n";

        std::vector<Subcase>& subcasesStack = contextState->subcasesStack;
        for(unsigned i = 0; i < subcasesStack.size(); ++i)
            if(subcasesStack[i].m_signature.m_name[0] != '\0')
                s << "  " << subcasesStack[i].m_signature.m_name << "\n";

        s << "\n";
    }

    void logTestEnd() {}

    void logTestException_impl(std::ostream& s, const TestCase& tc, const String& str, bool crash) {
        file_line_to_stream(s, tc.m_file, tc.m_line, " ");
        successOrFailColoredStringToStream(s, false,
                                           crash ? assertType::is_require : assertType::is_check);
        s << Color::Red << (crash ? "test case CRASHED: " : "test case THREW exception: ")
          << Color::Cyan << str << "\n";

        if(!contextState->exceptionalContexts.empty()) {
            s << Color::None << "  logged: ";
            for(size_t i = contextState->exceptionalContexts.size(); i > 0; --i)
                s << (i == contextState->exceptionalContexts.size() ? "" : "          ")
                  << contextState->exceptionalContexts[i - 1] << "\n";
        }
        s << "\n";
    }

    void logTestException(const TestCase& tc, const String& what, bool crash) {
        logTestException_impl(std::cout, tc, what, crash);
        DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_BEGIN;
        logTestException_impl(oss, tc, what, crash);
        DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_END;
    }

#if defined(DOCTEST_CONFIG_POSIX_SIGNALS) || defined(DOCTEST_CONFIG_WINDOWS_SEH)
    void reportFatal(const std::string& message) {
        DOCTEST_LOG_START(std::cout);

        contextState->numAssertions += contextState->numAssertionsForCurrentTestcase;
        logTestException(*contextState->currentTest, message.c_str(), true);
        logTestEnd();
        contextState->numFailed++;

        printSummary(std::cout);
    }
#endif // DOCTEST_CONFIG_POSIX_SIGNALS || DOCTEST_CONFIG_WINDOWS_SEH

    void logAssert_impl(std::ostream& s, bool passed, const String& dec, bool threw,
                        const String& ex, const char* expr, assertType::Enum at, const char* file,
                        int line) {
        file_line_to_stream(s, file, line, " ");
        successOrFailColoredStringToStream(s, passed, at);
        s << Color::Cyan << assertString(at) << "( " << expr << " ) " << Color::None
          << (threw ? "THREW exception: " : (passed ? "is correct!\n" : "is NOT correct!\n"));
        if(threw)
            s << ex << "\n";
        else
            s << "  values: " << assertString(at) << "( " << dec << " )\n";
        s << contextState->contexts;
    }

    void logAssert(bool passed, const String& dec, bool threw, const String& ex, const char* expr,
                   assertType::Enum at, const char* file, int line) {
        logAssert_impl(std::cout, passed, dec, threw, ex, expr, at, file, line);
        DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_BEGIN;
        logAssert_impl(oss, passed, dec, threw, ex, expr, at, file, line);
        DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_END;
    }

    void logAssertThrows_impl(std::ostream& s, bool threw, const char* expr, assertType::Enum at,
                              const char* file, int line) {
        file_line_to_stream(s, file, line, " ");
        successOrFailColoredStringToStream(s, threw, at);
        s << Color::Cyan << assertString(at) << "( " << expr << " ) " << Color::None
          << (threw ? "threw as expected!" : "did NOT throw at all!") << "\n";
        s << contextState->contexts;
    }

    void logAssertThrows(bool threw, const char* expr, assertType::Enum at, const char* file,
                         int line) {
        logAssertThrows_impl(std::cout, threw, expr, at, file, line);
        DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_BEGIN;
        logAssertThrows_impl(oss, threw, expr, at, file, line);
        DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_END;
    }

    void logAssertThrowsAs_impl(std::ostream& s, bool threw, bool threw_as, const char* as,
                                const String& ex, const char* expr, assertType::Enum at,
                                const char* file, int line) {
        file_line_to_stream(s, file, line, " ");
        successOrFailColoredStringToStream(s, threw_as, at);
        s << Color::Cyan << assertString(at) << "( " << expr << ", " << as << " ) " << Color::None
          << (threw ? (threw_as ? "threw as expected!" : "threw a DIFFERENT exception: ") :
                      "did NOT throw at all!")
          << Color::Cyan << ex << "\n";
        s << contextState->contexts;
    }

    void logAssertThrowsAs(bool threw, bool threw_as, const char* as, const String& ex,
                           const char* expr, assertType::Enum at, const char* file, int line) {
        logAssertThrowsAs_impl(std::cout, threw, threw_as, as, ex, expr, at, file, line);
        DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_BEGIN;
        logAssertThrowsAs_impl(oss, threw, threw_as, as, ex, expr, at, file, line);
        DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_END;
    }

    void logAssertNothrow_impl(std::ostream& s, bool threw, const String& ex, const char* expr,
                               assertType::Enum at, const char* file, int line) {
        file_line_to_stream(s, file, line, " ");
        successOrFailColoredStringToStream(s, !threw, at);
        s << Color::Cyan << assertString(at) << "( " << expr << " ) " << Color::None
          << (threw ? "THREW exception: " : "didn't throw!") << Color::Cyan << ex << "\n";
        s << contextState->contexts;
    }

    void logAssertNothrow(bool threw, const String& ex, const char* expr, assertType::Enum at,
                          const char* file, int line) {
        logAssertNothrow_impl(std::cout, threw, ex, expr, at, file, line);
        DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_BEGIN;
        logAssertNothrow_impl(oss, threw, ex, expr, at, file, line);
        DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_END;
    }

    ResultBuilder::ResultBuilder(assertType::Enum at, const char* file, int line, const char* expr,
                                 const char* exception_type)
            : m_assert_type(at)
            , m_file(file)
            , m_line(line)
            , m_expr(expr)
            , m_exception_type(exception_type)
            , m_threw(false)
            , m_threw_as(false)
            , m_failed(false) {
#if DOCTEST_MSVC
        if(m_expr[0] == ' ') // this happens when variadic macros are disabled under MSVC
            ++m_expr;
#endif // MSVC
    }

    ResultBuilder::~ResultBuilder() {}

    void ResultBuilder::unexpectedExceptionOccurred() {
        m_threw = true;

        m_exception = translateActiveException();
    }

    bool ResultBuilder::log() {
        if((m_assert_type & assertType::is_warn) == 0) //!OCLINT bitwise operator in conditional
            contextState->numAssertionsForCurrentTestcase++;

        if(m_assert_type & assertType::is_throws) { //!OCLINT bitwise operator in conditional
            m_failed = !m_threw;
        } else if(m_assert_type & //!OCLINT bitwise operator in conditional
                  assertType::is_throws_as) {
            m_failed = !m_threw_as;
        } else if(m_assert_type & //!OCLINT bitwise operator in conditional
                  assertType::is_nothrow) {
            m_failed = m_threw;
        } else {
            m_failed = m_result;
        }

        if(m_failed || contextState->success) {
            DOCTEST_LOG_START(std::cout);

            if(m_assert_type & assertType::is_throws) { //!OCLINT bitwise operator in conditional
                logAssertThrows(m_threw, m_expr, m_assert_type, m_file, m_line);
            } else if(m_assert_type & //!OCLINT bitwise operator in conditional
                      assertType::is_throws_as) {
                logAssertThrowsAs(m_threw, m_threw_as, m_exception_type, m_exception, m_expr,
                                  m_assert_type, m_file, m_line);
            } else if(m_assert_type & //!OCLINT bitwise operator in conditional
                      assertType::is_nothrow) {
                logAssertNothrow(m_threw, m_exception, m_expr, m_assert_type, m_file, m_line);
            } else {
                logAssert(m_result.m_passed, m_result.m_decomposition, m_threw, m_exception, m_expr,
                          m_assert_type, m_file, m_line);
            }
        }

        if(m_failed)
            addFailedAssert(m_assert_type);

        return m_failed && isDebuggerActive() && !contextState->no_breaks; // break into debugger
    }

    void ResultBuilder::react() const {
        if(m_failed && checkIfShouldThrow(m_assert_type))
            throwException();
    }

    MessageBuilder::MessageBuilder(const char* file, int line, assertType::Enum severity)
            : m_stream(createStream())
            , m_file(file)
            , m_line(line)
            , m_severity(severity) {}

    void MessageBuilder::log(std::ostream& s) {
        file_line_to_stream(s, m_file, m_line, " ");
        s << getSuccessOrFailColor(false, m_severity)
          << getSuccessOrFailString(m_severity & assertType::is_warn, m_severity, "MESSAGE: ");
        s << Color::None << getStreamResult(m_stream) << "\n";
        s << contextState->contexts;
    }

    bool MessageBuilder::log() {
        DOCTEST_LOG_START(std::cout);

        log(std::cout);
        DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_BEGIN;
        log(oss);
        DOCTEST_PRINT_TO_OUTPUT_WINDOW_IN_IDE_END;
        const bool isWarn = m_severity & assertType::is_warn;

        // warn is just a message in this context so we dont treat it as an assert
        if(!isWarn) {
            contextState->numAssertionsForCurrentTestcase++;
            addFailedAssert(m_severity);
        }

        return isDebuggerActive() && !contextState->no_breaks && !isWarn; // break into debugger
    }

    void MessageBuilder::react() {
        if(m_severity & assertType::is_require) //!OCLINT bitwise operator in conditional
            throwException();
    }

    MessageBuilder::~MessageBuilder() { freeStream(m_stream); }

    // the implementation of parseFlag()
    bool parseFlagImpl(int argc, const char* const* argv, const char* pattern) {
        using namespace std;
        for(int i = argc - 1; i >= 0; --i) {
            const char* temp = std::strstr(argv[i], pattern);
            if(temp && strlen(temp) == strlen(pattern)) {
                // eliminate strings in which the chars before the option are not '-'
                bool noBadCharsFound = true; //!OCLINT prefer early exits and continue
                while(temp != argv[i]) {
                    if(*--temp != '-') {
                        noBadCharsFound = false;
                        break;
                    }
                }
                if(noBadCharsFound && argv[i][0] == '-')
                    return true;
            }
        }
        return false;
    }

    // locates a flag on the command line
    bool parseFlag(int argc, const char* const* argv, const char* pattern) {
#ifndef DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
        if(!parseFlagImpl(argc, argv, pattern))
            return parseFlagImpl(argc, argv, pattern + 3); // 3 for "dt-"
        return true;
#else  // DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
        return parseFlagImpl(argc, argv, pattern);
#endif // DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
    }

    // the implementation of parseOption()
    bool parseOptionImpl(int argc, const char* const* argv, const char* pattern, String& res) {
        using namespace std;
        for(int i = argc - 1; i >= 0; --i) {
            const char* temp = std::strstr(argv[i], pattern);
            if(temp) { //!OCLINT prefer early exits and continue
                // eliminate matches in which the chars before the option are not '-'
                bool        noBadCharsFound = true;
                const char* curr            = argv[i];
                while(curr != temp) {
                    if(*curr++ != '-') {
                        noBadCharsFound = false;
                        break;
                    }
                }
                if(noBadCharsFound && argv[i][0] == '-') {
                    temp += strlen(pattern);
                    const unsigned len = strlen(temp);
                    if(len) {
                        res = temp;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    // parses an option and returns the string after the '=' character
    bool parseOption(int argc, const char* const* argv, const char* pattern, String& res,
                     const String& defaultVal = String()) {
        res = defaultVal;
#ifndef DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
        if(!parseOptionImpl(argc, argv, pattern, res))
            return parseOptionImpl(argc, argv, pattern + 3, res); // 3 for "dt-"
        return true;
#else  // DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
        return parseOptionImpl(argc, argv, pattern, res);
#endif // DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
    }

    // parses a comma separated list of words after a pattern in one of the arguments in argv
    bool parseCommaSepArgs(int argc, const char* const* argv, const char* pattern,
                           std::vector<String>& res) {
        using namespace std;
        String filtersString;
        if(parseOption(argc, argv, pattern, filtersString)) {
            // tokenize with "," as a separator
            // cppcheck-suppress strtokCalled
            char* pch = std::strtok(filtersString.c_str(), ","); // modifies the string
            while(pch != 0) {
                if(strlen(pch))
                    res.push_back(pch);
                // uses the strtok() internal state to go to the next token
                // cppcheck-suppress strtokCalled
                pch = std::strtok(0, ",");
            }
            return true;
        }
        return false;
    }

    enum optionType
    {
        option_bool,
        option_int
    };

    // parses an int/bool option from the command line
    bool parseIntOption(int argc, const char* const* argv, const char* pattern, optionType type,
                        int& res) {
        String parsedValue;
        if(!parseOption(argc, argv, pattern, parsedValue))
            return false;

        if(type == 0) {
            // boolean
            const char positive[][5] = {"1", "true", "on", "yes"};  // 5 - strlen("true") + 1
            const char negative[][6] = {"0", "false", "off", "no"}; // 6 - strlen("false") + 1

            // if the value matches any of the positive/negative possibilities
            for(unsigned i = 0; i < 4; i++) {
                if(parsedValue.compare(positive[i], true) == 0) {
                    res = 1; //!OCLINT parameter reassignment
                    return true;
                }
                if(parsedValue.compare(negative[i], true) == 0) {
                    res = 0; //!OCLINT parameter reassignment
                    return true;
                }
            }
        } else {
            // integer
            int theInt = std::atoi(parsedValue.c_str()); // NOLINT
            if(theInt != 0) {
                res = theInt; //!OCLINT parameter reassignment
                return true;
            }
        }
        return false;
    }

    void printVersion(std::ostream& s) {
        if(contextState->no_version == false)
            s << Color::Cyan << "[doctest] " << Color::None << "doctest version is \""
              << DOCTEST_VERSION_STR << "\"\n";
    }

    void printHelp(std::ostream& s) {
        printVersion(s);
        // clang-format off
        s << Color::Cyan << "[doctest]\n" << Color::None;
        s << Color::Cyan << "[doctest] " << Color::None;
        s << "boolean values: \"1/on/yes/true\" or \"0/off/no/false\"\n";
        s << Color::Cyan << "[doctest] " << Color::None;
        s << "filter  values: \"str1,str2,str3\" (comma separated strings)\n";
        s << Color::Cyan << "[doctest]\n" << Color::None;
        s << Color::Cyan << "[doctest] " << Color::None;
        s << "filters use wildcards for matching strings\n";
        s << Color::Cyan << "[doctest] " << Color::None;
        s << "something passes a filter if any of the strings in a filter matches\n";
        s << Color::Cyan << "[doctest]\n" << Color::None;
        s << Color::Cyan << "[doctest] " << Color::None;
        s << "ALL FLAGS, OPTIONS AND FILTERS ALSO AVAILABLE WITH A \"dt-\" PREFIX!!!\n";
        s << Color::Cyan << "[doctest]\n" << Color::None;
        s << Color::Cyan << "[doctest] " << Color::None;
        s << "Query flags - the program quits after them. Available:\n\n";
        s << " -?,   --help, -h                      prints this message\n";
        s << " -v,   --version                       prints the version\n";
        s << " -c,   --count                         prints the number of matching tests\n";
        s << " -ltc, --list-test-cases               lists all matching tests by name\n";
        s << " -lts, --list-test-suites              lists all matching test suites\n\n";
        // ================================================================================== << 79
        s << Color::Cyan << "[doctest] " << Color::None;
        s << "The available <int>/<string> options/filters are:\n\n";
        s << " -tc,  --test-case=<filters>           filters     tests by their name\n";
        s << " -tce, --test-case-exclude=<filters>   filters OUT tests by their name\n";
        s << " -sf,  --source-file=<filters>         filters     tests by their file\n";
        s << " -sfe, --source-file-exclude=<filters> filters OUT tests by their file\n";
        s << " -ts,  --test-suite=<filters>          filters     tests by their test suite\n";
        s << " -tse, --test-suite-exclude=<filters>  filters OUT tests by their test suite\n";
        s << " -sc,  --subcase=<filters>             filters     subcases by their name\n";
        s << " -sce, --subcase-exclude=<filters>     filters OUT subcases by their name\n";
        s << " -ob,  --order-by=<string>             how the tests should be ordered\n";
        s << "                                       <string> - by [file/suite/name/rand]\n";
        s << " -rs,  --rand-seed=<int>               seed for random ordering\n";
        s << " -f,   --first=<int>                   the first test passing the filters to\n";
        s << "                                       execute - for range-based execution\n";
        s << " -l,   --last=<int>                    the last test passing the filters to\n";
        s << "                                       execute - for range-based execution\n";
        s << " -aa,  --abort-after=<int>             stop after <int> failed assertions\n";
        s << " -scfl,--subcase-filter-levels=<int>   apply filters for the first <int> levels\n";
        s << Color::Cyan << "\n[doctest] " << Color::None;
        s << "Bool options - can be used like flags and true is assumed. Available:\n\n";
        s << " -s,   --success=<bool>                include successful assertions in output\n";
        s << " -cs,  --case-sensitive=<bool>         filters being treated as case sensitive\n";
        s << " -e,   --exit=<bool>                   exits after the tests finish\n";
        s << " -d,   --duration=<bool>               prints the time duration of each test\n";
        s << " -nt,  --no-throw=<bool>               skips exceptions-related assert checks\n";
        s << " -ne,  --no-exitcode=<bool>            returns (or exits) always with success\n";
        s << " -nr,  --no-run=<bool>                 skips all runtime doctest operations\n";
        s << " -nv,  --no-version=<bool>             omit the framework version in the output\n";
        s << " -nc,  --no-colors=<bool>              disables colors in output\n";
        s << " -fc,  --force-colors=<bool>           use colors even when not in a tty\n";
        s << " -nb,  --no-breaks=<bool>              disables breakpoints in debuggers\n";
        s << " -ns,  --no-skip=<bool>                don't skip test cases marked as skip\n";
        s << " -gfl, --gnu-file-line=<bool>          :n: vs (n): for line numbers in output\n";
        s << " -npf, --no-path-filenames=<bool>      only filenames and no paths in output\n";
        s << " -nln, --no-line-numbers=<bool>        0 instead of real line numbers in output\n";
        // ================================================================================== << 79
        // clang-format on

        s << Color::Cyan << "\n[doctest] " << Color::None;
        s << "for more information visit the project documentation\n\n";
    }

    void printSummary(std::ostream& s) {
        const ContextState* p = contextState;

        separator_to_stream(s);

        if(p->count || p->list_test_cases) {
            s << Color::Cyan << "[doctest] " << Color::None
              << "unskipped test cases passing the current filters: " << p->numTestsPassingFilters
              << "\n";
        } else if(p->list_test_suites) {
            s << Color::Cyan << "[doctest] " << Color::None
              << "unskipped test cases passing the current filters: " << p->numTestsPassingFilters
              << "\n";
            s << Color::Cyan << "[doctest] " << Color::None
              << "test suites with unskipped test cases passing the current filters: "
              << p->numTestSuitesPassingFilters << "\n";
        } else {
            const bool anythingFailed = p->numFailed > 0 || p->numFailedAssertions > 0;
            s << Color::Cyan << "[doctest] " << Color::None << "test cases: " << std::setw(6)
              << p->numTestsPassingFilters << " | "
              << ((p->numTestsPassingFilters == 0 || anythingFailed) ? Color::None : Color::Green)
              << std::setw(6) << p->numTestsPassingFilters - p->numFailed << " passed"
              << Color::None << " | " << (p->numFailed > 0 ? Color::Red : Color::None)
              << std::setw(6) << p->numFailed << " failed" << Color::None << " | ";
            if(p->no_skipped_summary == false) {
                const int numSkipped = static_cast<unsigned>(getRegisteredTests().size()) -
                                       p->numTestsPassingFilters;
                s << (numSkipped == 0 ? Color::None : Color::Yellow) << std::setw(6) << numSkipped
                  << " skipped" << Color::None;
            }
            s << "\n";
            s << Color::Cyan << "[doctest] " << Color::None << "assertions: " << std::setw(6)
              << p->numAssertions << " | "
              << ((p->numAssertions == 0 || anythingFailed) ? Color::None : Color::Green)
              << std::setw(6) << (p->numAssertions - p->numFailedAssertions) << " passed"
              << Color::None << " | " << (p->numFailedAssertions > 0 ? Color::Red : Color::None)
              << std::setw(6) << p->numFailedAssertions << " failed" << Color::None << " |\n";
            s << Color::Cyan << "[doctest] " << Color::None
              << "Status: " << (p->numFailed > 0 ? Color::Red : Color::Green)
              << ((p->numFailed > 0) ? "FAILURE!\n" : "SUCCESS!\n");
        }

        // remove any coloring
        s << Color::None;
    }
} // namespace detail

bool isRunningInTest() { return detail::contextState != 0; }

Context::Context(int argc, const char* const* argv)
        : p(new detail::ContextState) {
    parseArgs(argc, argv, true);
}

Context::~Context() { delete p; }

void Context::applyCommandLine(int argc, const char* const* argv) { parseArgs(argc, argv); }

// parses args
void Context::parseArgs(int argc, const char* const* argv, bool withDefaults) {
    using namespace detail;

    // clang-format off
    parseCommaSepArgs(argc, argv, "dt-source-file=",        p->filters[0]);
    parseCommaSepArgs(argc, argv, "dt-sf=",                 p->filters[0]);
    parseCommaSepArgs(argc, argv, "dt-source-file-exclude=",p->filters[1]);
    parseCommaSepArgs(argc, argv, "dt-sfe=",                p->filters[1]);
    parseCommaSepArgs(argc, argv, "dt-test-suite=",         p->filters[2]);
    parseCommaSepArgs(argc, argv, "dt-ts=",                 p->filters[2]);
    parseCommaSepArgs(argc, argv, "dt-test-suite-exclude=", p->filters[3]);
    parseCommaSepArgs(argc, argv, "dt-tse=",                p->filters[3]);
    parseCommaSepArgs(argc, argv, "dt-test-case=",          p->filters[4]);
    parseCommaSepArgs(argc, argv, "dt-tc=",                 p->filters[4]);
    parseCommaSepArgs(argc, argv, "dt-test-case-exclude=",  p->filters[5]);
    parseCommaSepArgs(argc, argv, "dt-tce=",                p->filters[5]);
    parseCommaSepArgs(argc, argv, "dt-subcase=",            p->filters[6]);
    parseCommaSepArgs(argc, argv, "dt-sc=",                 p->filters[6]);
    parseCommaSepArgs(argc, argv, "dt-subcase-exclude=",    p->filters[7]);
    parseCommaSepArgs(argc, argv, "dt-sce=",                p->filters[7]);
    // clang-format on

    int    intRes = 0;
    String strRes;

#define DOCTEST_PARSE_AS_BOOL_OR_FLAG(name, sname, var, default)                                   \
    if(parseIntOption(argc, argv, name "=", option_bool, intRes) ||                                \
       parseIntOption(argc, argv, sname "=", option_bool, intRes))                                 \
        p->var = !!intRes;                                                                         \
    else if(parseFlag(argc, argv, name) || parseFlag(argc, argv, sname))                           \
        p->var = true;                                                                             \
    else if(withDefaults)                                                                          \
    p->var = default

#define DOCTEST_PARSE_INT_OPTION(name, sname, var, default)                                        \
    if(parseIntOption(argc, argv, name "=", option_int, intRes) ||                                 \
       parseIntOption(argc, argv, sname "=", option_int, intRes))                                  \
        p->var = intRes;                                                                           \
    else if(withDefaults)                                                                          \
    p->var = default

#define DOCTEST_PARSE_STR_OPTION(name, sname, var, default)                                        \
    if(parseOption(argc, argv, name "=", strRes, default) ||                                       \
       parseOption(argc, argv, sname "=", strRes, default) || withDefaults)                        \
    p->var = strRes

    // clang-format off
    DOCTEST_PARSE_STR_OPTION("dt-order-by", "dt-ob", order_by, "file");
    DOCTEST_PARSE_INT_OPTION("dt-rand-seed", "dt-rs", rand_seed, 0);

    DOCTEST_PARSE_INT_OPTION("dt-first", "dt-f", first, 1);
    DOCTEST_PARSE_INT_OPTION("dt-last", "dt-l", last, 0);

    DOCTEST_PARSE_INT_OPTION("dt-abort-after", "dt-aa", abort_after, 0);
    DOCTEST_PARSE_INT_OPTION("dt-subcase-filter-levels", "dt-scfl", subcase_filter_levels, 2000000000);

    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-success", "dt-s", success, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-case-sensitive", "dt-cs", case_sensitive, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-exit", "dt-e", exit, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-duration", "dt-d", duration, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-no-throw", "dt-nt", no_throw, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-no-exitcode", "dt-ne", no_exitcode, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-no-run", "dt-nr", no_run, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-no-version", "dt-nv", no_version, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-no-colors", "dt-nc", no_colors, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-force-colors", "dt-fc", force_colors, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-no-breaks", "dt-nb", no_breaks, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-no-skip", "dt-ns", no_skip, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-gnu-file-line", "dt-gfl", gnu_file_line, bool(DOCTEST_GCC) || bool(DOCTEST_CLANG));
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-no-path-filenames", "dt-npf", no_path_in_filenames, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-no-line-numbers", "dt-nln", no_line_numbers, false);
    DOCTEST_PARSE_AS_BOOL_OR_FLAG("dt-no-skipped-summary", "dt-nss", no_skipped_summary, false);
    // clang-format on

#undef DOCTEST_PARSE_STR_OPTION
#undef DOCTEST_PARSE_INT_OPTION
#undef DOCTEST_PARSE_AS_BOOL_OR_FLAG

    if(withDefaults) {
        p->help             = false;
        p->version          = false;
        p->count            = false;
        p->list_test_cases  = false;
        p->list_test_suites = false;
    }
    if(parseFlag(argc, argv, "dt-help") || parseFlag(argc, argv, "dt-h") ||
       parseFlag(argc, argv, "dt-?")) {
        p->help = true;
        p->exit = true;
    }
    if(parseFlag(argc, argv, "dt-version") || parseFlag(argc, argv, "dt-v")) {
        p->version = true;
        p->exit    = true;
    }
    if(parseFlag(argc, argv, "dt-count") || parseFlag(argc, argv, "dt-c")) {
        p->count = true;
        p->exit  = true;
    }
    if(parseFlag(argc, argv, "dt-list-test-cases") || parseFlag(argc, argv, "dt-ltc")) {
        p->list_test_cases = true;
        p->exit            = true;
    }
    if(parseFlag(argc, argv, "dt-list-test-suites") || parseFlag(argc, argv, "dt-lts")) {
        p->list_test_suites = true;
        p->exit             = true;
    }
}

// allows the user to add procedurally to the filters from the command line
void Context::addFilter(const char* filter, const char* value) { setOption(filter, value); }

// allows the user to clear all filters from the command line
void Context::clearFilters() {
    for(unsigned i = 0; i < p->filters.size(); ++i)
        p->filters[i].clear();
}

// allows the user to override procedurally the int/bool options from the command line
void Context::setOption(const char* option, int value) {
    setOption(option, toString(value).c_str());
}

// allows the user to override procedurally the string options from the command line
void Context::setOption(const char* option, const char* value) {
    String      argv   = String("-") + option + "=" + value;
    const char* lvalue = argv.c_str();
    parseArgs(1, &lvalue);
}

// users should query this in their main() and exit the program if true
bool Context::shouldExit() { return p->exit; }

// the main function that does all the filtering and test running
int Context::run() {
    using namespace detail;

    Color::init();

    contextState = p;
    p->resetRunData();

    // handle version, help and no_run
    if(p->no_run || p->version || p->help) {
        if(p->version)
            printVersion(std::cout);
        if(p->help)
            printHelp(std::cout);

        contextState = 0;

        return EXIT_SUCCESS;
    }

    printVersion(std::cout);
    std::cout << Color::Cyan << "[doctest] " << Color::None << "run with \"--help\" for options\n";

    unsigned i = 0; // counter used for loops - here for VC6

    std::set<TestCase>& registeredTests = getRegisteredTests();

    std::vector<const TestCase*> testArray;
    for(std::set<TestCase>::iterator it = registeredTests.begin(); it != registeredTests.end();
        ++it)
        testArray.push_back(&(*it));

    // sort the collected records
    if(!testArray.empty()) {
        if(p->order_by.compare("file", true) == 0) {
            std::qsort(&testArray[0], testArray.size(), sizeof(TestCase*), fileOrderComparator);
        } else if(p->order_by.compare("suite", true) == 0) {
            std::qsort(&testArray[0], testArray.size(), sizeof(TestCase*), suiteOrderComparator);
        } else if(p->order_by.compare("name", true) == 0) {
            std::qsort(&testArray[0], testArray.size(), sizeof(TestCase*), nameOrderComparator);
        } else if(p->order_by.compare("rand", true) == 0) {
            std::srand(p->rand_seed);

            // random_shuffle implementation
            const TestCase** first = &testArray[0];
            for(i = testArray.size() - 1; i > 0; --i) {
                int idxToSwap = std::rand() % (i + 1); // NOLINT

                const TestCase* temp = first[i];

                first[i]         = first[idxToSwap];
                first[idxToSwap] = temp;
            }
        }
    }

    if(p->list_test_cases) {
        std::cout << Color::Cyan << "[doctest] " << Color::None << "listing all test case names\n";
        separator_to_stream(std::cout);
    }

    std::set<String> testSuitesPassingFilters;
    if(p->list_test_suites) {
        std::cout << Color::Cyan << "[doctest] " << Color::None << "listing all test suites\n";
        separator_to_stream(std::cout);
    }

    // invoke the registered functions if they match the filter criteria (or just count them)
    for(i = 0; i < testArray.size(); i++) {
        const TestCase& data = *testArray[i];

        if(data.m_skip && !p->no_skip)
            continue;

        if(!matchesAny(data.m_file, p->filters[0], 1, p->case_sensitive))
            continue;
        if(matchesAny(data.m_file, p->filters[1], 0, p->case_sensitive))
            continue;
        if(!matchesAny(data.m_test_suite, p->filters[2], 1, p->case_sensitive))
            continue;
        if(matchesAny(data.m_test_suite, p->filters[3], 0, p->case_sensitive))
            continue;
        if(!matchesAny(data.m_name, p->filters[4], 1, p->case_sensitive))
            continue;
        if(matchesAny(data.m_name, p->filters[5], 0, p->case_sensitive))
            continue;

        p->numTestsPassingFilters++;

        // do not execute the test if we are to only count the number of filter passing tests
        if(p->count)
            continue;

        // print the name of the test and don't execute it
        if(p->list_test_cases) {
            std::cout << Color::None << data.m_name << "\n";
            continue;
        }

        // print the name of the test suite if not done already and don't execute it
        if(p->list_test_suites) {
            if((testSuitesPassingFilters.count(data.m_test_suite) == 0) &&
               data.m_test_suite[0] != '\0') {
                std::cout << Color::None << data.m_test_suite << "\n";
                testSuitesPassingFilters.insert(data.m_test_suite);
                p->numTestSuitesPassingFilters++;
            }
            continue;
        }

        // skip the test if it is not in the execution range
        if((p->last < p->numTestsPassingFilters && p->first <= p->last) ||
           (p->first > p->numTestsPassingFilters))
            continue;

        // execute the test if it passes all the filtering
        {
            p->currentTest = &data;

            bool failed                              = false;
            p->hasLoggedCurrentTestStart             = false;
            p->numFailedAssertionsForCurrentTestcase = 0;
            p->subcasesPassed.clear();
            double duration = 0;
            Timer  timer;
            timer.start();
            do {
                // if the start has been logged from a previous iteration of this loop
                if(p->hasLoggedCurrentTestStart)
                    logTestEnd();
                p->hasLoggedCurrentTestStart = false;

                // if logging successful tests - force the start log
                if(p->success)
                    DOCTEST_LOG_START(std::cout);

                // reset the assertion state
                p->numAssertionsForCurrentTestcase = 0;
                p->hasCurrentTestFailed            = false;

                // reset some of the fields for subcases (except for the set of fully passed ones)
                p->subcasesHasSkipped   = false;
                p->subcasesCurrentLevel = 0;
                p->subcasesEnteredLevels.clear();

                // reset stuff for logging with INFO()
                p->exceptionalContexts.clear();

// execute the test
#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
                try {
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS
                    FatalConditionHandler fatalConditionHandler; // Handle signals
                    data.m_test();
                    fatalConditionHandler.reset();
                    if(contextState->hasCurrentTestFailed)
                        failed = true;
#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS
                } catch(const TestFailureException&) { failed = true; } catch(...) {
                    DOCTEST_LOG_START(std::cout);
                    logTestException(*contextState->currentTest, translateActiveException(), false);
                    failed = true;
                }
#endif // DOCTEST_CONFIG_NO_EXCEPTIONS

                p->numAssertions += p->numAssertionsForCurrentTestcase;

                // exit this loop if enough assertions have failed
                if(p->abort_after > 0 && p->numFailedAssertions >= p->abort_after) {
                    p->subcasesHasSkipped = false;
                    std::cout << Color::Red << "Aborting - too many failed asserts!\n";
                }

            } while(p->subcasesHasSkipped == true);

            duration = timer.getElapsedSeconds();

            if(Approx(p->currentTest->m_timeout).epsilon(DBL_EPSILON) != 0 &&
               Approx(duration).epsilon(DBL_EPSILON) > p->currentTest->m_timeout) {
                failed = true;
                DOCTEST_LOG_START(std::cout);
                std::cout << Color::Red << "Test case exceeded time limit of "
                          << std::setprecision(6) << std::fixed << p->currentTest->m_timeout
                          << "!\n";
            }

            if(p->duration)
                std::cout << Color::None << std::setprecision(6) << std::fixed << duration
                          << " s: " << p->currentTest->m_name << "\n";

            if(data.m_should_fail) {
                DOCTEST_LOG_START(std::cout);
                if(failed)
                    std::cout << Color::Yellow
                              << "Failed as expected so marking it as not failed\n";
                else
                    std::cout << Color::Red
                              << "Should have failed but didn't! Marking it as failed!\n";
                failed = !failed;
            } else if(failed && data.m_may_fail) {
                DOCTEST_LOG_START(std::cout);
                failed = false;
                std::cout << Color::Yellow << "Allowed to fail so marking it as not failed\n";
            } else if(data.m_expected_failures > 0) {
                DOCTEST_LOG_START(std::cout);
                if(p->numFailedAssertionsForCurrentTestcase == data.m_expected_failures) {
                    failed = false;
                    std::cout << Color::Yellow << "Failed exactly " << data.m_expected_failures
                              << " times as expected so marking it as not failed!\n";
                } else {
                    failed = true;
                    std::cout << Color::Red << "Didn't fail exactly " << data.m_expected_failures
                              << " times so marking it as failed!\n";
                }
            }
            std::cout << Color::None;

            if(p->hasLoggedCurrentTestStart)
                logTestEnd();

            if(failed) // if any subcase has failed - the whole test case has failed
                p->numFailed++;

            // stop executing tests if enough assertions have failed
            if(p->abort_after > 0 && p->numFailedAssertions >= p->abort_after)
                break;
        }
    }

    printSummary(std::cout);

    contextState = 0;

    if(p->numFailed && !p->no_exitcode)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
} // namespace doctest

#endif // DOCTEST_CONFIG_DISABLE

#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
int main(int argc, char** argv)
{
    const char* const *argv_c = argv;
    return doctest::Context(argc, argv_c).run();
}
#endif // DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

DOCTEST_CLANG_SUPPRESS_WARNING_POP
DOCTEST_MSVC_SUPPRESS_WARNING_POP
DOCTEST_GCC_SUPPRESS_WARNING_POP

#endif // DOCTEST_LIBRARY_IMPLEMENTATION
#endif // DOCTEST_CONFIG_IMPLEMENT

