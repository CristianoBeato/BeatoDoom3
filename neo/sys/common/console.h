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

#ifndef __SYS_CONSOLE_H__
#define __SYS_CONSOLE_H__

#define	COMMAND_HISTORY	64
#define CONSOLE_BUFFER_SIZE		16384

class btCanvas;
class btConsoleLog
{
public:
	btConsoleLog();
	~btConsoleLog();

	void	CreateConsole( void );
	void	DestroyConsole( void );
	void	ShowConsole( int visLevel, bool quitOnClose );
	char*	ConsoleInput( void );
	void	AppendText( const char *pMsg );
	void	SetErrorText( const char *buf );

private:

	char		errorString[80];

	char		consoleText[512], returnedText[512];
	bool		quitOnClose;
	int			windowWidth, windowHeight;


	idEditField	historyEditLines[COMMAND_HISTORY];

	int			nextHistoryLine;// the last line in the history buffer, not masked
	int			historyLine;	// the line being displayed from history buffer
								// will be <= nextHistoryLine
	idEditField	consoleField;

	btWindow*		m_consoleWindow;
	SDL_GLContext	m_consoleRenderer;
	btCanvas*		m_consoleCanvas;
};

#endif // !__SYS_CONSOLE_H__
