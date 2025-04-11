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
#include "qvk.h"
#include "vkContext.h"

#include <SDL3/SDL_vulkan.h>

crVulkanContext::crVulkanContext( void ) : 
    m_instance( nullptr ),
    m_physDevice( nullptr ),
    m_device( nullptr ),
    m_surface( nullptr ),
    m_allocationCallbacks( nullptr )
{
}

crVulkanContext::~crVulkanContext( void )
{
}

uint32_t crVulkanContext::FindMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags props )
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties( m_physDevice, &memProperties );

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) 
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & props) == props)
            return i;
    }

    throw idException( "Failed to find suitable memory type" );
}
