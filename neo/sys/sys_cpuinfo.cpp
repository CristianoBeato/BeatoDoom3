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

#include "idlib/precompiled.h"
#pragma hdrstop

#include <SDL3/SDL_cpuinfo.h>

#if ID_USE_INSTRINSEC

#include <immintrin.h>
#include <xmmintrin.h>
#include <pmmintrin.h>

#include <bitset>

#define _REG_EAX		0
#define _REG_EBX		1
#define _REG_ECX		2
#define _REG_EDX		3

#define CPU_VENDOR_BIT			0x00000000	// CPU Brand Bit
#define	CPU_BRAND_BIT			0x80000000  // CPU Model Bit
#define BIT_SSE3				( 1 << 0 )	// bit 0 of ECX denotes SSE3 existence
#define BIT_CMOV				( 1 << 15 ) // bit 15 of EDX denotes CMOV existence
#define BIT_MMX					( 1 << 23 ) // bit 23 of EDX denotes MMX existence
#define BIT_FXSAVE				( 1 << 24 ) // bit 24 of EDX denotes support for FXSAVE
#define BIT_SSE					( 1 << 25 ) // bit 25 of EDX denotes SSE existence
#define BIT_SSE2				( 1 << 26 ) // bit 26 of EDX denotes SSE2 existence
#define BIT_HTT					( 1 << 28 ) // bit 28 of EDX denotes HTT existence
#define BIT_3DNOW				( 1 << 31 ) // bit 31 of EDX denotes 3DNow! support

static char	vendorName[32] = { "Generic" };
static char	cpuBrand[64] = { "Generic" };

#if _WIN32 || _WIN64
#include <intrin.h>

// GCC convention
inline unsigned int __get_cpuid( unsigned int level, unsigned int* eax, unsigned int* ebx, unsigned int* ecx, unsigned int* edx )
{
	int regs[4] = { -1, -1, -1 ,-1 };
	__cpuid( regs, (int)level );
	*reinterpret_cast<int*>(eax) = regs[_REG_EAX];
	*reinterpret_cast<int*>(ebx) = regs[_REG_EBX];
	*reinterpret_cast<int*>(ecx) = regs[_REG_ECX];
	*reinterpret_cast<int*>(edx) = regs[_REG_EDX];
	return 1;
}

#else
#include <cpuid.h>
#endif

#endif //ID_USE_INSTRINSEC

#include "sys_platform.h"
#include "sys_main.h"

/*
================
Sys_GetCPUId
================
*/
void Sys_getCPUInfo( void )
{
	int flags;
	char vendor[0x20];
	char brand[0x40];

#if !ID_USE_INSTRINSEC
	// verify we're at least a Pentium or 486 with CPUID support
	sysVars.cpuid = CPUID_GENERIC;
#else

	uint32_t nIds = 0;
	uint32_t nExIds = 0;
	uint32_t cpui[4] = { 0, 0, 0, 0 };
	memset( vendor, 0, sizeof( vendor ) );
	memset( brand, 0, sizeof( brand ) );


	// Calling __cpuid with 0x0 as the function_id argument
	// gets the number of the highest valid function ID.

	__get_cpuid( CPU_VENDOR_BIT, &cpui[_REG_EAX], &cpui[_REG_EBX], &cpui[_REG_ECX], &cpui[_REG_EDX] );
	nIds = cpui[_REG_EAX];

	// Calling __cpuid with 0x80000000 as the function_id argument
	// gets the number of the highest valid extended ID.
	__get_cpuid( CPU_BRAND_BIT, &cpui[_REG_EAX], &cpui[_REG_EBX], &cpui[_REG_ECX], &cpui[_REG_EDX] );
	nExIds = cpui[_REG_EAX];

	// Capture vendor string
	__get_cpuid( CPU_BRAND_BIT, &cpui[_REG_EAX], &cpui[_REG_EBX], &cpui[_REG_ECX], &cpui[_REG_EDX] );
	*reinterpret_cast<uint32_t*>(vendor) = cpui[_REG_EBX];
	*reinterpret_cast<uint32_t*>(vendor + 4) = cpui[_REG_EDX];
	*reinterpret_cast<uint32_t*>(vendor + 8) = cpui[_REG_ECX];
	strcpy( const_cast<char*>(vendorName), vendor );

	// Interpret CPU brand string if reported
	if (nExIds >= 0x80000004)
	{
		

		for ( uint32_t i = CPU_BRAND_BIT; i <= nExIds; ++i )
		{
			__get_cpuid( i, &cpui[_REG_EAX], &cpui[_REG_EBX], &cpui[_REG_ECX], &cpui[_REG_EDX] );

			if (i == 0x80000002)
				memcpy( brand, cpui, sizeof( cpui ) );
			else if (i == 0x80000003)
				memcpy( brand + 16, cpui, sizeof( cpui ) );
			else if (i == 0x80000004)
				memcpy( brand + 32, cpui, sizeof( cpui ) );
		}

		strcpy( const_cast<char*>(cpuBrand), brand );
	}
	
	// check for an AMD
	if ( strcmp( vendor, "AuthenticAMD" ) == 0)
		flags = CPUID_AMD;
	// check for an Intel
	else if (strcmp( vendor, "GenuineIntel" ) == 0)
		flags = CPUID_INTEL;
	else
		flags = CPUID_GENERIC;

	if ( nIds >= 1 )
	{
		__get_cpuid( 1, &cpui[_REG_EAX], &cpui[_REG_EBX], &cpui[_REG_ECX], &cpui[_REG_EDX] );

		// check for Multi Media Extensions
		if (cpui[_REG_EDX] & BIT_MMX)
			flags |= CPUID_MMX;

		// check for Streaming SIMD Extensions
		if (cpui[_REG_EDX] & BIT_SSE)
			flags |= CPUID_SSE | CPUID_FTZ;

		// check for Streaming SIMD Extensions 2
		if (cpui[_REG_EDX] & BIT_SSE2)
			flags |= CPUID_SSE2;

		// check for Streaming SIMD Extensions 3 aka Prescott's New Instructions
		if (cpui[_REG_ECX] & BIT_SSE3)
			flags |= CPUID_SSE3;

		// check for Conditional Move (CMOV) and fast floating point comparison (FCOMI) instructions
		if (cpui[_REG_EDX] & BIT_CMOV)
			flags |= CPUID_CMOV;

		// check for Hyper-Threading Technology
//		if ( (cpui[_REG_EDX] & BIT_HTT) && HTTEnable() )
//			flags |= CPUID_HTT;
	}

	// load bitset with flags for function 0x80000001
	if (nExIds >= 0x80000001)
	{
		__get_cpuid( 0x80000001, &cpui[_REG_EAX], &cpui[_REG_EBX], &cpui[_REG_ECX], &cpui[_REG_EDX] );

		// check for 3DNow!
		if (cpui[_REG_EDX] & BIT_3DNOW)
			flags |= CPUID_3DNOW;
	}

	// check for Denormals-Are-Zero mode
	uint32_t dwMask = _MM_GET_DENORMALS_ZERO_MODE();
	if ((dwMask & (1 << 6)) == (1 << 6))
		flags |= CPUID_DAZ;

	sysVars.cpuid = (cpuid_t)flags;

	common->Printf( "Running on %s %s", vendorName, cpuBrand );
	
#endif //ID_USE_INSTRINSEC
}

/*
================
Sys_GetProcessorId
================
*/
cpuid_t Sys_GetProcessorId( void )
{
	return sysVars.cpuid;
}

/*
===============================================================================

	FPU

===============================================================================
*/

/*
================
Sys_FPU_SetDAZ
================
*/
void Sys_FPU_SetDAZ( bool enable )
{
#if ID_USE_INSTRINSEC
	int mode = _MM_GET_DENORMALS_ZERO_MODE();
	if (enable && mode != _MM_DENORMALS_ZERO_ON)
		_MM_SET_DENORMALS_ZERO_MODE( _MM_DENORMALS_ZERO_ON );
	else if (!enable && mode == _MM_DENORMALS_ZERO_ON)
		_MM_SET_DENORMALS_ZERO_MODE( _MM_DENORMALS_ZERO_OFF );
#endif // ID_USE_INSTRINSEC
}

/*
================
Sys_FPU_SetFTZ
================
*/
void Sys_FPU_SetFTZ( bool enable )
{
#if ID_USE_INSTRINSEC
	int mode = _MM_GET_FLUSH_ZERO_MODE();
	if (enable && mode != _MM_FLUSH_ZERO_ON)
		_MM_SET_FLUSH_ZERO_MODE( _MM_FLUSH_ZERO_ON );
	else if (!enable && mode == _MM_FLUSH_ZERO_ON)
		_MM_SET_FLUSH_ZERO_MODE( _MM_FLUSH_ZERO_OFF );
#endif // ID_USE_INSTRINSEC
}
