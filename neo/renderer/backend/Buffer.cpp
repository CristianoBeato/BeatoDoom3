/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "precompiled.h"
#pragma hdrstop

#include "renderer/renderer_common.h"
#include "Buffer.h"

crBuffer::crBuffer( void ) : 
    m_size( 0 ),
    m_map( nullptr )
{
#if CR_USE_VULKAN
    m_buffer = VK_NULL_HANDLER;
#elif CR_USE_OPENGL
    m_buffer = 0;
#endif // CR_USE_OPENGL
}

crBuffer::~crBuffer( void )
{
    Destroy();
}

bool crBuffer::Create( const size_t size, const bool write, const bool read )
{
#if CR_USE_VULKAN
#elif CR_USE_OPENGL
    GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    if( write )
        flags != GL_MAP_WRITE_BIT;

    if( read )
        flags != GL_MAP_READ_BIT;

    // create buffer object
    glCreateBuffers(1, &m_buffer );

    // reserve buffer memory 
    glNamedBufferStorage( m_buffer, size, nullptr, flags );

    // get buffer pointer 
    m_map = glMapNamedBufferRange( m_buffer, 0, m_size, flags );

#endif // CR_USE_OPENGL
    
    return true;
}

void crBuffer::Destroy(void)
{
#if CR_USE_VULKAN

#elif CR_USE_OPENGL
    if( m_buffer != 0 )
    {
        glUnmapNamedBuffer( m_buffer );
        glDeleteBuffers( 1, &m_buffer );
        m_buffer = 0;
    }
#endif    
}

void crBuffer::Upload( const void *data, const uintptr_t offset, const size_t size ) const
{
    ::memcpy( reinterpret_cast<void*>( reinterpret_cast<uintptr_t>( m_map ) + offset ), data, size );
}

void crBuffer::Download( void *data, const uintptr_t offset, const size_t size ) const
{
    ::memcpy( data, reinterpret_cast<void*>( reinterpret_cast<uintptr_t>( m_map ) + offset ), size );
}
