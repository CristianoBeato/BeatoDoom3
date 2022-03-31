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

#include "sys_platform.h"
#include "common/eventInput.h"
#include "sys_main.h"

idCVar in_mouse( "in_mouse", "1", CVAR_SYSTEM | CVAR_BOOL, "enable mouse input" );
/*
==================
IN_Frame

Called every frame, even if not generating commands
==================
*/
void IN_Frame( void )
{
	bool	shouldGrab = true;

	if (!in_mouse.GetBool())
	{
		shouldGrab = false;
	}
	// if fullscreen, we always want the mouse
	if (!win32.cdsFullscreen)
	{
		if (win32.mouseReleased)
		{
			shouldGrab = false;
		}
		if (win32.movingWindow)
		{
			shouldGrab = false;
		}
		if (!win32.activeApp)
		{
			shouldGrab = false;
		}
	}

	if (shouldGrab != win32.mouseGrabbed)
	{
		if (win32.mouseGrabbed)
		{
			IN_DeactivateMouse();
		}
		else
		{
			IN_ActivateMouse();

#if 0	// if we can't reacquire, try reinitializing
			if (!IN_InitDIMouse())
			{
				win32.in_mouse.SetBool( false );
				return;
			}
#endif
		}
	}
}

/*
========================================================================

EVENT LOOP

========================================================================
*/

/*
================
Sys_QueEvent

Ptr should either be null, or point to a block of data that can
be freed by the game later.
================
*/
void Sys_QueEvent( int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr )
{
	sysEvent_t	*ev;

	ev = &eventQue[eventHead & MASK_QUED_EVENTS];

	if (eventHead - eventTail >= MAX_QUED_EVENTS)
	{
		common->Printf( "Sys_QueEvent: overflow\n" );
		// we are discarding an event, but don't leak memory
		if (ev->evPtr)
		{
			Mem_Free( ev->evPtr );
		}
		eventTail++;
	}

	eventHead++;

	ev->evType = type;
	ev->evValue = value;
	ev->evValue2 = value2;
	ev->evPtrLength = ptrLength;
	ev->evPtr = ptr;
}

/*
=============
Sys_PumpEvents

This allows windows to be moved during renderbump
=============
*/
void Sys_PumpEvents( void )
{
	MSG msg;

	// pump the message loop
	while (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ))
	{
		if (!GetMessage( &msg, NULL, 0, 0 ))
		{
			common->Quit();
		}

		// save the msg time, because wndprocs don't have access to the timestamp
		if (win32.sysMsgTime && win32.sysMsgTime > (int)msg.time)
		{
			// don't ever let the event times run backwards	
//			common->Printf( "Sys_PumpEvents: win32.sysMsgTime (%i) > msg.time (%i)\n", win32.sysMsgTime, msg.time );
		}
		else
		{
			win32.sysMsgTime = msg.time;
		}

#ifdef ID_ALLOW_TOOLS
		if (GUIEditorHandleMessage( &msg ))
		{
			continue;
		}
#endif

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

/*
================
Sys_GenerateEvents
================
*/
void Sys_GenerateEvents( void )
{

}

/*
================
Sys_ClearEvents
================
*/
void Sys_ClearEvents( void )
{
}

/*
================
Sys_GetEvent
================
*/
sysEvent_t Sys_GetEvent( void )
{
	sysEvent_t	ev;

	sysVars.input->HandleEvents();

	return ev;
}
