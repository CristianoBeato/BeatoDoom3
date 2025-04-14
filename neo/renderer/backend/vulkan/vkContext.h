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

// Device suported features flags
enum class SuportedFeatures : uint32_t
{
    FEATURE_NONE                                        = 0,
    FEATURE_SWAP_CHAIN                                  = 1 << 0,
    FEATURE_GEOMETRY_SHADER                             = 1 << 1,
    FEATURE_TESSELATION_SHADER                          = 1 << 2,
    FEATURE_MULTIDRAW_INDIRECT                          = 1 << 3,
    FEATURE_MULTI_VIEWPORT                              = 1 << 4,
    FEATURE_SAMPLER_ANISOSTROPY                         = 1 << 5,
    FEATURE_COMPRESSION_ETC2                            = 1 << 6,
    FEATURE_OCLUSION_QUERY                              = 1 << 7,
    FEATURE_DEPTH_BOUNDS                                = 1 << 8,
    FEATURE_DEPTH_CLAMP                                 = 1 << 9,
    FEATURE_PRESENT_QUEUE                               = 1 << 10,
    FEATURE_GRAPHIC_QUEUE                               = 1 << 11,
    FEATURE_COMPUTE_QUEUE                               = 1 << 12,
    FEATURE_TRANSFER_QUEUE                              = 1 << 13,
    FEATURE_SWAP_CHAIN_MODE_IMMEDIATE                   = 1 << 14,
    FEATURE_SWAP_CHAIN_MODE_FIFO                        = 1 << 15,
    FEATURE_SWAP_CHAIN_MODE_FIFO_RELAXED                = 1 << 16,
    FEATURE_SWAP_CHAIN_MODE_MAILBOX                     = 1 << 17,
    FEATURE_SWAP_CHAIN_MODE_SHARED_DEMAND_REFRESH       = 1 << 18,
    FEATURE_SWAP_CHAIN_MODE_SHARED_CONTINUOUS_REFRESH   = 1 << 19,
    FEATURE_SWAP_CHAIN_FORMAT_BGRA8_SRGB                = 1 << 20,
};

// Habilita operadores bitwise pro enum class
ID_INLINE SuportedFeatures operator|( SuportedFeatures a, SuportedFeatures b ) 
{
    return static_cast<SuportedFeatures>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

ID_INLINE SuportedFeatures operator&( SuportedFeatures a, SuportedFeatures b ) 
{
    return static_cast<SuportedFeatures>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

ID_INLINE SuportedFeatures operator~( SuportedFeatures a ) 
{
    return static_cast<SuportedFeatures>(~static_cast<uint32_t>(a));
}

ID_INLINE SuportedFeatures& operator|=( SuportedFeatures& a, SuportedFeatures b ) 
{
    a = a | b;
    return a;
}

ID_INLINE SuportedFeatures& operator&=( SuportedFeatures& a, SuportedFeatures b ) 
{
    a = a & b;
    return a;
}

ID_INLINE bool hasFeature( SuportedFeatures value, SuportedFeatures flag) 
{
    return (value & flag) != SuportedFeatures::FEATURE_NONE;
}


// store que queue lists
struct vkQueueInto_t
{
    vkQueueInto_t( void ){}
    ~vkQueueInto_t( void )
    {
        presentFamily.Clear();
        presentFamilyCount.Clear();
        graphicsFamily.Clear();
        graphicsFamilyCount.Clear();
        computeFamily.Clear();
        computeFamilyCount.Clear();
        transferFamily.Clear();
        transferFamilyCount.Clear();
    }

    idList<uint32_t>    presentFamily;
    idList<uint32_t>    presentFamilyCount;
    idList<uint32_t>    graphicsFamily;
    idList<uint32_t>    graphicsFamilyCount;
    idList<uint32_t>    computeFamily;
    idList<uint32_t>    computeFamilyCount;
    idList<uint32_t>    transferFamily;
    idList<uint32_t>    transferFamilyCount;

    //
    bool HasPresent( void ) const { return !presentFamily.Empty(); }
    bool HasGraphic( void ) const { return !graphicsFamily.Empty(); }
    bool HasCompute( void ) const { return !computeFamily.Empty(); }
    bool HasTransfer( void ) const { return !transferFamily.Empty(); }

    //
    uint32_t PresentFamlilyCount( void ) const { return presentFamily.Num(); }
    uint32_t GraphicFamlilyCount( void ) const { return graphicsFamily.Num(); }
    uint32_t ComputeFamlilyCount( void ) const { return computeFamily.Num(); }
    uint32_t TransferFamlilyCount( void ) const { return transferFamily.Num(); }
};

// hold the phisical device properties and features
struct deviceProperties_t
{
    uint32_t                            score;
    SuportedFeatures                    suportedFeatures;
    VkPhysicalDeviceProperties          properties;
    VkPhysicalDeviceFeatures            features;
    VkSurfaceCapabilitiesKHR            surfaceCapabilities;
    VkPhysicalDeviceMemoryProperties    memoryProperties;
    vkQueueInto_t                       queuesList;
    idList<VkSurfaceFormatKHR>          surfaceFormats;
    idList<VkExtensionProperties>       availableExtensions;
    idList<VkQueueFamilyProperties>     queueFamily;
    idList<VkPresentModeKHR>            presentModes;
};

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

    ID_INLINE VkInstance                            GetInstance( void ) const { return m_instance; }
    ID_INLINE VkPhysicalDevice                      GetPhysicalDevice( void ) const { return m_physicalDevices[m_currentDevice]; }
    ID_INLINE VkDevice                              GetDevice( void ) const { return m_device; }
    ID_INLINE VkSurfaceKHR                          GetSurface( void ) const { return m_surface; }
    ID_INLINE crAutoPointer<VkAllocationCallbacks>  GetAllocator( void ) const { return m_allocationCallbacks; }
    ID_INLINE uint32_t                              GetGraphicsQueueFamilyIndex( void ) { return m_graphicsQueueFamilyIndex; }
    
private:
    bool                                    m_enableValidationLayers;
    uint32_t                                m_currentDevice;
    VkInstance                              m_instance;
    VkDevice                                m_device;
    VkSurfaceKHR                            m_surface;
    VkDebugUtilsMessengerEXT                m_debugOutput;
    idList<VkLayerProperties>               m_instanceLayerProperties;
    idList<const char*>                     m_enabledExtensions;              // 
    idList<const char*>                     m_enabledLayerNames;              // layers enable
    crAutoPointer<VkAllocationCallbacks>    m_allocationCallbacks;
    idList<deviceProperties_t>              m_devicesProperties;
    idList<VkPhysicalDevice>                m_physicalDevices;
    uint32_t                                m_graphicsQueueFamilyIndex;

    void        InitLibrary( void );
    void        ShutDownLibrary( void );
    void        LoadVulkanFunctions( void );
    bool        InstanceLayerPropertiePresent( const char* ext );
    void        StartInstance( void );
    void        ShutDownInstance( void );
    void        InitDebugOutput( void );
    void        ReleaseDebugOutput( void );
    void        ListDevices( void );
    uint32_t    GetBestDevice( void );
    bool        ExtensionPresent( const uint32_t device, const char *ext );
    bool        InitDevice( const uint32_t device );
    void        ReleaseDevice( void );
};

#endif //!__VK_CONTEXT_H__