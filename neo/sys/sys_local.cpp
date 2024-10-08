/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "idlib/precompiled.h"
#pragma hdrstop


// BEATO Begin
#include <SDL2/SDL_clipboard.h>
#include <SDL2/SDL_timer.h>
// BEATO End

#include "sys_main.h"
#include "sys_local.h"

const char * sysLanguageNames[] = {
	"english", "spanish", "italian", "german", "french", "russian", 
	"polish", "korean", "japanese", "chinese", NULL
};

idCVar sys_lang( "sys_lang", "english", CVAR_SYSTEM | CVAR_ARCHIVE,  "", sysLanguageNames, idCmdSystem::ArgCompletion_String<sysLanguageNames> );

void idSysLocal::DebugPrintf( const char *fmt, ... )
{
	va_list argptr;
	va_start( argptr, fmt );
	Sys_DebugVPrintf( fmt, argptr );
	va_end( argptr );
}

void idSysLocal::DebugVPrintf( const char *fmt, va_list arg ) 
{
	Sys_DebugVPrintf( fmt, arg );
}

double idSysLocal::GetClockTicks( void ) 
{
	return Sys_GetClockTicks();
}

double idSysLocal::ClockTicksPerSecond( void ) 
{
	return Sys_ClockTicksPerSecond();
}

cpuid_t idSysLocal::GetProcessorId( void ) 
{
	return Sys_GetProcessorId();
}

const char *idSysLocal::GetProcessorString( void ) 
{
	return Sys_GetProcessorString();
}

const char *idSysLocal::FPU_GetState( void ) 
{
	return Sys_FPU_GetState();
}

bool idSysLocal::FPU_StackIsEmpty( void ) 
{
	return Sys_FPU_StackIsEmpty();
}

void idSysLocal::FPU_SetFTZ( bool enable ) 
{
	Sys_FPU_SetFTZ( enable );
}

void idSysLocal::FPU_SetDAZ( bool enable ) 
{
	Sys_FPU_SetDAZ( enable );
}

bool idSysLocal::LockMemory( void *ptr, int bytes ) 
{
	return Sys_LockMemory( ptr, bytes );
}

bool idSysLocal::UnlockMemory( void *ptr, int bytes ) 
{
	return Sys_UnlockMemory( ptr, bytes );
}

void idSysLocal::DLL_GetFileName( const char *baseName, char *dllName, int maxLength ) 
{
#ifdef _WIN32
	idStr::snPrintf( dllName, maxLength, "%s" CPUSTRING ".dll", baseName );
#elif defined( __linux__ )
	idStr::snPrintf( dllName, maxLength, "%s" CPUSTRING ".so", baseName );
#elif defined( MACOS_X )
	idStr::snPrintf( dllName, maxLength, "%s" ".dylib", baseName );
#else
#error OS define is required
#endif
}

sysEvent_t idSysLocal::GenerateMouseButtonEvent( int button, bool down ) 
{
	sysEvent_t ev;
	ev.evType = SE_KEY;
	ev.evValue = K_MOUSE1 + button - 1;
	ev.evValue2 = down;
	ev.evPtrLength = 0;
	ev.evPtr = NULL;
	return ev;
}

sysEvent_t idSysLocal::GenerateMouseMoveEvent( int deltax, int deltay ) 
{
	sysEvent_t ev;
	ev.evType = SE_MOUSE;
	ev.evValue = deltax;
	ev.evValue2 = deltay;
	ev.evPtrLength = 0;
	ev.evPtr = NULL;
	return ev;
}

void idSysLocal::FPU_EnableExceptions( int exceptions ) {
	Sys_FPU_EnableExceptions( exceptions );
}

/*
=================
Sys_TimeStampToStr
=================
*/
const char *Sys_TimeStampToStr( ID_TIME_T timeStamp ) {
	static char timeString[MAX_STRING_CHARS];
	timeString[0] = '\0';

	tm*	time = localtime( &timeStamp );
	idStr out;
	
	idStr lang = cvarSystem->GetCVarString( "sys_lang" );
	if ( lang.Icmp( "english" ) == 0 ) {
		// english gets "month/day/year  hour:min" + "am" or "pm"
		out = va( "%02d", time->tm_mon + 1 );
		out += "/";
		out += va( "%02d", time->tm_mday );
		out += "/";
		out += va( "%d", time->tm_year + 1900 );
		out += "\t";
		if ( time->tm_hour > 12 ) {
			out += va( "%02d", time->tm_hour - 12 );
		} else if ( time->tm_hour == 0 ) {
				out += "12";
		} else {
			out += va( "%02d", time->tm_hour );
		}
		out += ":";
		out +=va( "%02d", time->tm_min );
		if ( time->tm_hour >= 12 ) {
			out += "pm";
		} else {
			out += "am";
		}
	} else {
		// europeans get "day/month/year  24hour:min"
		out = va( "%02d", time->tm_mday );
		out += "/";
		out += va( "%02d", time->tm_mon + 1 );
		out += "/";
		out += va( "%d", time->tm_year + 1900 );
		out += "\t";
		out += va( "%02d", time->tm_hour );
		out += ":";
		out += va( "%02d", time->tm_min );
	}
	idStr::Copynz( timeString, out, sizeof( timeString ) );

	return timeString;
}

// BEATO BEGIN

/*
==============================================================

	Clock ticks

==============================================================
*/

/*
==============
Sys_Sleep
==============
*/
void Sys_Sleep( int msec )
{
	SDL_Delay( msec );
}

/*
================
Sys_Milliseconds
================
*/
uint64_t Sys_Milliseconds( void )
{
	return SDL_GetTicks64();
}

/*
========================
Sys_Microseconds
========================
*/
uint64_t Sys_Microseconds( void ) 
{
	static uint64_t ticksPerMicrosecondTimes1024 = 0;

	Sys_GetClockTicks();
	if ( ticksPerMicrosecondTimes1024 == 0 ) 
	{
		ticksPerMicrosecondTimes1024 = ( SDL_GetPerformanceFrequency() << 10 ) / 1000000;
		assert( ticksPerMicrosecondTimes1024 > 0 );
	}

	return ( ( SDL_GetPerformanceCounter() << 10 ) ) / ticksPerMicrosecondTimes1024;
}


/*
================
Sys_LockMemory
================
*/
bool Sys_LockMemory( void *ptr, const size_t bytes ) 
{
#ifdef _WIN32
	return ( VirtualLock( ptr, (SIZE_T)bytes ) != FALSE );
#else
	return false;
#endif
}

/*
================
Sys_UnlockMemory
================
*/
bool Sys_UnlockMemory( void *ptr, const size_t bytes ) 
{
#ifdef _WIN32
	return ( VirtualUnlock( ptr, (SIZE_T)bytes ) != FALSE );
#else
	return false;
#endif
}

/*
================
Sys_SetPhysicalWorkMemory
================
*/
void Sys_SetPhysicalWorkMemory( const intptr_t minBytes, const intptr_t maxBytes ) 
{
#ifdef _WIN32
	::SetProcessWorkingSetSize( GetCurrentProcess(), minBytes, maxBytes );
#endif
}

/*
=================
Sys_GetMemoryStatus
=================
*/
void Sys_GetMemoryStatus( sysMemoryStats_t &stats ) 
{
	common->Printf( "FIXME: Sys_GetMemoryStatus stub\n" );
}

/*
=================
Sys_GetMemoryStatus
=================
*/
void Sys_GetCurrentMemoryStatus( sysMemoryStats_t &stats ) 
{
	common->Printf( "FIXME: Sys_GetCurrentMemoryStatus\n" );
}

/*
=================
Sys_GetMemoryStatus
=================
*/
void Sys_GetExeLaunchMemoryStatus( sysMemoryStats_t &stats ) 
{
	common->Printf( "FIXME: Sys_GetExeLaunchMemoryStatus\n" );
}

/*
 ==================
 Sys_DoPreferences
 ==================
 */
void Sys_DoPreferences( void ) 
{ 
}



/*
================
Sys_GetClockTicks
================
*/
double Sys_GetClockTicks( void )
{
	return SDL_GetPerformanceCounter();
}

/*
================
Sys_ClockTicksPerSecond
================
*/
double Sys_ClockTicksPerSecond( void )
{
	return SDL_GetPerformanceFrequency();
}


struct SDL_ClipBoart
{
	char* txt;

	SDL_ClipBoart( void ) : txt( nullptr )
	{
		txt = SDL_GetClipboardText();
	};

	~SDL_ClipBoart( void ) // auto clear when out of scope
	{
		SDL_free( txt );
	};

	const size_t size( void ) const
	{
		return sizeof( txt ) + 1;
	}
};

/*
================
Sys_GetClipboardData
================
*/
char *Sys_GetClipboardData( void )
{
	char *data = nullptr;
	if (SDL_HasClipboardText() == SDL_FALSE)
		return (char*)"0/";

	SDL_ClipBoart clipBoard = SDL_ClipBoart(); //Reads system clipboard

	data = (char *)Mem_Alloc( clipBoard.size() );
	SDL_memcpy( data, clipBoard.txt, clipBoard.size() );

	return data;
}

/*
================
Sys_SetClipboardData
================
*/
void Sys_SetClipboardData( const char *string )
{
	SDL_SetClipboardText( string );
}

/*
================
Sys_GetSystemRam

	returns amount of physical memory in MB
================
*/
int Sys_GetSystemRam( void )
{
	return SDL_GetSystemRAM();
}

// BEATO END