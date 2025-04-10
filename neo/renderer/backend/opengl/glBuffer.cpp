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

#include "renderer/renderer_common.h"
#include "renderer/backend/Backend_apiwrapper.h"
#include "qgl.h"
#include "glBuffer.h"

crGLBuffer::crGLBuffer( void ) : 
    crBuffer(), 
    m_buffer( 0 )
{
}

crGLBuffer::~crGLBuffer( void )
{
    Destroy();
}

bool crGLBuffer::Create( const size_t size, const bool write, const bool read )
{
    GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    // create buffer object
    glCreateBuffers(1, &m_buffer );

    // reserve buffer memory 
    glNamedBufferStorage( m_buffer, size, nullptr, flags );

    if( write )
        flags != GL_MAP_WRITE_BIT;

    if( read )
        flags != GL_MAP_READ_BIT;

    // get buffer pointer 
    m_map = glMapNamedBufferRange( m_buffer, 0, m_size, flags );

    return true;
}

void crGLBuffer::Destroy(void)
{
    if( m_buffer != 0 )
    {
        glUnmapNamedBuffer( m_buffer );
        glDeleteBuffers( 1, &m_buffer );
        m_buffer = 0;
    }
}
