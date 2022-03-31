/*
===========================================================================

Beato Tech 4 Source Code
Copyright (C) 2017-2021 Cristiano Beato

This file is part of the Beato Tech 4 Source Code ("Beato Tech 4 Source Code").

Beato Tech 4 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Beato Tech 4 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Beato Tech 4 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Beato Tech 4 Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following the terms and conditions of the
GNU General Public License which accompanied the Beato Tech 4 Source Code.

===========================================================================
*/

#include "idlib/precompiled.h"
#pragma hdrstop

#include "idlib/toolsGui/GuiCommon.h"
#include "sys/sys_main.h"
#include "window.h"
#include "console.h"

#if 0 //_WIN32
#include "sys/win32/win_local.h"

#define COPY_ID			1
#define QUIT_ID			2
#define CLEAR_ID		3

#define ERRORBOX_ID		10
#define ERRORTEXT_ID	11

#define EDIT_ID			100
#define INPUT_ID		101

static LONG WINAPI ConWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	char *cmdString;
	static bool s_timePolarity;

	switch (uMsg)
	{
	case WM_ACTIVATE:
		if (LOWORD( wParam ) != WA_INACTIVE)
		{
			SetFocus( s_wcd.hwndInputLine );
		}
		break;
	case WM_CLOSE:
		if (cvarSystem->IsInitialized() && com_skipRenderer.GetBool())
		{
			cmdString = Mem_CopyString( "quit" );
			Sys_QueEvent( 0, SE_CONSOLE, 0, 0, strlen( cmdString ) + 1, cmdString );
		}
		else if (s_wcd.quitOnClose)
		{
			PostQuitMessage( 0 );
		}
		else
		{
			Sys_ShowConsole( 0, false );
			win32.win_viewlog.SetBool( false );
		}
		return 0;
	case WM_CTLCOLORSTATIC:
		if ((HWND)lParam == s_wcd.hwndBuffer)
		{
			SetBkColor( (HDC)wParam, RGB( 0x00, 0x00, 0x80 ) );
			SetTextColor( (HDC)wParam, RGB( 0xff, 0xff, 0x00 ) );
			return (long)s_wcd.hbrEditBackground;
		}
		else if ((HWND)lParam == s_wcd.hwndErrorBox)
		{
			if (s_timePolarity & 1)
			{
				SetBkColor( (HDC)wParam, RGB( 0x80, 0x80, 0x80 ) );
				SetTextColor( (HDC)wParam, RGB( 0xff, 0x0, 0x00 ) );
			}
			else
			{
				SetBkColor( (HDC)wParam, RGB( 0x80, 0x80, 0x80 ) );
				SetTextColor( (HDC)wParam, RGB( 0x00, 0x0, 0x00 ) );
			}
			return (long)s_wcd.hbrErrorBackground;
		}
		break;
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE)
		{
			PostQuitMessage( 0 );
		}
		break;
	case WM_COMMAND:
		if (wParam == COPY_ID)
		{
			SendMessage( s_wcd.hwndBuffer, EM_SETSEL, 0, -1 );
			SendMessage( s_wcd.hwndBuffer, WM_COPY, 0, 0 );
		}
		else if (wParam == QUIT_ID)
		{
			if (s_wcd.quitOnClose)
			{
				PostQuitMessage( 0 );
			}
			else
			{
				cmdString = Mem_CopyString( "quit" );
				Sys_QueEvent( 0, SE_CONSOLE, 0, 0, strlen( cmdString ) + 1, cmdString );
			}
		}
		else if (wParam == CLEAR_ID)
		{
			SendMessage( s_wcd.hwndBuffer, EM_SETSEL, 0, -1 );
			SendMessage( s_wcd.hwndBuffer, EM_REPLACESEL, FALSE, (LPARAM) "" );
			UpdateWindow( s_wcd.hwndBuffer );
		}
		break;
	case WM_CREATE:
		s_wcd.hbrEditBackground = CreateSolidBrush( RGB( 0x00, 0x00, 0x80 ) );
		s_wcd.hbrErrorBackground = CreateSolidBrush( RGB( 0x80, 0x80, 0x80 ) );
		SetTimer( hWnd, 1, 1000, NULL );
		break;
		/*
				case WM_ERASEBKGND:
					HGDIOBJ oldObject;
					HDC hdcScaled;
					hdcScaled = CreateCompatibleDC( ( HDC ) wParam );
					assert( hdcScaled != 0 );
					if ( hdcScaled ) {
						oldObject = SelectObject( ( HDC ) hdcScaled, s_wcd.hbmLogo );
						assert( oldObject != 0 );
						if ( oldObject )
						{
							StretchBlt( ( HDC ) wParam, 0, 0, s_wcd.windowWidth, s_wcd.windowHeight,
								hdcScaled, 0, 0, 512, 384,
								SRCCOPY );
						}
						DeleteDC( hdcScaled );
						hdcScaled = 0;
					}
					return 1;
		*/
	case WM_TIMER:
		if (wParam == 1)
		{
			s_timePolarity = (bool)!s_timePolarity;
			if (s_wcd.hwndErrorBox)
			{
				InvalidateRect( s_wcd.hwndErrorBox, NULL, FALSE );
			}
		}
		break;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

LONG WINAPI InputLineWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	int key, cursor;
	switch (uMsg)
	{
	case WM_KILLFOCUS:
		if ((HWND)wParam == s_wcd.hWnd || (HWND)wParam == s_wcd.hwndErrorBox)
		{
			SetFocus( hWnd );
			return 0;
		}
		break;

	case WM_KEYDOWN:
		key = MapKey( lParam );

		// command history
		if ((key == K_UPARROW) || (key == K_KP_UPARROW))
		{
			if (s_wcd.nextHistoryLine - s_wcd.historyLine < COMMAND_HISTORY && s_wcd.historyLine > 0)
			{
				s_wcd.historyLine--;
			}
			s_wcd.consoleField = s_wcd.historyEditLines[s_wcd.historyLine % COMMAND_HISTORY];

			SetWindowText( s_wcd.hwndInputLine, s_wcd.consoleField.GetBuffer() );
			SendMessage( s_wcd.hwndInputLine, EM_SETSEL, s_wcd.consoleField.GetCursor(), s_wcd.consoleField.GetCursor() );
			return 0;
		}

		if ((key == K_DOWNARROW) || (key == K_KP_DOWNARROW))
		{
			if (s_wcd.historyLine == s_wcd.nextHistoryLine)
			{
				return 0;
			}
			s_wcd.historyLine++;
			s_wcd.consoleField = s_wcd.historyEditLines[s_wcd.historyLine % COMMAND_HISTORY];

			SetWindowText( s_wcd.hwndInputLine, s_wcd.consoleField.GetBuffer() );
			SendMessage( s_wcd.hwndInputLine, EM_SETSEL, s_wcd.consoleField.GetCursor(), s_wcd.consoleField.GetCursor() );
			return 0;
		}
		break;

	case WM_CHAR:
		key = MapKey( lParam );

		GetWindowText( s_wcd.hwndInputLine, s_wcd.consoleField.GetBuffer(), MAX_EDIT_LINE );
		SendMessage( s_wcd.hwndInputLine, EM_GETSEL, (WPARAM)NULL, (LPARAM)&cursor );
		s_wcd.consoleField.SetCursor( cursor );

		// enter the line
		if (key == K_ENTER || key == K_KP_ENTER)
		{
			strncat( s_wcd.consoleText, s_wcd.consoleField.GetBuffer(), sizeof( s_wcd.consoleText ) - strlen( s_wcd.consoleText ) - 5 );
			strcat( s_wcd.consoleText, "\n" );
			SetWindowText( s_wcd.hwndInputLine, "" );

			Sys_Printf( "]%s\n", s_wcd.consoleField.GetBuffer() );

			// copy line to history buffer
			s_wcd.historyEditLines[s_wcd.nextHistoryLine % COMMAND_HISTORY] = s_wcd.consoleField;
			s_wcd.nextHistoryLine++;
			s_wcd.historyLine = s_wcd.nextHistoryLine;

			s_wcd.consoleField.Clear();

			return 0;
		}

		// command completion
		if (key == K_TAB)
		{
			s_wcd.consoleField.AutoComplete();

			SetWindowText( s_wcd.hwndInputLine, s_wcd.consoleField.GetBuffer() );
			//s_wcd.consoleField.SetWidthInChars( strlen( s_wcd.consoleField.GetBuffer() ) );
			SendMessage( s_wcd.hwndInputLine, EM_SETSEL, s_wcd.consoleField.GetCursor(), s_wcd.consoleField.GetCursor() );

			return 0;
		}

		// clear autocompletion buffer on normal key input
		if ((key >= K_SPACE && key <= K_BACKSPACE) ||
			(key >= K_KP_SLASH && key <= K_KP_PLUS) || (key >= K_KP_STAR && key <= K_KP_EQUALS))
		{
			s_wcd.consoleField.ClearAutoComplete();
		}
		break;
	}

	return CallWindowProc( s_wcd.SysInputLineWndProc, hWnd, uMsg, wParam, lParam );
}

#endif

btConsoleLog::btConsoleLog( void ) :
	m_consoleWindow( nullptr ),
	m_consoleRenderer( nullptr )
{
}

btConsoleLog::~btConsoleLog( void )
{
}

void btConsoleLog::CreateConsole( void )
{
	const char *DEDCLASS = WIN32_CONSOLE_CLASS;
	SDL_Rect screen = {0 ,0, 540, 450};

	int DEDSTYLE = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_POPUP_MENU;

	m_consoleWindow = new btWindow();
	if(!m_consoleWindow->Create( DEDCLASS, screen, DEDSTYLE ))
	{
		SAFE_DELETE( m_consoleWindow );
		return;
	}

	m_consoleRenderer = SDL_GL_CreateContext( m_consoleWindow->GetHandler() );
	if( !m_consoleRenderer)
	{
		SAFE_DELETE( m_consoleWindow );
		return;
	}

	//s_wcd.hbmLogo = LoadBitmap( win32.hInstance, MAKEINTRESOURCE( IDB_BITMAP_LOGO) );


	//
	// create fonts
	//
	
	//
	// create the input line
	//
//	s_wcd.hwndInputLine = CreateWindow( "edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
//		ES_LEFT | ES_AUTOHSCROLL,
//		6, 400, 528, 20,
//		s_wcd.hWnd,
//		(HMENU)INPUT_ID,	// child window ID
//		win32.hInstance, NULL );

	//
	// create the buttons
	//
//	s_wcd.hwndButtonCopy = CreateWindow( "button", NULL, BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
//		5, 425, 72, 24,
//		s_wcd.hWnd,
//		(HMENU)COPY_ID,	// child window ID
//		win32.hInstance, NULL );
//	SendMessage( s_wcd.hwndButtonCopy, WM_SETTEXT, 0, (LPARAM) "copy" );

//	s_wcd.hwndButtonClear = CreateWindow( "button", NULL, BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
//		82, 425, 72, 24,
//		s_wcd.hWnd,
//		(HMENU)CLEAR_ID,	// child window ID
//		win32.hInstance, NULL );
//	SendMessage( s_wcd.hwndButtonClear, WM_SETTEXT, 0, (LPARAM) "clear" );

//	s_wcd.hwndButtonQuit = CreateWindow( "button", NULL, BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
//		462, 425, 72, 24,
//		s_wcd.hWnd,
//		(HMENU)QUIT_ID,	// child window ID
//		win32.hInstance, NULL );
//	SendMessage( s_wcd.hwndButtonQuit, WM_SETTEXT, 0, (LPARAM) "quit" );


	//
	// create the scrollbuffer
	//
//	s_wcd.hwndBuffer = CreateWindow( "edit", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
//		ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
//		6, 40, 526, 354,
//		s_wcd.hWnd,
//		(HMENU)EDIT_ID,	// child window ID
//		win32.hInstance, NULL );
//	SendMessage( s_wcd.hwndBuffer, WM_SETFONT, (WPARAM)s_wcd.hfBufferFont, 0 );

//	s_wcd.SysInputLineWndProc = (WNDPROC)SetWindowLong( s_wcd.hwndInputLine, GWL_WNDPROC, (long)InputLineWndProc );
//	SendMessage( s_wcd.hwndInputLine, WM_SETFONT, (WPARAM)s_wcd.hfBufferFont, 0 );

	// don't show it now that we have a splash screen up
	if (sysVars.sys_viewlog.GetBool())
	{
		m_consoleWindow->Show();
		m_consoleWindow->SetFocus();
	}

	consoleField.Clear();

	for ( int i = 0; i < COMMAND_HISTORY; i++)
	{
		historyEditLines[i].Clear();
	}
}

void btConsoleLog::DestroyConsole( void )
{
	if (m_consoleRenderer)
	{
		SDL_GL_DeleteContext( m_consoleRenderer );
		m_consoleRenderer = nullptr;
	}

	if ( m_consoleWindow )
	{
		m_consoleWindow->Destroy();
		SAFE_DELETE( m_consoleWindow );
	}
}

void btConsoleLog::ShowConsole( int visLevel, bool quitOnClose )
{
	quitOnClose = quitOnClose;
	
	if ((!m_consoleWindow) || (!m_consoleRenderer))
		return;

	switch (visLevel)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	default:
		Sys_Error( "Invalid visLevel %d sent to Sys_ShowConsole\n", visLevel );
		break;
	}
}

char * btConsoleLog::ConsoleInput( void )
{
	if (consoleText[0] == 0)
		return NULL;

	strcpy( returnedText, consoleText );
	consoleText[0] = 0;

	return returnedText;
}

void btConsoleLog::AppendText( const char * pMsg )
{
	char buffer[CONSOLE_BUFFER_SIZE * 2];
	char *b = buffer;
	const char *msg;
	int bufLen;
	int i = 0;
	static unsigned long s_totalChars;

	//
	// if the message is REALLY long, use just the last portion of it
	//
	if (strlen( pMsg ) > CONSOLE_BUFFER_SIZE - 1)
		msg = pMsg + strlen( pMsg ) - CONSOLE_BUFFER_SIZE + 1;
	else
		msg = pMsg;

	//
	// copy into an intermediate buffer
	//
	while (msg[i] && ((b - buffer) < sizeof( buffer ) - 1))
	{
		if (msg[i] == '\n' && msg[i + 1] == '\r')
		{
			b[0] = '\r';
			b[1] = '\n';
			b += 2;
			i++;
		}
		else if (msg[i] == '\r')
		{
			b[0] = '\r';
			b[1] = '\n';
			b += 2;
		}
		else if (msg[i] == '\n')
		{
			b[0] = '\r';
			b[1] = '\n';
			b += 2;
		}
		else if (idStr::IsColor( &msg[i] ))
		{
			i++;
		}
		else
		{
			*b = msg[i];
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
//	if (s_totalChars > 0x7000)
//	{
//		SendMessage( s_wcd.hwndBuffer, EM_SETSEL, 0, -1 );
//		s_totalChars = bufLen;
//	}

	//
	// put this text into the windows console
	//
//	SendMessage( s_wcd.hwndBuffer, EM_LINESCROLL, 0, 0xffff );
//	SendMessage( s_wcd.hwndBuffer, EM_SCROLLCARET, 0, 0 );
//	SendMessage( s_wcd.hwndBuffer, EM_REPLACESEL, 0, (LPARAM)buffer );
}

void btConsoleLog::SetErrorText( const char * buf )
{
	idStr::Copynz( errorString, buf, sizeof( errorString ) );
	/*
	if (!hwndErrorBox)
	{
		s_wcd.hwndErrorBox = CreateWindow( "static", NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN,
			6, 5, 526, 30,
			s_wcd.hWnd,
			(HMENU)ERRORBOX_ID,	// child window ID
			win32.hInstance, NULL );
		SendMessage( s_wcd.hwndErrorBox, WM_SETFONT, (WPARAM)s_wcd.hfBufferFont, 0 );
		SetWindowText( s_wcd.hwndErrorBox, s_wcd.errorString );

		DestroyWindow( s_wcd.hwndInputLine );
		s_wcd.hwndInputLine = NULL;
	}
	*/
}