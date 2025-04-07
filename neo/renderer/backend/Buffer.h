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