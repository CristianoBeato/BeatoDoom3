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

#include <SDL3/SDL_properties.hpp>

/*
================================================================================================

	idSysThread

================================================================================================
*/

/*
========================
idSysThread::idSysThread
========================
*/
idSysThread::idSysThread( void ) :
	threadHandle( nullptr ),
	isWorker( false ),
	isRunning( false ),
	isTerminating( false ),
	moreWorkToDo( false ),
	signalWorkerDone( true ),
	forceStop( false )
{
}

/*
========================
idSysThread::~idSysThread
========================
*/
idSysThread::~idSysThread( void )
{
	StopThread( !forceStop );
	threadHandle = nullptr;
}

/*
========================
idSysThread::StartThread
========================
*/
bool idSysThread::StartThread( const char* name_, int stackSize )
{
	SDLProperties threadProperties;
	if( isRunning )
		return false;
	
	this->name = name_;
	isTerminating = false;
	assert( threadHandle == true ); // thread already running
	
#if 1
	//Create tread using properties
	threadProperties.CreateProperties();
	threadProperties.SetStringProperty( SDL_PROP_THREAD_CREATE_NAME_STRING, name_ );
    threadProperties.SetNumberProperty( SDL_PROP_THREAD_CREATE_STACKSIZE_NUMBER, stackSize );
    threadProperties.SetPointerProperty( SDL_PROP_THREAD_CREATE_ENTRY_FUNCTION_POINTER, (void*)ThreadProc );
    threadProperties.SetPointerProperty( SDL_PROP_THREAD_CREATE_USERDATA_POINTER, static_cast<void*>( this ) );

	if( !threadHandle.CreateWithProperties( threadProperties ) ) 
		idLib::Error( "idSysThread::StartThread( %s )::Error: %s\n", name_, SDL_GetError() );
	
	threadProperties.DestroyProperties();
#else
	// Create thread object and run
	threadHandle.Create( static_cast<SDL_FunctionPointer>( ThreadProc ), name, static_cast<void*>( this ) );
#endif

	isRunning = true;
	return true;
}

/*
========================
idSysThread::StartWorkerThread
========================
*/
bool idSysThread::StartWorkerThread( const char* name_, int stackSize )
{
	if( isRunning )
		return false;
	
	isWorker = true;
	
	bool result = StartThread( name_, stackSize );
	
	signalWorkerDone.Wait( idSysSignal::WAIT_INFINITE );
	
	return result;
}

/*
========================
idSysThread::StopThread
========================
*/
void idSysThread::StopThread( bool wait )
{
	if( !isRunning )
		return;
	
	if( isWorker )
	{
		signalMutex.Lock();
		moreWorkToDo = true;
		signalWorkerDone.Clear();
		isTerminating = true;
		signalMoreWorkToDo.Raise();
		signalMutex.Unlock();
	}
	else
		isTerminating = true;
	
	if( wait )
		WaitForThread();
}

/*
========================
idSysThread::WaitForThread
========================
*/
void idSysThread::WaitForThread( void )
{
	if( isWorker )
		signalWorkerDone.Wait( idSysSignal::WAIT_INFINITE );
	else if( isRunning )
		threadHandle.Wait( nullptr );
}

/*
========================
idSysThread::SignalWork
========================
*/
void idSysThread::SignalWork( void )
{
	if( isWorker )
	{
		signalMutex.Lock();
		moreWorkToDo = true;
		signalWorkerDone.Clear();
		signalMoreWorkToDo.Raise();
		signalMutex.Unlock();
	}
}

/*
========================
idSysThread::IsWorkDone
========================
*/
bool idSysThread::IsWorkDone( void )
{
	if( isWorker )
	{
		// a timeout of 0 will return immediately with true if signaled
		if( signalWorkerDone.Wait( 0 ) )
		{
			return true;
		}
	}
	return false;
}

/*
========================
idSysThread::ThreadProc
========================
*/
int idSysThread::ThreadProc( idSysThread* thread )
{
	int retVal = 0;

	try
	{
		if( thread->isWorker )
		{
			for( ; ; )
			{
				thread->signalMutex.Lock();
				if( thread->moreWorkToDo )
				{
					thread->moreWorkToDo = false;
					thread->signalMoreWorkToDo.Clear();
					thread->signalMutex.Unlock();
				}
				else
				{
					thread->signalWorkerDone.Raise();
					thread->signalMutex.Unlock();
					thread->signalMoreWorkToDo.Wait( idSysSignal::WAIT_INFINITE );
					continue;
				}
				
				if( thread->isTerminating )
					break;

				retVal = thread->Run();
			}
			thread->signalWorkerDone.Raise();
		}
		else
		{
			retVal = thread->Run();
		}
	}
	catch( idException& ex )
	{
		idLib::Warning( "Fatal error in thread %s: %s", thread->GetName(), ex.What() );
		
		// We don't handle threads terminating unexpectedly very well, so just terminate the whole process
		exit( 0 );
	}
	
	thread->isRunning = false;
	
	return retVal;
}

/*
========================
idSysThread::Run
========================
*/
int idSysThread::Run( void )
{
	// The Run() is not pure virtual because on destruction of a derived class
	// the virtual function pointer will be set to NULL before the idSysThread
	// destructor actually stops the thread.
	return 0;
}
