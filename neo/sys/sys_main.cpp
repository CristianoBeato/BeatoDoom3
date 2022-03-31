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

//
// Only include here, to don't leak to typeInfo executable the main entry point 
//#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "common/console.h"
#include "common/window.h"
#include "sys_main.h"
#include "sys_local.h"

idSysLocal			sysLocal;
idSys *				sys = &sysLocal;

#define TEST_FPU_EXCEPTIONS	/*	FPU_EXCEPTION_INVALID_OPERATION |		*/	\
							/*	FPU_EXCEPTION_DENORMALIZED_OPERAND |	*/	\
							/*	FPU_EXCEPTION_DIVIDE_BY_ZERO |			*/	\
							/*	FPU_EXCEPTION_NUMERIC_OVERFLOW |		*/	\
							/*	FPU_EXCEPTION_NUMERIC_UNDERFLOW |		*/	\
							/*	FPU_EXCEPTION_INEXACT_RESULT |			*/	\
								0

static void Sys_CreateWindow( void )
{
	SDL_Rect win = { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480 };
	assert( !sysVars.window );
	sysVars.window = new btWindow();

	// Create a small hiden window, and show and recise when the renderer is initialized
	sysVars.window->Create( GAME_NAME, win, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL );
}

static void Sys_DestroyWindow( void )
{
	if (sysVars.window)
		sysVars.window->Destroy();

	SAFE_DELETE( sysVars.window );
}

/*
==============
Sys_ShowWindow
==============
*/
void Sys_ShowWindow( bool show )
{
	assert( sysVars.window != nullptr );
	if (show)
		sysVars.window->Show();
	else
		sysVars.window->Hide();
}

/*
==============
Sys_IsWindowVisible
==============
*/
bool Sys_IsWindowVisible( void )
{
	assert( sysVars.window != nullptr );
	return sysVars.window->IsFlagActive( SDL_WINDOW_SHOWN );
}

/*
==============
Sys_Quit
==============
*/
void Sys_Quit( void )
{
	timeEndPeriod( 1 );
	Sys_ShutdownInput();
	Sys_DestroyConsole();
	Sys_ShutDownOpenGL();
#if _WIN32 || _WIN64
	ExitProcess( 0 );
#endif
}

/*
==============
MainInit
==============
*/
void		MainInit( int argc, char *argv[] )
{
	//init SDL
	SDL_assert( SDL_Init( SDL_INIT_EVERYTHING ) == 0 );

	//
	Sys_getCPUInfo();

	// 
	Sys_StartThreadPool();

	//
	Sys_CreateWindow();

	Sys_SetPhysicalWorkMemory( 192 << 20, 1024 << 20 );
	Sys_GetCurrentMemoryStatus( sysVars.exeLaunchMemoryStats );

	// done before Com/Sys_Init since we need this for error output
	Sys_CreateConsole();

#if _WIN32 && _MSVC_LANG
	// no abort/retry/fail errors
	SetErrorMode( SEM_FAILCRITICALERRORS );

#ifdef _DEBUG 
	// disable the painfully slow MS heap check every 1024 allocs
	_CrtSetDbgFlag( 0 );
#endif
#endif //_WIN32

	Sys_FPU_SetPrecision( FPU_PRECISION_DOUBLE_EXTENDED );

	if (argc > 1)
		common->Init( argc - 1, &argv[1], NULL );
	else
		common->Init( 0, NULL, NULL );

	// Initialize OpenGL config
	Sys_InitOpenGL();

#if TEST_FPU_EXCEPTIONS != 0
	common->Printf( Sys_FPU_GetState() );
#endif

	// hide or show the early console as necessary
	if (sysVars.sys_viewlog.GetInteger() || com_skipRenderer.GetBool() || idAsyncNetwork::serverDedicated.GetInteger())
		Sys_ShowConsole( 1, true );
	else
		Sys_ShowConsole( 0, false );

	// Launch the script debugger
	//if (strstr( lpCmdLine, "+debugger" ))
	//{
		// DebuggerClientInit( lpCmdLine );
	//	return 0;
	//}

#ifndef	ID_DEDICATED
	// Initialization done 
	sysVars.window->SetFocus();
#endif // ID_DEDICATED
}

static void	Sys_toolsFrame( void )
{
	if (com_editors)
	{
		if (com_editors & EDITOR_GUI)
		{
			// GUI editor
			GUIEditorRun();
		}
		else if (com_editors & EDITOR_RADIANT)
		{
			// Level Editor
			RadiantRun();
		}
		else if (com_editors & EDITOR_MATERIAL)
		{
			//BSM Nerve: Add support for the material editor
			MaterialEditorRun();
		}
		else
		{
			if (com_editors & EDITOR_LIGHT)
			{
				// in-game Light Editor
				LightEditorRun();
			}
			if (com_editors & EDITOR_SOUND)
			{
				// in-game Sound Editor
				SoundEditorRun();
			}
			if (com_editors & EDITOR_DECL)
			{
				// in-game Declaration Browser
				DeclBrowserRun();
			}
			if (com_editors & EDITOR_AF)
			{
				// in-game Articulated Figure Editor
				AFEditorRun();
			}
			if (com_editors & EDITOR_PARTICLE)
			{
				// in-game Particle Editor
				ParticleEditorRun();
			}
			if (com_editors & EDITOR_SCRIPT)
			{
				// in-game Script Editor
				ScriptEditorRun();
			}
			if (com_editors & EDITOR_PDA)
			{
				// in-game PDA Editor
				PDAEditorRun();
			}
		}
	}
}

/*
==============
MainLoop
==============
*/
void		MainLoop( void )
{
	// if "viewlog" has been modified, show or hide the log console
	if (sysVars.sys_viewlog.IsModified())
	{
		if (!com_skipRenderer.GetBool() && idAsyncNetwork::serverDedicated.GetInteger() != 1)
		{
			Sys_ShowConsole( sysVars.sys_viewlog.GetInteger(), false );
		}
		sysVars.sys_viewlog.ClearModified();

#if 0 //def _DEBUG
		Sys_MemFrame();
#endif

		// set exceptions, even if some crappy syscall changes them!
		Sys_FPU_EnableExceptions( TEST_FPU_EXCEPTIONS );

#ifdef ID_ALLOW_TOOLS
		Sys_toolsFrame();
#endif
		// run the game
		common->Frame();
	}
}

int main( int argc, char *argv[] )
{
	MainInit( argc, argv );

	while (true)
	{
		MainLoop();
	}

	return 0;
};