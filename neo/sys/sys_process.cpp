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

idCVar sys_cpustring( "sys_cpustring", "detect", CVAR_SYSTEM | CVAR_INIT, "" );

#if defined( __linux__ )
#	include <sched.h>
#   include <unistd.h>
#endif // __linux__

#include "sys_main.h"

// pid - useful when you attach to gdb..
idCVar com_pid( "com_pid", "0", CVAR_INTEGER | CVAR_INIT | CVAR_SYSTEM, "process id" );



#if defined( __linux__ )

#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <execinfo.h>

static char fatalError[ 1024 ];
static int set_exit = 0;
static char exit_spawn[ 1024 ];

const int siglist[] = 
{
	SIGHUP,
	SIGQUIT,
	SIGILL,
	SIGTRAP,
	SIGIOT,
	SIGBUS,
	SIGFPE,
	SIGSEGV,
	SIGPIPE,
	SIGABRT,
	//	SIGTTIN,
	//	SIGTTOU,
	-1
	};

const char *signames[] = 
{
	"SIGHUP",
	"SIGQUIT",
	"SIGILL",
	"SIGTRAP",
	"SIGIOT",
	"SIGBUS",
	"SIGFPE",
	"SIGSEGV",
	"SIGPIPE",
	"SIGABRT",
	//	"SIGTTIN",
	//	"SIGTTOUT"
};

static void Sys_DoStartProcess( const char *exeName, bool dofork = false );

/*
================
Posix_Exit
================
*/
static void Posix_Exit(int ret) 
{
	// in case of signal, handler tries a common->Quit
	// we use set_exit to maintain a correct exit code
	if ( set_exit ) 
		exit( set_exit );

	exit( ret );
}

/*
================
Posix_SetExit
================
*/
static void Posix_SetExit(int ret) 
{
	set_exit = 0;
}

/*
===============
Posix_SetExitSpawn
set the process to be spawned when we quit
===============
*/
static void Posix_SetExitSpawn( const char *exeName ) 
{
	idStr::Copynz( exit_spawn, exeName, 1024 );
}

/*
===============
Sys_FPE_handler
===============
*/
static void Sys_FPE_handler( int signum, siginfo_t *info, void *context ) 
{
	assert( signum == SIGFPE );
	Sys_Printf( "FPE\n" );
}

/*
================
sig_handler
================
*/
static void Posix_SigHandler( int signum, siginfo_t *info, void *context ) 
{
	static bool double_fault = false;
	
	if ( double_fault ) 
    {
		Sys_Printf( "double fault %s, bailing out\n", strsignal( signum ) );
		Posix_Exit( signum );
	}
	
	double_fault = true;
	
	// NOTE: see sigaction man page, could verbose the whole siginfo_t and print human readable si_code
	Sys_Printf( "signal caught: %s\nsi_code %d\n", strsignal( signum ), info->si_code );	
	
#ifndef ID_BT_STUB
	Sys_Printf( "callstack:\n%s", Sys_GetCallStackCurStr( 30 ) );
#endif

	if ( fatalError[ 0 ] ) 
    {
		Sys_Printf( "Was in fatal error shutdown: %s\n", fatalError );
	}
	
	Sys_Printf( "Trying to exit gracefully..\n" );
	
	Posix_SetExit( signum );
	
	common->Quit();
}

/*
================
Posix_InitSigs
================
*/
static void Posix_InitSigs( void )
{
	struct sigaction action;
	int i;

	fatalError[0] = '\0';
	
	/* Set up the structure */
	action.sa_sigaction = Posix_SigHandler;
	sigemptyset( &action.sa_mask );
	action.sa_flags = SA_SIGINFO | SA_NODEFER;

	i = 0;
	while ( siglist[ i ] != -1 ) {
		if ( siglist[ i ] == SIGFPE ) 
        {
			action.sa_sigaction = Sys_FPE_handler;
			if ( sigaction( siglist[ i ], &action, NULL ) != 0 ) 
            {
				Sys_Printf( "Failed to set SIGFPE handler: %s\n", strerror( errno ) );
			}
			action.sa_sigaction = Posix_SigHandler;
		} 
        else if ( sigaction( siglist[ i ], &action, NULL ) != 0 ) 
        {
			Sys_Printf( "Failed to set %s handler: %s\n", signames[ i ], strerror( errno ) );
		}
		i++;
	}

	// if the process is backgrounded (running non interactively)
	// then SIGTTIN or SIGTOU could be emitted, if not caught, turns into a SIGSTP
	signal( SIGTTIN, SIG_IGN );
	signal( SIGTTOU, SIG_IGN );	
}

/*
================
Posix_ClearSigs
================
*/
static void Posix_ClearSigs( void ) 
{
	struct sigaction action;
	int i;
	
	// Set up the structure 
	action.sa_handler = SIG_DFL;
	sigemptyset( &action.sa_mask );
	action.sa_flags = 0;

	i = 0;
	while ( siglist[ i ] != -1 ) 
    {
		if ( sigaction( siglist[ i ], &action, NULL ) != 0 ) 
        {
			Sys_Printf( "Failed to reset %s handler: %s\n", signames[ i ], strerror( errno ) );
		}
		i++;
	}
}

#endif //__linux__

/*
================
Sys_AlreadyRunning

returns true if there is a copy of D3 running already
================
*/
bool Sys_AlreadyRunning( void )
{
	return false;
}

/*
==============
Sys_Quit
==============
*/
void Sys_Quit( const int code )
{
#if defined(_WIN32)
	timeEndPeriod( 1 );
#endif

	Sys_ShutdownInput();
	Sys_DestroyConsole();

#if _WIN32 || _WIN64
	ExitProcess( code );
#endif
}

/*
===============
Sys_Shutdown
===============
*/
void Sys_Shutdown( void )
{
    Sys_ShutdownInput();
    Sys_videoShutDown();
		
#if defined( _WIN32 )
	CoUninitialize();
#endif //_WIN32
}

/*
=================
Sys_Init
=================
*/
void Sys_Init( void ) 
{
#if defined( ID_OS_WINDOWS )
	CoInitialize( NULL );
#endif //_WIN32

#if defined(__linux__)
	com_pid.SetInteger( getpid() );
	common->Printf( "pid: %d\n", com_pid.GetInteger() );
#endif // linux 

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
#ifndef ID_DEDICATED
	common->Printf( "%d MB Video Memory\n", Sys_GetVideoRam() );
#endif

	// Get Video Info, and create the main window
	//Sys_videoStartUp();

	// Init Render library API Config
#if CR_USE_VULKAN
	Sys_StartUpVulkanLib();
#else
	Sys_StartUpOpenGLAPI();
#endif
		// input and sound systems need to be tied to the new window
	Sys_InitInput();
	soundSystem->InitHW();
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
==================
idSysLocal::StartProcess
==================
*/
void idSysLocal::StartProcess( const char *exeName, bool quit ) 
{
#if defined( _WIN32 )

	TCHAR				szPathOrig[_MAX_PATH];
	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);

	strncpy( szPathOrig, exeName, _MAX_PATH );

	if( !CreateProcess( nullptr, szPathOrig, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi ) ) 
	{
        common->Error( "Could not start process: '%s' ", szPathOrig );
	    return;
	}

	if ( quit ) 
		cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "quit\n" );

#elif defined( __linux__ )
	//if !quit, start the process asap
	//otherwise, push it for execution at exit
	//(i.e. let complete shutdown of the game and freeing of resources happen)
	//NOTE: might even want to add a small delay?
	if ( quit ) 
	{
		common->DPrintf( "Sys_StartProcess %s (delaying until final exit)\n", exeName );
		Posix_SetExitSpawn( exeName );
		cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "quit\n" );
		return;
	}

	common->DPrintf( "Sys_StartProcess %s\n", exeName );
	Sys_DoStartProcess( exeName );
#endif
}

/*
=================
Sys_OpenURL
=================
*/
void idSysLocal::OpenURL( const char *url, bool quit ) 
{
#if defined(_WIN32)

	static bool doexit_spamguard = false;
	HWND wnd;

	if (doexit_spamguard) 
	{
		common->DPrintf( "OpenURL: already in an exit sequence, ignoring %s\n", url );
		return;
	}

	common->Printf("Open URL: %s\n", url);

	if ( !ShellExecute( NULL, "open", url, NULL, NULL, SW_RESTORE ) ) 
	{
		common->Error( "Could not open url: '%s' ", url );
		return;
	}

	wnd = GetForegroundWindow();
	if ( wnd ) 
	{
		ShowWindow( wnd, SW_MAXIMIZE );
	}

	if ( quit ) 
	{
		doexit_spamguard = true;
		cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "quit\n" );
	}

#elif defined(__linux__)
	const char	*script_path;
	idFile		*script_file;
	char		cmdline[ 1024 ];

	static bool	quit_spamguard = false;

	if ( quit_spamguard ) 
	{
		common->DPrintf( "Sys_OpenURL: already in a doexit sequence, ignoring %s\n", url );
		return;
	}

	common->Printf( "Open URL: %s\n", url );
	// opening an URL on *nix can mean a lot of things .. 
	// just spawn a script instead of deciding for the user :-)

	// look in the savepath first, then in the basepath
	script_path = fileSystem->BuildOSPath( cvarSystem->GetCVarString( "fs_savepath" ), "", "openurl.sh" );
	script_file = fileSystem->OpenExplicitFileRead( script_path );
	if ( !script_file ) 
	{
		script_path = fileSystem->BuildOSPath( cvarSystem->GetCVarString( "fs_basepath" ), "", "openurl.sh" );
		script_file = fileSystem->OpenExplicitFileRead( script_path );
	}

	if ( !script_file ) 
	{
		common->Printf( "Can't find URL script 'openurl.sh' in either savepath or basepath\n" );
		common->Printf( "OpenURL '%s' failed\n", url );
		return;
	}
	fileSystem->CloseFile( script_file );

	// if we are going to quit, only accept a single URL before quitting and spawning the script
	if ( quit ) 
	{
		quit_spamguard = true;
	}

	common->Printf( "URL script: %s\n", script_path );

	// StartProcess is going to execute a system() call with that - hence the &
	idStr::snPrintf( cmdline, 1024, "%s '%s' &",  script_path, url );
	sys->StartProcess( cmdline, quit );
#endif 
}

#if defined(__linux__)
/*
==================
Sys_DoStartProcess
if we don't fork, this function never returns
the no-fork lets you keep the terminal when you're about to spawn an installer

if the command contains spaces, system() is used. Otherwise the more straightforward execl ( system() blows though )
==================
*/
void Sys_DoStartProcess( const char *exeName, bool dofork ) 
{	
	bool use_system = false;
	if ( strchr( exeName, ' ' ) ) 
	{
		use_system = true;
	} 
	else 
	{
		// set exec rights when it's about a single file to execute
		struct stat buf;
		if ( stat( exeName, &buf ) == -1 ) 
		{
			printf( "stat %s failed: %s\n", exeName, strerror( errno ) );
		} 
		else 
		{
			if ( chmod( exeName, buf.st_mode | S_IXUSR ) == -1 ) 
			{
				printf( "cmod +x %s failed: %s\n", exeName, strerror( errno ) );
			}
		}
	}
	
	if ( dofork ) 
	{
		switch ( fork() ) 
		{
		case -1:
			// main thread
			break;
		case 0:
			if ( use_system ) 
			{
				printf( "system %s\n", exeName );
				system( exeName );
				_exit( 0 );
			} 
			else 
			{
				printf( "execl %s\n", exeName );
				execl( exeName, exeName, NULL );
				printf( "execl failed: %s\n", strerror( errno ) );
				_exit( -1 );
			}
			break;
		}
	} 
	else 
	{
		if ( use_system ) 
		{
			printf( "system %s\n", exeName );
			system( exeName );
			sleep( 1 );	// on some systems I've seen that starting the new process and exiting this one should not be too close
		} 
		else 
		{
			printf( "execl %s\n", exeName );
			execl( exeName, exeName, nullptr );
			printf( "execl failed: %s\n", strerror( errno ) );
		}
		// terminate
		_exit( 0 );
	}
}

void ExitProcess( int ret )
{
	// at this point, too late to catch signals
	Posix_ClearSigs();

	// process spawning. it's best when it happens after everything has shut down
	if ( exit_spawn[0] ) 
		Sys_DoStartProcess( exit_spawn, false );

	// in case of signal, handler tries a common->Quit
	// we use set_exit to maintain a correct exit code
	if ( set_exit ) 
		exit( set_exit );

	exit( ret );
}

#endif //__linux__


// allows retrieving the call stack at execution points
//void			Sys_GetCallStack( address_t *callStack, const int callStackSize );
//const char *	Sys_GetCallStackStr( const address_t *callStack, const int callStackSize );
//const char *	Sys_GetCallStackCurStr( int depth );
//const char *	Sys_GetCallStackCurAddressStr( int depth );
//void			Sys_ShutdownSymbols( void );

void idSysLocal::GetCallStack( address_t *callStack, const int callStackSize ) 
{
#if 0 
	void* stacks[callStackSize];
	CaptureStackBackTrace( 0, callStackSize, stacks, nullptr );

	while( i < callStackSize ) 
	{
		callStack[i++] = 0;
	}
#elif defined(__linux__) &&  defined( _DEBUG )

	int i;
	i = backtrace( (void **)callStack, callStackSize );	
	while( i < callStackSize ) 
	{
		callStack[i++] = 0;
	}
#endif //
}

const char * idSysLocal::GetCallStackStr( const address_t *callStack, const int callStackSize ) 
{
#if 0 

#elif defined(__linux__) &&  defined( _DEBUG )

	static char string[MAX_STRING_CHARS*2];
	char **strings = nullptr;
	int i;
	
	strings = backtrace_symbols( (void **)callStack, callStackSize );
	string[ 0 ] = '\0';
	
	for ( i = 0; i < callStackSize; i++ ) 
	{
		idStr::snPrintf( string + strlen( string ), MAX_STRING_CHARS*2 - strlen( string ) - 1, "%s\n", strings[ i ] );
	}
	
	free( strings );
	
	return string;	

#endif //

	return nullptr;
}

const char * idSysLocal::GetCallStackCurStr( int depth ) 
{
#if defined( _WIN32 )

	address_t *callStack;
	callStack = (address_t *) _alloca( depth * sizeof( address_t ) );
	GetCallStack( callStack, depth );
	return GetCallStackStr( callStack, depth );

#elif defined(__linux__) &&  defined( _DEBUG )

	address_t array[ 32 ];
	size_t size;
	size = backtrace( (void **)array, min( depth, 32 ) );
	return GetCallStackStr( array, (int)size );

#endif //
	return nullptr;
}

void idSysLocal::ShutdownSymbols( void ) 
{
#if defined( _WIN32 )
#elif defined(__linux__) 
#endif
}


/*
========================
Sys_YieldThread
========================
*/
void 	Sys_YieldThread( void )
{
#if defined( __linux__ )
	sched_yield();
#elif defined( _WIN32 )
	Yield();
#endif // _WIN32
}