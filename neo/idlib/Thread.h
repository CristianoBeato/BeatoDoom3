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

#ifndef _THREAD_H_
#define _THREAD_H_

#include <SDL_atomic.h>
#include <SDL_mutex.h>

struct btAtommicCounter
{
public:

	//
	// 
	const int	Set( int val )
	{
		return SDL_AtomicSet( &m_atomic, val );
	}

	//
	//
	const int	Get( void ) const
	{
		return SDL_AtomicGet( (SDL_atomic_t*)&m_atomic );
	}

	//
	//
	const int	Add( int val )
	{
		return SDL_AtomicAdd( &m_atomic, val );
	}

	const int Sub( int val )
	{
		return SDL_AtomicAdd( &m_atomic, -val );
	}

	//
	//
	const int	Increment( void )
	{
		return SDL_AtomicAdd( &m_atomic, 1 ) + 1;
	}

	//
	//
	const int	Decrement( void )
	{
		return SDL_AtomicAdd( &m_atomic, -1 ) - 1;
	}

	ID_INLINE btAtommicCounter operator = ( int & v )
	{
		Set( v );
		return *this;
	}

	//
	//
	ID_INLINE int operator ++ ( void )
	{
		return SDL_AtomicAdd( &m_atomic, 1 );
	}

	ID_INLINE int operator ++( int v )
	{
		return SDL_AtomicAdd( &m_atomic, v );
	}

	//
	//
	ID_INLINE int operator -- ( void )
	{
		return SDL_AtomicAdd( &m_atomic, -1 );
	}

	ID_INLINE int operator --( int v )
	{
		return SDL_AtomicAdd( &m_atomic, -v );
	}

	//
	//
	ID_INLINE bool	operator ==( int v )
	{
		return (Get() == v);
	}

	ID_INLINE const bool operator ==( const int v ) const
	{
		return (Get() == v);
	}

	//
	//
	ID_INLINE bool operator == ( btAtommicCounter & compare ) const
	{
		return (Get() == compare.Get());
	}

private:
	SDL_atomic_t	m_atomic;

};

class btMutex
{
public:
	btMutex( void ) : m_mtxhnd( nullptr )
	{
		m_mtxhnd = SDL_CreateMutex();
	}

	~btMutex( void )
	{
		if (m_mtxhnd != nullptr)
		{
			SDL_DestroyMutex( m_mtxhnd );
			m_mtxhnd = nullptr;
		}
	}

	void Lock( void ) const
	{
		SDL_LockMutex( m_mtxhnd );
	}

	void Unlock( void ) const
	{
		SDL_UnlockMutex( m_mtxhnd );
	}

private:
	// Acesso ao ponteiro do  mutex, para trava de condição
	// Mutex pointer acess, for condition lock
	friend class btCondition;
	SDL_mutex*	m_mtxhnd;

	// prevent object copy
	btMutex( const btCondition & s ) {}
	void			operator=( const btMutex & s ) {}
};

class btScopeLock
{
public:
	ID_INLINE btScopeLock( const btMutex * lock ) : m_lock( lock )
	{
		if (m_lock)
			m_lock->Lock();
	}

	ID_INLINE ~btScopeLock( void )
	{
		if (m_lock)
			m_lock->Unlock();
	}

private:
	const btMutex* m_lock;
};

class btCondition
{
public:
	btCondition( void ) : m_cndhnd( nullptr )
	{
		m_cndhnd = SDL_CreateCond();
	}

	ID_INLINE ~btCondition( void )
	{
		if (m_cndhnd != nullptr)
		{
			SDL_DestroyCond( m_cndhnd );
			m_cndhnd = nullptr;
		}
	}

	// Destrava uma thread
	// Unlock one thread 
	ID_INLINE void	Signal( void ) const
	{
		SDL_CondSignal( m_cndhnd );
	}

	// Destrava todas as threads esperando pelo sinal
	// Unlock all thread waiting for the signal
	ID_INLINE void	SignalAll( void ) const
	{
		SDL_CondBroadcast( m_cndhnd );
	}

	// Trava a execução da atual thread
	// Lock the current thread execution
	ID_INLINE void	Wait( const btMutex * lock, const Uint32 timeout = 0 ) const
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

private:
	SDL_cond*	m_cndhnd;

	// prevent object copy
	btCondition( const btCondition & s ) {}
	void			operator=( const btCondition & s ) {}
};

class btSemaphore
{
public:
	btSemaphore( void ) : m_sem(nullptr)
	{
		m_sem = SDL_CreateSemaphore( 1 );
	}

	~btSemaphore( void )
	{
		if (m_sem != nullptr)
		{
			SDL_DestroySemaphore( m_sem );
			m_sem = nullptr;
		}
	}

	ID_INLINE void	Wait( const Uint32 timeout = 0 ) const
	{
		assert( m_sem );
		if (timeout > 0)
			SDL_SemWaitTimeout( m_sem, timeout );
		else
			SDL_SemWait( m_sem );
	}

	SDL_INLINE void	Trigger( void )
	{
		assert( m_sem );
		SDL_SemPost( m_sem );
	}

private:
	SDL_semaphore*	m_sem;
};


class btThreadExecution
{
public:
	btThreadExecution( const char* name );
	~btThreadExecution( void );

	void			StartExecution( void );
	void			FinishExecution( void );
	const char*		GetName( void ) const;
	unsigned long	GetID( void ) const;
	const bool		IsCurretThread( void ) const;
	const bool		IsExitPending( void ) const;

protected:
	friend int threadStaticEntryPoint( void * threadRef );
	// NEVER CALL THESE 
	virtual void	Run( void ) = 0;
	virtual void	NotifyExit( void ) = 0;

private:
	volatile bool	pendingExit;
	int				threadHandle;
	unsigned long	threadId;
	const char *	name;

	// prevent object copy
	btThreadExecution( const btThreadExecution & s ) {}
	void			operator=( const btThreadExecution & s ) {}
};

#endif // !_THREAD_H_

