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

#include <unistd.h>
#include <fcntl.h>

#if defined( ID_OS_LINUX )
#include <termios.h>
#endif // ID_OS_LINUX

#include "sys_main.h"

#define MAXPRINTMSG 4096
#define COMMAND_HISTORY 64

static struct
{
#if defined( _WIN32 )
	HWND		hWnd;
	HWND		hwndBuffer;
	HWND		hwndButtonClear;
	HWND		hwndButtonCopy;
	HWND		hwndButtonQuit;
	HWND		hwndErrorBox;
	HWND		hwndErrorText;
	HBITMAP		hbmLogo;
	HBITMAP		hbmClearBitmap;
	HBRUSH		hbrEditBackground;
	HBRUSH		hbrErrorBackground;
	HFONT		hfBufferFont;
	HFONT		hfButtonFont;
	HWND		hwndInputLine;
	WNDPROC		SysInputLineWndProc;

	bool		quitOnClose;
	int			windowWidth;
	int			windowHeight;
	int			nextHistoryLine;// the last line in the history buffer, not masked
	int			historyLine;	// the line being displayed from history buffer
								// will be <= nextHistoryLine
	char		errorString[80];
	char		consoleText[512]; 
	char		returnedText[512];
	idEditField	historyEditLines[COMMAND_HISTORY];
	idEditField	consoleField;
#elif defined(__linux__)
	bool			ttyEnabled;
	int				inputHide;
	int				historyCount;					// buffer fill up
	int				historyStart;					// current history start
	int				historyCurrent;				// goes back in history
	char			inputRet[256];
	idStr			history[ COMMAND_HISTORY ];	// cycle buffer
	idEditField		inputField;
	idEditField		historyBackup;				// the base edit line
	struct termios*	ttyTC;
#endif
}consoleInfo;


idCVar sys_viewlog( "sys_viewlog", "0", CVAR_SYSTEM | CVAR_INTEGER, "" );
idCVar sys_outputEditString( "sys_outputEditString", "1", CVAR_SYSTEM | CVAR_BOOL, "" );

#if defined( _WIN32 )

static void	ShowConsole( int visLevel, bool quitOnClose );
static void	SetErrorText( const char *buf );
static void	AppendText( const char *pMsg );

#elif defined( __linux__)

static void	ttyHide( void );
static void	ttyShow( void );
static void	ttyDel( void );
static void	ttyLeft( void );
static void	ttyRight( void );
static void	ttyFlushIn( void );

#endif

void	Sys_CreateConsole( void )
{
}

void Sys_DestroyConsole( void )
{
}

void Sys_ShowConsole( int visLevel, bool quitOnClose )
{
#if defined( _WIN32 )
	consoleInfo.quitOnClose = quitOnClose;

	if ( !consoleInfo.hWnd ) 
		return;
	
	switch ( visLevel ) 
	{
		case 0:
			ShowWindow( consoleInfo.hWnd, SW_HIDE );
		break;
		case 1:
			ShowWindow( consoleInfo.hWnd, SW_SHOWNORMAL );
			SendMessage( consoleInfo.hwndBuffer, EM_LINESCROLL, 0, 0xffff );
		break;
		case 2:
			ShowWindow( consoleInfo.hWnd, SW_MINIMIZE );
		break;
		default:
			Sys_Error( "Invalid visLevel %d sent to Sys_ShowConsole\n", visLevel );
		break;
	}
#else
#endif
}

const char*	Sys_ConsoleInput( void )
{
	return nullptr;
}

/*
=============
Sys_Error

Show the early console as an error dialog
=============
*/
void Sys_Error(const char *error, ...) 
{
	va_list argptr;

	Sys_Printf( "Sys_Error: " );
	va_start( argptr, error );
	Sys_DebugVPrintf( error, argptr );
	va_end( argptr );
	Sys_Printf( "\n" );

	Sys_Quit( EXIT_FAILURE );
}

/*
==============
Sys_Printf
==============
*/
void Sys_Printf( const char *fmt, ... )
{
	va_list 	argptr;

#if defined( _WIN32 )

	char		msg[MAXPRINTMSG];
	va_start(argptr, fmt);
	idStr::vsnPrintf( msg, MAXPRINTMSG-1, fmt, argptr );
	va_end(argptr);
	msg[sizeof(msg)-1] = '\0';

#if NDEBUG // aways output on debug
	if ( win_outputDebugString.GetBool() ) 
		OutputDebugString( msg );
#else
	OutputDebugStr( msg );
#endif

	if ( win_outputEditString.GetBool() ) 
		consoleLocal.AppendText( msg );

#elif defined(__linux__)
	
	ttyHide();
	va_start( argptr, fmt );
	vprintf( fmt, argptr );
	va_end( argptr );
	ttyShow();

#endif //__linux__
}

/*
==============
Sys_DebugPrintf
==============
*/
void Sys_DebugPrintf( const char *fmt, ... )
{
	va_list argptr;	
	va_start( argptr, fmt );
	Sys_DebugVPrintf( fmt, argptr );
	va_end( argptr );

}

/*
==============
Sys_DebugVPrintf
==============
*/
void Sys_DebugVPrintf( const char *fmt, va_list arg ) 
{
#if defined( _WIN32 )
	char msg[2048];
	idStr::vsnPrintf( msg, 2048-1, fmt, arg );
	msg[ sizeof(msg)-1 ] = '\0';
	OutputDebugString( msg );
#elif defined( __linux__ )
	ttyHide();
	vprintf( fmt, arg );
	ttyShow();
#endif
}

/*
==================
Sys_SetFatalError
==================
*/
void Sys_SetFatalError( const char *error )
{
	// TODO: flush to a log file
}

#if defined( _WIN32 )

void	ShowConsole( int visLevel, bool quitOnClose )
{
}

void	SetErrorText( const char *buf )
{
	m_quitOnClose = quitOnClose;

	if ( !hWnd ) 
		return;
	
	switch ( visLevel ) 
	{
		case 0:
			ShowWindow( hWnd, SW_HIDE );
		break;
		case 1:
			ShowWindow( hWnd, SW_SHOWNORMAL );
			SendMessage( hwndBuffer, EM_LINESCROLL, 0, 0xffff );
		break;
		case 2:
			ShowWindow( hWnd, SW_MINIMIZE );
		break;
		default:
			Sys_Error( "Invalid visLevel %d sent to Sys_ShowConsole\n", visLevel );
		break;
	}
}

void	AppendText( const char *pMsg )
{
	#define CONSOLE_BUFFER_SIZE		16384

	char buffer[CONSOLE_BUFFER_SIZE*2];
	char *b = buffer;
	const char *msg;
	int bufLen;
	int i = 0;
	static unsigned long s_totalChars;

	//
	// if the message is REALLY long, use just the last portion of it
	//
	if ( strlen( pMsg ) > CONSOLE_BUFFER_SIZE - 1 )	
		msg = pMsg + strlen( pMsg ) - CONSOLE_BUFFER_SIZE + 1;
	else 
		msg = pMsg;

	//
	// copy into an intermediate buffer
	//
	while ( msg[i] && ( ( b - buffer ) < sizeof( buffer ) - 1 ) ) {
		if ( msg[i] == '\n' && msg[i+1] == '\r' ) {
			b[0] = '\r';
			b[1] = '\n';
			b += 2;
			i++;
		} else if ( msg[i] == '\r' ) {
			b[0] = '\r';
			b[1] = '\n';
			b += 2;
		} else if ( msg[i] == '\n' ) {
			b[0] = '\r';
			b[1] = '\n';
			b += 2;
		} else if ( idStr::IsColor( &msg[i] ) ) {
			i++;
		} else {
			*b= msg[i];
			b++;
		}
		i++;
	}
	*b = 0;
	bufLen = b - buffer;

	s_totalChars += bufLen;

	//
	// replace selection instead of appending if we're overflowing
	//
	if ( s_totalChars > 0x7000 ) 
	{
		SendMessage( hwndBuffer, EM_SETSEL, 0, -1 );
		s_totalChars = bufLen;
	}

	//
	// put this text into the windows console
	//
	SendMessage( hwndBuffer, EM_LINESCROLL, 0, 0xffff );
	SendMessage( hwndBuffer, EM_SCROLLCARET, 0, 0 );
	SendMessage( hwndBuffer, EM_REPLACESEL, 0, (LPARAM) buffer );
}

#endif // _WIN32

#if defined( __linux__ )

// clear the display of the line currently edited
// bring cursor back to beginning of line
void	ttyHide( void )
{
	int len, buf_len;
	if ( !consoleInfo.ttyEnabled ) 
		return;
	
	if ( consoleInfo.inputHide ) 
    {
		consoleInfo.inputHide++;
		return;
	}
	
    // clear after cursor
	len = strlen( consoleInfo.inputField.GetBuffer() ) - consoleInfo.inputField.GetCursor();
	while ( len > 0 ) 
    {
		ttyRight();
		len--;
	}

	buf_len = strlen( consoleInfo.inputField.GetBuffer() );
	while ( buf_len > 0 ) 
    {
		ttyDel();
		buf_len--;
	}

	consoleInfo.inputHide++;
}

void	ttyShow( void )
{
	//	int i;
	if ( !consoleInfo.ttyEnabled ) 
		return;
	
    assert( consoleInfo.inputHide > 0 );
	consoleInfo.inputHide--;
	if ( consoleInfo.inputHide == 0 ) 
    {
		char *buf = consoleInfo.inputField.GetBuffer();
		if ( buf[0] ) 
        {
			write( STDOUT_FILENO, buf, strlen( buf ) );
			int back = strlen( buf ) - consoleInfo.inputField.GetCursor();
			while ( back > 0 ) 
            {
				ttyLeft();
				back--;
			}
		}
	}
}

void	ttyDel( void )
{
	char key;
	key = '\b';
	write( STDOUT_FILENO, &key, 1 );
	key = ' ';
	write( STDOUT_FILENO, &key, 1 );
	key = '\b';
	write( STDOUT_FILENO, &key, 1 );
}

void	ttyLeft( void )
{
	char key = '\b';
	write( STDOUT_FILENO, &key, 1 );
}

void	ttyRight( void )
{
	char key = 27;
	write( STDOUT_FILENO, &key, 1 );
	write( STDOUT_FILENO, "[C", 2 );
}

void	ttyFlushIn( void )
{
	char key;
    while ( read(0, &key, 1) != -1 ) 
    {
        Sys_Printf( "'%d' ", key );
    }
    
    Sys_Printf( "\n" );
}

#endif //__linux__