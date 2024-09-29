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

#include <SDL2/SDL_cpuinfo.h>

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

#if _WIN32
/*
================
LogicalProcPerPhysicalProc
================
*/
#define NUM_LOGICAL_BITS   0x00FF0000     // EBX[23:16] Bit 16-23 in ebx contains the number of logical
// processors per physical processor when execute cpuid with 
// eax set to 1

static int LogicalProcPerPhysicalProc( void )
{
	unsigned int regs[4];
	__get_cpuid( 1, &regs[_REG_EAX], &regs[_REG_EBX], &regs[_REG_ECX], &regs[_REG_EDX] );
	return (unsigned char)((regs[_REG_EBX] & NUM_LOGICAL_BITS) >> 16);
}

#define INITIAL_APIC_ID_BITS  0xFF000000  // EBX[31:24] Bits 24-31 (8 bits) return the 8-bit unique 
// initial APIC ID for the processor this code is running on.
// Default value = 0xff if HT is not supported
static unsigned char GetAPIC_ID( void )
{
	unsigned int regs[4];
	__get_cpuid( 1, &regs[_REG_EAX], &regs[_REG_EBX], &regs[_REG_ECX], &regs[_REG_EDX] );
	return (unsigned char)((regs[_REG_EBX] & INITIAL_APIC_ID_BITS) >> 24);
}

/*
================
CPUCount

	logicalNum is the number of logical CPU per physical CPU
	physicalNum is the total number of physical processor
	returns one of the HT_* flags
================
*/
#define HT_NOT_CAPABLE				0
#define HT_ENABLED					1
#define HT_DISABLED					2
#define HT_SUPPORTED_NOT_ENABLED	3
#define HT_CANNOT_DETECT			4

int CPUCount( int &logicalNum, int &physicalNum )
{
	int statusFlag;
	SYSTEM_INFO info;

	physicalNum = 1;
	logicalNum = 1;
	statusFlag = HT_NOT_CAPABLE;

	info.dwNumberOfProcessors = 0;
	GetSystemInfo( &info );

	// Number of physical processors in a non-Intel system
	// or in a 32-bit Intel system with Hyper-Threading technology disabled
	physicalNum = info.dwNumberOfProcessors;

	unsigned char HT_Enabled = 0;

	logicalNum = LogicalProcPerPhysicalProc();

	if (logicalNum >= 1)
	{	// > 1 doesn't mean HT is enabled in the BIOS
		HANDLE hCurrentProcessHandle;
		DWORD  dwProcessAffinity;
		DWORD  dwSystemAffinity;
		DWORD  dwAffinityMask;

		// Calculate the appropriate  shifts and mask based on the 
		// number of logical processors.

		unsigned char i = 1, PHY_ID_MASK = 0xFF, PHY_ID_SHIFT = 0;

		while (i < logicalNum)
		{
			i *= 2;
			PHY_ID_MASK <<= 1;
			PHY_ID_SHIFT++;
		}

		hCurrentProcessHandle = GetCurrentProcess();
		GetProcessAffinityMask( hCurrentProcessHandle, &dwProcessAffinity, &dwSystemAffinity );

		// Check if available process affinity mask is equal to the
		// available system affinity mask
		if (dwProcessAffinity != dwSystemAffinity)
		{
			statusFlag = HT_CANNOT_DETECT;
			physicalNum = -1;
			return statusFlag;
		}

		dwAffinityMask = 1;
		while (dwAffinityMask != 0 && dwAffinityMask <= dwProcessAffinity)
		{
			// Check if this CPU is available
			if (dwAffinityMask & dwProcessAffinity)
			{
				if (SetProcessAffinityMask( hCurrentProcessHandle, dwAffinityMask ))
				{
					unsigned char APIC_ID, LOG_ID, PHY_ID;

					Sleep( 0 ); // Give OS time to switch CPU

					APIC_ID = GetAPIC_ID();
					LOG_ID = APIC_ID & ~PHY_ID_MASK;
					PHY_ID = APIC_ID >> PHY_ID_SHIFT;

					if (LOG_ID != 0)
					{
						HT_Enabled = 1;
					}
				}
			}
			dwAffinityMask = dwAffinityMask << 1;
		}

		// Reset the processor affinity
		SetProcessAffinityMask( hCurrentProcessHandle, dwProcessAffinity );

		if (logicalNum == 1)
		{  // Normal P4 : HT is disabled in hardware
			statusFlag = HT_DISABLED;
		}
		else
		{
			if (HT_Enabled)
			{
				// Total physical processors in a Hyper-Threading enabled system.
				physicalNum /= logicalNum;
				statusFlag = HT_ENABLED;
			}
			else
			{
				statusFlag = HT_SUPPORTED_NOT_ENABLED;
			}
		}
	}
	return statusFlag;
}
#endif //_WIN32

/*
================
HasHTT
================
*/
static bool HTTEnable( void )
{
#if _WIN32
	int logicalNum, physicalNum, HTStatusFlag;
	HTStatusFlag = CPUCount( logicalNum, physicalNum );
	if (HTStatusFlag == HT_ENABLED)
		return true;
#endif //_WIN32
	return false;
}

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
		if ( (cpui[_REG_EDX] & BIT_HTT) && HTTEnable() )
			flags |= CPUID_HTT;
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
struct FPUEnvironment 
{
    uint16_t controlWord;
    uint16_t statusWord;
    uint16_t tagWord;
    uint32_t instructionPointer;
    uint32_t dataPointer;
    uint16_t opcode;
};

typedef struct bitFlag_s
{
	char *		name;
	int			bit;
} bitFlag_t;

static bitFlag_t controlWordFlags[] = {
	{ "Invalid operation", 0 },
	{ "Denormalized operand", 1 },
	{ "Divide-by-zero", 2 },
	{ "Numeric overflow", 3 },
	{ "Numeric underflow", 4 },
	{ "Inexact result (precision)", 5 },
	{ "Infinity control", 12 },
	{ "", 0 }
};
static char *precisionControlField[] = {
	"Single Precision (24-bits)",
	"Reserved",
	"Double Precision (53-bits)",
	"Double Extended Precision (64-bits)"
};
static char *roundingControlField[] = {
	"Round to nearest",
	"Round down",
	"Round up",
	"Round toward zero"
};
static bitFlag_t statusWordFlags[] = {
	{ "Invalid operation", 0 },
	{ "Denormalized operand", 1 },
	{ "Divide-by-zero", 2 },
	{ "Numeric overflow", 3 },
	{ "Numeric underflow", 4 },
	{ "Inexact result (precision)", 5 },
	{ "Stack fault", 6 },
	{ "Error summary status", 7 },
	{ "FPU busy", 15 },
	{ "", 0 }
};

// EXEPTION FLAGS
static const uint32_t CTW_EXCEPTION_INVALID_OPERATION = 1 << 0;
static const uint32_t CTW_EXCEPTION_DENORMALIZED_OPERAND = 1 << 1;
static const uint32_t CTW_EXCEPTION_DIVIDE_BY_ZERO = 1 << 2;
static const uint32_t CTW_EXCEPTION_NUMERIC_OVERFLOW = 1 << 3;
static const uint32_t CTW_EXCEPTION_NUMERIC_UNDERFLOW = 1 << 4;
static const uint32_t CTW_EXCEPTION_INEXACT_RESULT = 1 << 5;

//
static const uint32_t CTW_PRECISION_SINGLE24 = 0 << 8;
static const uint32_t CTW_PRECISION_NAD = 1 << 8;
static const uint32_t CTW_PRECISION_DOUBLE52 = 2 << 8;
static const uint32_t CTW_PRECISION_EXTENDEND64 = 3 << 8;

//
static const uint32_t CTW_ROUNDING_NEAREST = 0 << 10;
static const uint32_t CTW_ROUNDING_DOWN = 1 << 10;
static const uint32_t CTW_ROUNDING_UP = 2 << 10;
static const uint32_t CTW_ROUNDING_TRUNCATE = 3 << 10;

//
static const uint32_t TAG_STATE_VALID = 0;
static const uint32_t TAG_STATE_ZERO = 1;
static const uint32_t TAG_STATE_SPECIAL = 2;
static const uint32_t TAG_STATE_EMPTY = 3;

/*
===============
Sys_FPU_StackIsEmpty
===============
*/
bool Sys_FPU_StackIsEmpty( void )
{
#if ID_USE_INSTRINSEC
	// Inline assembly to obtain FPU status word
	uint16_t tagWord = _mm_fstenv();

// Verifica cada par de bits do Tag Word (16 bits no total, 2 bits por registrador)
    for (int i = 0; i < 8; ++i) 
	{
        // Verifica se o par de bits para o registrador st(i) é diferente de 00 (registrador vazio)
        if ( ( (tagWord >> ( i * 2 ) ) & 0x3 ) != 0x0) 
            return false;  // Se algum registrador não estiver vazio, a pilha não está vazia
    }
#endif	
	return true;
}

/*
===============
Sys_FPU_ClearStack
===============
*/

void Sys_FPU_ClearStack( void )
{
#if ID_USE_INSTRINSEC
	_mm_fninit(); 
#endif 
}

/*
===============
Sys_FPU_GetState

  gets the FPU state without changing the state
===============
*/
const char *Sys_FPU_GetState( void )
{
	FPUEnvironment env;
	__asm__ __volatile__ ( "fnstenv %0" : "=m" ( env ) );

	return "TODO:";
}

/*
===============
Sys_FPU_EnableExceptions
===============
*/
void Sys_FPU_EnableExceptions( int exceptions )
{
#if ID_USE_INSTRINSEC
	// Get current control world state 
	unsigned short controlWord = _mm_fnstcw();
	controlWord |= exceptions;

	// Update control world state
	_mm_fldcw( controlWord );
#endif
}

/*
===============
Sys_FPU_SetPrecision
===============
*/
void Sys_FPU_SetPrecision( int precision )
{
#if ID_USE_INSTRINSEC
	
	// Get current control world state 
	unsigned short controlWord = _mm_fnstcw();
	
	// Limpa os bits de precisão (bits 11-10)
    // Configura os bits de precisão desejados
	switch ( precision )
	{
	case FPU_PRECISION_SINGLE:
		controlWord |= CTW_PRECISION_SINGLE24;
		break;
	case FPU_PRECISION_DOUBLE:
		controlWord |= CTW_PRECISION_DOUBLE52;
		break;
	case FPU_PRECISION_DOUBLE_EXTENDED:
		controlWord |= CTW_PRECISION_EXTENDEND64;
		break;
	default:
		break;
	}

    controlWord |= CTW_ROUNDING_NEAREST;
	controlWord |= CTW_ROUNDING_DOWN;
	controlWord |= CTW_ROUNDING_UP;

	// update control world
	_mm_fldcw( controlWord );

#endif // ID_USE_INSTRINSEC
}

/*
================
Sys_FPU_SetRounding
================
*/
void Sys_FPU_SetRounding( int rounding )
{
#if ID_USE_INSTRINSEC

	unsigned short roundingBitTable[4] = { 0, 1, 2, 3 };
	unsigned short roundingBits = roundingBitTable[rounding & 3] << 10;
	
	// Get current control world state 
	unsigned short controlWord = _mm_fnstcw();

	// Limpa os bits de arredondamento (bits 10-12)
    controlWord &= 0xF3FF;

	// Configura os bits de arredondamento desejados
    controlWord |= roundingBits;

	// update control world
	_mm_fldcw( controlWord );

#endif // ID_USE_INSTRINSEC
}

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
