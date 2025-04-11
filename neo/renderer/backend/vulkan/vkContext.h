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

#ifndef __VK_CONTEXT_H__
#define __VK_CONTEXT_H__

#include "qvk.h"

class crVulkanContext
{
public:
    crVulkanContext( void );
    ~crVulkanContext( void );

    void    StartUp( void );
    void    ShutDown( void );

    /// @brief find a suitabe device buffer memmory 
    /// @param typeFilter 
    /// @param props memmory properties  
    /// @return the device memmoey index 
    uint32_t FindMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags props );

    ID_INLINE VkInstance              GetInstance( void ) const { return m_instance; }
    ID_INLINE VkPhysicalDevice        GetPhysicalDevice( void ) const { return m_physDevice; }
    ID_INLINE VkDevice                GetDevice( void ) const { return m_device; }
    ID_INLINE VkSurfaceKHR            GetSurface( void ) const { return m_surface; }
    ID_INLINE VkAllocationCallbacks*  GetAllocator( void ) const { return m_allocationCallbacks; }

private:
    VkInstance              m_instance;
    VkPhysicalDevice        m_physDevice;
    VkDevice                m_device;
    VkSurfaceKHR            m_surface;
    VkAllocationCallbacks*  m_allocationCallbacks;

    /// @brief load all subjacent vulkan functions pointer from library 
    /// @param instance current vulkan instance  
    void LoadVulkanFunctions( const VkInstance instance );
};

#endif //!__VK_CONTEXT_H__