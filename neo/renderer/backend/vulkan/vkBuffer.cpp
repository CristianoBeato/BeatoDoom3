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

#include "vkBuffer.h"

crVKBuffer::crVKBuffer( void ) : 
    crBuffer(),
    m_buffer( nullptr ),
    m_memory( nullptr )
{
}

crVKBuffer::~crVKBuffer(void)
{
}

bool crVKBuffer::Create( const bufferUsage_t usage, const size_t size, const bool write, const bool read )
{
    VkBufferUsageFlags  vkusage;
    VkResult res = VK_SUCCESS;
    m_size = size;

    // VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    switch ( usage )
    {
    case BUFFER_USAGE_VERTEX:
        vkusage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;
    case BUFFER_USAGE_ELEMENT:
        vkusage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;
    case BUFFER_USAGE_PIXEL:
        vkusage = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;
    case BUFFER_USAGE_SHADER:
        vkusage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;
    default:
        break;
    }

    // Create the stage buffer
    res = CreateBuffer( m_stagingBuffer, m_memory, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
    if ( res != VK_SUCCESS ) 
    {
        common->Error( "Failed to create stagin buffer" );
        return false; // TODO: add debug output 
    }

    // Create buffer main
    res = CreateBuffer( m_buffer, m_memory, size, vkusage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
    if ( res != VK_SUCCESS ) 
    {
        common->Error("vkCreateBuffer Failed");
        return false; // TODO: add debug output 
    }

    return true;
}

void crVKBuffer::Destroy( void ) 
{
    auto allccbk = tr.vulkan->GetAllocator();
    auto device = tr.vulkan->GetDevice();

    // unmap memmory 
    if ( m_map != VK_NULL_HANDLE )
    {
        vkUnmapMemory( device, m_memory );
        m_map = nullptr;
    }

    // Release device memmory
    if ( m_memory != VK_NULL_HANDLE )
    {
        vkFreeMemory( device, m_memory, &allccbk );
        m_memory = VK_NULL_HANDLE;
    }
    
    // destroy buffer objcet
    if (m_buffer != VK_NULL_HANDLE )
    {
        vkDestroyBuffer( device, m_buffer, &allccbk );
        m_buffer = VK_NULL_HANDLE;
    }
}

void crVKBuffer::Flush(const uintptr_t offset, const size_t size) const
{
    // copy content from staging buffer to GPU memory ( in openGL the driver done this, in vulkan we do manualy )
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    copyRegion.dstOffset = offset;
    copyRegion.srcOffset = offset;
    vkCmdCopyBuffer( commandBuffer, m_stagingBuffer, m_buffer, 1, &copyRegion );
}

VkResult crVKBuffer::CreateBuffer( VkBuffer &buffer, VkDeviceMemory &memory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties )
{
    uint32_t memTypeIndex = 0;
    VkResult res = VK_SUCCESS;
    VkMemoryRequirements memRequirements{};
    VkBufferCreateInfo bufferInfo{};
    auto allccbk = tr.vulkan->GetAllocator();
    auto device = tr.vulkan->GetDevice();

    // Criação do buffer
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    res = vkCreateBuffer( device, &bufferInfo, &allccbk, &buffer );
    if( res != VK_SUCCESS )
        return res;

    // get buffer memory resources 
    vkGetBufferMemoryRequirements( device, buffer, &memRequirements );

    // Selecionar tipo de memória
    memTypeIndex = tr.vulkan->FindMemoryType( memRequirements.memoryTypeBits, properties );

    // Alocar memória
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memTypeIndex;

    res = vkAllocateMemory( device, &allocInfo, &allccbk, &memory );
    if ( res != VK_SUCCESS)
        return res;

    // Associar memória ao buffer
    res = vkBindBufferMemory( device, buffer, memory, 0 );
    if ( res != VK_SUCCESS)
        return res;
}

void crVKBuffer::MapBuffer(void)
{
    // Mapear memória
    auto res = vkMapMemory( tr.vulkan->GetDevice(), m_stagingMemory, 0, VK_WHOLE_SIZE, 0, &m_map );
    if ( res != VK_SUCCESS )
        common->Error( "vkMapMemory::Failed" );
}
