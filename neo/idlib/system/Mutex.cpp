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

#include <SDL3/SDL_mutex.hpp>

// RAII helper for SDL mutex 
class SDLMutexLock
{
private:
    SDLMutex*  mutex;

public:
    explicit SDLMutexLock( SDLMutex* mtx ) : mutex( mtx )
    {
		mutex->Lock();
    }

    ~SDLMutexLock( void )
    {
		mutex->Unlock();
    }
};

const int32_t k_MAX_TIMEOUT = -1;

crMutex::crMutex( void ) : m_mtxhnd( nullptr )
{
	m_mtxhnd = new SDLMutex();
	m_mtxhnd->Create();
}

crMutex::~crMutex( void )
{
	if ( m_mtxhnd != nullptr )
	{
		m_mtxhnd->Destroy();
		delete m_mtxhnd;
		m_mtxhnd = nullptr;
	}
}

void crMutex::Lock( void ) const
{
	m_mtxhnd->Lock();
}

void crMutex::Unlock( void ) const
{	
	m_mtxhnd->Unlock();
}

crCondition::crCondition( void ) : m_cndhnd( nullptr )
{
	m_cndhnd = new SDLCondition();
	m_cndhnd->Create();
}

crCondition::~crCondition( void )
{
	if ( m_cndhnd != nullptr )
	{
		m_cndhnd->Destroy();
		delete m_cndhnd;
		m_cndhnd = nullptr;
	}
}

// Unlock one thread
void crCondition::Signal( void ) const
{
	m_cndhnd->Signal();
}

// Unlock all thread waiting for the signal
void crCondition::SignalAll( void ) const
{
	m_cndhnd->Broadcast();
}

// Lock the current thread execution
void crCondition::Wait( const crMutex * lock, const uint32_t timeout ) const
{
	assert( lock );
	
	// Must be locked before
	lock->Lock();
	if (timeout > 0)
		m_cndhnd->WaitTimeout( *lock->m_mtxhnd, timeout );
	else
		m_cndhnd->Wait( *lock->m_mtxhnd );
}

crSemaphore::crSemaphore( void ) : m_sem(nullptr)
{
	m_sem = new SDLSemaphore();
	m_sem->Create( 1 );
}

crSemaphore::~crSemaphore( void )
{
	if (m_sem != nullptr)
	{
		m_sem->Destroy();
		delete m_sem;
		m_sem = nullptr;
	}
}

void crSemaphore::Wait( const uint32_t timeout ) const
{
	assert( m_sem );
	if (timeout > 0)
		m_sem->WaitTimeout( timeout );
	else
		m_sem->Wait();
}

void crSemaphore::Trigger( void )
{
	assert( m_sem );
	m_sem->Signal();
}

/*
================================================================================================
idSysSignal
================================================================================================
*/

#define SIGNALED 1
#define NOT_SIGNALED 0

/*
========================
idSysSignal::idSysSignal
========================
*/
idSysSignal::idSysSignal( bool manualReset ) : 
	m_manualReset( false ),
	m_cond( nullptr ),
	m_mutex( nullptr )
{
	// if this is true, the signal is only set to nonsignaled when Clear() is called,
    // else it's "auto-reset" and the state is set to !signaled after a single waiting
	// thread has been released
	m_manualReset = manualReset;
	
	// the inital state is always "not signaled"
    SDL_SetAtomicInt( &m_signaled, NOT_SIGNALED );
    SDL_SetAtomicInt( &m_waiting, 0 ); 
    m_mutex = new SDLMutex();
	m_mutex->Create();
	
    m_cond = new SDLCondition();
	m_cond->Create();
}

/*
========================
idSysSignal::~idSysSignal
========================
*/
idSysSignal::~idSysSignal( void )
{
	m_manualReset = false;
	SDL_SetAtomicInt( &m_signaled, NOT_SIGNALED ); // handle.signaled = false;
    SDL_SetAtomicInt( &m_waiting, 0 );
    
	if( m_cond != nullptr )
	{
		m_cond->Destroy();
		m_cond = nullptr;
	}

	if ( m_mutex )
	{
		m_mutex->Destroy();
		delete m_mutex;
		m_mutex = nullptr;
	}
}

/*
========================
idSysSignal::Raise
========================
*/	
void idSysSignal::Raise( void )
{
	SDLMutexLock lock( m_mutex );
	if( m_manualReset )
	{
		// signaled until reset
		SDL_SetAtomicInt( &m_signaled, SIGNALED ); // m_signaled = true;
		// wake *all* threads waiting on this cond
		m_cond->Broadcast();
	}
	else
	{
		// automode: signaled until first thread is released
		if( SDL_GetAtomicInt( &m_waiting ) > 0 )
		{
			// there are waiting threads => release one
			m_cond->Signal();
		}
		else
		{
			// while the MSDN documentation is a bit unspecific about what happens
			// when SetEvent() is called n times without a wait inbetween
			// (will only one wait be successful afterwards or n waits?)
			// it seems like the signaled state is a flag, not a counter.
			// http://stackoverflow.com/a/13703585 claims the same.
			
            // no waiting threads, save signal
			SDL_SetAtomicInt( &m_signaled, SIGNALED ); // m_signaled = true;
		}
	}
}

/*
========================
idSysSignal::Clear
========================
*/
void idSysSignal::Clear( void )
{	
	//SDLMutexLock lock( handle.mutex );
    SDL_SetAtomicInt( &m_signaled, NOT_SIGNALED ); // m_signaled = false; 
}

/*
========================
idSysSignal::Wait
========================
*/	
bool idSysSignal::Wait( int timeout )
{
	bool status = true;

	SDLMutexLock lock( m_mutex );

    if( SDL_GetAtomicInt( &m_signaled ) == SIGNALED ) // there is a signal that hasn't been used yet
    {
    	if( !m_manualReset ) // for auto-mode only one thread may be released - this one.
               SDL_SetAtomicInt( &m_signaled, NOT_SIGNALED ); // handle.signaled = false;
   
    	status = true; // success!
    }
    else // we'll have to wait for a signal
    {
        SDL_AtomicIncRef( &m_waiting );
    	if( timeout == idSysSignal::WAIT_INFINITE )
			m_cond->Wait( *m_mutex );
    	else
			m_cond->WaitTimeout( *m_mutex, timeout );
        SDL_AtomicDecRef( &m_waiting );
    }

	return status;
}


/*
================================================================================================
idSysInterlockedInteger
================================================================================================
*/

/*
========================
idSysInterlockedInteger::idSysInterlockedInteger
========================
*/
idSysInterlockedInteger::idSysInterlockedInteger() : m_value( nullptr ) 
{
	m_value = new SDL_AtomicInt();
	m_value->value = 0;
}

/*
========================
idSysInterlockedInteger::idSysInterlockedInteger
========================
*/
idSysInterlockedInteger::idSysInterlockedInteger(const idSysInterlockedInteger &ref) : m_value( ref.m_value )
{
}


idSysInterlockedInteger::~idSysInterlockedInteger( void )
{
	SAFE_DELETE( m_value );
}

/*
========================
idSysInterlockedInteger::Increment
========================
*/	
int idSysInterlockedInteger::Increment( void )
{
	return SDL_AddAtomicInt( m_value, 1 );
}

/*
========================
idSysInterlockedInteger::Decrement
========================
*/	
int	idSysInterlockedInteger::Decrement( void )
{
	return SDL_AddAtomicInt( m_value, -1 );
}

/*
========================
idSysInterlockedInteger::Add
========================
*/	
int idSysInterlockedInteger::Add( int v )
{
	return SDL_AddAtomicInt( m_value, v );
}

/*
========================
idSysInterlockedInteger::Sub
========================
*/	
int idSysInterlockedInteger::Sub( int v )
{
	return SDL_AddAtomicInt( m_value, -v );
}

/*
========================
idSysInterlockedInteger::GetValue
========================
*/	
int	idSysInterlockedInteger::GetValue( void ) const
{
	return SDL_GetAtomicInt( m_value );
}

/*
========================
idSysInterlockedInteger::SetValue
========================
*/	
void idSysInterlockedInteger::SetValue( int v )
{
	SDL_SetAtomicInt( m_value, v );
}

