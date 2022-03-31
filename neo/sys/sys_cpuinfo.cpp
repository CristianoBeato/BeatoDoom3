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

#include <SDL_cpuinfo.h>

#if ID_USE_INSTRINSEC

#include <immintrin.h>
#include <xmmintrin.h>
#include <pmmintrin.h>

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

idCVar sys_cpustring( "sys_cpustring", "detect", CVAR_SYSTEM | CVAR_INIT, "" );

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

#if !ID_USE_INSTRINSEC
	// verify we're at least a Pentium or 486 with CPUID support
	return CPUID_UNSUPPORTED;
#else
	idStr	vendorName = idStr("Generic");
	idStr	cpuBrand = idStr("Generic");

	unsigned int nIds = 0;
	unsigned int nExIds = 0;
	unsigned int cpui[4] = { 0, 0, 0, 0 };

	// Calling __cpuid with 0x0 as the function_id argument
	// gets the number of the highest valid function ID.

	__get_cpuid( CPU_VENDOR_BIT, &cpui[_REG_EAX], &cpui[_REG_EBX], &cpui[_REG_ECX], &cpui[_REG_EDX] );
	nIds = cpui[_REG_EAX];

	// Calling __cpuid with 0x80000000 as the function_id argument
	// gets the number of the highest valid extended ID.
	__get_cpuid( CPU_BRAND_BIT, &cpui[_REG_EAX], &cpui[_REG_EBX], &cpui[_REG_ECX], &cpui[_REG_EDX] );
	nExIds = cpui[_REG_EAX];

	// Capture vendor string
	char vendor[0x20];
	memset( vendor, 0, sizeof( vendor ) );
	__get_cpuid( CPU_BRAND_BIT, &cpui[_REG_EAX], &cpui[_REG_EBX], &cpui[_REG_ECX], &cpui[_REG_EDX] );
	*reinterpret_cast<unsigned int*>(vendor) = cpui[_REG_EBX];
	*reinterpret_cast<unsigned int*>(vendor + 4) = cpui[_REG_EDX];
	*reinterpret_cast<unsigned int*>(vendor + 8) = cpui[_REG_ECX];
	vendorName = idStr( vendor );

	// Interpret CPU brand string if reported
	if (nExIds >= 0x80000004)
	{
		char brand[0x40];
		memset( brand, 0, sizeof( brand ) );

		for (int i = CPU_BRAND_BIT; i <= nExIds; ++i)
		{
			__get_cpuid( i, &cpui[_REG_EAX], &cpui[_REG_EBX], &cpui[_REG_ECX], &cpui[_REG_EDX] );

			if (i == 0x80000002)
				memcpy( brand, cpui, sizeof( cpui ) );
			else if (i == 0x80000003)
				memcpy( brand + 16, cpui, sizeof( cpui ) );
			else if (i == 0x80000004)
				memcpy( brand + 32, cpui, sizeof( cpui ) );
		}

		cpuBrand = idStr( brand );
	}
	
	// check for an AMD
	if (vendorName.Cmp( "AuthenticAMD" ) == 0)
		flags = CPUID_AMD;
	// check for an Intel
	else if (vendorName.Cmp( "GenuineIntel" ) == 0)
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
	dword dwMask = _MM_GET_DENORMALS_ZERO_MODE();
	if ((dwMask & (1 << 6)) == (1 << 6))
		flags |= CPUID_DAZ;

	sysVars.cpuid = (cpuid_t)flags;

	//
	// CPU type
	//
	if (!idStr::Icmp( sys_cpustring.GetString(), "detect" ))
	{
		idStr string;

		common->Printf( "%1.0f MHz ", Sys_ClockTicksPerSecond() / 1000000.0f );

		string.Clear();

		if (sysVars.cpuid & CPUID_AMD)
			string += "AMD CPU";
		else if (sysVars.cpuid & CPUID_INTEL)
			string += "Intel CPU";
		else if (sysVars.cpuid & CPUID_UNSUPPORTED)
			string += "unsupported CPU";
		else
			string += "generic CPU";

		string += " with ";
		if (sysVars.cpuid & CPUID_MMX)
			string += "MMX & ";
		if (sysVars.cpuid & CPUID_3DNOW)
			string += "3DNow! & ";
		if (sysVars.cpuid & CPUID_SSE)
			string += "SSE & ";
		if (sysVars.cpuid & CPUID_SSE2)
			string += "SSE2 & ";
		if (sysVars.cpuid & CPUID_SSE3)
			string += "SSE3 & ";
		if (sysVars.cpuid & CPUID_HTT)
			string += "HTT & ";

		string.StripTrailing( " & " );
		string.StripTrailing( " with " );
	}
	else
	{
		common->Printf( "forcing CPU type to " );
		idLexer src( sys_cpustring.GetString(), idStr::Length( sys_cpustring.GetString() ), "sys_cpustring" );
		idToken token;

		int id = CPUID_NONE;
		while (src.ReadToken( &token ))
		{
			if (token.Icmp( "generic" ) == 0)
				id |= CPUID_GENERIC;
			else if (token.Icmp( "intel" ) == 0)
				id |= CPUID_INTEL;
			else if (token.Icmp( "amd" ) == 0)
				id |= CPUID_AMD;
			else if (token.Icmp( "mmx" ) == 0)
				id |= CPUID_MMX;
			else if (token.Icmp( "3dnow" ) == 0)
				id |= CPUID_3DNOW;
			else if (token.Icmp( "sse" ) == 0)
				id |= CPUID_SSE;
			else if (token.Icmp( "sse2" ) == 0)
				id |= CPUID_SSE2;
			else if (token.Icmp( "sse3" ) == 0)
				id |= CPUID_SSE3;
			else if (token.Icmp( "htt" ) == 0)
				id |= CPUID_HTT;
		}
		if (id == CPUID_NONE)
		{
			common->Printf( "WARNING: unknown sys_cpustring '%s'\n", sys_cpustring.GetString() );
			id = CPUID_GENERIC;
		}
	}

	common->Printf( "%s\n", sys_cpustring.GetString() );
	common->Printf( "%d MB System Memory\n", Sys_GetSystemRam() );
	common->Printf( "%d MB Video Memory\n", Sys_GetVideoRam() );

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
================
Sys_GetProcessorString
================
*/
const char *Sys_GetProcessorString( void )
{
	return sys_cpustring.GetString();
}


/*
===============================================================================

	FPU

===============================================================================
*/

typedef struct bitFlag_s
{
	char *		name;
	int			bit;
} bitFlag_t;

static byte fpuState[128], *statePtr = fpuState;
static char fpuString[2048];
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

/*
===============
Sys_FPU_PrintStateFlags
===============
*/
int Sys_FPU_PrintStateFlags( char *ptr, int ctrl, int stat, int tags, int inof, int inse, int opof, int opse )
{
	int i, length = 0;

	length += sprintf( ptr + length, "CTRL = %08x\n"
		"STAT = %08x\n"
		"TAGS = %08x\n"
		"INOF = %08x\n"
		"INSE = %08x\n"
		"OPOF = %08x\n"
		"OPSE = %08x\n"
		"\n",
		ctrl, stat, tags, inof, inse, opof, opse );

	length += sprintf( ptr + length, "Control Word:\n" );
	for (i = 0; controlWordFlags[i].name[0]; i++)
	{
		length += sprintf( ptr + length, "  %-30s = %s\n", controlWordFlags[i].name, (ctrl & (1 << controlWordFlags[i].bit)) ? "true" : "false" );
	}
	length += sprintf( ptr + length, "  %-30s = %s\n", "Precision control", precisionControlField[(ctrl >> 8) & 3] );
	length += sprintf( ptr + length, "  %-30s = %s\n", "Rounding control", roundingControlField[(ctrl >> 10) & 3] );

	length += sprintf( ptr + length, "Status Word:\n" );
	for (i = 0; statusWordFlags[i].name[0]; i++)
	{
		ptr += sprintf( ptr + length, "  %-30s = %s\n", statusWordFlags[i].name, (stat & (1 << statusWordFlags[i].bit)) ? "true" : "false" );
	}
	length += sprintf( ptr + length, "  %-30s = %d%d%d%d\n", "Condition code", (stat >> 8) & 1, (stat >> 9) & 1, (stat >> 10) & 1, (stat >> 14) & 1 );
	length += sprintf( ptr + length, "  %-30s = %d\n", "Top of stack pointer", (stat >> 11) & 7 );

	return length;
}

/*
===============
Sys_FPU_StackIsEmpty
===============
*/
bool Sys_FPU_StackIsEmpty( void )
{
#if 0
	__asm {
		mov			eax, statePtr
		fnstenv[eax]
		mov			eax, [eax + 8]
		xor eax, 0xFFFFFFFF
		and eax, 0x0000FFFF
		jz			empty
	}
	return false;
empty:
	return true;
#else
	
	return false;
#endif

}

/*
===============
Sys_FPU_ClearStack
===============
*/
void Sys_FPU_ClearStack( void )
{
#if 0
	__asm {
		mov			eax, statePtr
		fnstenv[eax]
		mov			eax, [eax + 8]
		xor eax, 0xFFFFFFFF
		mov			edx, (3 << 14)
		emptyStack:
		mov			ecx, eax
			and			ecx, edx
			jz			done
			fstp		st
			shr			edx, 2
			jmp			emptyStack
			done :
	}
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
	double fpuStack[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	double *fpuStackPtr = fpuStack;
	int i, numValues;
	char *ptr;
#if 0
	__asm {
		mov			esi, statePtr
		mov			edi, fpuStackPtr
		fnstenv[esi]
		mov			esi, [esi + 8]
		xor esi, 0xFFFFFFFF
		mov			edx, (3 << 14)
		xor eax, eax
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fst			qword ptr[edi + 0]
		inc			eax
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st( 1 )
		fst			qword ptr[edi + 8]
		inc			eax
		fxch		st( 1 )
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st( 2 )
		fst			qword ptr[edi + 16]
		inc			eax
		fxch		st( 2 )
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st( 3 )
		fst			qword ptr[edi + 24]
		inc			eax
		fxch		st( 3 )
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st( 4 )
		fst			qword ptr[edi + 32]
		inc			eax
		fxch		st( 4 )
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st( 5 )
		fst			qword ptr[edi + 40]
		inc			eax
		fxch		st( 5 )
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st( 6 )
		fst			qword ptr[edi + 48]
		inc			eax
		fxch		st( 6 )
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st( 7 )
		fst			qword ptr[edi + 56]
		inc			eax
		fxch		st( 7 )
		done:
		mov			numValues, eax
	}

	int ctrl = *(int *)&fpuState[0];
	int stat = *(int *)&fpuState[4];
	int tags = *(int *)&fpuState[8];
	int inof = *(int *)&fpuState[12];
	int inse = *(int *)&fpuState[16];
	int opof = *(int *)&fpuState[20];
	int opse = *(int *)&fpuState[24];

	ptr = fpuString;
	ptr += sprintf( ptr, "FPU State:\n"
		"num values on stack = %d\n", numValues );
	for (i = 0; i < 8; i++)
	{
		ptr += sprintf( ptr, "ST%d = %1.10e\n", i, fpuStack[i] );
	}

	Sys_FPU_PrintStateFlags( ptr, ctrl, stat, tags, inof, inse, opof, opse );
#endif
	return fpuString;
}

/*
===============
Sys_FPU_EnableExceptions
===============
*/
void Sys_FPU_EnableExceptions( int exceptions )
{
	int mode = _MM_GET_EXCEPTION_STATE();
	if (exceptions != mode)
		_MM_SET_EXCEPTION_STATE( exceptions );
}

/*
===============
Sys_FPU_SetPrecision
===============
*/
void Sys_FPU_SetPrecision( int precision )
{
#if ID_USE_INSTRINSEC
	short precisionBitTable[4] = { 0, 1, 3, 0 };
	short precisionBits = precisionBitTable[precision & 3] << 8;
	short precisionMask = ~((1 << 9) | (1 << 8));
	_mm_setcsr( (_mm_getcsr() & ~precisionMask) | (precisionBits) );
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
	short roundingBitTable[4] = { 0, 1, 2, 3 };
	short roundingBits = roundingBitTable[rounding & 3] << 10;
	_MM_SET_ROUNDING_MODE( roundingBits );
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
