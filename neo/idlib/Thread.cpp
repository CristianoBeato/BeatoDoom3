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
#include "Thread.h"

#include <SDL_thread.h>
#include "sys/sys_main.h"

btThreadExecution::btThreadExecution( const char* _name ) : name( _name )
{
}

btThreadExecution::~btThreadExecution( void )
{
}

void btThreadExecution::StartExecution( void )
{
	pendingExit = false;
	Sys_StartThread( this );
}

void btThreadExecution::FinishExecution( void )
{
	pendingExit = true;
}

const char * btThreadExecution::GetName( void ) const
{
	return name;
}

unsigned long btThreadExecution::GetID( void ) const
{
	return threadId;
}

const bool btThreadExecution::IsCurretThread( void ) const
{
	return SDL_ThreadID() == threadId;
}

const bool btThreadExecution::IsExitPending( void ) const
{
	return pendingExit;
}