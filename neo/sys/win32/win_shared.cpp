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

#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "win_local.h"
#include <lmerr.h>
#include <lmcons.h>
#include <lmwksta.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <conio.h>
#include <float.h>
#include <mapi.h>
#include <ShellAPI.h>

#ifndef __MRC__
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "rc/CreateResourceIDs.h"

#ifndef	ID_DEDICATED
#include <comdef.h>
#include <comutil.h>
#include <Wbemidl.h>

#pragma comment (lib, "wbemuuid.lib")
#endif

idCVar Win32Vars_t::win_allowAltTab( "win_allowAltTab", "0", CVAR_SYSTEM | CVAR_BOOL, "allow Alt-Tab when fullscreen" );
idCVar Win32Vars_t::win_notaskkeys( "win_notaskkeys", "0", CVAR_SYSTEM | CVAR_INTEGER, "disable windows task keys" );
idCVar Win32Vars_t::win_username( "win_username", "", CVAR_SYSTEM | CVAR_INIT, "windows user name" );
idCVar Win32Vars_t::win_xpos( "win_xpos", "3", CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_INTEGER, "horizontal position of window" );
idCVar Win32Vars_t::win_ypos( "win_ypos", "22", CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_INTEGER, "vertical position of window" );
idCVar Win32Vars_t::win_timerUpdate( "win_timerUpdate", "0", CVAR_SYSTEM | CVAR_BOOL, "allows the game to be updated while dragging the window" );
idCVar Win32Vars_t::win_allowMultipleInstances( "win_allowMultipleInstances", "0", CVAR_SYSTEM | CVAR_BOOL, "allow multiple instances running concurrently" );


/*
==================
idSysLocal::OpenURL
==================
*/
void idSysLocal::OpenURL( const char *url, bool doexit )
{
	static bool doexit_spamguard = false;
	HWND wnd;

	if (doexit_spamguard)
	{
		common->DPrintf( "OpenURL: already in an exit sequence, ignoring %s\n", url );
		return;
	}

	common->Printf( "Open URL: %s\n", url );

	if (!ShellExecute( NULL, "open", url, NULL, NULL, SW_RESTORE ))
	{
		common->Error( "Could not open url: '%s' ", url );
		return;
	}

	wnd = GetForegroundWindow();
	if (wnd)
	{
		ShowWindow( wnd, SW_MAXIMIZE );
	}

	if (doexit)
	{
		doexit_spamguard = true;
		cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "quit\n" );
	}
}

Win32Vars_t	win32;

#pragma optimize( "", on )


/*
================
Sys_Init

The cvar system must already be setup
================
*/
#define OSR2_BUILD_NUMBER 1111
#define WIN98_BUILD_NUMBER 1998

void Sys_Init( void )
{

	CoInitialize( NULL );

	// make sure the timer is high precision, otherwise
	// NT gets 18ms resolution
	timeBeginPeriod( 1 );

	// get WM_TIMER messages pumped every millisecond
//	SetTimer( NULL, 0, 100, NULL );

	cmdSystem->AddCommand( "in_restart", Sys_In_Restart_f, CMD_FL_SYSTEM, "restarts the input system" );
#ifdef DEBUG
	cmdSystem->AddCommand( "createResourceIDs", CreateResourceIDs_f, CMD_FL_TOOL, "assigns resource IDs in _resouce.h files" );
#endif
#if 0
	cmdSystem->AddCommand( "setAsyncSound", Sys_SetAsyncSound_f, CMD_FL_SYSTEM, "set the async sound option" );
#endif

	//
	// Windows user name
	//
	win32.win_username.SetString( Sys_GetCurrentUser() );

	//
	// Windows version
	//
	win32.osversion.dwOSVersionInfoSize = sizeof( win32.osversion );

	if (!GetVersionEx( (LPOSVERSIONINFO)&win32.osversion ))
		Sys_Error( "Couldn't get OS info" );

	if (win32.osversion.dwMajorVersion < 4)
	{
		Sys_Error( GAME_NAME " requires Windows version 4 (NT) or greater" );
	}
	if (win32.osversion.dwPlatformId == VER_PLATFORM_WIN32s)
	{
		Sys_Error( GAME_NAME " doesn't run on Win32s" );
	}

	if (win32.osversion.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if (win32.osversion.dwMajorVersion <= 4)
		{
			win32.sys_arch.SetString( "WinNT (NT)" );
		}
		else if (win32.osversion.dwMajorVersion == 5 && win32.osversion.dwMinorVersion == 0)
		{
			win32.sys_arch.SetString( "Win2K (NT)" );
		}
		else if (win32.osversion.dwMajorVersion == 5 && win32.osversion.dwMinorVersion == 1)
		{
			win32.sys_arch.SetString( "WinXP (NT)" );
		}
		else if (win32.osversion.dwMajorVersion == 6)
		{
			win32.sys_arch.SetString( "Vista" );
		}
		else
		{
			win32.sys_arch.SetString( "Unknown NT variant" );
		}
	}
	else if (win32.osversion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		if (win32.osversion.dwMajorVersion == 4 && win32.osversion.dwMinorVersion == 0)
		{
			// Win95
			if (win32.osversion.szCSDVersion[1] == 'C')
			{
				win32.sys_arch.SetString( "Win95 OSR2 (95)" );
			}
			else
			{
				win32.sys_arch.SetString( "Win95 (95)" );
			}
		}
		else if (win32.osversion.dwMajorVersion == 4 && win32.osversion.dwMinorVersion == 10)
		{
			// Win98
			if (win32.osversion.szCSDVersion[1] == 'A')
			{
				win32.sys_arch.SetString( "Win98SE (95)" );
			}
			else
			{
				win32.sys_arch.SetString( "Win98 (95)" );
			}
		}
		else if (win32.osversion.dwMajorVersion == 4 && win32.osversion.dwMinorVersion == 90)
		{
			// WinMe
			win32.sys_arch.SetString( "WinMe (95)" );
		}
		else
		{
			win32.sys_arch.SetString( "Unknown 95 variant" );
		}
	}
	else
	{
		win32.sys_arch.SetString( "unknown Windows variant" );
	}

	
}

/*
================
Sys_Shutdown
================
*/
void Sys_Shutdown( void )
{
	CoUninitialize();
}


#ifdef DEBUG


static unsigned int debug_total_alloc = 0;
static unsigned int debug_total_alloc_count = 0;
static unsigned int debug_current_alloc = 0;
static unsigned int debug_current_alloc_count = 0;
static unsigned int debug_frame_alloc = 0;
static unsigned int debug_frame_alloc_count = 0;

idCVar sys_showMallocs( "sys_showMallocs", "0", CVAR_SYSTEM, "" );

// _HOOK_ALLOC, _HOOK_REALLOC, _HOOK_FREE

typedef struct CrtMemBlockHeader
{
	struct _CrtMemBlockHeader *pBlockHeaderNext;	// Pointer to the block allocated just before this one:
	struct _CrtMemBlockHeader *pBlockHeaderPrev;	// Pointer to the block allocated just after this one
	char *szFileName;    // File name
	int nLine;           // Line number
	size_t nDataSize;    // Size of user block
	int nBlockUse;       // Type of block
	long lRequest;       // Allocation number
	byte		gap[4];								// Buffer just before (lower than) the user's memory:
} CrtMemBlockHeader;

#include <crtdbg.h>

/*
==================
Sys_AllocHook

	called for every malloc/new/free/delete
==================
*/
int Sys_AllocHook( int nAllocType, void *pvData, size_t nSize, int nBlockUse, long lRequest, const unsigned char * szFileName, int nLine )
{
	CrtMemBlockHeader	*pHead;
	byte				*temp;

	if (nBlockUse == _CRT_BLOCK)
	{
		return(TRUE);
	}

	// get a pointer to memory block header
	temp = (byte *)pvData;
	temp -= 32;
	pHead = (CrtMemBlockHeader *)temp;

	switch (nAllocType)
	{
	case	_HOOK_ALLOC:
		debug_total_alloc += nSize;
		debug_current_alloc += nSize;
		debug_frame_alloc += nSize;
		debug_total_alloc_count++;
		debug_current_alloc_count++;
		debug_frame_alloc_count++;
		break;

	case	_HOOK_FREE:
		assert( pHead->gap[0] == 0xfd && pHead->gap[1] == 0xfd && pHead->gap[2] == 0xfd && pHead->gap[3] == 0xfd );

		debug_current_alloc -= pHead->nDataSize;
		debug_current_alloc_count--;
		debug_total_alloc_count++;
		debug_frame_alloc_count++;
		break;

	case	_HOOK_REALLOC:
		assert( pHead->gap[0] == 0xfd && pHead->gap[1] == 0xfd && pHead->gap[2] == 0xfd && pHead->gap[3] == 0xfd );

		debug_current_alloc -= pHead->nDataSize;
		debug_total_alloc += nSize;
		debug_current_alloc += nSize;
		debug_frame_alloc += nSize;
		debug_total_alloc_count++;
		debug_current_alloc_count--;
		debug_frame_alloc_count++;
		break;
	}
	return(TRUE);
}

/*
==================
Sys_DebugMemory_f
==================
*/
void Sys_DebugMemory_f( void )
{
	common->Printf( "Total allocation %8dk in %d blocks\n", debug_total_alloc / 1024, debug_total_alloc_count );
	common->Printf( "Current allocation %8dk in %d blocks\n", debug_current_alloc / 1024, debug_current_alloc_count );
}

/*
==================
Sys_MemFrame
==================
*/
void Sys_MemFrame( void )
{
	if (sys_showMallocs.GetInteger())
	{
		common->Printf( "Frame: %8dk in %5d blocks\n", debug_frame_alloc / 1024, debug_frame_alloc_count );
	}

	debug_frame_alloc = 0;
	debug_frame_alloc_count = 0;
}

#endif

/*
==================
Sys_FlushCacheMemory

On windows, the vertex buffers are write combined, so they
don't need to be flushed from the cache
==================
*/
void Sys_FlushCacheMemory( void *base, int bytes )
{
}

/*
=================
Sys_In_Restart_f

Restart the input subsystem
=================
*/
void Sys_In_Restart_f( const idCmdArgs &args )
{
	Sys_ShutdownInput();
	Sys_InitInput();
}

/*
================
Sys_AlreadyRunning

returns true if there is a copy of D3 running already
================
*/
bool Sys_AlreadyRunning( void )
{
#ifndef DEBUG
	if (!win32.win_allowMultipleInstances.GetBool())
	{
		HANDLE hMutexOneInstance = ::CreateMutex( NULL, FALSE, "DOOM3" );
		if (::GetLastError() == ERROR_ALREADY_EXISTS || ::GetLastError() == ERROR_ACCESS_DENIED)
		{
			return true;
		}
	}
#endif
	return false;
}

/*
==================
idSysLocal::StartProcess
==================
*/
void idSysLocal::StartProcess( const char *exePath, bool doexit )
{
	TCHAR				szPathOrig[_MAX_PATH];
	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;

	ZeroMemory( &si, sizeof( si ) );
	si.cb = sizeof( si );

	strncpy( szPathOrig, exePath, _MAX_PATH );

	if (!CreateProcess( NULL, szPathOrig, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ))
	{
		common->Error( "Could not start process: '%s' ", szPathOrig );
		return;
	}

	if (doexit)
	{
		cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "quit\n" );
	}
}

/*
==================
Sys_SetFatalError
==================
*/
void Sys_SetFatalError( const char *error )
{
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
Sys_GetDriveFreeSpace
returns in megabytes
================
*/
int Sys_GetDriveFreeSpace( const char *path ) {
	DWORDLONG lpFreeBytesAvailable;
	DWORDLONG lpTotalNumberOfBytes;
	DWORDLONG lpTotalNumberOfFreeBytes;
	int ret = 26;
	//FIXME: see why this is failing on some machines
	if ( ::GetDiskFreeSpaceEx( path, (PULARGE_INTEGER)&lpFreeBytesAvailable, (PULARGE_INTEGER)&lpTotalNumberOfBytes, (PULARGE_INTEGER)&lpTotalNumberOfFreeBytes ) ) {
		ret = ( double )( lpFreeBytesAvailable ) / ( 1024.0 * 1024.0 );
	}
	return ret;
}


/*
================
Sys_GetVideoRam
returns in megabytes
================
*/
int Sys_GetVideoRam( void ) {
#if 1 //def	ID_DEDICATED
	return 0;
#else
	unsigned int retSize = 64;

	CComPtr<IWbemLocator> spLoc = NULL;
	HRESULT hr = CoCreateInstance( CLSID_WbemLocator, 0, CLSCTX_SERVER, IID_IWbemLocator, ( LPVOID * ) &spLoc );
	if ( hr != S_OK || spLoc == NULL ) {
		return retSize;
	}

	CComBSTR bstrNamespace( _T( "\\\\.\\root\\CIMV2" ) );
	CComPtr<IWbemServices> spServices;

	// Connect to CIM
	hr = spLoc->ConnectServer( bstrNamespace, NULL, NULL, 0, NULL, 0, 0, &spServices );
	if ( hr != WBEM_S_NO_ERROR ) {
		return retSize;
	}

	// Switch the security level to IMPERSONATE so that provider will grant access to system-level objects.  
	hr = CoSetProxyBlanket( spServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );
	if ( hr != S_OK ) {
		return retSize;
	}

	// Get the vid controller
	CComPtr<IEnumWbemClassObject> spEnumInst = NULL;
	hr = spServices->CreateInstanceEnum( CComBSTR( "Win32_VideoController" ), WBEM_FLAG_SHALLOW, NULL, &spEnumInst ); 
	if ( hr != WBEM_S_NO_ERROR || spEnumInst == NULL ) {
		return retSize;
	}

	ULONG uNumOfInstances = 0;
	CComPtr<IWbemClassObject> spInstance = NULL;
	hr = spEnumInst->Next( 10000, 1, &spInstance, &uNumOfInstances );

	if ( hr == S_OK && spInstance ) {
		// Get properties from the object
		CComVariant varSize;
		hr = spInstance->Get( CComBSTR( _T( "AdapterRAM" ) ), 0, &varSize, 0, 0 );
		if ( hr == S_OK ) {
			retSize = varSize.intVal / ( 1024 * 1024 );
			if ( retSize == 0 ) {
				retSize = 64;
			}
		}
	}
	return retSize;
#endif
}

/*
================
Sys_GetCurrentMemoryStatus

	returns OS mem info
	all values are in kB except the memoryload
================
*/
void Sys_GetCurrentMemoryStatus( sysMemoryStats_t &stats ) {
	MEMORYSTATUSEX statex;
	unsigned __int64 work;

	memset( &statex, sizeof( statex ), 0 );
	statex.dwLength = sizeof( statex );
	GlobalMemoryStatusEx( &statex );

	memset( &stats, 0, sizeof( stats ) );

	stats.memoryLoad = statex.dwMemoryLoad;

	work = statex.ullTotalPhys >> 20;
	stats.totalPhysical = *(int*)&work;

	work = statex.ullAvailPhys >> 20;
	stats.availPhysical = *(int*)&work;

	work = statex.ullAvailPageFile >> 20;
	stats.availPageFile = *(int*)&work;

	work = statex.ullTotalPageFile >> 20;
	stats.totalPageFile = *(int*)&work;

	work = statex.ullTotalVirtual >> 20;
	stats.totalVirtual = *(int*)&work;

	work = statex.ullAvailVirtual >> 20;
	stats.availVirtual = *(int*)&work;

	work = statex.ullAvailExtendedVirtual >> 20;
	stats.availExtendedVirtual = *(int*)&work;
}

/*
================
Sys_LockMemory
================
*/
bool Sys_LockMemory( void *ptr, int bytes ) {
	return ( VirtualLock( ptr, (SIZE_T)bytes ) != FALSE );
}

/*
================
Sys_UnlockMemory
================
*/
bool Sys_UnlockMemory( void *ptr, int bytes ) {
	return ( VirtualUnlock( ptr, (SIZE_T)bytes ) != FALSE );
}

/*
================
Sys_SetPhysicalWorkMemory
================
*/
void Sys_SetPhysicalWorkMemory( int minBytes, int maxBytes ) {
	::SetProcessWorkingSetSize( GetCurrentProcess(), minBytes, maxBytes );
}

/*
================
Sys_GetCurrentUser
================
*/
char *Sys_GetCurrentUser( void ) {
	static char s_userName[1024];
	unsigned long size = sizeof( s_userName );


	if ( !GetUserName( s_userName, &size ) ) {
		strcpy( s_userName, "player" );
	}

	if ( !s_userName[0] ) {
		strcpy( s_userName, "player" );
	}

	return s_userName;
}	


/*
===============================================================================

	Call stack

===============================================================================
*/


#define PROLOGUE_SIGNATURE 0x00EC8B55

#include <dbghelp.h>

const int UNDECORATE_FLAGS =	UNDNAME_NO_MS_KEYWORDS |
								UNDNAME_NO_ACCESS_SPECIFIERS |
								UNDNAME_NO_FUNCTION_RETURNS |
								UNDNAME_NO_ALLOCATION_MODEL |
								UNDNAME_NO_ALLOCATION_LANGUAGE |
								UNDNAME_NO_MEMBER_TYPE;

#if defined(_DEBUG) && 1

typedef struct symbol_s {
	int					address;
	char *				name;
	struct symbol_s *	next;
} symbol_t;

typedef struct module_s {
	int					address;
	char *				name;
	symbol_t *			symbols;
	struct module_s *	next;
} module_t;

module_t *modules;

/*
==================
SkipRestOfLine
==================
*/
void SkipRestOfLine( const char **ptr ) {
	while( (**ptr) != '\0' && (**ptr) != '\n' && (**ptr) != '\r' ) {
		(*ptr)++;
	}
	while( (**ptr) == '\n' || (**ptr) == '\r' ) {
		(*ptr)++;
	}
}

/*
==================
SkipWhiteSpace
==================
*/
void SkipWhiteSpace( const char **ptr ) {
	while( (**ptr) == ' ' ) {
		(*ptr)++;
	}
}

/*
==================
ParseHexNumber
==================
*/
int ParseHexNumber( const char **ptr ) {
	int n = 0;
	while( (**ptr) >= '0' && (**ptr) <= '9' || (**ptr) >= 'a' && (**ptr) <= 'f' ) {
		n <<= 4;
		if ( **ptr >= '0' && **ptr <= '9' ) {
			n |= ( (**ptr) - '0' );
		} else {
			n |= 10 + ( (**ptr) - 'a' );
		}
		(*ptr)++;
	}
	return n;
}

/*
==================
Sym_Init
==================
*/
void Sym_Init( long addr ) {
	TCHAR moduleName[MAX_STRING_CHARS];
	MEMORY_BASIC_INFORMATION mbi;

	VirtualQuery( (void*)addr, &mbi, sizeof(mbi) );

	GetModuleFileName( (HMODULE)mbi.AllocationBase, moduleName, sizeof( moduleName ) );

	char *ext = moduleName + strlen( moduleName );
	while( ext > moduleName && *ext != '.' ) {
		ext--;
	}
	if ( ext == moduleName ) {
		strcat( moduleName, ".map" );
	} else {
		strcpy( ext, ".map" );
	}

	module_t *module = (module_t *) malloc( sizeof( module_t ) );
	module->name = (char *) malloc( strlen( moduleName ) + 1 );
	strcpy( module->name, moduleName );
	module->address = (int)mbi.AllocationBase;
	module->symbols = NULL;
	module->next = modules;
	modules = module;

	FILE *fp = fopen( moduleName, "rb" );
	if ( fp == NULL ) {
		return;
	}

	int pos = ftell( fp );
	fseek( fp, 0, SEEK_END );
	int length = ftell( fp );
	fseek( fp, pos, SEEK_SET );

	char *text = (char *) malloc( length+1 );
	fread( text, 1, length, fp );
	text[length] = '\0';
	fclose( fp );

	const char *ptr = text;

	// skip up to " Address" on a new line
	while( *ptr != '\0' ) {
		SkipWhiteSpace( &ptr );
		if ( idStr::Cmpn( ptr, "Address", 7 ) == 0 ) {
			SkipRestOfLine( &ptr );
			break;
		}
		SkipRestOfLine( &ptr );
	}

	int symbolAddress;
	int symbolLength;
	char symbolName[MAX_STRING_CHARS];
	symbol_t *symbol;

	// parse symbols
	while( *ptr != '\0' ) {

		SkipWhiteSpace( &ptr );

		ParseHexNumber( &ptr );
		if ( *ptr == ':' ) {
			ptr++;
		} else {
			break;
		}
		ParseHexNumber( &ptr );

		SkipWhiteSpace( &ptr );

		// parse symbol name
		symbolLength = 0;
		while( *ptr != '\0' && *ptr != ' ' ) {
			symbolName[symbolLength++] = *ptr++;
			if ( symbolLength >= sizeof( symbolName ) - 1 ) {
				break;
			}
		}
		symbolName[symbolLength++] = '\0';

		SkipWhiteSpace( &ptr );

		// parse symbol address
		symbolAddress = ParseHexNumber( &ptr );

		SkipRestOfLine( &ptr );

		symbol = (symbol_t *) malloc( sizeof( symbol_t ) );
		symbol->name = (char *) malloc( symbolLength );
		strcpy( symbol->name, symbolName );
		symbol->address = symbolAddress;
		symbol->next = module->symbols;
		module->symbols = symbol;
	}

	free( text );
}

/*
==================
Sym_Shutdown
==================
*/
void Sym_Shutdown( void ) {
	module_t *m;
	symbol_t *s;

	for ( m = modules; m != NULL; m = modules ) {
		modules = m->next;
		for ( s = m->symbols; s != NULL; s = m->symbols ) {
			m->symbols = s->next;
			free( s->name );
			free( s );
		}
		free( m->name );
		free( m );
	}
	modules = NULL;
}

/*
==================
Sym_GetFuncInfo
==================
*/
void Sym_GetFuncInfo( long addr, idStr &module, idStr &funcName ) {
	MEMORY_BASIC_INFORMATION mbi;
	module_t *m;
	symbol_t *s;

	VirtualQuery( (void*)addr, &mbi, sizeof(mbi) );

	for ( m = modules; m != NULL; m = m->next ) {
		if ( m->address == (int) mbi.AllocationBase ) {
			break;
		}
	}
	if ( !m ) {
		Sym_Init( addr );
		m = modules;
	}

	for ( s = m->symbols; s != NULL; s = s->next ) {
		if ( s->address == addr ) {

			char undName[MAX_STRING_CHARS];
			if ( UnDecorateSymbolName( s->name, undName, sizeof(undName), UNDECORATE_FLAGS ) ) {
				funcName = undName;
			} else {
				funcName = s->name;
			}
			for ( int i = 0; i < funcName.Length(); i++ ) {
				if ( funcName[i] == '(' ) {
					funcName.CapLength( i );
					break;
				}
			}
			module = m->name;
			return;
		}
	}

	sprintf( funcName, "0x%08x", addr );
	module = "";
}

#elif defined(_DEBUG)

DWORD lastAllocationBase = -1;
HANDLE processHandle;
idStr lastModule;

/*
==================
Sym_Init
==================
*/
void Sym_Init( long addr ) {
	TCHAR moduleName[MAX_STRING_CHARS];
	TCHAR modShortNameBuf[MAX_STRING_CHARS];
	MEMORY_BASIC_INFORMATION mbi;

	if ( lastAllocationBase != -1 ) {
		Sym_Shutdown();
	}

	VirtualQuery( (void*)addr, &mbi, sizeof(mbi) );

	GetModuleFileName( (HMODULE)mbi.AllocationBase, moduleName, sizeof( moduleName ) );
	_splitpath( moduleName, NULL, NULL, modShortNameBuf, NULL );
	lastModule = modShortNameBuf;

	processHandle = GetCurrentProcess();
	if ( !SymInitialize( processHandle, NULL, FALSE ) ) {
		return;
	}
	if ( !SymLoadModule( processHandle, NULL, moduleName, NULL, (DWORD)mbi.AllocationBase, 0 ) ) {
		SymCleanup( processHandle );
		return;
	}

	SymSetOptions( SymGetOptions() & ~SYMOPT_UNDNAME );

	lastAllocationBase = (DWORD) mbi.AllocationBase;
}

/*
==================
Sym_Shutdown
==================
*/
void Sym_Shutdown( void ) {
	SymUnloadModule( GetCurrentProcess(), lastAllocationBase );
	SymCleanup( GetCurrentProcess() );
	lastAllocationBase = -1;
}

/*
==================
Sym_GetFuncInfo
==================
*/
void Sym_GetFuncInfo( long addr, idStr &module, idStr &funcName ) {
	MEMORY_BASIC_INFORMATION mbi;

	VirtualQuery( (void*)addr, &mbi, sizeof(mbi) );

	if ( (DWORD) mbi.AllocationBase != lastAllocationBase ) {
		Sym_Init( addr );
	}

	BYTE symbolBuffer[ sizeof(IMAGEHLP_SYMBOL) + MAX_STRING_CHARS ];
	PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)&symbolBuffer[0];
	pSymbol->SizeOfStruct = sizeof(symbolBuffer);
	pSymbol->MaxNameLength = 1023;
	pSymbol->Address = 0;
	pSymbol->Flags = 0;
	pSymbol->Size =0;

	DWORD symDisplacement = 0;
	if ( SymGetSymFromAddr( processHandle, addr, &symDisplacement, pSymbol ) ) {
		// clean up name, throwing away decorations that don't affect uniqueness
	    char undName[MAX_STRING_CHARS];
		if ( UnDecorateSymbolName( pSymbol->Name, undName, sizeof(undName), UNDECORATE_FLAGS ) ) {
			funcName = undName;
		} else {
			funcName = pSymbol->Name;
		}
		module = lastModule;
	}
	else {
		LPVOID lpMsgBuf;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						GetLastError(),
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR) &lpMsgBuf,
						0,
						NULL 
						);
		LocalFree( lpMsgBuf );

		// Couldn't retrieve symbol (no debug info?, can't load dbghelp.dll?)
		sprintf( funcName, "0x%08x", addr );
		module = "";
    }
}

#else

/*
==================
Sym_Init
==================
*/
void Sym_Init( long addr ) {
}

/*
==================
Sym_Shutdown
==================
*/
void Sym_Shutdown( void ) {
}

/*
==================
Sym_GetFuncInfo
==================
*/
void Sym_GetFuncInfo( long addr, idStr &module, idStr &funcName ) {
	module = "";
	sprintf( funcName, "0x%08x", addr );
}

#endif

/*
==================
GetFuncAddr
==================
*/
address_t GetFuncAddr( address_t midPtPtr ) {
	long temp;
	do {
		temp = (long)(*(long*)midPtPtr);
		if ( (temp&0x00FFFFFF) == PROLOGUE_SIGNATURE ) {
			break;
		}
		midPtPtr--;
	} while(true);

	return midPtPtr;
}

/*
==================
GetCallerAddr
==================
*/
address_t GetCallerAddr( long _ebp ) {
	long midPtPtr;
	long res = 0;

	__asm {
		mov		eax, _ebp
		mov		ecx, [eax]		// check for end of stack frames list
		test	ecx, ecx		// check for zero stack frame
		jz		label
		mov		eax, [eax+4]	// get the ret address
		test	eax, eax		// check for zero return address
		jz		label
		mov		midPtPtr, eax
	}
	res = GetFuncAddr( midPtPtr );
label:
	return res;
}

/*
==================
Sys_GetCallStack

 use /Oy option
==================
*/
void Sys_GetCallStack( address_t *callStack, const int callStackSize ) {
#if 1 //def _DEBUG
	int i;
	long m_ebp;

	__asm {
		mov eax, ebp
		mov m_ebp, eax
	}
	// skip last two functions
	m_ebp = *((long*)m_ebp);
	m_ebp = *((long*)m_ebp);
	// list functions
	for ( i = 0; i < callStackSize; i++ ) {
		callStack[i] = GetCallerAddr( m_ebp );
		if ( callStack[i] == 0 ) {
			break;
		}
		m_ebp = *((long*)m_ebp);
	}
#else
	int i = 0;
#endif
	while( i < callStackSize ) {
		callStack[i++] = 0;
	}
}

/*
==================
Sys_GetCallStackStr
==================
*/
const char *Sys_GetCallStackStr( const address_t *callStack, const int callStackSize ) {
	static char string[MAX_STRING_CHARS*2];
	int index, i;
	idStr module, funcName;

	index = 0;
	for ( i = callStackSize-1; i >= 0; i-- ) {
		Sym_GetFuncInfo( callStack[i], module, funcName );
		index += sprintf( string+index, " -> %s", funcName.c_str() );
	}
	return string;
}

/*
==================
Sys_GetCallStackCurStr
==================
*/
const char *Sys_GetCallStackCurStr( int depth ) {
	address_t *callStack;

	callStack = (address_t *) _alloca( depth * sizeof( address_t ) );
	Sys_GetCallStack( callStack, depth );
	return Sys_GetCallStackStr( callStack, depth );
}

/*
==================
Sys_GetCallStackCurAddressStr
==================
*/
const char *Sys_GetCallStackCurAddressStr( int depth ) {
	static char string[MAX_STRING_CHARS*2];
	address_t *callStack;
	int index, i;

	callStack = (address_t *) _alloca( depth * sizeof( address_t ) );
	Sys_GetCallStack( callStack, depth );

	index = 0;
	for ( i = depth-1; i >= 0; i-- ) {
		index += sprintf( string+index, " -> 0x%08x", callStack[i] );
	}
	return string;
}

/*
==================
Sys_ShutdownSymbols
==================
*/
void Sys_ShutdownSymbols( void ) {
	Sym_Shutdown();
}
