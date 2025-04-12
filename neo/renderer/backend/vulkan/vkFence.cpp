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
    auto allocator = tr.vulkan->GetAllocator();
    auto device = tr.vulkan->GetDevice();

    // free fence 
    if ( m_fence )
    {
        vkDestroyFence( device, m_fence, &allocator );
        m_fence = nullptr;
    }
}

void crVKFence::Create( void )
{
    auto device = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetAllocator();

    VkResult res = VK_SUCCESS;
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.flags = 0;
    res = vkCreateFence( device, &info, &allocator, &m_fence );
    if ( res != VK_SUCCESS) 
        throw crVkException( "vkCreateFence failed!", res );
}

void crVKFence::Wait( void )
{
    auto device = tr.vulkan->GetDevice();
    vkWaitForFences( device, 1, &m_fence, VK_TRUE, 0xFFFFFFFFFFFFFFFF );
}

void crVKFence::Reset(void)
{
    auto device = tr.vulkan->GetDevice();
    vkResetFences( device, 1, &m_fence );
}

bool crVKFence::IsSignaled( void ) const
{
    auto device = tr.vulkan->GetDevice();
    return vkGetFenceStatus( device, m_fence ) == VK_SUCCESS;
}

