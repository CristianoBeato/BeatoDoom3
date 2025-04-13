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

#ifndef __GL_COMMAND_QUEUE_H__
#define __GL_COMMAND_QUEUE_H__

struct glAsyncCommand
{
public:
    // Run the method
    virtual void Invoke( void ) = 0; 

    // get next in the chain    
    crAutoPointer<glAsyncCommand>   GetNext( void ) { return m_next; };

protected:
    friend class glCommandQueue;
    void    SetNext( crAutoPointer<glAsyncCommand> next ) { m_next = next; };

private:
    crAutoPointer<glAsyncCommand> m_next = crAutoPointer<glAsyncCommand>();
};

// command queue
class glCommandQueue
{
public:
    glCommandQueue( void );
    ~glCommandQueue( void );

    // Put the event into the events queue
	void    AppendCommand( const crAutoPointer<glAsyncCommand>& Capsule );
    bool    Empty( void );
    void    Wait( void );
    
protected:
    friend class crGLContext;
    void                            SetNext( crAutoPointer<glCommandQueue> next ) { m_next = next; }
    crAutoPointer<glCommandQueue>   GetNext( void ) { return m_next; };    
    void    ExecuteQueue( void );

private:
    crMutex*                        m_lock;
    crMutex*                        m_doneLock;
    crCondition*                    m_doneSignal;
    crAutoPointer<glCommandQueue>   m_next;
    crAutoPointer<glAsyncCommand>   m_head;
    crAutoPointer<glAsyncCommand>   m_tail;
    crAutoPointer<glAsyncCommand>   Get( void );
};

#endif //__GL_COMMAND_QUEUE_H__
