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

class crBuffer
{
public:
    crBuffer( void );
    ~crBuffer( void );

    /// @brief create buffer object and reserve size 
    /// @param size of the buffer 
    /// @return true on sucess 
    bool        Create( const size_t size, const bool write = true, const bool read = false );
    
    /// @brief destroy object and release memory
    /// @param  
    void        Destroy( void );

    /// @brief Upload buffer data
    /// @param data pointer to by copy
    /// @param offset of the data to copy 
    /// @param size of the memory to be copied
    void        Upload( const void* data, const uintptr_t offset, const size_t size ) const;

    /// @brief copy data from buffer to pointer 
    /// @param data location to copy 
    /// @param offset of the buffer to copy
    /// @param size size to copy 
    void        Download( void* data, const uintptr_t offset, const size_t size ) const;

    /// @brief buffer raw pointer
    /// @return 
    void*       GetMap( void ) const { return m_map; }

    /// @brief buffer total size 
    /// @return size of the buffer 
    size_t      GetSize( void ) const { return m_size; }

#if CR_USE_VULKAN
    VKBuffer    GetHandler( void ) const { return m_buffer; }
#elif CR_USE_OPENGL
    GLuint      GetHandler( void ) const { return m_buffer; }
#endif    

private:
    size_t  m_size;
    void*   m_map;
#if CR_USE_VULKAN
#elif CR_USE_OPENGL
    GLuint  m_buffer;
#endif
};

#endif //__BUFFER_H__