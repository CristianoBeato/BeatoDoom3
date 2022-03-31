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

#ifndef __SYS_MAIN__
#define __SYS_MAIN__

void Sys_getCPUInfo( void );

void Sys_CreateConsole( void );
void Sys_StartThreadPool( void );
void Sys_ShutDownThreadPool( void );
void Sys_StartThread( const btThreadExecution* thread );

void Sys_DestroyConsole( void );
void Sys_CreateConsole( void );

//
void Sys_InitOpenGL( void );
void Sys_ShutDownOpenGL( void );

struct btThreadContext;
typedef struct
{
	cpuid_t							cpuid;
	Uint32							numThreads;					// System Thread Count
	btAtommicCounter				threadCount;				// Running Threads
	idStaticList< btThreadContext*, MAX_THREADS>	threadPool;	// Thread Pool

	sysMemoryStats_t				exeLaunchMemoryStats;

	class btWindow*			window;		// Main Game Window Handle
	class btInputEvent*		input;		// Event Input Manager
	class btConsoleLog*		console;	// Console/Dedicate mode log window

	static idCVar sys_viewlog;			// Enable Log Console in game
	static idCVar sys_outputEditString; 
} sysVars_t;
static sysVars_t sysVars;

#endif /* !__SYS_MAIN__ */
