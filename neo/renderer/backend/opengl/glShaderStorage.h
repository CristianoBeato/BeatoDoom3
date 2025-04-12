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

#ifndef __GL_SHADER_STORAGE_H__
#define __GL_SHADER_STORAGE_H__

/// @brief 
class crGLShaderStorage : public crShaderStorage
{
public:
    crGLShaderStorage( void );
    ~crGLShaderStorage( void );    
    void    StartUp( void );
    void    ShutDown( void );
    void    Begin( void );
    void    BindTexture( const uint32_t binding, crAutoPointer<crTexture> texture, crAutoPointer<crTextureSampler> sampler );
        
private:
    GLuint64*                           m_handlers;         // acess diret from buffer 
};

#endif //__UNIFORM_H__