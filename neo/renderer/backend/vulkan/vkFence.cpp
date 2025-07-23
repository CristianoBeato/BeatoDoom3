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

// FenceVK.cpp
#include "vkFence.h"

crVKFence::crVKFence( void ): m_fence( VK_NULL_HANDLE )
{
}

crVKFence::~crVKFence( void )
{
    // free fence 
    if ( m_fence )
    {
        vkDestroyFence( m_device, m_fence, &m_allocationCallbacks );
        m_fence = nullptr;
    }

    m_allocationCallbacks = nullptr;
    m_device = nullptr;
}

void crVKFence::Create( void )
{
    VkResult res = VK_SUCCESS;
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    crAutoPointer<crVulkanContext> vk = tr.m_renderContext.DynamicCast<crVulkanContext>();

    m_device = vk->Device();
    m_allocationCallbacks = vk->AllocationCallbacks();

    info.flags = 0;
    res = vkCreateFence( m_device, &info, m_allocationCallbacks, &m_fence );
    if ( res != VK_SUCCESS) 
        throw crVkException( "vkCreateFence failed!", res );
}

void crVKFence::Wait( const uint64_t in_timeout )
{
    vkWaitForFences( m_device, 1, &m_fence, VK_TRUE, in_timeout );
}

void crVKFence::Reset(void)
{
    vkResetFences( m_device, 1, &m_fence );
}

bool crVKFence::IsSignaled( void ) const
{
    return vkGetFenceStatus( m_device, m_fence ) == VK_SUCCESS;
}

