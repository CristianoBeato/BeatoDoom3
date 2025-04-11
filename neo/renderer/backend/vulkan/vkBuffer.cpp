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

bool crVKBuffer::Create(size_t size, const bool write, const bool read )
{
    VkResult res = VK_SUCCESS;
    VkAllocationCallbacks *allccbk = tr.vulkan->GetAllocator();
    VkDevice device = tr.vulkan->GetDevice();
    m_size = size;

    // Criação do buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    res = vkCreateBuffer( device, &bufferInfo, allccbk, &m_buffer );
    if ( res != VK_SUCCESS ) 
        return false; // TODO: add debug output 

    // Obter os requisitos de memória
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements( device, m_buffer, &memRequirements );

    // Selecionar tipo de memória
    uint32_t memTypeIndex = tr.vulkan->FindMemoryType( memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Alocar memória
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memTypeIndex;

    res = vkAllocateMemory( device, &allocInfo, allccbk, &m_memory);
    if ( res != VK_SUCCESS)
        return false;

    // Associar memória ao buffer
    vkBindBufferMemory( device, m_buffer, m_memory, 0);

    // Mapear memória
    vkMapMemory( device, m_memory, 0, VK_WHOLE_SIZE, 0, &m_map );

    return true;
}

void crVKBuffer::Destroy( void ) 
{
    VkAllocationCallbacks *allccbk = tr.vulkan->GetAllocator();
    VkDevice device = tr.vulkan->GetDevice();

    // unmap memmory 
    if ( m_map != VK_NULL_HANDLE )
        vkUnmapMemory( device, m_memory);
    
    
    // Release device memmory
    if (m_memory != VK_NULL_HANDLE )
        vkFreeMemory( device, m_memory, allccbk);

    // destroy buffer objcet
    if (m_buffer != VK_NULL_HANDLE )
        vkDestroyBuffer( device, m_buffer, allccbk);
    
    m_map = nullptr;
    m_buffer = VK_NULL_HANDLE;
    m_memory = VK_NULL_HANDLE;
}