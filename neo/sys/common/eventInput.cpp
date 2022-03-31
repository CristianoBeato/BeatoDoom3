/*
===========================================================================

Beato Tech 4 Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
Copyright (C) 2012 dhewg (dhewm3)
Copyright (C) 2012 Robert Beckebans
Copyright (C) 2013 Daniel Gibson
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

#include <SDL_hints.h>
#include <SDL_haptic.h>
#include <SDL_gamecontroller.h>

#include "sys/sys_main.h"
#include "eventInput.h"

// when this is returned, it's assumed that there are no more events!
static const sysEvent_t k_no_more_events = { SE_NONE, 0, 0, 0, NULL };

btInputEvent::btInputEvent( void )
{
}

btInputEvent::~btInputEvent( void )
{
}

void btInputEvent::StartUp( void )
{
	common->Printf( "\n------- Input Initialization -------\n" );
	m_mouseEventArray.SetGranularity( 64 );
	m_keyboardEventArray.SetGranularity( 64 );
	m_controlerEventArray.SetGranularity( 64 );

	common->Printf( "------------------------------------\n" );
}

void btInputEvent::HandleEvents( void )
{
	while (SDL_PollEvent( &m_event ))
	{
		// Check if are a event from the mais game window, else is a tool event
		if (m_event.window.windowID != sysVars.window->id())
		{
			HandleToolsEvent();
			continue;
		}

		switch (m_event.type)
		{
		case SDL_QUIT:
		case SDL_APP_TERMINATING:
		{
			common->Quit();
		}break;
		case SDL_WINDOWEVENT:
			ProcessWindowEvent();

		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
		}	break;
		case SDL_TEXTEDITING:
		case SDL_TEXTINPUT:
		{
		}	break;
		case SDL_MOUSEMOTION:
		case SDL_MOUSEWHEEL:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
		}	break;
		case SDL_CONTROLLERBUTTONUP:
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERAXISMOTION:
		{
		}	break;
		case SDL_CONTROLLERDEVICEADDED:
		case SDL_CONTROLLERDEVICEREMOVED:
		{
		}	break;

		default:
			break;
		}
	}
}

void btInputEvent::ActivateKeyboard( const bool activate )
{
	const char* grabKeyboardEnv = SDL_getenv( SDL_HINT_GRAB_KEYBOARD );
	if (grabKeyboardEnv)
	{
		common->Printf( "The SDL_GRAB_KEYBOARD environment variable is set, setting the in_grabKeyboard CVar to the same value (%s)\n", grabKeyboardEnv );
		in_grabKeyboard.SetString( grabKeyboardEnv );
	}
	else
	{
		in_grabKeyboard.SetModified();
	}
}

int btInputEvent::ProcessTextEvent( void )
{
	switch (m_event.window.event)
	{
	case SDL_WINDOWEVENT_CLOSE:
	case SDL_WINDOWEVENT_FOCUS_GAINED:
	case SDL_WINDOWEVENT_FOCUS_LOST:

	default:
		break;
	}
	return 0;
}