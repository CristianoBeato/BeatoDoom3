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

#include <SDL_mutex.h>
#include <SDL_thread.h>

#include "sys_main.h"

idCVar sys_threadCount( "sys_threadCount", "0", CVAR_SYSTEM | CVAR_INTEGER, "override the system Thread Count" );
 
struct btThreadLocalStorage
{
	btThreadLocalStorage( void ) :
		_localStorage( 0 ),
		_localAcess( 0 )
	{
		SDL_AtomicLock( &_localAcess );
		_localStorage = SDL_TLSCreate();
		SDL_AtomicUnlock( &_localAcess );
	}

	~btThreadLocalStorage( void )
	{
		SDL_AtomicLock( &_localAcess );

		SDL_AtomicUnlock( &_localAcess );
	}

	void Set( void* val, void (SDLCALL *destructor)(void*) )
	{
		SDL_TLSSet( _localStorage, val, destructor );
	}

	const void*	Get( void ) const
	{
		return SDL_TLSGet( _localStorage );
	}
	
	SDL_TLSID		_localStorage;
	SDL_SpinLock	_localAcess;
};

struct btThreadContext
{
	bool				exit = false;			// Signal thread exit 
	volatile bool		done = false;
	volatile bool		sleeping = true;
#if 0
	SDL_mutex*			lock = nullptr;			// Thread Idle Lock
	SDL_cond*			signal = nullptr;		// Thread Wake Up Signal
#else
	SDL_semaphore*		wakeSignal = nullptr;
#endif
	SDL_Thread*			thread = nullptr;		// System Thread Handle
	volatile const btThreadExecution* thrObject = nullptr;
};

static int threadStaticEntryPoint( void * threadRef )
{
	btThreadContext* threadReference = static_cast<btThreadContext*>(threadRef);
	while (threadReference->exit)
	{
		if (threadReference->thrObject)
		{
			const_cast<btThreadExecution*>(threadReference->thrObject)->Run();
			threadReference->done = true;
			threadReference->thrObject = nullptr;
		}

		// Thread iddle, start to wait
		threadReference->sleeping = true;
#if 0
		SDL_LockMutex( threadReference->lock );
		SDL_CondWait( threadReference->singal, threadReference->lock );
#else
		SDL_SemWait( threadReference->wakeSignal );
#endif
		threadReference->sleeping = false;
	}

	return 0;
}


void Sys_StartThreadPool( void )
{
	if (sys_threadCount.GetInteger() > 0 )
	{
		sysVars.numThreads = Clamp( (Uint32)sys_threadCount.GetInteger(), MIN_THREADS, MAX_THREADS );
	}
	else
	{
		Uint32 sysThreadCount = SDL_GetCPUCount() - 1;
		sysVars.numThreads = Clamp( sysThreadCount, MIN_THREADS, MAX_THREADS );
	}

	for (Uint32 i = 0; i < sysVars.numThreads; i++)
	{
		sysVars.threadPool[i]->exit = false;
		sysVars.threadPool[i]->done = true;
		sysVars.threadPool[i] = Mem_AllocType<btThreadContext>();
#if 0
		sysVars.threadPool[i]->lock = SDL_CreateMutex();
		sysVars.threadPool[i]->singal = SDL_CreateCond();
#else
		sysVars.threadPool[i]->wakeSignal = SDL_CreateSemaphore( 1 );
#endif
		sysVars.threadPool[i]->thread = SDL_CreateThread( threadStaticEntryPoint, nullptr, static_cast<void*>(sysVars.threadPool[i]) );
		sysVars.threadPool[i]->thrObject = nullptr;
	}
}

/*
==================
Sys_StartThread
==================
*/
void Sys_StartThread( const btThreadExecution * thread )
{
	// Get the first idle thread
	Uint32 index = 0;
	while (true)
	{
		assert( index < sysVars.numThreads ); // Max Thread Reached
		if (!sysVars.threadPool[index++]->done)
			continue;

		sysVars.threadPool[index]->thrObject = thread;
		sysVars.threadPool[index]->done = false;

		// Wake Up Curretn 
#if 0
		SDL_CondSignal( sysVars.threadPool[index]->singal );
#else
		SDL_SemPost( sysVars.threadPool[index]->wakeSignal );
#endif
	}
}
