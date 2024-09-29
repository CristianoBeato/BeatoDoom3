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
#include "Mutex.h"

#include <SDL2/SDL_mutex.h>

const uint32_t k_MAX_TIMEOUT = SDL_MUTEX_MAXWAIT;

crMutex::crMutex( void ) : m_mtxhnd( nullptr )
{
		m_mtxhnd = SDL_CreateMutex();
}

crMutex::~crMutex( void )
{
	if (m_mtxhnd != nullptr)
	{
		SDL_DestroyMutex( m_mtxhnd );
		m_mtxhnd = nullptr;
	}
}

void crMutex::Lock( void ) const
{
	SDL_LockMutex( m_mtxhnd );
}

void crMutex::Unlock( void ) const
{	
	SDL_UnlockMutex( m_mtxhnd );
}

crCondition::crCondition( void ) : m_cndhnd( nullptr )
{
	m_cndhnd = SDL_CreateCond();
}

crCondition::~crCondition( void )
{
	if (m_cndhnd != nullptr)
	{
		SDL_DestroyCond( m_cndhnd );
		m_cndhnd = nullptr;
	}
}

// Unlock one thread
void crCondition::Signal( void ) const
{
	SDL_CondSignal( m_cndhnd );
}

// Unlock all thread waiting for the signal
void crCondition::SignalAll( void ) const
{
	SDL_CondBroadcast( m_cndhnd );
}

// Lock the current thread execution
void crCondition::Wait( const crMutex * lock, const uint32_t timeout ) const
{
	assert( lock );
	// Precisa estar travado antes
	// Must be locked before
	lock->Lock();
	if (timeout > 0)
		SDL_CondWaitTimeout( m_cndhnd, lock->m_mtxhnd, timeout );
	else
		SDL_CondWait( m_cndhnd, lock->m_mtxhnd );
}

crSemaphore::crSemaphore( void ) : m_sem(nullptr)
{
	m_sem = SDL_CreateSemaphore( 1 );
}

crSemaphore::~crSemaphore( void )
{
	if (m_sem != nullptr)
	{
		SDL_DestroySemaphore( m_sem );
		m_sem = nullptr;
	}
}

void crSemaphore::Wait( const uint32_t timeout ) const
{
	assert( m_sem );
	if (timeout > 0)
		SDL_SemWaitTimeout( m_sem, timeout );
	else
		SDL_SemWait( m_sem );
}

void crSemaphore::Trigger( void )
{
	assert( m_sem );
	SDL_SemPost( m_sem );
}

crEvent::crEvent( const bool manualReset ) :
	m_manualReset( manualReset ),
	crMutex(),
	crCondition(),
	m_signaled( false ),
	m_waiting( 0 )
{
}

crEvent::~crEvent( void )
{
	m_signaled = false;
	m_waiting = 0;
}

void crEvent::Raise( void )
{
	// SetEvent( handle );
	Lock();

	if( m_manualReset )
	{
		// signaled until reset
		m_signaled.exchange( true );
		// wake *all* threads waiting on this cond
		SignalAll();
	}
	else
	{
		
		if( m_waiting > 0 ) // automode: signaled until first thread is released
			Signal(); // there are waiting threads => release one
		else
			m_signaled.exchange( true ); // no waiting threads, save signal
			// while the MSDN documentation is a bit unspecific about what happens
			// when SetEvent() is called n times without a wait inbetween
			// (will only one wait be successful afterwards or n waits?)
			// it seems like the signaled state is a flag, not a counter.
			// http://stackoverflow.com/a/13703585 claims the same.
	}
	
	Unlock();
}

void crEvent::Clear( void )
{
	m_signaled.exchange( false );
}

bool crEvent::Wait( unsigned int timeout )
{
	int status = 0;
	Lock();
	
	if( m_signaled ) // there is a signal that hasn't been used yet
	{
		if( ! m_manualReset ) // for auto-mode only one thread may be released - this one.
			m_signaled.exchange( false );

		status = 0; // success!
	}
	else // we'll have to wait for a signal
	{
		m_waiting.fetch_add( 1 );
		if( timeout == SDL_MUTEX_MAXWAIT )
			status = SDL_CondWait( m_cndhnd, m_mtxhnd );
		else
			status = SDL_CondWaitTimeout( m_cndhnd, m_mtxhnd, timeout );

		m_waiting.fetch_sub( 1 );
	}

	Unlock();

	assert( status == 0 || ( timeout != SDL_MUTEX_MAXWAIT && status == SDL_MUTEX_TIMEDOUT ) );

	return ( status == 0 );
}
