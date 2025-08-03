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

#include <SDL3/SDL_thread.hpp>
#include <SDL3/SDL_atomic.h>

static const uint32_t DEFAULT_THREAD_STACK_SIZE	= 256 * 1024; // 256kb
static uint32_t THREAD_NORMAL = 0;

#define SYS_MEMORYBARRIER SDL_CompilerBarrier()

/*
================================================
idSysThread is an abstract base class, to be extended by classes implementing the
idSysThread::Run() method.

	class idMyThread : public idSysThread {
	public:
		virtual int Run() {
			// run thread code here
			return 0;
		}
		// specify thread data here
	};

	idMyThread thread;
	thread.Start( "myThread" );

A worker thread is a thread that waits in place (without consuming CPU)
until work is available. A worker thread is implemented as normal, except that, instead of
calling the Start() method, the StartWorker() method is called to start the thread.
Note that the Sys_CreateThread function does not support the concept of worker threads.

	class idMyWorkerThread : public idSysThread {
	public:
		virtual int Run() {
			// run thread code here
			return 0;
		}
		// specify thread data here
	};

	idMyWorkerThread thread;
	thread.StartThread( "myWorkerThread" );

	// main thread loop
	for ( ; ; ) {
		// setup work for the thread here (by modifying class data on the thread)
		thread.SignalWork();           // kick in the worker thread
		// run other code in the main thread here (in parallel with the worker thread)
		thread.WaitForThread();        // wait for the worker thread to finish
		// use results from worker thread here
	}

In the above example, the thread does not continuously run in parallel with the main Thread,
but only for a certain period of time in a very controlled manner. Work is set up for the
Thread and then the thread is signalled to process that work while the main thread continues.
After doing other work, the main thread can wait for the worker thread to finish, if it has not
finished already. When the worker thread is done, the main thread can safely use the results
from the worker thread.

Note that worker threads are useful on all platforms but they do not map to the SPUs on the PS3.
================================================
*/
class idSysThread
{
public:
	idSysThread();
	virtual			~idSysThread( void );
	
	ID_INLINE const char* 	GetName() const
	{
		return name.c_str();
	}

// BEATO: Don't expose the handle, why you want use it outside of the thread class ?  
//	ID_INLINE uintptr_t		GetThreadHandle( void ) const
//	{
//		return threadHandle;
//	}
	
	ID_INLINE bool			IsRunning( void ) const
	{
		return isRunning;
	}
	
	ID_INLINE bool			IsTerminating( void ) const
	{
		return isTerminating;
	}
	
	//------------------------
	// Thread Start/Stop/Wait
	//------------------------
	bool			StartThread( const char* name, int stackSize = DEFAULT_THREAD_STACK_SIZE );
								 
	bool			StartWorkerThread( const char* name, int stackSize = DEFAULT_THREAD_STACK_SIZE );
									   
	void			StopThread( bool wait = true );
	
	// This can be called from multiple other threads. However, in the case
	// of a worker thread, the work being "done" has little meaning if other
	// threads are continuously signalling more work.
	void			WaitForThread( void );
	
	//------------------------
	// Worker Thread
	//------------------------
	
	// Signals the thread to notify work is available.
	// This can be called from multiple other threads.
	void			SignalWork( void );
	
	// Returns true if the work is done without waiting.
	// This can be called from multiple other threads. However, the work
	// being "done" has little meaning if other threads are continuously
	// signalling more work.
	bool			IsWorkDone( void );
	
protected:
	// The routine that performs the work.
	virtual int		Run( void );
	
	bool			forceStop;

private:
	idStr				name;
	bool				isWorker;
	bool				isRunning;
	volatile bool		isTerminating;
	volatile bool		moreWorkToDo;
	idSysSignal			signalWorkerDone;
	idSysSignal			signalMoreWorkToDo;
	crMutex				signalMutex;
// BEATO Begin:
	SDL::Thread			threadHandle;
// BEATO End

	static int		ThreadProc( idSysThread* thread );
	
	idSysThread( const idSysThread& s ) {}
	void			operator=( const idSysThread& s ) {}
};

/*
================================================
idSysWorkerThreadGroup implements a group of worker threads that
typically crunch through a collection of similar tasks.

	class idMyWorkerThread : public idSysThread {
	public:
		virtual int Run() {
			// run thread code here
			return 0;
		}
		// specify thread data here
	};

	idSysWorkerThreadGroup<idMyWorkerThread> workers( "myWorkers", 4 );
	for ( ; ; ) {
		for ( int i = 0; i < workers.GetNumThreads(); i++ ) {
			// workers.GetThread( i )-> // setup work for this thread
		}
		workers.SignalWorkAndWait();
		// use results from the worker threads here
	}

The concept of worker thread Groups is probably most useful for tools and compilers.
For instance, the AAS Compiler is using a worker thread group. Although worker threads
will work well on the PC, Mac and the 360, they do not directly map to the PS3,
in that the worker threads won't automatically run on the SPUs.
================================================
*/
template<class threadType>
class idSysWorkerThreadGroup
{
public:
	idSysWorkerThreadGroup( const char* name, int numThreads, int stackSize = DEFAULT_THREAD_STACK_SIZE );
							
	virtual			~idSysWorkerThreadGroup( void );
	
	int				GetNumThreads( void ) const
	{
		return threadList.Num();
	}

	threadType& 	GetThread( int i )
	{
		return *threadList[i];
	}
	
	void			SignalWorkAndWait( void );
	
private:
	idList<threadType*>		threadList;
	bool					runOneThreadInline;	// use the signalling thread as one of the threads
	bool					singleThreaded;		// set to true for debugging
};

/*
========================
idSysWorkerThreadGroup<threadType>::idSysWorkerThreadGroup
========================
*/
template<class threadType>
ID_INLINE idSysWorkerThreadGroup<threadType>::idSysWorkerThreadGroup( const char* name, int numThreads, int stackSize )
{
	runOneThreadInline = ( numThreads < 0 );
	singleThreaded = false;
	numThreads = abs( numThreads );
	for( int i = 0; i < numThreads; i++ )
	{
		threadType* thread = new threadType();
		thread->StartWorkerThread( va( "%s_worker%i", name, i ),  stackSize );
		threadList.Append( thread );
	}
}

/*
========================
idSysWorkerThreadGroup<threadType>::~idSysWorkerThreadGroup
========================
*/
template<class threadType>
ID_INLINE idSysWorkerThreadGroup<threadType>::~idSysWorkerThreadGroup()
{
	threadList.DeleteContents();
}

/*
========================
idSysWorkerThreadGroup<threadType>::SignalWorkAndWait
========================
*/
template<class threadType>
ID_INLINE void idSysWorkerThreadGroup<threadType>::SignalWorkAndWait()
{
	if( singleThreaded )
	{
		for( int i = 0; i < threadList.Num(); i++ )
		{
			threadList[ i ]->Run();
		}
		return;
	}
	for( int i = 0; i < threadList.Num() - runOneThreadInline; i++ )
	{
		threadList[ i ]->SignalWork();
	}
	if( runOneThreadInline )
	{
		threadList[ threadList.Num() - 1 ]->Run();
	}
	for( int i = 0; i < threadList.Num() - runOneThreadInline; i++ )
	{
		threadList[ i ]->WaitForThread();
	}
}

/*
================================================
idSysThreadSynchronizer, allows a group of threads to
synchronize with each other half-way through execution.

	idSysThreadSynchronizer sync;

	class idMyWorkerThread : public idSysThread {
	public:
		virtual int Run() {
			// perform first part of the work here
			sync.Synchronize( threadNum );	// synchronize all threads
			// perform second part of the work here
			return 0;
		}
		// specify thread data here
		unsigned int threadNum;
	};

	idSysWorkerThreadGroup<idMyWorkerThread> workers( "myWorkers", 4 );
	for ( int i = 0; i < workers.GetNumThreads(); i++ ) {
		workers.GetThread( i )->threadNum = i;
	}

	for ( ; ; ) {
		for ( int i = 0; i < workers.GetNumThreads(); i++ ) {
			// workers.GetThread( i )-> // setup work for this thread
		}
		workers.SignalWorkAndWait();
		// use results from the worker threads here
	}

================================================
*/
class idSysThreadSynchronizer
{
public:
	static const int	WAIT_INFINITE = -1;
	
	ID_INLINE	void			SetNumThreads( unsigned int num );
	ID_INLINE	void			Signal( unsigned int threadNum );
	ID_INLINE	bool			Synchronize( unsigned int threadNum, int timeout = WAIT_INFINITE );
	
private:
	idList< idSysSignal*>		signals;
	idSysInterlockedInteger		busyCount;
};

/*
========================
idSysThreadSynchronizer::SetNumThreads
========================
*/
ID_INLINE void idSysThreadSynchronizer::SetNumThreads( unsigned int num )
{
	assert( busyCount.GetValue() == signals.Num() );
	if( ( int )num != signals.Num() )
	{
		signals.DeleteContents( true );
		signals.SetNum( ( int )num );
		for( uint32_t i = 0; i < num; i++ )
		{
			signals[i] = new idSysSignal();
		}
		busyCount.SetValue( num );
		SYS_MEMORYBARRIER;
	}
}

/*
========================
idSysThreadSynchronizer::Signal
========================
*/
ID_INLINE void idSysThreadSynchronizer::Signal( unsigned int threadNum )
{
	if( busyCount.Decrement() == 0 )
	{
		busyCount.SetValue( ( unsigned int ) signals.Num() );
		SYS_MEMORYBARRIER;
		for( int i = 0; i < signals.Num(); i++ )
		{
			signals[i]->Raise();
		}
	}
}

/*
========================
idSysThreadSynchronizer::Synchronize
========================
*/
ID_INLINE bool idSysThreadSynchronizer::Synchronize( unsigned int threadNum, int timeout )
{
	return signals[threadNum]->Wait( timeout );
}

#endif // !_THREAD_H_

