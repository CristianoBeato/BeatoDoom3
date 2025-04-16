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

#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <atomic>

extern const int32_t k_MAX_TIMEOUT;

class crMutex
{
public:
	crMutex( void );
	~crMutex( void );

	// lock a section acess 
	void Lock( void ) const;

	// release the lock 
	void Unlock( void ) const;

private:
	// Mutex pointer acess, for condition lock
	friend class 	crCondition;
	friend class 	crEvent;
	class SDLMutex*	m_mtxhnd;

	// prevent object copy
	crMutex( const crMutex & s ) = delete;
	void			operator=( const crMutex & s ) = delete;
};

class crScopeLock
{
public:
	ID_INLINE crScopeLock( const crMutex * lock ) : m_lock( lock )
	{
		if (m_lock)
			m_lock->Lock();
	}

	ID_INLINE ~crScopeLock( void )
	{
		if (m_lock)
			m_lock->Unlock();
	}

private:
	const crMutex* m_lock;
};

class crCondition
{
public:
	crCondition( void );
	~crCondition( void );

	// Unlock one thread 
	void	Signal( void ) const;

	// Unlock all thread waiting for the signal
	void	SignalAll( void ) const;

	// Lock the current thread execution
	void	Wait( const crMutex * lock, const uint32_t timeout = 0 ) const;

private:
	friend class crEvent;
	class SDLCondition*		m_cndhnd;

	// prevent object copy
	crCondition( const crCondition & s ) = delete;
	void			operator=( const crCondition & s ) = delete;
};

class crSemaphore
{
public:
	crSemaphore( void );
	~crSemaphore( void );
	void	Wait( const uint32_t timeout = 0 ) const;
	void	Trigger( void );

private:
	class SDLSemaphore*	m_sem;
};

/*
================================================
idSysSignal is a C++ wrapper for the low level system signal functions.  A signal is an object
that a thread can wait on for it to be raised.  It's used to indicate data is available or that
a thread has reached a specific point.
================================================
*/
class idSysSignal
{
public:
	static const int	WAIT_INFINITE = -1;
	
	idSysSignal( bool manualReset = false );
	~idSysSignal( void );
	void	Raise( void );
	void	Clear( void );
	
	// Wait returns true if the object is in a signalled state and
	// returns false if the wait timed out. Wait also clears the signalled
	// state when the signalled state is reached within the time out period.
	bool	Wait( int timeout = WAIT_INFINITE );
	
private:
	// DG: all this stuff is needed to emulate Window's Event API
	//     (CreateEvent(), SetEvent(), WaitForSingleObject(), ...)
	bool 						m_manualReset;
	SDL_AtomicInt				m_signaled; 		    // is it signaled right now?
	SDL_AtomicInt				m_waiting;            // number of threads waiting for a signal
	struct SDLCondition*		m_cond;
	struct SDLMutex* 			m_mutex;
	
	idSysSignal( const idSysSignal& s ) = delete;
	void				operator=( const idSysSignal& s ) = delete;
};

/*
================================================
idSysInterlockedInteger is a C++ wrapper for the low level system interlocked integer
routines to atomically increment or decrement an integer.
================================================
*/
class idSysInterlockedInteger
{
public:
	idSysInterlockedInteger( void );
	idSysInterlockedInteger( const idSysInterlockedInteger &ref );
	~idSysInterlockedInteger( void );
	int					Increment( void ); 		// atomically increments the integer and returns the new value
	int					Decrement( void );		// atomically decrements the integer and returns the new value
	int					Add( int v );			// atomically adds a value to the integer and returns the new value
	int					Sub( int v );			// atomically subtracts a value from the integer and returns the new value
	int					GetValue( void ) const;	// returns the current value of the integer
	void				SetValue( int v );		// sets a new value, Note: this operation is not atomic

private:
	struct SDL_AtomicInt*	m_value;
};

#endif // !_MUTEX_H_

