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

#ifndef __BUFFER_H__
#define __BUFFER_H__

class crBufferData
{
public:
    crBufferData( void );
    crBufferData( crBufferData* prev, crBufferData* next, const size_t size, const uintptr_t offset, byte* data );
    ~crBufferData( void );

    ID_INLINE const size_t        Size( void ) const { return m_size; }
    ID_INLINE const uintptr_t     Offset( void ) const { return m_offset; }
    ID_INLINE const byte*         Pointer( void ) const { return m_data; }
    ID_INLINE void                Copy( void* data ) const { memcpy( m_data, data, m_size ); }

protected:
    friend class crBuffer;

    ID_INLINE crBufferData* Next( void ) const { return m_next; }
    ID_INLINE crBufferData* Prev( void ) const { return m_prev; }

    ID_INLINE void            UpdateData( const uintptr_t offset, byte* data )
    {
        m_offset = offset;
        m_data = data;
    }
    
    ID_INLINE void            UpdateParents( crBufferData* next, crBufferData* previous )
    {
        m_next = next;
        m_prev = previous;
    }

private:
    size_t          m_size;
    uintptr_t       m_offset;
    byte*           m_data;
    crBufferData*   m_next;
    crBufferData*   m_prev;

    // disable copy constructors
    crBufferData( const crBufferData& ref ){};
    void operator = ( const crBufferData & ref ){};
};

class crBuffer
{
public:
    crBuffer( void );
    ~crBuffer( void );

    void    Create( const size_t size );
    void    Resize( const size_t size );
    void    Destroy( void );

    crBufferData*   Alloc( const size_t bytes );
    void            Free( crBufferData* block );

    size_t  GetSize( void ) const { return m_size; }
    size_t  GetFree( void ) const { return m_free; }
    size_t  GetUsed( void ) const { return m_size - m_free; }

private:
    size_t          m_size;     // buffer size
    size_t          m_free;     // free buffer size
    uintptr_t       m_offset;   // allocation pointer offset 
    byte*           m_map;      // buffer data map
    crBufferData*   m_freeList; // free data blocks 
    crBufferData*   m_usedList; // used data blocks

#if CR_USE_VULKAN
    VkBuffer        m_bufferHandler;
    VkBufferView    m_bufferView;
    VkDeviceMemory  m_bufferMemory;
#else
    GLuint          m_bufferID;
#endif
};

#endif //!__BUFFER_H__