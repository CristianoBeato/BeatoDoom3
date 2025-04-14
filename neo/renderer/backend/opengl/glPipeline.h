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

#ifndef __GL_RENDERPIPE_H__
#define __GL_RENDERPIPE_H__

class crGLPipeline : public crPipeline
{
public:
    crGLPipeline( void );
    ~crGLPipeline( void );

    virtual void    Create( const shaderProgram_t* program, const vertexAttribute_t* attributes, const size_t numAttributes );
    virtual void    Destroy( void );
    virtual void    Begin( void );
    virtual void    End( void );
    virtual void    AttachVertexBuffer( crBuffer* buffer, uintptr_t offset, const size_t size );
    virtual void    AttachIndexBuffer( crBuffer* buffer, uintptr_t offset, const size_t size );
    virtual void    AttachUniformBuffer( crBuffer* buffer, const uint32_t bindingID, uintptr_t offset, const size_t size );
    virtual void    SetViewport( int x, int y, int width, int height );
    virtual void    SetScissor( int x, int y, int width, int height );
    virtual void    ClearColor( const float red, const float green, const float blue, const float alpha );

private:
    GLuint            m_programPipeline;
    GLuint            m_vertexArrayObject;
    GLuint            m_shaderStages[SHADER_STAGE_MAX];

    void    CreateVertexPipeline( const vertexAttribute_t* attributes, const size_t numAttributes );
    void    DestroyVertexPipeline( void );
    void    CreateShaderProgram( const shaderProgram_t* program );
    void    DestroyShaderProgram( void );
};

#endif // __GL_RENDERPIPE_H__