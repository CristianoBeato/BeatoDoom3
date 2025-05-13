/*
===========================================================================

Beato idTech 4 Source Code 
Copyright (C) 2016-2024 Cristiano B. Santos <cristianobeato_dm@hotmail.com>.

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

#include "precompiled.h"
#pragma hdrstop

#include "qgl.h"
#include "glCommandQueue.h"

glCommandQueue::glCommandQueue(void) :
    m_lock( nullptr ),
    m_doneLock( nullptr ),
    m_doneSignal( nullptr )
{
    m_lock = new crMutex();
    m_doneLock = new crMutex();
    m_doneSignal = new crCondition();
}

glCommandQueue::~glCommandQueue(void)
{
    SAFE_DELETE( m_doneSignal );
    SAFE_DELETE( m_doneLock );
    SAFE_DELETE( m_lock );
}

void glCommandQueue::AppendCommand(const crAutoPointer<glAsyncCommand> &command )
{
    m_head->SetNext( command );
    m_head = command;
    
    if ( !m_tail )
        m_tail = m_head;
}

bool glCommandQueue::Empty(void)
{
    return ( !m_head && !m_tail );
}

void glCommandQueue::Wait( void )
{
    m_doneLock->Lock();
    m_doneSignal->Wait( m_doneLock );    
}

void glCommandQueue::ExecuteQueue(void)
{
    // get the tail
    crAutoPointer<glAsyncCommand> command = Get();

    // loop throug commands
    do
    {
        command->Invoke();

        // get next 
        command = Get();
    } while ( command );

    m_doneSignal->SignalAll();
}

crAutoPointer<glAsyncCommand> glCommandQueue::Get(void)
{
    crAutoPointer<glAsyncCommand> command = crAutoPointer<glAsyncCommand>();
    {
        crScopeLock lock( m_lock );
        if ( !m_tail )
            return command;
        
        // get tail
        command = m_tail;
        
        // get next in the queue 
        m_tail = command->GetNext();
        if( m_tail == m_head )
            m_head = nullptr;
    }
    return command;
}
