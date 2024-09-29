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

#include "idlib/precompiled.h"
#pragma hdrstop

#include "qvk.h"
#include "buffer.h"

static const GLbitfield s_staticBufferFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
static const GLbitfield s_staticBufferMapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

crBufferData::crBufferData( void ) : 
    m_size( 0 ),
    m_offset( 0 ),
    m_next( nullptr ),
    m_prev( nullptr ),
    m_data( nullptr )
{
}

crBufferData::crBufferData( crBufferData *prev, crBufferData *next, const size_t size, const uintptr_t offset, byte *data ) :
    m_prev( prev ),
    m_next( next ),
    m_size( size ),
    m_offset( offset ),
    m_data( data )
{
}

crBufferData::~crBufferData( void )
{
}

crBuffer::crBuffer( void ) : 
    m_size( 0 ),
    m_free( 0 ),
    m_offset( 0 ),
    m_map( nullptr ),
    m_freeList( nullptr ),
    m_usedList( nullptr )
{
}

crBuffer::~crBuffer( void )
{
}

void crBuffer::Create( const size_t size )
{
    m_size = size;
#if CR_USE_VULKAN
#else
    glCreateBuffers( 1, &m_bufferID );
    glNamedBufferStorage( m_bufferID, m_size, nullptr, s_staticBufferFlags );
    m_map = static_cast<byte*>( glMapNamedBufferRange( m_bufferID, 0, m_size, s_staticBufferMapFlags ) );
#endif
}

void crBuffer::Resize( const size_t size )
{
    crBufferData* block = nullptr;
    crBufferData* last = nullptr;
    m_size = size;
    m_offset = 0;

    // Create a new buffer 
#if CR_USE_VULKAN
    
#else
    GLuint old = m_bufferID;
    // Releas buffer map
    glUnmapNamedBuffer( m_bufferID );

    // Create new buffer 
    glCreateBuffers( 1, &m_bufferID );
    glNamedBufferStorage( m_bufferID, m_size, nullptr, s_staticBufferFlags );

    // map new buffer content
    m_map = static_cast<byte*>( glMapNamedBufferRange( m_bufferID, 0, m_size, s_staticBufferMapFlags ) );
#endif

    block = m_usedList; 
    crBufferData* last = m_usedList->Prev(); 
    while ( block != nullptr )
    {
#if CR_USE_CULKAN
    

#else
        glCopyBufferSubData( old, m_bufferID, block->Offset(), m_offset, block->Size() );
#endif

        block->UpdateData( m_offset, m_map + m_offset );

        // Get next block or end loop
        if( block != last )
            block = block->Next();
        else
            break; 
    }
    
#if CR_USE_VULKAN

#else
    // delete old buffer 
    glDeleteBuffers( 1, &old );
#endif
}

void crBuffer::Destroy(void)
{
#if CR_USE_VULKAN
 
    VkDevice device = Sys_GetVulkanDevice();
    if( m_bufferView != nullptr )
    {
        vkDestroyBufferView( device, m_bufferView, nullptr );
        m_bufferView = nullptr;
    }
    
    if( m_bufferHandler != nullptr )
    {
        vkDestroyBuffer( device, m_bufferHandler, nullptr );
        m_bufferHandler = nullptr;
    }

    if( m_bufferMemory != nullptr )
    {
        vkFreeMemory( device, m_bufferMemory, nullptr );
        m_bufferMemory = nullptr;
    }

#else
    
    if ( m_bufferID != 0 )
    {
        glDeleteBuffers( 1, &m_bufferID );
        m_bufferID = 0;
    }

#endif

    m_size = 0;
    m_free = 0;
    m_offset = 0;
    m_map = 0;
}
