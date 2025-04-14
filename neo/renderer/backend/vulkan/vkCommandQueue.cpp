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

#include "qvk.h"
#include "vkCommandQueue.h"

vkCommandQueue::vkCommandQueue( void ) :
    m_commandBuffer( nullptr ),
    m_commandPool( nullptr )
{

}

vkCommandQueue::~vkCommandQueue( void )
{
}

void vkCommandQueue::Create(void)
{
    VkResult res = VK_SUCCESS;
    auto device = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetAllocator();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = tr.vulkan->GetGraphicsQueueFamilyIndex(); // índice da fila gráfica
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    res = vkCreateCommandPool( device, &poolInfo, &allocator, &m_commandPool ); 
    if ( res != VK_SUCCESS) 
        throw idException("Falha ao criar command pool!");

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // ou SECONDARY, conforme necessário
    allocInfo.commandBufferCount = 1; // número de buffers que você deseja alocar
        
    res = vkAllocateCommandBuffers( device, &allocInfo, &m_commandBuffer );
    if ( res != VK_SUCCESS)
            throw idException("Falha ao alocar command buffer!");
}

void vkCommandQueue::Destroy(void)
{
    auto device = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetAllocator();

    if ( m_commandBuffer != nullptr )
    {
        vkFreeCommandBuffers( device, m_commandPool, 1, &m_commandBuffer );
        m_commandBuffer = nullptr;
    }

    if( m_commandPool != nullptr )
    {
        vkDestroyCommandPool( device, m_commandPool, &allocator );
        m_commandPool = nullptr;
    }
}

void vkCommandQueue::Begin(void)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // flags e informações extras aqui, se necessário
    VkResult res = vkBeginCommandBuffer( m_commandBuffer, &beginInfo );
    if ( res != VK_SUCCESS )
        common->Error( "Falha ao iniciar a gravação do command buffer!" );
}

void vkCommandQueue::End(void)
{
    VkResult res = vkEndCommandBuffer( m_commandBuffer ); 
    if ( res != VK_SUCCESS ) 
        common->Error("Falha ao finalizar o command buffer!");   
}
