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

#include <SDL2/SDL_thread.h>

/*
================================================================================================
	crSysThread
================================================================================================
*/

/*
========================
crSysThread::crSysThread
========================
*/
crSysThread::crSysThread(void) : 
	m_name(),
	m_threadHandle( nullptr ),
	m_priority( 0 ),
	m_isRunning( false ),
	m_isTerminating( false ),
	m_isWorker( false ),
	m_moreWorkToDo( false ),
	m_signalWorkerDone( nullptr ),
	m_signalMoreWorkToDo( nullptr ),
	m_signalMutex( nullptr )
{
	m_signalMutex = new crMutex();
	m_signalMoreWorkToDo = new crEvent( true );
	m_signalWorkerDone = new crEvent( true );
}

/*
========================
crSysThread::~crSysThread
========================
*/
crSysThread::~crSysThread(void)
{
	StopThread( true );
	SAFE_DELETE( m_signalWorkerDone );
	SAFE_DELETE( m_signalMoreWorkToDo );
	SAFE_DELETE( m_signalMutex );
}

/*
========================
crSysThread::StartThread
========================
*/
bool crSysThread::StartThread( const char *name, bool worker, int priority, uint32_t stackSize )
{
	// alreay running TODO: use assert instead ?
	if ( m_isRunning )
		return false;

	m_name = name;
	m_isTerminating = false;
	m_priority = priority;
	m_isWorker = worker;

	// Creathe thread 
	m_threadHandle = SDL_CreateThreadWithStackSize( ThreadProc, name, stackSize, this );
	if ( !m_threadHandle )
	{
		throw idException( "Failed to create Thread: %s", SDL_GetError() );
	}

	//
	if (worker)
		m_signalWorkerDone->Wait( SDL_MUTEX_MAXWAIT );
	
	m_isRunning = true;
	return true;
}

/*
========================
crSysThread::StopThread
========================
*/
void crSysThread::StopThread( const bool wait )
{
	if ( !m_isRunning ) 
		return;
	
	if ( m_isWorker ) 
	{
		m_signalMutex->Lock();
		m_moreWorkToDo = true;
		m_signalWorkerDone->Clear();
		m_isTerminating = true;
		m_signalMoreWorkToDo->Raise();
		m_signalMutex->Unlock();
	} 
	else 
	{
		m_isTerminating = true;
	}
	
	if ( wait )
		WaitForThread();
}

/*
========================
crSysThread::WaitForThread
========================
*/
void crSysThread::WaitForThread(void)
{
	if ( m_isWorker ) 
		m_signalWorkerDone->Wait( SDL_MUTEX_MAXWAIT );
	else if ( m_isRunning ) 
		m_threadHandle = nullptr; //Sys_DestroyThread( threadHandle ); // not needed for SDL2, thread clear it own handler
}

/*
========================
crSysThread::SignalWork
========================
*/
void crSysThread::SignalWork(void)
{
	if ( m_isWorker ) 
	{
		m_signalMutex->Lock();
		m_moreWorkToDo = true;
		m_signalWorkerDone->Clear();
		m_signalMoreWorkToDo->Raise();
		m_signalMutex->Unlock();
	}
}

/*
========================
crSysThread::IsWorkDone
========================
*/
bool crSysThread::IsWorkDone(void)
{
	if ( m_isWorker ) 
	{
		// a timeout of 0 will return immediately with true if signaled
		if ( m_signalWorkerDone->Wait( 0 ) ) 
			return true;
	}
	
	return false;
}

/*
========================
crSysThread::ThreadProc
========================
*/
int crSysThread::ThreadProc( void* threadPtr ) 
{
	int retVal = 0;
	crSysThread* thread = static_cast<crSysThread*>( threadPtr );
	try 
	{
		if ( thread->m_isWorker ) 
		{
			for( ; ; ) 
			{
				thread->m_signalMutex->Lock();
				if ( thread->m_moreWorkToDo ) 
				{
					thread->m_moreWorkToDo = false;
					thread->m_signalMoreWorkToDo->Clear();
					thread->m_signalMutex->Unlock();
				} 
				else 
				{
					thread->m_signalWorkerDone->Raise();
					thread->m_signalMutex->Unlock();
					thread->m_signalMoreWorkToDo->Wait( SDL_MUTEX_MAXWAIT );
					continue;
				}

				if ( thread->m_isTerminating ) 
					break;

				retVal = thread->Run();
			}
			thread->m_signalWorkerDone->Raise();
		} 
		else 
		{
			retVal = thread->Run();
		}
	} 
	catch ( idException &ex ) 
	{
		idLib::Warning( "Fatal error in thread %s: %s", thread->GetName(), ex.What() );
		// We don't handle threads terminating unexpectedly very well, so just terminate the whole process
		exit( -1 );
	}

	thread->m_isRunning = false;

	return retVal;
}

/*
========================
crSysThread::Run
========================
*/
int crSysThread::Run( void ) 
{
	// The Run() is not pure virtual because on destruction of a derived class
	// the virtual function pointer will be set to NULL before the idSysThread
	// destructor actually stops the thread.
	return 0;
}
