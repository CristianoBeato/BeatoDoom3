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

#include "precompiled.h"
#pragma hdrstop

#include "renderer/renderer_common.h"
#include "Texture.h"

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

uint32_t internalFormat_t::GetFormat( void ) const
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
}

uint32_t internalFormat_t::GetDataType(void) const
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
}

crTexture::crTexture( void )
{
    m_width = 0;
    m_height = 0;
    m_depth = 0;
    m_layers = 0;
    m_mipcount = 0;
    
#if CR_USE_VULKAN
    m_image = VK_NULL_HANDLE;
    m_memory = VK_NULL_HANDLE;
    m_imageView = VK_NULL_HANDLE;
#else CR_USE_OPENGL
    m_texture = 0;
    m_target = 0;
    m_format = 0;
#endif // CR_USE_OPENGL

}

crTexture::~crTexture( void )
{
    Destroy();
}

bool crTexture::Create( const uint32_t width, const uint32_t height, const uint32_t depth, const uint32_t layers, const uint32_t mips, const GLenum format, const uint32_t type )
{
    m_width = width;
    m_height = height;
    m_depth = depth;
    m_layers = layers;
    m_mipcount = mips;

#if CR_USE_VULKAN
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = format;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = depth;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if ( vkCreateImage( m_device, &imageInfo, nullptr, &m_image ) != VK_SUCCESS )
    {
        common->Printf( "Failed to create texture image!\n" );
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements( m_device, m_image, &memRequirements );
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType( memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
    if ( vkAllocateMemory( m_device, &allocInfo, nullptr, &m_memory ) != VK_SUCCESS )
    {
        common->Printf( "Failed to allocate texture image memory!\n" );
        return false;
    }

    vkBindImageMemory( m_device, m_image, m_memory, 0 );
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    if ( vkCreateImageView( m_device, &viewInfo, nullptr, &m_imageView ) != VK_SUCCESS )
    {
        common->Printf( "Failed to create texture image view!\n" );
        return false;
    }

#elif CR_USE_OPENGL

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
        break;
    }

#endif // CR_USE_OPENGL
    return true;
}

void crTexture::CopyBufferToImage(const crBuffer *buffer, const uint32_t rowLength, const bufferTextureRect_t *imageMap, const uint32_t count )
{
#if CR_USE_VULKAN
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;    // 0 = sem padding, tightly packed
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = { texWidth, texHeight, 1 };

    // texture copy command 
    vkCmdCopyBufferToImage( commandBuffer, stagingBuffer, textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, count, &region );
#elif CR_USE_OPENGL
    GLint currentBuffer = 0;
    glGetIntegerv( GL_PIXEL_UNPACK_BUFFER_BINDING, &currentBuffer );

    GLuint bufferhnd = buffer->GetHandler();
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

#endif // CR_USE_OPENGL
}

crTextureSampler::crTextureSampler( void )
{
#if CR_USE_VULKAN
    m_sampler = VK_NULL_HANDLE;
#elif CR_USE_OPENGL
    m_sampler = 0;
#endif // CR_USE_OPENGL
}

crTextureSampler::~crTextureSampler( void )
{
    Destroy();
}

bool crTextureSampler::Create(const uint32_t minFilter, const uint32_t magFilter, const uint32_t wrapS, const uint32_t wrapT, const float anisotropicLevel)
{
#if CR_USE_VULKAN
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = magFilter;
    samplerInfo.minFilter = minFilter;
    samplerInfo.addressModeU = wrapS;
    samplerInfo.addressModeV = wrapT;
    samplerInfo.addressModeW = wrapS;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = anisotropicLevel;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    if ( vkCreateSampler( m_device, &samplerInfo, nullptr, &m_sampler ) != VK_SUCCESS )
    {
        common->Printf( "Failed to create texture sampler!\n" );
        return false;
    }
#elif CR_USE_OPENGL
    glCreateSamplers( 1, &m_sampler );
    glSamplerParameteri( m_sampler, GL_TEXTURE_MIN_FILTER, minFilter );
    glSamplerParameteri( m_sampler, GL_TEXTURE_MAG_FILTER, magFilter );
    glSamplerParameteri( m_sampler, GL_TEXTURE_WRAP_S, wrapS );
    glSamplerParameteri( m_sampler, GL_TEXTURE_WRAP_T, wrapT );
    glSamplerParameteri( m_sampler, GL_TEXTURE_WRAP_R, wrapS );
    glSamplerParameterf( m_sampler, GL_TEXTURE_MAX_ANISOTROPY, anisotropicLevel );
    glSamplerParameteri( m_sampler, GL_TEXTURE_COMPARE_MODE, GL_NONE );
    glSamplerParameteri( m_sampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
    glSamplerParameteri( m_sampler, GL_TEXTURE_BORDER_COLOR, 0xFFFFFFFF );
    glSamplerParameteri( m_sampler, GL_TEXTURE_MIN_LOD, 0 );
    glSamplerParameteri( m_sampler, GL_TEXTURE_MAX_LOD, 0 );
    glSamplerParameteri( m_sampler, GL_TEXTURE_LOD_BIAS, 0 );
    glSamplerParameteri( m_sampler, GL_TEXTURE_BASE_LEVEL, 0 );
    glSamplerParameteri( m_sampler, GL_TEXTURE_MAX_LEVEL, 0 );
#endif // CR_USE_OPENGL
    return true;
}

void crTextureSampler::Destroy(void)
{
#if CR_USE_VULKAN
    if ( m_sampler != VK_NULL_HANDLE )
    {
        vkDestroySampler( m_device, m_sampler, nullptr );
        m_sampler = VK_NULL_HANDLE;
    }
#elif CR_USE_OPENGL
    if ( m_sampler != 0 )
    {
        glDeleteSamplers( 1, &m_sampler );
        m_sampler = 0;
    }
#endif // CR_USE_OPENGL
}

