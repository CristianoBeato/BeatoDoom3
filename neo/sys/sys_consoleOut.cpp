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

#include "common/console.h"
#include "sys_main.h"

#define MAXPRINTMSG 4096

idCVar sysVars_t::sys_viewlog( "sys_viewlog", "0", CVAR_SYSTEM | CVAR_INTEGER, "" );
idCVar sysVars_t::sys_outputEditString( "sys_outputEditString", "1", CVAR_SYSTEM | CVAR_BOOL, "" );

#ifdef _WIN32
idCVar win_outputDebugString( "win_outputDebugString", "0", CVAR_SYSTEM | CVAR_BOOL, "" );
#else
#include <iostream>
#endif

void	Sys_CreateConsole( void )
{
	sysVars.console = new btConsoleLog();
	sysVars.console->CreateConsole();
}

void Sys_DestroyConsole( void )
{
	if (sysVars.console)
		sysVars.console->DestroyConsole();

	SAFE_DELETE( sysVars.console );
}

static void Sys_ShowConsole( int visLevel, bool quitOnClose )
{
	assert( sysVars.console );
	sysVars.console->ShowConsole( visLevel, quitOnClose );
}


/*
=============
Sys_Error

Show the early console as an error dialog
=============
*/
void Sys_Error( const char *error, ... )
{
	va_list		argptr;
	char		text[4096];
	MSG        msg;

	va_start( argptr, error );
	vsprintf( text, error, argptr );
	va_end( argptr );

	assert( sysVars.console );
	sysVars.console->AppendText( text );
	sysVars.console->AppendText( "\n" );

	sysVars.console->SetErrorText( text );
	sysVars.console->ShowConsole( 1, true );

	timeEndPeriod( 1 );

	Sys_ShutdownInput();

//	TODO:
//	GLimp_Shutdown();

	// wait for the user to quit
//	while (1)
//	{
//		if (!GetMessage( &msg, NULL, 0, 0 ))
//		{
//			common->Quit();
//		}
//		TranslateMessage( &msg );
//		DispatchMessage( &msg );
//	}

	Sys_DestroyConsole();

	exit( 1 );
}


/*
==============
Sys_Printf
==============
*/
void Sys_Printf( const char *fmt, ... )
{
	char		msg[MAXPRINTMSG];

	va_list argptr;
	va_start( argptr, fmt );
	idStr::vsnPrintf( msg, MAXPRINTMSG - 1, fmt, argptr );
	va_end( argptr );
	msg[sizeof( msg ) - 1] = '\0';

#ifdef _WIN32
	if (win_outputDebugString.GetBool())
		OutputDebugString( msg );
#else
	std::cerr << msg;
#endif

	if (sysVars.sys_outputEditString.GetBool())
		sysVars.console->AppendText( msg );
}

/*
==============
Sys_DebugPrintf
==============
*/
void Sys_DebugPrintf( const char *fmt, ... )
{
	char msg[MAXPRINTMSG];

	va_list argptr;
	va_start( argptr, fmt );
	idStr::vsnPrintf( msg, MAXPRINTMSG - 1, fmt, argptr );
	msg[sizeof( msg ) - 1] = '\0';
	va_end( argptr );

	OutputDebugString( msg );
}

/*
==============
Sys_DebugVPrintf
==============
*/
void Sys_DebugVPrintf( const char *fmt, va_list arg )
{
	char msg[MAXPRINTMSG];

	idStr::vsnPrintf( msg, MAXPRINTMSG - 1, fmt, arg );
	msg[sizeof( msg ) - 1] = '\0';

#ifdef _WIN32
	OutputDebugString( msg );
#else
std::cerr << msg;
#endif
}
