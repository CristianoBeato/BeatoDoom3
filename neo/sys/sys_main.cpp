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
#include <SDL2/SDL.h>

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

/*
==============
MainInit
==============
*/
void		MainInit( int argc, const char **argv )
{
	//init SDL
	SDL_assert( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) == 0 );

	// BEATO Move here, to prevent: "munmap_chunk(): invalid pointer"
	Sys_videoStartUp();

	//
	Sys_getCPUInfo();

	//
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

#if TEST_FPU_EXCEPTIONS
	Sys_FPU_SetPrecision( FPU_PRECISION_DOUBLE_EXTENDED );
#endif //TEST_FPU_EXCEPTIONS

	if (argc > 1)
		common->Init( argc - 1, argv, nullptr );
	else
		common->Init( 0, nullptr, nullptr );

#if TEST_FPU_EXCEPTIONS != 0
	common->Printf( Sys_FPU_GetState() );
#endif //TEST_FPU_EXCEPTIONS

	// hide or show the early console as necessary
//	if ( sys_viewlog.GetInteger() || com_skipRenderer.GetBool() || idAsyncNetwork::serverDedicated.GetInteger())
//		Sys_ShowConsole( 1, true );
//	else
//		Sys_ShowConsole( 0, false );
//
	// Launch the script debugger
	//if (strstr( lpCmdLine, "+debugger" ))
	//{
		// DebuggerClientInit( lpCmdLine );
	//	return 0;
	//}
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
//	if (sysVars.sys_viewlog.IsModified())
//	{
//		if (!com_skipRenderer.GetBool() && idAsyncNetwork::serverDedicated.GetInteger() != 1)
//		{
//			Sys_ShowConsole( sysVars.sys_viewlog.GetInteger(), false );
//		}
//		sysVars.sys_viewlog.ClearModified();
//	}

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

int main( int argc, const char *argv[] )
{
	MainInit( argc, argv );

	while (true)
	{
		MainLoop();
	}

	return 0;
};