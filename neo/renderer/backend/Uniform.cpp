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
#include "Uniform.h"

//vertex shader storage estructure:
//struct vetexTransform
//{
//  vec4 rpLocalViewOrigin; 
//  vec4 rpColorModulate;   //
//  vec4 rpColorAdd;        //
//  vec4 rpClipBounds;
//  mat4 rpTextureMatrix;
//  mat4 rpModelMatrix;
//  mat4 rpViewMatrix;
//  mat4 rpProjectionMatrix;
//};

// fragment shader storage structure 
//struct fragmentTransfom
//{
//    uint32_t    sampler[8]; // we can acess the max of 8 textures from the sampler buffer 
//    vec4        rpCurrentRenderSize;
//    vec4        rpDiffuseColor;
//    vec4        rpSpecularColor;
//    vec4        shaderParm0;
//    vec4        shaderParm1;
//    vec4        shaderParm2;
//    vec4        shaderParm3;
//};

//interaction shader pass transform
//struct lightTransform
//{
//    vec4 rpLocalLightOrigin;
//    vec4 rpLightProjectionS;
//    vec4 rpLightProjectionT;
//    vec4 rpLightProjectionQ;
//    vec4 rpLightFallOff;
//
//};

// type size_s helpers 
static const uint32_t FLOAT_SIZE = sizeof( float );   
static const uint32_t INT_SIZE = sizeof( int );
static const uint32_t UINT_SIZE = sizeof( uint32_t );
static const uint32_t VEC3F_SIZE = FLOAT_SIZE * 3;
static const uint32_t VEC4F_SIZE = FLOAT_SIZE * 4;
static const uint32_t MAT4X2_SIZE = FLOAT_SIZE * 8;
static const uint32_t MAT4X3_SIZE = FLOAT_SIZE * 12;
static const uint32_t MAX4x4_SIZE = FLOAT_SIZE * 16;

// Vertex Uniforms (buffer vertexStorageBlock)
// Observação: alguns gaps podem existir por alinhamento.
static const uint32_t VERTEX_OFFSET_LOCALVIEWORIGIN    = 0;
static const uint32_t VERTEX_OFFSET_COLORMODULATE      = VEC4F_SIZE;
static const uint32_t VERTEX_OFFSET_COLORADD           = VEC4F_SIZE * 2;
static const uint32_t VERTEX_OFFSET_CLIP_BOUDS         = VEC4F_SIZE * 3;
static const uint32_t VERTEX_OFFSET_TEXMATRIX          = VEC4F_SIZE * 4;
static const uint32_t VERTEX_OFFSET_MODELMATRIX        = VEC4F_SIZE * 8;
static const uint32_t VERTEX_OFFSET_VIEWMATRIX         = VEC4F_SIZE * 12;
static const uint32_t VERTEX_OFFSET_PROJECTIONMATRIX   = VEC4F_SIZE * 16;
static const uint32_t SHADER_VERTEX_BLOCK_SIZE         = VEC4F_SIZE * 20;

// Fragment Uniforms (buffer fragmentStorageBlock)
static const uint32_t FRAG_OFFSET_SAMPLER0             = 0;
static const uint32_t FRAG_OFFSET_SAMPLER1             = UINT_SIZE * 1;
static const uint32_t FRAG_OFFSET_SAMPLER2             = UINT_SIZE * 2;
static const uint32_t FRAG_OFFSET_SAMPLER3             = UINT_SIZE * 3;
static const uint32_t FRAG_OFFSET_SAMPLER4             = UINT_SIZE * 4;
static const uint32_t FRAG_OFFSET_SAMPLER5             = UINT_SIZE * 5;
static const uint32_t FRAG_OFFSET_SAMPLER6             = UINT_SIZE * 6;
static const uint32_t FRAG_OFFSET_SAMPLER7             = UINT_SIZE * 7;
static const uint32_t FRAG_OFFSET_CURRENTRENDERSIZE    = UINT_SIZE * 8;
static const uint32_t FRAG_OFFSET_DIFFUSECOLOR       = FRAG_OFFSET_CURRENTRENDERSIZE + VEC4F_SIZE;
static const uint32_t FRAG_OFFSET_SPECULARCOLOR      = FRAG_OFFSET_CURRENTRENDERSIZE + ( 2 * VEC4F_SIZE );
static const uint32_t FRAG_OFFSET_SHADERPARM0          = FRAG_OFFSET_CURRENTRENDERSIZE + ( 3 * VEC4F_SIZE );
static const uint32_t FRAG_OFFSET_SHADERPARM1          = FRAG_OFFSET_CURRENTRENDERSIZE + ( 4 * VEC4F_SIZE );
static const uint32_t FRAG_OFFSET_SHADERPARM2          = FRAG_OFFSET_CURRENTRENDERSIZE + ( 5 * VEC4F_SIZE );
static const uint32_t FRAG_OFFSET_SHADERPARM3          = FRAG_OFFSET_CURRENTRENDERSIZE + ( 6 * VEC4F_SIZE );
static const uint32_t SHADER_FRAGMENT_BLOCK_SIZE       = ( UINT_SIZE * 8 ) + ( 7 * VEC4F_SIZE );

// Light Uniforms (buffer lightStorageBlock)
static const uint32_t LIGHT_OFFSET_LOCALORIGIN         = 0;
static const uint32_t LIGHT_OFFSET_PROJECTIONS         = VEC4F_SIZE * 1;
static const uint32_t LIGHT_OFFSET_PROJECTIONT         = VEC4F_SIZE * 2;
static const uint32_t LIGHT_OFFSET_PROJECTIONQ         = VEC4F_SIZE * 3;
static const uint32_t LIGHT_OFFSET_FALLOFF             = VEC4F_SIZE * 4;
static const uint32_t SHADER_LIGHT_BLOCK_SIZE          = 5 * VEC4F_SIZE;

static struct uniformLocation_t 
{
    uintptr_t offset;
    size_t    size; 
} 
uniformList[MAX_UNIFORMS] = 
{
    // Vertex Uniforms
    { VERTEX_OFFSET_LOCALVIEWORIGIN,     VEC4F_SIZE },    // 0 - vec4 rpLocalViewOrigin
    { VERTEX_OFFSET_COLORMODULATE,       VEC4F_SIZE },    // 1 - vec4 rpColorModulate
    { VERTEX_OFFSET_COLORADD,            VEC4F_SIZE },    // 2 - vec4 rpColorAdd
    { VERTEX_OFFSET_CLIP_BOUDS,          VEC4F_SIZE },    // 3 - vec4 rpClipBounds
    { VERTEX_OFFSET_TEXMATRIX,           MAX4x4_SIZE },   // 4 - mat4 rpTextureMatrix
    { VERTEX_OFFSET_MODELMATRIX,         MAX4x4_SIZE },   // 5 - mat4 rpModelMatrix
    { VERTEX_OFFSET_VIEWMATRIX,          MAX4x4_SIZE },   // 6 - mat4 rpViewMatrix
    { VERTEX_OFFSET_PROJECTIONMATRIX,    MAX4x4_SIZE },   // 7 - mat4 rpProjectionMatrix

    // Fragment Uniforms
    { FRAG_OFFSET_SAMPLER0,                UINT_SIZE },     // 8  - uint32_t sampler[0]
    { FRAG_OFFSET_SAMPLER1,                UINT_SIZE },     // 9  - uint32_t sampler[1]
    { FRAG_OFFSET_SAMPLER2,                UINT_SIZE },     // 10  - uint32_t sampler[2]
    { FRAG_OFFSET_SAMPLER3,                UINT_SIZE },     // 11 - uint32_t sampler[3]
    { FRAG_OFFSET_SAMPLER4,                UINT_SIZE },     // 12 - uint32_t sampler[4]
    { FRAG_OFFSET_SAMPLER5,                UINT_SIZE },     // 13 - uint32_t sampler[5]
    { FRAG_OFFSET_SAMPLER6,                UINT_SIZE },     // 14 - uint32_t sampler[6]
    { FRAG_OFFSET_SAMPLER7,                UINT_SIZE },     // 15 - uint32_t sampler[7]
    { FRAG_OFFSET_CURRENTRENDERSIZE,       VEC4F_SIZE },    // 16 - vec4 rpCurrentRenderSize
    { FRAG_OFFSET_DIFFUSECOLOR,            VEC4F_SIZE },    // 17 - vec4 rpDiffuseColor
    { FRAG_OFFSET_SPECULARCOLOR,           VEC4F_SIZE },    // 18 - vec4 rpSpecularColor
    { FRAG_OFFSET_SHADERPARM0,             VEC4F_SIZE },    // 19 - vec4 shaderParm0
    { FRAG_OFFSET_SHADERPARM1,             VEC4F_SIZE },    // 20 - vec4 shaderParm1
    { FRAG_OFFSET_SHADERPARM2,             VEC4F_SIZE },    // 21 - vec4 shaderParm2
    { FRAG_OFFSET_SHADERPARM3,             VEC4F_SIZE },    // 22 - vec4 shaderParm3

    // Light Uniforms
    { LIGHT_OFFSET_LOCALORIGIN,            VEC4F_SIZE },    // 23 - vec4 rpLocalLightOrigin
    { LIGHT_OFFSET_PROJECTIONS,            VEC4F_SIZE },    // 24 - vec4 rpLightProjectionS
    { LIGHT_OFFSET_PROJECTIONT,            VEC4F_SIZE },    // 25 - vec4 rpLightProjectionT
    { LIGHT_OFFSET_PROJECTIONQ,            VEC4F_SIZE },    // 26 - vec4 rpLightProjectionQ
    { LIGHT_OFFSET_FALLOFF,                VEC4F_SIZE }     // 27 - vec4 rpLightFallOff
};


// ATTENTION: Must match the definition in the shader.
// 0 is for the texture sampler array 
static const uint32_t SHADER_BUFFER_BINDING_VERTEX_BLOCK = 1;     // layout( std430, binding = 1 ) buffer vertexStorageBlock
static const uint32_t SHADER_BUFFER_BINDING_FRAGMENT_BLOCK = 2;   // layout( std430, binding = 2 ) buffer fragmentStorageBlock
static const uint32_t SHADER_BUFFER_BINDING_LIGHT_BLOCK = 3;      // layout( std430, binding = 3 ) buffer lightStorageBlock

static const size_t   FRAME_UNIFORM_VERTEX_SIZE = SHADER_VERTEX_BLOCK_SIZE * MAX_FRAME_DRAW_CALL;       // ~1,25 mb
static const size_t   FRAME_UNIFORM_FRAGMENT_SIZE = SHADER_FRAGMENT_BLOCK_SIZE * MAX_FRAME_DRAW_CALL;   // ~576 kb
static const size_t   FRAME_UNIFORM_LIGHT_SIZE = SHADER_LIGHT_BLOCK_SIZE * MAX_FRAME_DRAW_CALL;         // ~320 kbb
static const size_t   FRAME_TEXTURE_HANDLE_SIZE = ( sizeof( GLuint64 ) * 8 ) * MAX_FRAME_DRAW_CALL;     // ~256 kb

static const size_t   UNIFORMS_BUFFER_VERTEX_SIZE = FRAME_UNIFORM_VERTEX_SIZE * SMP_FRAMES;
static const size_t   UNIFORMS_BUFFER_FRAGMENT_UNIFORMS_SIZE = FRAME_UNIFORM_FRAGMENT_SIZE * SMP_FRAMES;
static const size_t   UNIFORMS_BUFFER_LIGHT_UNIFORMS_SIZE = FRAME_UNIFORM_LIGHT_SIZE * SMP_FRAMES;
static const size_t   TEXTURE_BUFFER_HANDLES_SIZE = FRAME_TEXTURE_HANDLE_SIZE * SMP_FRAMES;


/*
=====================================================================================
crUniform
=====================================================================================
*/
crUniform::crUniform( void ) :
#if CR_USE_OPENGL
    m_handlers( nullptr ),
#endif
    m_textureCount( 0 ),
    m_currentTextureIndex( 0 ),
    m_unformOffsetVertex( 0 ),
    m_unformOffsetFragment( 0 ),
    m_unformOffsetLight( 0 ),
    m_frameOffsetVertex( 0 ),
    m_frameOffsetFragment( 0 ),
    m_frameOffsetLight( 0 ),
    m_frameOffsetTextureHandler( 0 )
{
}

crUniform::~crUniform( void )
{
}

void crUniform::StartUp(void)
{
    m_vertexUniformSSBO.New( crBuffer() );
    m_fragmentUniformSSBO.New( crBuffer() );
    m_lightUniformSSBO.New( crBuffer() );

    // Create buffers 
    m_vertexUniformSSBO->Create( UNIFORMS_BUFFER_VERTEX_SIZE ); // Create vertex uniform buffer storage 
    m_fragmentUniformSSBO->Create( UNIFORMS_BUFFER_FRAGMENT_UNIFORMS_SIZE ); // Create fragment uniform buffer storage 
    m_lightUniformSSBO->Create( UNIFORMS_BUFFER_LIGHT_UNIFORMS_SIZE ); // Create light uniform buffer stogare
    m_textureHandlerSSBO->Create( TEXTURE_BUFFER_HANDLES_SIZE ); // Create texture binding buffer 

    // reserve temp unifom 
    m_vertexUniform.Alloc( SHADER_VERTEX_BLOCK_SIZE );
    m_fragmentUniform.Alloc( SHADER_FRAGMENT_BLOCK_SIZE );
    m_lightUniform.Alloc( SHADER_LIGHT_BLOCK_SIZE );

    // get texture buffer array
    m_handlers = static_cast<GLuint64*>( m_textureHandlerSSBO->GetMap() );
}

void crUniform::ShutDown(void)
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
        m_lightUniformSSBO.Delete();

    if( m_fragmentUniformSSBO ) 
        m_fragmentUniformSSBO.Delete();

    if( m_vertexUniformSSBO ) 
        m_vertexUniformSSBO.Delete();
}

void crUniform::Begin(void)
{
#if CR_USE_VULKAN
#elif CR_USE_OPENGL
    // 0 texture  
    // 1 vertex 
    // 2 fragement
    // 3 light 
    GLuint      buffers[4];
    GLintptr    offsets[4]; 
    GLsizeiptr  sizes[4];
    
    // buffer handlers
    buffers[0] = m_textureHandlerSSBO->GetHandler();
    buffers[1] = m_vertexUniformSSBO->GetHandler();
    buffers[2] = m_fragmentUniformSSBO->GetHandler();
    buffers[3] = m_lightUniformSSBO->GetHandler();
        
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
#endif
}

void crUniform::End(void)
{
    
    // update buffer positions ( swap buffers )
    m_frameOffsetTextureHandler = ( m_frameOffsetTextureHandler + FRAME_TEXTURE_HANDLE_SIZE ) % TEXTURE_BUFFER_HANDLES_SIZE;
    m_frameOffsetVertex = ( m_frameOffsetVertex + FRAME_UNIFORM_VERTEX_SIZE ) % UNIFORMS_BUFFER_VERTEX_SIZE;
    m_frameOffsetFragment = ( m_frameOffsetFragment + FRAME_UNIFORM_FRAGMENT_SIZE ) % UNIFORMS_BUFFER_FRAGMENT_UNIFORMS_SIZE;
    m_frameOffsetLight = ( m_frameOffsetLight + FRAME_UNIFORM_LIGHT_SIZE ) % UNIFORMS_BUFFER_LIGHT_UNIFORMS_SIZE;

    // release textures 
    for ( uint32_t i = 0; i < m_textureCount; i++)
    {
        // remove texture binding 
        m_bindTextures[i]->SetBinding( -1 );
        m_bindTextures[i] = nullptr;
    }
    
    m_textureCount = 0;
    m_currentTextureIndex = 0;
}

void crUniform::BindTexture(const uint32_t binding, crAutoPointer<crTexture> texture, crAutoPointer<crTextureSampler> sampler)
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
    m_handlers[index] = texture->GetBindingHandler();

    // set the texture as binded 
    texture->SetBinding( index );

    m_bindTextures[m_textureCount++] = texture;
}

void crUniform::SetUniform( const void *uniform, const uint32_t location )
{
    if ( location < FRAGMENT_UNIFORM_LOCATION_SAMPLERS0 )
        m_vertexUniform.Memcpy( uniform, uniformList[location].offset, uniformList->size );
    else if ( location < LIGHT_UNIFORM_LOCATION_LOCAL_ORIGIN )
        m_fragmentUniform.Memcpy( uniform, uniformList[location].offset, uniformList->size );
    else if( location < MAX_UNIFORMS )
        m_lightUniform.Memcpy( uniform, uniformList[location].offset, uniformList->size );
}

void crUniform::Submit(void)
{
    // copy current uniform to our buffer 
    m_vertexUniformSSBO->Upload( &m_vertexUniform, m_unformOffsetVertex, SHADER_VERTEX_BLOCK_SIZE );
    m_fragmentUniformSSBO->Upload( &m_fragmentUniform, m_unformOffsetFragment, SHADER_FRAGMENT_BLOCK_SIZE );
    m_lightUniformSSBO->Upload( &m_lightUniform, m_unformOffsetLight, SHADER_LIGHT_BLOCK_SIZE );    

    // Move copy offsets
    m_unformOffsetVertex += SHADER_VERTEX_BLOCK_SIZE;
    m_unformOffsetFragment += SHADER_FRAGMENT_BLOCK_SIZE;
    m_unformOffsetLight += SHADER_LIGHT_BLOCK_SIZE;    
}
