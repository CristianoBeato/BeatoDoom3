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

#ifndef __GL_BUFFER_H__
#define __GL_BUFFER_H__

class crGLBuffer : public crBuffer
{
public:
    crGLBuffer( void );
    ~crGLBuffer( void );

    virtual bool        Create( const bufferUsage_t usage, const size_t size, const bool write = true, const bool read = false );
    virtual void        Destroy( void );
    virtual void        Flush( const uintptr_t offset,  const size_t size ) const;
    GLuint              GetHandler( void ) const { return m_buffer; }

private:
    GLuint  m_buffer;
};

#endif //__GL_BUFFER_H__