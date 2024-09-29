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

static const uint32_t DEFAULT_THREAD_STACK_SIZE	= 256 * 1024; // 256kb
static uint32_t THREAD_NORMAL = 0;

// BEATO: From DOOM-3-BFG
/*
================================================
crSysThread is an abstract base class, to be extended by classes implementing the
crSysThread::Run() method. 

	class idMyThread : public crSysThread 
	{
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

	class idMyWorkerThread : public crSysThread 
	{
	public:
		virtual int Run() 
		{
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
================================================
*/
struct SDL_Thread;
class crSysThread 
{
public:
					crSysThread( void );
	virtual			~crSysThread( void );

	const char *	GetName( void ) const { return m_name.c_str(); }
	SDL_Thread*		GetThreadHandle() const { return m_threadHandle; }
	bool			IsRunning( void ) const { return m_isRunning; }
	bool			IsTerminating( void ) const { return m_isTerminating; }

	//------------------------
	// Thread Start/Stop/Wait
	//------------------------

	bool			StartThread( const char * name, bool worker, int priority = 0, uint32_t stackSize = DEFAULT_THREAD_STACK_SIZE );

	// singal thread to exit 
	void			StopThread( const bool wait = true );

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

private:
	bool			m_isWorker;
	bool			m_isRunning;
	volatile bool	m_isTerminating;
	volatile bool	m_moreWorkToDo;
	int				m_priority;
	crEvent*		m_signalWorkerDone;
	crEvent*		m_signalMoreWorkToDo;
	crMutex*		m_signalMutex;
	SDL_Thread*		m_threadHandle;
	idStr			m_name;

	static int		ThreadProc( void * thread );

	// Disable copy contructor, and asignament 
					crSysThread( const crSysThread & s ) {}
	void			operator=( const crSysThread & s ) {}
};

#endif // !_THREAD_H_

