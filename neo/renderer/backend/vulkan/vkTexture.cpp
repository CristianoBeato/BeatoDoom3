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
#include "qvk.h"
#include "vkTexture.h"

static VkImageType vkTextureTypeVulkan( const uint32_t type )
{
    VkImageType vkType = VK_IMAGE_TYPE_1D;
    switch ( type )
    {
    case TEXTURE_TYPE_1D:
        vkType = VK_IMAGE_TYPE_1D;
        break;
    case TEXTURE_TYPE_1D_ARRAY:
        vkType = VK_IMAGE_TYPE_2D;
        break;
    case TEXTURE_TYPE_2D:
        vkType = VK_IMAGE_TYPE_2D;
        break;
    case TEXTURE_TYPE_2D_ARRAY:
        vkType = VK_IMAGE_TYPE_3D;
        break;
    case TEXTURE_TYPE_3D:
        vkType = VK_IMAGE_TYPE_3D;
        break;
    case TEXTURE_TYPE_CUBE:
        vkType = VK_IMAGE_TYPE_3D;
        break;
    case TEXTURE_TYPE_CUBE_ARRAY:
        vkType = VK_IMAGE_TYPE_3D;
        break;
    default:
        // todo cast a error 
        break;
    }
    return vkType;
}

static VkImageViewType vkViewTextureTypeVulkan( const uint32_t type )
{
    VkImageViewType viewType;

    switch ( type )
    {
    case TEXTURE_TYPE_1D:
        viewType = VK_IMAGE_VIEW_TYPE_1D;
        break;
    case TEXTURE_TYPE_1D_ARRAY:
        viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
        break;
    case TEXTURE_TYPE_2D:
        viewType = VK_IMAGE_VIEW_TYPE_2D;
        break;
    case TEXTURE_TYPE_2D_ARRAY:
        viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        break;
    case TEXTURE_TYPE_3D:
        viewType = VK_IMAGE_VIEW_TYPE_3D;
        break;
    case TEXTURE_TYPE_CUBE:
        viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        break;
    case TEXTURE_TYPE_CUBE_ARRAY:
        viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
        break;
    default:
        // todo cast a error 
        break;
    }

    return viewType;
}

static VkFormat VkTextureFormat( uint32_t format )
{
    VkFormat vkfmt;
    switch ( format )
    {
    case PIXEL_COLOR_R8I:
        vkfmt = VK_FORMAT_R8_SINT;
        break;
    case PIXEL_COLOR_R8UI:
        vkfmt = VK_FORMAT_R8_UINT;
        break;
    case PIXEL_COLOR_R16I:
        vkfmt = VK_FORMAT_R16_SINT;
        break;
    case PIXEL_COLOR_R16UI:
        vkfmt = VK_FORMAT_R16_UINT;
        break;
    case PIXEL_COLOR_R32I:
        vkfmt = VK_FORMAT_R32_SINT;
        break;
    case PIXEL_COLOR_R32UI:
        vkfmt = VK_FORMAT_R32_UINT;
        break;
    case PIXEL_COLOR_R16F:
        vkfmt = VK_FORMAT_R16_SFLOAT;
        break;
    case PIXEL_COLOR_R32F:
        vkfmt = VK_FORMAT_R32_SFLOAT;
        break;
    case PIXEL_COLOR_RG8I:
        vkfmt = VK_FORMAT_R8G8_SINT;
        break;
    case PIXEL_COLOR_RG8UI:
        vkfmt = VK_FORMAT_R8G8_UINT;
        break;
    case PIXEL_COLOR_RG16I:
        vkfmt = VK_FORMAT_R16G16_SINT;
        break;
    case PIXEL_COLOR_RG16UI:
        vkfmt = VK_FORMAT_R16G16_UINT;
        break;
    case PIXEL_COLOR_RG32I:
        vkfmt = VK_FORMAT_R32G32_SINT;
        break;
    case PIXEL_COLOR_RG32UI:
        vkfmt = VK_FORMAT_R32G32_UINT;
        break;
    case PIXEL_COLOR_RG16F:
        vkfmt = VK_FORMAT_R16G16_SFLOAT;
        break;
    case PIXEL_COLOR_RG32F:
        vkfmt = VK_FORMAT_R32G32_SFLOAT;
        break;
    case PIXEL_COLOR_RGB8I:
        vkfmt = VK_FORMAT_R8G8B8_SINT;
        break;
    case PIXEL_COLOR_RGB8UI:
        vkfmt = VK_FORMAT_R8G8B8_UINT;
        break;
    case PIXEL_COLOR_RGB16I:
        vkfmt = VK_FORMAT_R16G16B16_SINT;
        break;
    case PIXEL_COLOR_RGB16UI:
        vkfmt = VK_FORMAT_R16G16B16_UINT;
        break;
    case PIXEL_COLOR_RGB32I:
        vkfmt = VK_FORMAT_R32G32B32_SINT;
        break;
    case PIXEL_COLOR_RGB32UI:
        vkfmt = VK_FORMAT_R32G32_UINT;
        break;
    case PIXEL_COLOR_RGB16F:
        vkfmt = VK_FORMAT_R16G16B16_SFLOAT;
        break;
    case PIXEL_COLOR_RGB32F:
        vkfmt = VK_FORMAT_R32G32B32_SFLOAT;
        break;
    case PIXEL_COLOR_RGBA8I:
        vkfmt = VK_FORMAT_R8G8B8A8_SINT;
        break;
    case PIXEL_COLOR_RGBA8UI:
        vkfmt = VK_FORMAT_R8G8B8A8_UINT;
        break;
    case PIXEL_COLOR_RGBA16I:
        vkfmt = VK_FORMAT_R16G16B16_SINT;
        break;
    case PIXEL_COLOR_RGBA16UI:
        vkfmt = VK_FORMAT_R16G16B16_UINT;
        break;
    case PIXEL_COLOR_RGBA32I:
        vkfmt = VK_FORMAT_R32G32B32A32_SINT;
        break;
    case PIXEL_COLOR_RGBA32UI:
        vkfmt = VK_FORMAT_R32G32B32A32_UINT;
        break;
    case PIXEL_COLOR_RGBA16F:
        vkfmt = VK_FORMAT_R16G16B16_SFLOAT;
        break;
    case PIXEL_COLOR_RGBA32F:
        vkfmt = VK_FORMAT_R32G32B32A32_SFLOAT;
        break;
    case PIXEL_DEPTH_D16UI:
        vkfmt = VK_FORMAT_D16_UNORM;
        break;
    case PIXEL_DEPTH_D32F:
        vkfmt = VK_FORMAT_D32_SFLOAT;
        break;
    case PIXEL_DEPTH_STENCIL_D24S8UI:
        vkfmt = VK_FORMAT_D24_UNORM_S8_UINT;
        break;
    case PIXEL_DEPTH_STENCIL_D32F_S8UI:
        vkfmt = VK_FORMAT_D32_SFLOAT_S8_UINT;
        break;

    default:
        //TODO: error call 
        break;
    }

    return vkfmt;
}

crVKTexture::crVKTexture(void) : 
    crTexture(),
    m_texture( VK_NULL_HANDLE ),
    m_view( VK_NULL_HANDLE ),
    m_memory( VK_NULL_HANDLE )
{
}

bool crVKTexture::Create(const uint32_t width, const uint32_t height, const uint32_t depth, const uint32_t layers, const uint32_t mips, const uint32_t format, const uint32_t type)
{
    VkResult res = VK_SUCCESS;
    auto device = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetAllocator();

    m_width = width;
    m_height = height;
    m_mipcount = mips;
    VkImageType vktype = vkTextureTypeVulkan( type );
    VkFormat tformat = VkTextureFormat( format );

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = vktype;
    imageInfo.extent.width = m_width;
    imageInfo.extent.height = m_height;
    imageInfo.extent.depth = m_depth;
    imageInfo.mipLevels = m_mipcount;
    imageInfo.arrayLayers = m_layers;
    imageInfo.format = tformat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // create texture handler 
    res = vkCreateImage(device, &imageInfo, &allocator, &m_texture );
    if ( res != VK_SUCCESS)
        return false;
    
    // create image view
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_texture;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = tformat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = m_mipcount;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = m_layers;

    res = vkCreateImageView( device, &viewInfo, &allocator, &m_view );
    if ( res != VK_SUCCESS)
        return false;

    // get texture memory requeriments 
    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(device, m_texture, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = tr.vulkan->FindMemoryType( memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    // reserve device memory to texture 
    res = vkAllocateMemory(device, &allocInfo, &allocator, &m_memory);
    if ( res != VK_SUCCESS)
        return false;

    // bind reserved device memory to texture 
    vkBindImageMemory(device, m_texture, m_memory, 0 );

    return true;
}

void crVKTexture::Destroy(void)
{
    auto device = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetAllocator();

    if( m_view != VK_NULL_HANDLE )
    {
        vkDestroyImageView( device, m_view, &allocator );
        m_view = VK_NULL_HANDLE;
    }

    if( m_memory != VK_NULL_HANDLE )
    {
        vkFreeMemory( device, m_memory, &allocator );
        m_memory = VK_NULL_HANDLE;
    }

    if( m_texture != VK_NULL_HANDLE )
    {
        vkDestroyImage( device, m_texture, &allocator );
        m_texture = VK_NULL_HANDLE;
    }    
}

void crVKTexture::CopyBufferToImage(const crBuffer *buffer, const uint32_t rowLength, const bufferTextureRect_t *imageMap, const uint32_t count)
{
    VkImageLayout layout;
    crPointer<VkBufferImageCopy> regions;
    regions.Alloc( count );

    for ( uint32_t i = 0; i < count; i++)
    {
        bufferTextureRect_t tr = imageMap[i];
        VkBufferImageCopy &region = regions[i];

        region.bufferOffset = tr.bufferOffset;
        region.bufferRowLength = rowLength;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = tr.mipLevel;
        region.imageSubresource.baseArrayLayer = tr.layerLevel;
        region.imageSubresource.layerCount = 1;

        // layer offset 
        region.imageOffset = 
        {
            static_cast<int32_t>( tr.xOffset ),
            static_cast<int32_t>( tr.yOffset ), 
            static_cast<int32_t>( tr.zOffset ) 
        };

        // layer size 
        region.imageExtent = 
        {
        tr.width,
        tr.height,
        tr.depth
        };

    }
    
    VkCommandBuffer commandBuffer; // TODO: get the command buffer from backend 
    vkCmdCopyBufferToImage( commandBuffer, dynamic_cast<const crVKBuffer*>( buffer )->GetHandler(), m_texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, count, &regions );
}

void crVKTexture::MakeResident(const crGLTextureSampler *sampler)
{
}

void crVKTexture::Unmakeresident(void)
{
}

/*
=========================================================
=========================================================
*/
crVKTextureSampler::crVKTextureSampler(void) : 
    crTextureSampler(),
    m_sampler( nullptr )
{
}

crVKTextureSampler::~crVKTextureSampler(void)
{
}

bool crVKTextureSampler::Create(const uint32_t minFilter, const uint32_t magFilter, const uint32_t wrapS, const uint32_t wrapT, const float anisotropicLevel, const float LODBias )
{
    VkResult res = VK_SUCCESS;
    auto device = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetAllocator(); 
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    // Filtros de magnificação/minificação
    samplerInfo.magFilter = static_cast<VkFilter>( minFilter );
    samplerInfo.minFilter = static_cast<VkFilter>( magFilter );

    // Anisostropic filtering 
    samplerInfo.anisotropyEnable = anisotropicLevel > 0.0f ? VK_TRUE : VK_FALSE;
    samplerInfo.maxAnisotropy = std::min( 16.0f, anisotropicLevel ); //
    
    // Repeating
    samplerInfo.addressModeU = static_cast<VkSamplerAddressMode>( wrapS );
    samplerInfo.addressModeV = static_cast<VkSamplerAddressMode>( wrapT );
    samplerInfo.addressModeW = static_cast<VkSamplerAddressMode>( wrapS );

    // Mipmapping
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = LODBias;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 1.0f; // static_cast<float>(m_mipLevels); // ou 0.0f se sem mipmap

    // texture compare ( shadow mapping )
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    // Coord norm vs absolute texel
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    // Create Sampler
    res = vkCreateSampler( device, &samplerInfo, &allocator, &m_sampler );
    if ( res != VK_SUCCESS)
        return false;

    return true;
}

void crVKTextureSampler::Destroy(void)
{
    auto device = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetAllocator(); 
    if( m_sampler != VK_NULL_HANDLE )
    {
        vkDestroySampler( device, m_sampler, &allocator );
        m_sampler = VK_NULL_HANDLE;
    }
}
