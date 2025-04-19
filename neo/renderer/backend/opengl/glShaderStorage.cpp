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
#include "glShaderStorage.h"

/*
=====================================================================================
crGLShaderStorage
=====================================================================================
*/
crGLShaderStorage::crGLShaderStorage( void ) : 
    crShaderStorage(),
    m_handlers( nullptr )
{
}

crGLShaderStorage::~crGLShaderStorage( void )
{
}

void crGLShaderStorage::StartUp(void)
{
    crAutoPointer<crGLBuffer> vertexUniformSSBO;
    crAutoPointer<crGLBuffer> fragmentUniformSSBO;
    crAutoPointer<crGLBuffer> lightUniformSSBO;
    crAutoPointer<crGLBuffer> textureHandlerSSBO;

    vertexUniformSSBO.New();
    fragmentUniformSSBO.New();
    lightUniformSSBO.New();
    textureHandlerSSBO.New();

    // Create buffers 
    vertexUniformSSBO->Create( UNIFORMS_BUFFER_VERTEX_SIZE ); // Create vertex uniform buffer storage 
    fragmentUniformSSBO->Create( UNIFORMS_BUFFER_FRAGMENT_UNIFORMS_SIZE ); // Create fragment uniform buffer storage 
    lightUniformSSBO->Create( UNIFORMS_BUFFER_LIGHT_UNIFORMS_SIZE ); // Create light uniform buffer stogare
    textureHandlerSSBO->Create( TEXTURE_BUFFER_HANDLES_SIZE ); // Create texture binding buffer 

    // reference the buffer 
    m_vertexUniformSSBO = vertexUniformSSBO.DynamicCast<crBuffer>();
    m_fragmentUniformSSBO = m_fragmentUniformSSBO.DynamicCast<crBuffer>();
    m_lightUniformSSBO = m_lightUniformSSBO.DynamicCast<crBuffer>();
    m_textureHandlerSSBO = m_textureHandlerSSBO.DynamicCast<crBuffer>();


    // reserve temp unifom 
    m_vertexUniform.Alloc( SHADER_VERTEX_BLOCK_SIZE );
    m_fragmentUniform.Alloc( SHADER_FRAGMENT_BLOCK_SIZE );
    m_lightUniform.Alloc( SHADER_LIGHT_BLOCK_SIZE );

    // get texture buffer array
    m_handlers = static_cast<GLuint64*>( m_textureHandlerSSBO->GetMap() );
}

void crGLShaderStorage::ShutDown(void)
{
    // release out copy space 
    if( m_vertexUniform ) 
        m_vertexUniform.Free();

    if( m_fragmentUniform ) 
        m_fragmentUniform.Free();
    
    if( m_lightUniform ) 
        m_lightUniform.Free();   

    // release our buffer
    if( m_lightUniformSSBO )
    { 
        m_lightUniformSSBO->Destroy();
        m_lightUniformSSBO.Delete();
    }

    if( m_fragmentUniformSSBO ) 
    {
        m_fragmentUniformSSBO->Destroy();
        m_fragmentUniformSSBO.Delete();
    }

    if( m_vertexUniformSSBO ) 
    {
        m_vertexUniformSSBO->Destroy();
        m_vertexUniformSSBO.Delete();
    }

    if ( m_textureHandlerSSBO )
    {
        m_textureHandlerSSBO->Destroy();
        m_textureHandlerSSBO.Delete();
    }
}

void crGLShaderStorage::Begin(void)
{
    // 0 texture  
    // 1 vertex 
    // 2 fragement
    // 3 light 
    GLuint      buffers[4];
    GLintptr    offsets[4]; 
    GLsizeiptr  sizes[4];
    
    // buffer handlers
    buffers[0] = m_textureHandlerSSBO.DynamicCast<crGLBuffer>()->GetHandler();
    buffers[1] = m_vertexUniformSSBO.DynamicCast<crGLBuffer>()->GetHandler();
    buffers[2] = m_fragmentUniformSSBO.DynamicCast<crGLBuffer>()->GetHandler();
    buffers[3] = m_lightUniformSSBO.DynamicCast<crGLBuffer>()->GetHandler();
        
    // location offsets s
    offsets[0] = m_frameOffsetTextureHandler;
    offsets[1] = m_frameOffsetVertex;
    offsets[2] = m_frameOffsetFragment;
    offsets[3] = m_frameOffsetLight;
        
    // block size 
    sizes[0] = FRAME_TEXTURE_HANDLE_SIZE;
    sizes[1] = FRAME_UNIFORM_VERTEX_SIZE;
    sizes[2] = FRAME_UNIFORM_FRAGMENT_SIZE;
    sizes[3] = FRAME_UNIFORM_LIGHT_SIZE;
    
    glBindBuffersRange( GL_SHADER_STORAGE_BUFFER, 0, 3, buffers, offsets, sizes );

    // update the copy uniforms 
    m_unformOffsetVertex = m_frameOffsetVertex; 
    m_unformOffsetFragment = m_frameOffsetFragment; 
    m_unformOffsetLight = m_frameOffsetLight; 
    m_currentTextureIndex = m_frameOffsetTextureHandler != 0 ? ( m_frameOffsetTextureHandler / sizeof( GLuint64 ) ) : 0;
}

void crGLShaderStorage::BindTexture(const uint32_t binding, crAutoPointer<crTexture> texture, crAutoPointer<crTextureSampler> sampler)
{
    uint32_t index = 0;
    assert( texture && sampler );

    // texture is already bind, get current index 
    if ( texture->GetBindingIndex() > -1 )
    {
        index = texture->GetBindingIndex();
        SetUniform( &index, FRAGMENT_UNIFORM_LOCATION_SAMPLERS0 + binding );
        return;
    }

    // create texture sampler handler 
    texture->MakeResident( &sampler );

    // get the last offset in the buffer 
    index = m_currentTextureIndex++;

    // set texture handler in the buffer 
    m_handlers[index] = texture.DynamicCast<crGLTexture>()->GetBindingHandler();

    // set the texture as binded 
    texture->SetBinding( index );

    SetUniform( &index, FRAGMENT_UNIFORM_LOCATION_SAMPLERS0 + binding );

    m_bindTextures[m_textureCount++] = texture;
}
