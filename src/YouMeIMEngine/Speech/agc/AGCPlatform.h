/*
 * Copyright (c) 2013 刘志鹏. All Rights Reserved.
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#if     defined(_WIN32) || defined(WIN32) || defined(_WIN64)
#define OS_WINSYS       1
#elif   defined(__linux__) || defined(__linuxppc__) || defined(__FreeBSD__)
#define OS_LINUX        2
#elif defined(__MacOSX__) || defined(__APPLE__) || defined(__MACH__) || defined(OS_IOS)
#define OS_APPLE        3
#endif

// Windows system's version
#define OS_NONE     0   // 错误
#define OS_WIN2000  1   // Windows 2000
#define OS_WINXP    2   // Windows XP
#define OS_WINVISTA 3   // Windows Vista
#define OS_WIN7     4   // Win7
#define OS_WIN8     5   // Win8
#define OS_WIN8_1   6   // Win8.1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#ifdef __cplusplus
#include <string>
#include <vector>
#include <map>
#include <list>
#include <deque>
#include <algorithm>
#include <functional>
#else
#include <string.h>
#include <ctype.h>
#endif
#ifdef  OS_WINSYS
// for CoInitializeEx
#define _WIN32_DCOM
//#include <WinSock2.h>
//#include <MSWSock.h>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>
#ifndef __MINGW32__
#ifdef  __cplusplus
#include <atlbase.h>
#endif
#endif
#include <time.h>
#include <io.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <fcntl.h>
#elif   OS_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <sys/uio.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <dlfcn.h>
#elif   OS_APPLE

#else
#error "No platform defined for OS (Platform.h)"
#endif

/*****************************  type defines for all platforms *****************************/



#ifndef OS_WINSYS
#include <stdint.h>
#else
typedef signed char         int8_t;
//typedef char                int8_t;
typedef short               int16_t;
typedef int                 int32_t;
typedef __int64             int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned __int64    uint64_t;
#endif

typedef float               OsFlt32;
typedef double              OsFlt64;
typedef long                OsLong;
typedef unsigned long       OsULong;
typedef int64_t             OsInt64;
typedef uint64_t            OsUInt64;
typedef int32_t             OsInt32;
typedef uint32_t            OsUInt32;
typedef int16_t             OsInt16;
typedef uint16_t            OsUInt16;
typedef char                OsInt8;
typedef uint8_t             OsUInt8;

#ifdef  OS_ARCH_LITTLEENDIAN
#define MakeFourCc(inChar0,inChar1,inChar2,inChar3)     \
                ((OsUInt32)(OsUInt8)(inChar3)        |  \
                ((OsUInt32)(OsUInt8)(inChar2) << 8)  |  \
                ((OsUInt32)(OsUInt8)(inChar1) << 16) |  \
                ((OsUInt32)(OsUInt8)(inChar0) << 24))
#else
#define MakeFourCc(inChar0,inChar1,inChar2,inChar3)     \
                ((OsUInt32)(OsUInt8)(inChar0)        |  \
                ((OsUInt32)(OsUInt8)(inChar1) << 8)  |  \
                ((OsUInt32)(OsUInt8)(inChar2) << 16) |  \
                ((OsUInt32)(OsUInt8)(inChar3) << 24))
#endif

typedef enum {
    OS_FALSE = 0,
    OS_TRUE
} OsBool;

typedef enum
{
    OS_EOS              = 1,    // Indicates the end of a stream or of a file
    OS_OK               = 0,    // Operation success
    OS_BAD_PARAM        = -1,   // The input parameter is not correct
    OS_OUT_OF_MEMORY    = -2,   // Memory allocation failure
    OS_IO_ERROR         = -3,   // Input/Output failure (disk access, system call failures)
    OS_NOT_SUPPORTED    = -4,   // Operation is not supported by the framework
} OsRet;

// fprintf(1,'\t%.14f\n',w) for matlab
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

/***************************  macro defines for compiler preprocess ***************************/

#if defined(_MSC_VER)
// msvc 6.0   1200
// msvs 2005  1400
// msvs 2008  1500
// msvs 2010  1600
#if _MSC_VER <= 1500
// POSIX socket errorcodes. defines for compatible POSIX
#define ENOTCONN        1002
#define EADDRINUSE      1004
#define EINPROGRESS     1007
#define ENOBUFS         1008
#define EADDRNOTAVAIL   1009
#endif
// for same function
#if _MSC_VER >= 1200
#define ftruncate   _chsize
#define strncasecmp _strnicmp
#define snprintf    _snprintf
#define vsnprintf   _vsnprintf
#pragma warning(disable:4996)   // not print message of unsafe warning
#pragma warning(disable:4244)   // not print message of possible loss of data
#endif
// strtok_r是linux平台下的strtok函数的线程安全版，Windows下为strtok_s
// strtok并不能通过两层循环的办法，解决提取多人信息的问题
#if _MSC_VER > 1200
#define strtok_r    strtok_s
#endif
// for export function
#define OS_INLINE   __inline
#define OS_NOINLINE __declspec(noinline)
// #define OS_FORCEINLINE  __forceinline
#define likely(x)   (x)
#define unlikely(x) (x)

#elif defined(__GNUC__)

#if __GNUC__ >= 4
#define likely(x)   __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect(!!(x),0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif
#define OS_INLINE   __attribute__((__always_inline__)) inline
#define OS_NOINLINE __attribute__((noinline))

#endif

#ifdef  OS_WINSYS
#define OS_EXTERN   __declspec(dllexport)
#else
#if __GNUC__ >= 4
#define OS_EXTERN   __attribute__((visibility("default")))
// 配合链接器选项：-fvisibility=hidden
#else
#define OS_EXTERN   // nothing
#endif
#endif

#ifdef  OS_DEBUG
#define OsLog(inFile,inFormat,...) fprintf(inFile,inFormat,__VA_ARGS__)
// fprintf(inFile,inFormat##" [%d:%s]\n",__VA_ARGS__,__LINE__,__FUNCTION__)
// fprintf(inFile,inFormat##" [%d:%s] -> %s\n",__VA_ARGS__,__LINE__,__FUNCTION__,__FILE__)
#else
#define OsLog(inFile,inFormat,...)
#endif


// Macros specific for the fixed point implementation
#define WEBRTC_SPL_WORD16_MAX       32767
#define WEBRTC_SPL_WORD16_MIN       -32768
#define WEBRTC_SPL_WORD32_MAX       (int32_t)0x7fffffff
#define WEBRTC_SPL_WORD32_MIN       (int32_t)0x80000000
#define WEBRTC_SPL_MAX_LPC_ORDER    14
#define WEBRTC_SPL_MIN(A, B)        (A < B ? A : B)  // Get min value
#define WEBRTC_SPL_MAX(A, B)        (A > B ? A : B)  // Get max value
// TODO(kma/bjorn): For the next two macros, investigate how to correct the code
// for inputs of a = WEBRTC_SPL_WORD16_MIN or WEBRTC_SPL_WORD32_MIN.
#define WEBRTC_SPL_ABS_W16(a) \
	(((int16_t)a >= 0) ? ((int16_t)a) : -((int16_t)a))
#define WEBRTC_SPL_ABS_W32(a) \
	(((int32_t)a >= 0) ? ((int32_t)a) : -((int32_t)a))

#define WEBRTC_SPL_MUL(a, b) \
	((int32_t)((int32_t)(a)* (int32_t)(b)))
#define WEBRTC_SPL_UMUL(a, b) \
	((uint32_t)((uint32_t)(a)* (uint32_t)(b)))
#define WEBRTC_SPL_UMUL_32_16(a, b) \
	((uint32_t)((uint32_t)(a)* (uint16_t)(b)))
#define WEBRTC_SPL_MUL_16_U16(a, b) \
	((int32_t)(int16_t)(a)* (uint16_t)(b))

#ifndef WEBRTC_ARCH_ARM_V7
// For ARMv7 platforms, these are inline functions in spl_inl_armv7.h
#ifndef MIPS32_LE
// For MIPS platforms, these are inline functions in spl_inl_mips.h
#define WEBRTC_SPL_MUL_16_16(a, b) \
	((int32_t)(((int16_t)(a)) * ((int16_t)(b))))
#define WEBRTC_SPL_MUL_16_32_RSFT16(a, b) \
	(WEBRTC_SPL_MUL_16_16(a, b >> 16) \
	+ ((WEBRTC_SPL_MUL_16_16(a, (b & 0xffff) >> 1) + 0x4000) >> 15))
#endif
#endif

#define WEBRTC_SPL_MUL_16_32_RSFT11(a, b)          \
	(WEBRTC_SPL_MUL_16_16(a, (b) >> 16) * (1 << 5) + \
	(((WEBRTC_SPL_MUL_16_U16(a, (uint16_t)(b)) >> 1) + 0x0200) >> 10))
#define WEBRTC_SPL_MUL_16_32_RSFT14(a, b)          \
	(WEBRTC_SPL_MUL_16_16(a, (b) >> 16) * (1 << 2) + \
	(((WEBRTC_SPL_MUL_16_U16(a, (uint16_t)(b)) >> 1) + 0x1000) >> 13))
#define WEBRTC_SPL_MUL_16_32_RSFT15(a, b)            \
	((WEBRTC_SPL_MUL_16_16(a, (b) >> 16) * (1 << 1)) + \
	(((WEBRTC_SPL_MUL_16_U16(a, (uint16_t)(b)) >> 1) + 0x2000) >> 14))

#define WEBRTC_SPL_MUL_16_16_RSFT(a, b, c) \
	(WEBRTC_SPL_MUL_16_16(a, b) >> (c))

#define WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(a, b, c) \
	((WEBRTC_SPL_MUL_16_16(a, b) + ((int32_t) \
	(((int32_t)1) << ((c)-1)))) >> (c))

// C + the 32 most significant bits of A * B
#define WEBRTC_SPL_SCALEDIFF32(A, B, C) \
	(C + (B >> 16) * A + (((uint32_t)(0x0000FFFF & B) * A) >> 16))

#define WEBRTC_SPL_SAT(a, b, c)         (b > a ? a : b < c ? c : b)

// Shifting with negative numbers allowed
// Positive means left shift
#define WEBRTC_SPL_SHIFT_W32(x, c) ((c) >= 0 ? (x) * (1 << (c)) : (x) >> -(c))

// Shifting with negative numbers not allowed
// We cannot do casting here due to signed/unsigned problem
#define WEBRTC_SPL_LSHIFT_W32(x, c)     ((x) << (c))

#define WEBRTC_SPL_RSHIFT_U32(x, c)     ((uint32_t)(x) >> (c))

#define WEBRTC_SPL_RAND(a) \
	((int16_t)((((int16_t)a * 18816) >> 7) & 0x00007fff))
#endif // PLATFORM_H
