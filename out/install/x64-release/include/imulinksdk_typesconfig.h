/* #define IMULINKSDK_DLL_API */
/* #define IMULINKSDK_NOEXPORT */
/* Disable automatic library linking / 禁用自动库链接 */
#define IMULINKSDK_NO_AUTOLIB


/* include this file in Visual Studio using C/C++->Advanced->Force Includes (the /FI option)
   在 Visual Studio 中通过 C/C++->Advanced->Force Includes (/FI 选项) 强制包含此文件 */
#ifndef IMULINKSDK_CONFIG_H
#define IMULINKSDK_CONFIG_H

//////////////////////////////////////////////////
/* generic preprocessor defines / 通用预处理器定义 */

/* Reference: http://support.microsoft.com/kb/155196 */
#define IMULINKSDK_TRINGIFY2(s) 	#s
#define IMULINKSDK_TRINGIFY(s) 	IMULINKSDK_TRINGIFY2(s)
#define __LOC__ __FILE__ "(" IMULINKSDK_TRINGIFY(__LINE__)") : WARNING: "

/* make sure both _WIN32 and WIN32 are defined if either of them is.
   如果任一被定义，则确保同时定义 _WIN32 和 WIN32。 */
#if defined(_WIN32) || defined(_M_IX86)
	#ifndef WIN32
		#define WIN32
	#endif
	#define IMULINKSDK_WINDOWS
	/* suggested by https://www.fluentcpp.com/2019/08/30/how-to-disable-a-warning-in-cpp/
	   建议使用方式见链接 */
	#define IMULINKSDK_MSC_WARNING_SUPPRESS(...) 		__pragma(warning(suppress: __VA_ARGS__))
#else
	#define IMULINKSDK_MSC_WARNING_SUPPRESS(...)
#endif

#ifdef WIN32
	#ifndef _WIN32
		#define _WIN32
		#define IMULINKSDK_WINDOWS
	#endif
#endif
#if !defined(_WIN32) && defined(_MSC_VER) && !defined(_WIN64)
	#define _WIN64
#endif

/* make things as secure as possible without modifying the code...
   在不修改代码的前提下尽可能增强安全性 */
#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
	#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef __cplusplus
	/* make sure that const-correctness is enforced
	   确保 const 正确性被强制 */
	#ifdef _MSC_VER
		#pragma warning(error : 4090)
	#endif
	#define IMULINKSDK_CCONST	const
	#define IMULINKSDK_CPPPROTECTED
#else
	#define IMULINKSDK_CCONST
	#define IMULINKSDK_CPPPROTECTED	protected:
#endif

#ifdef __GNUC__
	#include <limits.h>
	#if __WORDSIZE == 64
		#define IMULINKSDK_64BIT
	#else
		#define IMULINKSDK_32BIT
	#endif
	#ifndef IMULINKSDK_PFSHARED
		#define IMULINKSDK_PFSHARED ".so"
		#define IMULINKSDK_PFPRE	  "lib"
	#endif
#else
	#ifndef IMULINKSDK_PFSHARED
		#define IMULINKSDK_PFSHARED ".dll"
		#define IMULINKSDK_PFPRE	  ""
	#endif
#endif

#if defined(_WIN64) || defined(_M_X64) || defined(_M_IA64)
	#ifndef IMULINKSDK_64BIT
		#define IMULINKSDK_64BIT
	#endif
	#ifndef IMULINKSDK_WINDOWS
		#define IMULINKSDK_WINDOWS
	#endif
	#ifndef _WIN64
		#define _WIN64
	#endif
	#ifndef WIN64
		#define WIN64
	#endif
	#ifndef IMULINKSDK_PFBITS
		#ifdef __GNUC__
			#define IMULINKSDK_PFBITS ""
		#else
			#define IMULINKSDK_PFBITS	"64"
		#endif
	#endif
#else
	#ifndef IMULINKSDK_32BIT
		#define IMULINKSDK_32BIT
	#endif
	#ifndef IMULINKSDK_PFBITS
		#ifdef __GNUC__
			#define IMULINKSDK_PFBITS ""
		#else
			#define IMULINKSDK_PFBITS	"32"
		#endif
	#endif
#endif

/* all xsens libraries should use unicode / 所有库应使用 Unicode */
#ifndef UNICODE
	#define UNICODE
#endif

/* use IMULINKSDK_32BIT and IMULINKSDK_64BIT to check for 32/64 bit builds in your application
   在应用中使用 IMULINKSDK_32BIT 和 IMULINKSDK_64BIT 来检查 32/64 位构建 */
// on non-windows systems these should be defined in this file

/*
    Configuration | Runtime | DebInfo | Defines
    --------------+---------------------------------------
    Debug         | MDd     | Yes     | IMULINKSDK_DEBUG;_DEBUG
    RelWithDeb    | MD      | Yes     | IMULINKSDK_DEBUG;IMULINKSDK_RELEASE;_DEBUG
    Release       | MD      | No      | IMULINKSDK_RELEASE;NDEBUG

    The common way to setup configuration-dependent defines:
    #if defined(IMULINKSDK_DEBUG)
	//// Debug or RelWithDeb build
	#if defined(IMULINKSDK_RELEASE)
		//// RelWithDeb build
	#else
		//// Debug build
	#endif
    #else
	//// Release build
    #endif
*/

#if defined(IMULINKSDK_DEBUG)
	/* Debug or RelWithDeb build */
	#ifndef IMULINKSDK_NO_STL
		#define IMULINKSDK_USE_DEBUG_COUNTERS
	#endif

	#if !defined(IMULINKSDK_RELEASE)
		/* Debug build */
		#if !defined(QT_DEBUG) && !defined(QT_NO_DEBUG)
			#define QT_DEBUG	1
		#endif
		#ifndef IMULINKSDK_PFCONF
			#ifdef __GNUC__
				#define IMULINKSDK_PFCONF	""
			#else
				#define IMULINKSDK_PFCONF	"d"
			#endif
		#endif
	#else
		/* RelWithDeb build */
		#if !defined(QT_DEBUG) && !defined(QT_NO_DEBUG)
			#define QT_NO_DEBUG	1
		#endif
		#ifndef IMULINKSDK_PFCONF
			#ifdef __GNUC__
				#define IMULINKSDK_PFCONF	""
			#else
				#define IMULINKSDK_PFCONF	"rd"
			#endif
		#endif
	#endif
#else
	/* Release build */
	#if !defined(IMULINKSDK_RELEASE)
		#define IMULINKSDK_RELEASE
	#endif

	#if !defined(QT_DEBUG) && !defined(QT_NO_DEBUG)
		#define QT_NO_DEBUG	1
	#endif
	#ifndef NDEBUG
		#ifndef KEEP_ASSERTS
			#define NDEBUG		// make sure assertions and other debug options are compiled away by MSVC
		#endif
	#endif
	#ifndef IMULINKSDK_PFCONF
		#define IMULINKSDK_PFCONF	""
	#endif
#endif

#ifndef IMULINKSDK_PFFULL
	#define IMULINKSDK_PFFULL	IMULINKSDK_PFBITS IMULINKSDK_PFCONF
#endif

//////////////////////////////////////////////////
/* more generic preprocessor defines
   required for gnu c++ compiler versions due to difference in attribute declarations
   通用的预处理器定义：解决 GNU C++ 在属性声明上的差异 */
#if defined(__AVR32__)
	#define __cdecl
	#define __stdcall
#elif defined(_ADI_COMPILER)
	#define __cdecl
	#define __stdcall
#elif defined(__GNUC__) && !defined(HAVE_CDECL)
	#if !defined(__cdecl)
		#if defined(__x86_64__)
			#define __cdecl
		#else
			#define __cdecl __attribute__((cdecl))
		#endif
	#endif
	#if !defined(__stdcall)
		#if defined(__x86_64__)
			#define __stdcall
		#else
			#define __stdcall __attribute__((stdcall))
		#endif
	#endif
#elif defined(IAR_ARM_CM3_NAMESPACE)
	#define __cdecl
	#define __stdcall
	#define IMULINKSDK_32BIT
#endif

//////////////////////////////////////////////////
/* generic preprocessor defines / 通用预处理器定义 */

/* use IMULINKSDK_32BIT and IMULINKSDK_64BIT to check for 32/64 bit builds in your application
   在应用中使用 IMULINKSDK_32BIT 和 IMULINKSDK_64BIT 来检测 32/64 位构建 */
// on non-windows systems these should be defined

#ifndef IMULINKSDK_DLL_API
	#ifdef IMULINKSDK_DLL_EXPORT
		#ifdef _WIN32
			#define IMULINKSDK_DLL_API __declspec(dllexport)
		#else
			#define IMULINKSDK_DLL_API __attribute__((visibility("default")))
		#endif
	#else	// ifdef IMULINKSDK_DLL_EXPORT
		#ifdef IMULINKSDK_STATIC_LIB
			#define IMULINKSDK_DLL_API
		#elif defined(_WIN32)
			#define IMULINKSDK_DLL_API __declspec(dllimport)
		#else
			#define IMULINKSDK_DLL_API
		#endif
	#endif	// ifdef IMULINKSDK_DLL_EXPORT - else
#endif	// ifndef IMULINKSDK_DLL_API

#include <assert.h>

/* since this is (almost) always required and it does not conflict with pstdint, include the file here
   由于此文件几乎总是被需要，并且与 pstdint 不冲突，所以在此包含它 */
//#define IMULINKSDK_SINGLE_PRECISION // enable for single precision
#include "imulinksdk_typedefs.h"

#ifndef IMULINKSDK_NOCOMEXPORT
	#define IMULINKSDK_NOCOMEXPORT
#endif

#ifdef IMULINKSDK_DEBUG
	#define IMULINKSDK_DEBUGLINE(a)	a
	#define IMULINKSDK_NODEBUGLINE(...)
#else
	#define IMULINKSDK_DEBUGLINE(...)
	#define IMULINKSDK_NODEBUGLINE(a)	a
#endif

#if defined(_MSC_VER) && !defined(__cplusplus) && !defined(inline)
	/* MSVC doesn't know the inline keyword in C mode, but it does know __inline
	   MSVC 在 C 模式下不识别 inline 关键字，但识别 __inline */
	#define inline __inline
#endif

#endif /* IMULINKSDK_CONFIG_H */
