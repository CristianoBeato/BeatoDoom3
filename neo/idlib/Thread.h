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
#include <SDL_thread.h>

struct btAtommicCounter
{
public:
	SDL_atomic_t	m_atomic;

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

	SDL_INLINE btAtommicCounter operator = ( int & v )
	{
		Set( v );
		return *this;
	}

	//
	//
	SDL_INLINE int operator ++ ( void )
	{
		return SDL_AtomicAdd( &m_atomic, 1 );
	}

	SDL_INLINE int operator ++( int v )
	{
		return SDL_AtomicAdd( &m_atomic, v );
	}

	//
	//
	SDL_INLINE int operator -- ( void )
	{
		return SDL_AtomicAdd( &m_atomic, -1 );
	}

	SDL_INLINE int operator --( int v )
	{
		return SDL_AtomicAdd( &m_atomic, -v );
	}

	//
	//
	SDL_INLINE bool	operator ==( int v )
	{
		return (Get() == v);
	}

	SDL_INLINE const bool operator ==( const int v ) const
	{
		return (Get() == v);
	}

	//
	//
	SDL_INLINE bool operator == ( btAtommicCounter & compare ) const
	{
		return (Get() == compare.Get());
	}
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
	btMutex( const btCondition & s ) {}
	void			operator=( const btMutex & s ) {}
};

class btScopeLock
{
public:
	SDL_INLINE btScopeLock( const btMutex * lock ) : m_lock( lock )
	{
		if (m_lock)
			m_lock->Lock();
	}

	SDL_INLINE ~btScopeLock( void )
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

	SDL_INLINE ~btCondition( void )
	{
		if (m_cndhnd != nullptr)
		{
			SDL_DestroyCond( m_cndhnd );
			m_cndhnd = nullptr;
		}
	}

	// Destrava uma thread
	// Unlock one thread 
	SDL_INLINE void	Signal( void ) const
	{
		SDL_CondSignal( m_cndhnd );
	}

	// Destrava todas as threads esperando pelo sinal
	// Unlock all thread waiting for the signal
	SDL_INLINE void	SignalAll( void ) const
	{
		SDL_CondBroadcast( m_cndhnd );
	}

	// Trava a execução da atual thread
	// Lock the current thread execution
	SDL_INLINE void	Wait( const btMutex * lock, const Uint32 timeout = 0 ) const
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
	btCondition( const btCondition & s ) {}
	void			operator=( const btCondition & s ) {}
};


#endif // !_THREAD_H_

