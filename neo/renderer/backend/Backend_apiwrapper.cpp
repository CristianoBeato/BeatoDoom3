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
#include "Backend_apiwrapper.h"

/*
===========================================================================
crBuffer
===========================================================================
*/
crBuffer::crBuffer(void) :
    m_size( 0 ),
    m_map( nullptr )
{
}

void crBuffer::Upload( const void *data, const uintptr_t offset, const size_t size ) const
{
    ::memcpy( reinterpret_cast<void*>( reinterpret_cast<uintptr_t>( m_map ) + offset ), data, size );
}

void crBuffer::Download( void *data, const uintptr_t offset, const size_t size ) const
{
    ::memcpy( data, reinterpret_cast<void*>( reinterpret_cast<uintptr_t>( m_map ) + offset ), size );
}

/*
===========================================================================
crTexture
===========================================================================
*/
crTexture::crTexture(void) :     
    m_width( 0 ),
    m_height( 0 ),
    m_depth( 0 ),
    m_layers( 0 ),
    m_mipcount( 0 ),
    m_bindindex( 0 ),
    m_samples( 0 )
{
}

/*
===========================================================================
crShaderStorage
===========================================================================
*/

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
const size_t SHADER_VERTEX_BLOCK_SIZE         = VEC4F_SIZE * 20;

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
const size_t SHADER_FRAGMENT_BLOCK_SIZE       = ( UINT_SIZE * 8 ) + ( 7 * VEC4F_SIZE );

// Light Uniforms (buffer lightStorageBlock)
static const uint32_t LIGHT_OFFSET_LOCALORIGIN         = 0;
static const uint32_t LIGHT_OFFSET_PROJECTIONS         = VEC4F_SIZE * 1;
static const uint32_t LIGHT_OFFSET_PROJECTIONT         = VEC4F_SIZE * 2;
static const uint32_t LIGHT_OFFSET_PROJECTIONQ         = VEC4F_SIZE * 3;
static const uint32_t LIGHT_OFFSET_FALLOFF             = VEC4F_SIZE * 4;
const size_t SHADER_LIGHT_BLOCK_SIZE          = 5 * VEC4F_SIZE;

const size_t   FRAME_UNIFORM_VERTEX_SIZE = SHADER_VERTEX_BLOCK_SIZE * MAX_FRAME_DRAW_CALL;       // ~1,25 mb
const size_t   FRAME_UNIFORM_FRAGMENT_SIZE = SHADER_FRAGMENT_BLOCK_SIZE * MAX_FRAME_DRAW_CALL;   // ~576 kb
const size_t   FRAME_UNIFORM_LIGHT_SIZE = SHADER_LIGHT_BLOCK_SIZE * MAX_FRAME_DRAW_CALL;         // ~320 kbb
const size_t   FRAME_TEXTURE_HANDLE_SIZE = ( sizeof( GLuint64 ) * 8 ) * MAX_FRAME_DRAW_CALL;     // ~256 kb

const size_t   UNIFORMS_BUFFER_VERTEX_SIZE = FRAME_UNIFORM_VERTEX_SIZE * SMP_FRAMES;
const size_t   UNIFORMS_BUFFER_FRAGMENT_UNIFORMS_SIZE = FRAME_UNIFORM_FRAGMENT_SIZE * SMP_FRAMES;
const size_t   UNIFORMS_BUFFER_LIGHT_UNIFORMS_SIZE = FRAME_UNIFORM_LIGHT_SIZE * SMP_FRAMES;
const size_t   TEXTURE_BUFFER_HANDLES_SIZE = FRAME_TEXTURE_HANDLE_SIZE * SMP_FRAMES;

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

crShaderStorage::crShaderStorage(void) :
    m_currentTextureIndex( 0 ),
    m_textureCount( 0 ),
    m_unformOffsetVertex( 0 ),
    m_unformOffsetFragment( 0 ),
    m_unformOffsetLight( 0 ),
    m_frameOffsetVertex( 0 ),
    m_frameOffsetFragment( 0 ),
    m_frameOffsetLight( 0 ),
    m_frameOffsetTextureHandler( 0 )
{
}

void crShaderStorage::End(void)
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

void crShaderStorage::SetUniform(const void *uniform, const uint32_t location)
{
    if ( location < FRAGMENT_UNIFORM_LOCATION_SAMPLERS0 )
        m_vertexUniform.Memcpy( uniform, uniformList[location].offset, uniformList->size );
    else if ( location < LIGHT_UNIFORM_LOCATION_LOCAL_ORIGIN )
        m_fragmentUniform.Memcpy( uniform, uniformList[location].offset, uniformList->size );
    else if( location < MAX_UNIFORMS )
        m_lightUniform.Memcpy( uniform, uniformList[location].offset, uniformList->size );
}

void crShaderStorage::Submit(void)
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

/*
===========================================================================
crSwapChain
===========================================================================
*/
crSwapChain::crSwapChain( void ) : 
    m_imageCount( 0 ),
    m_currentFrame( 0 ),
    m_width( 0 ),
    m_height( 0 )
{
}
