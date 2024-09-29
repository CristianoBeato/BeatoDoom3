/*
===========================================================================

Beato idTech 4 Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
Copyright (C) 2016-2022 Cristiano B. Santos <cristianobeato_dm@hotmail.com>.

This file is part of the Beato idTech 4  GPL Source Code (?Beato idTech 4  Source Code?).

Beato idTech 4  Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Beato idTech 4  Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Beato idTech 4  Source Code.  If not, see <http://www.gnu.org/licenses/>.

===========================================================================
*/

#ifndef _SYS_PLATFORM_H_
#define _SYS_PLATFORM_H_

// c/c++ standarts
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <typeinfo>
#include <errno.h>
#include <math.h>
#include <algorithm>

// SDL2 common
#include <SDL2/SDL_stdinc.h>	
#include <SDL2/SDL_assert.h>
#include <SDL2/SDL_endian.h>
#include <SDL2/SDL_cpuinfo.h>
#include <SDL2/SDL_loadso.h>
#include <SDL2/SDL_cpuinfo.h>

/*
#define _ARCH_x86_32_
#define _ARCH_ARM_32_
#define _ARCH_PPC_32_
#define _ARCH_x86_64_
#define _ARCH_ARM_64_
#define _ARCH_PPC_64_
*/

//Get current architecture, detectx nearly every architecture. Coded by Freak
// https://stackoverflow.com/questions/152016/detecting-cpu-architecture-compile-time
#if defined(__x86_64__) || defined(_M_X64)
#define _ARCH_x86_64_ 1
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define _ARCH_x86_32_ 1
#elif defined(__arm__) || defined(_M_ARM)
#elif defined(__aarch64__) || defined(_M_ARM64)
#define _ARCH_ARM_64_ 1
#elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
#define _ARCH_PPC_32_ 1
#elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
#define _ARCH_PPC_64_ 1
#endif

#if _ARCH_x86_32_ ||  _ARCH_ARM32_ || _ARCH_PPC32_
# define PLATFORM 32
#define CPUSTRING					"x32"
#elif _ARCH_x86_64_ || _ARCH_ARM_64_ || _ARCH_PPC_64_
# define PLATFORM 64
#define CPUSTRING					"x64"
#else
#error "UNSUPORTED_PLATFORM"
#endif

#define ID_TIME_T time_t
#ifndef NULL
#define NULL	nullptr //((void *)0)
#endif

#ifndef ID_INLINE
#define ID_INLINE inline
#endif //!ID_INLINE

// 
// bypass for typeinfo
#define BT_TYPEINFO_ACESS friend class idTypeInfoTools

#ifdef assert
#undef assert
#endif //!assert

#define assert SDL_assert
#define ASSERT SDL_assert

#if BT_USE_SDL_MALLOC

#ifdef malloc
#undef malloc
#endif // malloc
#define malloc SDL_malloc

#ifdef calloc
#undef calloc
#endif // calloc
#define calloc SDL_calloc

#ifdef realloc
#undef realloc
#endif // realloc
#define realloc SDL_realloc

#ifdef free
#undef free
#endif // free
#define free SDL_free
#endif // BT_USE_SDL_MALLOC

#if !defined( _DEBUG ) && !defined( NDEBUG )
	// don't generate asserts
#define NDEBUG
#endif

/*
===============================================================================

	Non-portable system services.

===============================================================================
*/

#if defined(WIN32) || defined(_WIN32) // Windows

#include <winsock2.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <malloc.h>							// no malloc.h on mac or unix
#include <windows.h>						// for qgl.h

// stupid namespace poluting Microsoft monkeys
#ifdef FindText
#undef FindText
#endif //FindText
#ifdef GetObject
#undef GetObject
#endif//GetObjec

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// prevent auto literal to string conversion

#define ID_STATIC_TEMPLATE				static

#define	BUILD_STRING					"win"
#define BUILD_OS_ID						0
#define CPU_EASYARGS					1
#define PATHSEPERATOR_STR				"\\"
#define PATHSEPERATOR_CHAR				'\\'
#define WINVER				0x501


#elif  defined(MACOS_X) || defined(__APPLE__)
#define BUILD_STRING				"MacOSX-universal"

#include <alloca.h>
#define _alloca							alloca


#define PATHSEPERATOR_STR				"/"
#define PATHSEPERATOR_CHAR				'/'

#elif  defined(  __linux__ ) // Linux distro build
#define	BUILD_STRING				"linux"
#define _alloca							alloca
#define BUILD_OS_ID						1

#define PATHSEPERATOR_STR				"/"
#define PATHSEPERATOR_CHAR				'/'

#define __cdecl 

#endif

#if defined ( _MSVC_LANG ) // Microsof Visual C 

#define  assertmem( x, y )	SDL_assert( _CrtIsValidPointer( x, y, true ) ) //may work on Clang on windows ?


#elif defined( __GNUC__ ) // Gnu GCC

#define id_attribute(x) __attribute__(x)
#define PACKED			__attribute__((packed))

#elif defined( __clang__ ) // Clang compile 

#define id_attribute(x) __attribute__(x)

#elif defined( __MWERKS__ ) // 

#endif

// Type alignament
#define _ALIGN_(S) alignas(S)
#define TYPE_ALIGN( T, S ) alignas(S) T
#define ALIGN16( X ) alignas( 16 ) X
#define ALIGN4( X ) alignas( 4) X

#if !defined( id_attribute )
#define id_attribute( x )
#endif // !id_attribute

#if !defined (assertmem)
#define assertmem( x, y )
#endif // !assertmem

#if !defined (PACKED )
#define PACKED
#endif //! PACKED

#if !defined(ID_STATIC_TEMPLATE)
#define ID_STATIC_TEMPLATE
#endif //!ID_STATIC_TEMPLATE

#endif /* !_SYS_PLATFORM_H_ */
