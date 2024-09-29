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

extern const uint32_t k_MAX_TIMEOUT;

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
	friend class crCondition;
	friend class crEvent;
	struct SDL_mutex*	m_mtxhnd;

	// prevent object copy
	crMutex( const crMutex & s ) {}
	void			operator=( const crMutex & s ) {}
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

	struct SDL_cond*	m_cndhnd;

	// prevent object copy
	crCondition( const crCondition & s ) {}
	void			operator=( const crCondition & s ) {}
};

class crSemaphore
{
public:
	crSemaphore( void );
	~crSemaphore( void );
	void	Wait( const uint32_t timeout = 0 ) const;
	void	Trigger( void );

private:
	struct SDL_semaphore*	m_sem;
};

//
class	crEvent : 
	public crMutex,
	public crCondition
{
public: 
	crEvent( const bool manualReset );
	~crEvent( void );

	void	Raise( void );
	void	Clear( void );
	bool	Wait( unsigned int timeout );

private:
	std::atomic<uint32_t>	m_waiting; 		// number of threads waiting for a signal
	std::atomic<bool>		m_signaled; 	// is it signaled right now?
	bool					m_manualReset;	//
};

#endif // !_MUTEX_H_

