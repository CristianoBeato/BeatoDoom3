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
#include "glBuffer.h"
#include "glTexture.h"

// OpenGL
// internal formats suported (OpenGL 4.6) 

// - COMMON
// GL_RGB8
// GL_RGBA8
// GL_RGB10_A2
// GL_SRGB8
// GL_SRGB8_ALPHA8
// GL_R8
// GL_RG8
// GL_R16
// GL_RG16
// GL_RGBA4
// GL_RGB5_A1

// - FLOAT
// GL_R16F
// GL_RG16F
// GL_RGB16F
// GL_RGBA16F
// GL_R32F
// GL_RG32F
// GL_RGB32F
// GL_RGBA32F
// GL_R11F_G11F_B10F

// - INTEGERS (SIGNED / UNSIGNED) ==
// GL_R8I
// GL_R8UI
// GL_RG8I
// GL_RG8UI
// GL_RGBA8I
// GL_RGBA8UI
// GL_R16I
// GL_R16UI
// GL_RG16I
// GL_RG16UI
// GL_RGBA16I
// GL_RGBA16UI
// GL_R32I
// GL_R32UI
// GL_RG32I
// GL_RG32UI
// GL_RGBA32I
// GL_RGBA32UI

// FORMATOS DEPTH / STENCIL
// GL_DEPTH_COMPONENT16
// GL_DEPTH_COMPONENT24
// GL_DEPTH_COMPONENT32F
// GL_DEPTH24_STENCIL8
// GL_DEPTH32F_STENCIL8

// ESPECIAL
// GL_RGB9_E5                // RGB compacto (9+9+9+5 bits)
// GL_RGB10_A2UI             // RGBA inteiro com 10+10+10+2 bits

uint32_t glInternalFormat_t::GetFormat( void ) const
{
    switch ( format )
    {
        // RED component
        case GL_R8:
        case GL_R8I:
        case GL_R8UI:
        case GL_R16:
        case GL_R16F:
        case GL_R16I:
        case GL_R16UI:
        case GL_R32F:
        case GL_R32I:
        case GL_R32UI:
            return GL_R;

        // RED GREEN component 
        case GL_RG8:
        case GL_RG8I:
        case GL_RG8UI:
        case GL_RG16:
        case GL_RG16I:
        case GL_RG16UI:
        case GL_RG16F:
        case GL_RG32F:
        case GL_RG32I:
        case GL_RG32UI:
            return GL_RG;
        
        // RED GREEN BLUE
        case GL_RGB8:
        case GL_SRGB8:
        case GL_RGB16F:
        case GL_RGB32F:
        case GL_RGB9_E5:                // RGB compacto (9+9+9+5 bits)
            return GL_RGB;

        // RED GREEN BLUE ALPHA
        case GL_RGBA4:
        case GL_RGBA8:
        case GL_RGBA8I:
        case GL_RGBA8UI:
        case GL_RGBA16I:
        case GL_RGBA16UI:
        case GL_RGBA16F:
        case GL_RGBA32I:
        case GL_RGBA32UI:
        case GL_RGBA32F:
        case GL_SRGB8_ALPHA8:
        case GL_RGB5_A1:
        case GL_RGB10_A2:
        case GL_R11F_G11F_B10F:
        case GL_RGB10_A2UI:             // RGBA inteiro com 10+10+10+2 bits
            return GL_RGBA;

        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32F:
            return GL_DEPTH;
        
        case GL_DEPTH24_STENCIL8:
        case GL_DEPTH32F_STENCIL8:
            return GL_DEPTH_STENCIL;
        
        default:
        assert( !"invalid format!" );
        break;
    }

    return GL_NONE;
}

GLenum glInternalFormat_t::GetDataType(void) const
{
    switch ( format )
    {
        // int 8
        case GL_R8:
        case GL_RG8:
        case GL_R16:
        case GL_RGB8:
        case GL_RGBA8:
        case GL_SRGB8:
        case GL_SRGB8_ALPHA8:
        case GL_R8I:
        case GL_RG8I:
        case GL_RGBA8I:
            return GL_BYTE;

        // unsigned int 8
        case GL_R8UI:
        case GL_RG8UI:
        case GL_RGBA8UI:
            return GL_UNSIGNED_BYTE;

        // int16
        case GL_RGBA4:
        case GL_RG16:
        case GL_R16I:
        case GL_RG16I:
        case GL_RGBA16I:
        case GL_DEPTH_COMPONENT16:
            return GL_SHORT;

        // unsigned int16
        case GL_R16UI:
        case GL_RG16UI:
        case GL_RGBA16UI:
            return GL_UNSIGNED_SHORT;

        // int
        case GL_R32I:
        case GL_RG32I:
        case GL_RGBA32I:
        return GL_INT;
        
        // unsigned int 
        case GL_R32UI:
        case GL_RG32UI:
        case GL_RGBA32UI:
            return GL_UNSIGNED_INT;

        // half float ( float16 )
        case GL_R16F:
        case GL_RG16F:
        case GL_RGB16F:
        case GL_RGBA16F:
            return GL_HALF_FLOAT;
        
        // float 32
        case GL_R32F:
        case GL_RG32F:
        case GL_RGB32F:
        case GL_RGBA32F:
        case GL_DEPTH_COMPONENT32F:
            return GL_FLOAT;
        
        case GL_RGB10_A2:
            return GL_INT_2_10_10_10_REV;

        case GL_RGB10_A2UI:             // RGBA inteiro com 10+10+10+2 bits
            return GL_UNSIGNED_INT_10_10_10_2;

        case GL_R11F_G11F_B10F:
            return GL_UNSIGNED_INT_10F_11F_11F_REV;
        
        case GL_RGB5_A1:
            return GL_UNSIGNED_SHORT_5_5_5_1;

        case GL_RGB9_E5:                // RGB compacto (9+9+9+5 bits)
            return GL_UNSIGNED_INT_5_9_9_9_REV;

        case GL_DEPTH_COMPONENT24:
            return GL_UNSIGNED_INT;
        
        case GL_DEPTH24_STENCIL8:
            return GL_UNSIGNED_INT_24_8;

        case GL_DEPTH32F_STENCIL8:
            return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
    
    default:
        assert( !"invalid format" );
        break;
    }

    return GL_NONE;
}

static GLenum GLTextureTypeTarget( const uint32_t type )
{
    GLenum target = GL_NONE;

    switch ( type )
    {
    case TEXTURE_TYPE_1D:
        target = GL_TEXTURE_1D;
        break;
    case TEXTURE_TYPE_1D_ARRAY:
        target = GL_TEXTURE_1D_ARRAY;
        break;
    case TEXTURE_TYPE_2D:
        target = GL_TEXTURE_2D;
        break;
    case TEXTURE_TYPE_2D_ARRAY:
        target = GL_TEXTURE_2D_ARRAY;
        break;
    case TEXTURE_TYPE_3D:
        target = GL_TEXTURE_3D;
        break;
    case TEXTURE_TYPE_CUBE:
        target = GL_TEXTURE_CUBE_MAP;
        break;
    case TEXTURE_TYPE_CUBE_ARRAY:
        target = GL_TEXTURE_CUBE_MAP_ARRAY;
        break;
    default:
        // TODO: Cast a error 
        break;
    }

    return target;
}

static const GLenum GLTextureFormat( uint32_t format )
{
    GLenum internalFormat = GL_NONE;

    return internalFormat;
}

/*
===========================================================================
crGLTexture
===========================================================================
*/
crGLTexture::crGLTexture( void ) : 
    crTexture(),
    m_bindingHandler( 0 ),
    m_texture( 0 ),
    m_target( 0 ),
    m_format( 0 )
{
}

crGLTexture::~crGLTexture( void )
{
    Destroy();
}

bool crGLTexture::Create( const uint32_t width, const uint32_t height, const uint32_t depth, const uint32_t layers, const uint32_t mips, const GLenum format, const uint32_t type )
{
    m_width = width;
    m_height = height;
    m_depth = depth;
    m_layers = layers;
    m_mipcount = mips;
    m_target = GLTextureTypeTarget( type );
    m_format = GLTextureFormat( format );

    //todo: check multisamples and cubemaps
    if ( m_height != 0 )
    {
        if ( m_depth != 0 )
            m_target = GL_TEXTURE_3D;
        else
            m_target = m_layers > 0 ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;        
    }
    else
        m_target = m_layers > 0 ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
    
    // create the texture handler 
    glCreateTextures( m_target, 1, &m_texture );

    // allocate the texture memory
    switch ( m_target )
    {
    case GL_TEXTURE_1D:
        glTextureStorage1D( m_texture, m_mipcount, m_format.GetFormat(), width );
        break;
    case GL_TEXTURE_1D_ARRAY:
        glTextureStorage2D( m_texture, m_mipcount, m_format.GetFormat(), width, m_layers );
        break;
    case GL_TEXTURE_2D:
        glTextureStorage2D( m_texture, m_mipcount, m_format.GetFormat(), width, height );
        break;
    case GL_TEXTURE_3D:
        glTextureStorage3D( m_texture, m_mipcount, m_format.GetFormat(), m_width, m_height, m_depth );
        break;
    case GL_TEXTURE_2D_ARRAY:
        glTextureStorage3D( m_texture, m_mipcount, m_format.GetFormat(), m_width, m_height, m_depth );
        break;
    case GL_TEXTURE_CUBE_MAP:
        glTextureStorage3D( m_texture, m_width, m_format.GetFormat(), m_width, m_width, 6 );
        break;
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        glTextureStorage3D( m_texture, m_width, m_format.GetFormat(), m_width, m_width, 6 * layers );
        break;
    default:
        // TODO: cast a error
        return false;
    }

    return true;
}

void crGLTexture::CopyBufferToImage(const crBuffer *buffer, const uint32_t rowLength, const bufferTextureRect_t *imageMap, const uint32_t count )
{
    GLint currentBuffer = 0;
    glGetIntegerv( GL_PIXEL_UNPACK_BUFFER_BINDING, &currentBuffer );

    GLuint bufferhnd = dynamic_cast<const crGLBuffer*>(buffer)->GetHandler();
    if ( currentBuffer == bufferhnd )
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, bufferhnd );

    glPixelStorei(GL_UNPACK_ALIGNMENT, rowLength ); // if data is unaligned 

    GLenum format = m_format.GetFormat();
    GLenum type = m_format.GetDataType();
    for ( uint32_t i = 0; i < count; i++)
    {
        bufferTextureRect_t region = imageMap[i];
        switch ( m_target )
        {
        case GL_TEXTURE_1D: 
            glTextureSubImage1D( m_texture, region.mipLevel, region.xOffset, region.width, format, type, reinterpret_cast<const void*>( region.bufferOffset ) );        
            break;
        case GL_TEXTURE_1D_ARRAY: 
            glTextureSubImage2D( m_texture, region.mipLevel, region.xOffset, 0, region.width, region.layerLevel, format, type, reinterpret_cast<const void*>( region.bufferOffset ) );
            break;
        case GL_TEXTURE_2D: 
            glTextureSubImage2D( m_texture, region.mipLevel, region.xOffset, region.yOffset, region.width, region.height, format, type, reinterpret_cast<const void*>( region.bufferOffset ) );
            break;
        case GL_TEXTURE_2D_ARRAY: 
            glTextureSubImage3D( m_texture, region.mipLevel, region.xOffset, region.yOffset, 0, region.width, region.height, region.layerLevel, format, type, reinterpret_cast<const void*>( region.bufferOffset ) );            
            break;
        case GL_TEXTURE_3D: 
            glTextureSubImage3D( m_texture, region.mipLevel, region.xOffset, region.yOffset, region.zOffset, region.width, region.height, region.depth, format, type, reinterpret_cast<const void*>( region.bufferOffset ) );        
            break;
        case GL_TEXTURE_CUBE_MAP: 
        case GL_TEXTURE_CUBE_MAP_ARRAY: 
            glTextureSubImage3D( m_texture, region.mipLevel, region.xOffset, region.yOffset, 0, region.width, region.height, region.layerLevel, format, type, reinterpret_cast<const void*>( region.bufferOffset ) );
            break;
        default:
            // TODO: cast a error
            break;
        }
    }
    // restaure bind buffer 
    if ( currentBuffer == bufferhnd )
        glBindBuffer( GL_PIXEL_UNPACK_BUFFER, currentBuffer );
}

void crGLTexture::MakeResident( const crGLTextureSampler* sampler )
{
    if( m_bindingHandler == 0 )
        m_bindingHandler = glGetTextureSamplerHandleARB( m_texture, sampler->GetHandler() );
}

void crGLTexture::Unmakeresident(void)
{
    if( m_bindingHandler != 0 )
        glMakeTextureHandleNonResidentARB( m_bindingHandler );
}

/*
===========================================================================
crGLTextureSampler
===========================================================================
*/
crGLTextureSampler::crGLTextureSampler( void ) : m_sampler( 0 )
{
}

crGLTextureSampler::~crGLTextureSampler( void )
{
    Destroy();
}

bool crGLTextureSampler::Create(const uint32_t minFilter, const uint32_t magFilter, const uint32_t wrapS, const uint32_t wrapT, const float anisotropicLevel, const float LODBias )
{
    glCreateSamplers( 1, &m_sampler );
 
    // Magnification/Minification filtering 
    glSamplerParameteri( m_sampler, GL_TEXTURE_MIN_FILTER, minFilter );
    glSamplerParameteri( m_sampler, GL_TEXTURE_MAG_FILTER, magFilter );

    // Anisostropic filtering 
    glSamplerParameterf( m_sampler, GL_TEXTURE_MAX_ANISOTROPY, anisotropicLevel );

    // Repeating
    glSamplerParameteri( m_sampler, GL_TEXTURE_WRAP_S, wrapS );
    glSamplerParameteri( m_sampler, GL_TEXTURE_WRAP_T, wrapT );
    glSamplerParameteri( m_sampler, GL_TEXTURE_WRAP_R, wrapS );

    // Mipmaping 
    glSamplerParameterf( m_sampler, GL_TEXTURE_MIN_LOD, 0.0f );
    glSamplerParameterf( m_sampler, GL_TEXTURE_MAX_LOD, 1.0f );
    glSamplerParameterf( m_sampler, GL_TEXTURE_LOD_BIAS, LODBias );

    // texture compare 
    glSamplerParameteri( m_sampler, GL_TEXTURE_COMPARE_MODE, GL_NONE );
    glSamplerParameteri( m_sampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );

    //
    glSamplerParameteri( m_sampler, GL_TEXTURE_BORDER_COLOR, 0xFFFFFFFF );
    glSamplerParameteri( m_sampler, GL_TEXTURE_BASE_LEVEL, 0 );
    glSamplerParameteri( m_sampler, GL_TEXTURE_MAX_LEVEL, 0 );
    
    return true;
}

void crGLTextureSampler::Destroy(void)
{
    if ( m_sampler != 0 )
    {
        glDeleteSamplers( 1, &m_sampler );
        m_sampler = 0;
    }
}
