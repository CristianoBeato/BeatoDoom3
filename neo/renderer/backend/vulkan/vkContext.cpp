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

// Our internal allocators 
static void* VKAPI_CALL vkAllocation( void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
static void* VKAPI_CALL vkReallocation( void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
static void VKAPI_CALL  vkFree( void* pUserData, void* pMemory );
static void VKAPI_CALL  vkInternalAllocation( void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);
static void VKAPI_CALL  vkInternalFree( void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope );
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData );

crVulkanContext::crVulkanContext( void ) : 
    m_graphicsQueueFamilyIndex( 0 ),
    m_enableValidationLayers( false ),
    m_currentDevice( 0 ),
    m_instance( nullptr ),
    m_device( nullptr ),
    m_surface( nullptr ),
    m_allocationCallbacks( nullptr )
{
}

crVulkanContext::~crVulkanContext( void )
{
}

void crVulkanContext::StartUp(void)
{
    int                     device = 0;
    uint32_t                layerCount = 0;
    uint32_t                SDLextensionsCount = 0;
    uint32_t                instanceLayerPropertiesCount = 0;
    SDL_Window*             window = nullptr;
    char const * const *    SDLExtensions = nullptr;
#if !defined( _DEBUG ) && !defined( NDEBUG )
    m_enableValidationLayers = true;
#else 
    m_enableValidationLayers = false;
#endif 

    // our custom allocator
    m_allocationCallbacks.New();
    m_allocationCallbacks->pfnAllocation = vkAllocation;
    m_allocationCallbacks->pfnReallocation = vkReallocation;
    m_allocationCallbacks->pfnFree = vkFree;
    m_allocationCallbacks->pfnInternalAllocation = vkInternalAllocation;
    m_allocationCallbacks->pfnInternalFree = vkInternalFree;
    m_allocationCallbacks->pUserData = static_cast<void*>( this );

    // list SDL3 present extensions
    SDLExtensions = SDL_Vulkan_GetInstanceExtensions( &SDLextensionsCount );

    // Load dynamic Vulkan Lib
    InitLibrary();

    // List avaidable layers
    vkEnumerateInstanceLayerProperties( &instanceLayerPropertiesCount, nullptr );
    m_instanceLayerProperties.Resize( instanceLayerPropertiesCount );
    vkEnumerateInstanceLayerProperties( &instanceLayerPropertiesCount, m_instanceLayerProperties.Ptr() );
    
    m_enabledExtensions.Resize( SDLextensionsCount );
    for (size_t i = 0; i < SDLextensionsCount; i++)
    {
        m_enabledExtensions[i] = SDLExtensions[i];
        common->Printf( m_enabledExtensions[i] );
    }

    // Enable validation layer 
    if ( InstanceLayerPropertiePresent( "VK_LAYER_KHRONOS_validation" ) && m_enableValidationLayers )
        m_enabledExtensions.Append( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
    else
        m_enableValidationLayers = false; // disable 

    // Create vulkan library instance
    StartInstance();

    // Load vulkan functions from instance
    LoadVulkanFunctions();

    // Create the debug messager output structure
    InitDebugOutput();

    // Create the window surface
    window = static_cast<SDL_Window*>( Sys_videoWindowHandler() );
    if( !SDL_Vulkan_CreateSurface( window, m_instance, &m_allocationCallbacks, &m_surface ) ) 
        throw idException( SDL_GetError() );

    // List fisical devices
    ListDevices();

    // get defined 
    device = r_rendererDevice.GetInteger();
    
    // try Initialize some device
    while ( device < m_physicalDevices.Num() )
    {
        if ( device < 0 )
            device = GetBestDevice();
        
        if( InitDevice( device ) )
            break;

        // try next device 
        device++;
    }

    // no device initialized 
    if ( m_device == nullptr )
        throw idException( "No device initialized" );
}

void crVulkanContext::ShutDown(void)
{
    ReleaseDebugOutput();
    ShutDownInstance();
    ShutDownLibrary();
}

uint32_t crVulkanContext::FindMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags props )
{
    VkPhysicalDeviceMemoryProperties memProperties = m_devicesProperties[m_currentDevice].memoryProperties;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) 
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & props) == props)
            return i;
    }

    throw idException( "Failed to find suitable memory type" );
}

VkSurfaceTransformFlagBitsKHR crVulkanContext::GetDeviceSurfaceTransform(void) const
{
    return m_devicesProperties[m_currentDevice].surfaceCapabilities.currentTransform;
}

uint32_t crVulkanContext::GetDeviceSurfaceImagesCount(void) const
{
    uint32_t maxImageCount = m_devicesProperties[m_currentDevice].surfaceCapabilities.maxImageCount;
    uint32_t minImageCount = m_devicesProperties[m_currentDevice].surfaceCapabilities.minImageCount;
    return std::clamp( SMP_FRAMES, maxImageCount, minImageCount );
}

void crVulkanContext::InitLibrary( void )
{
    const char* libname = r_vkDriver.GetString();    

    // Load vulkan library 
    if ( !SDL_Vulkan_LoadLibrary( libname ) )
    {
        // if fail load the custom lib, try load the defalt, if fail, the we get a error 
        if ( !SDL_Vulkan_LoadLibrary( nullptr ) )
            throw idException( SDL_GetError() );
    }

    // Get instance base functions
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)SDL_Vulkan_GetVkGetInstanceProcAddr();
    vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr( nullptr, "vkCreateInstance" );  
    vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)vkGetInstanceProcAddr( nullptr, "vkEnumerateInstanceLayerProperties" );

    if ( ( !vkGetInstanceProcAddr ) || ( !vkCreateInstance ) || ( !vkEnumerateInstanceLayerProperties ) )
        throw idException( "can't load from %s, vkGetInstanceProcAddr, vkCreateInstance or vkEnumerateInstanceLayerProperties\n" );
}

void crVulkanContext::ShutDownLibrary(void)
{
    // Release vulkan library 
    SDL_Vulkan_UnloadLibrary();
}

bool crVulkanContext::InstanceLayerPropertiePresent( const char *layerName )
{
    bool layerFound = false;
    for ( uint32_t i = 0; i < m_instanceLayerProperties.Num(); i++)
    {
        auto layerProperties = m_instanceLayerProperties[i];
        if ( strcmp( layerName, layerProperties.layerName ) == 0 ) 
        {
            layerFound = true;
            break;
        }
    }

    if ( layerFound )
        common->Printf( "Vulkan layer %s found\n", layerName );
    else
        common->Printf( "Vulkan layer %s not found!\n", layerName );

    return layerFound;
}

void crVulkanContext::StartInstance(void)
{
    VkInstanceCreateInfo    instanceCI{};
    VkApplicationInfo       applicationI{};
    applicationI.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationI.pApplicationName = GAME_NAME;
    applicationI.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationI.pEngineName = ENGINE_VERSION;
    applicationI.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationI.apiVersion = VK_API_VERSION_1_0;
    
    // 
    instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCI.pApplicationInfo = &applicationI;
    instanceCI.enabledExtensionCount = m_enabledExtensions.Num();
    instanceCI.ppEnabledExtensionNames = m_enabledExtensions.Ptr();
    
    // is we have a layer avaidable make ative 
    if ( m_enableValidationLayers )
    {
        instanceCI.enabledLayerCount = m_enabledLayerNames.Num();
        instanceCI.ppEnabledLayerNames = m_enabledLayerNames.Ptr();
    }
    else
    {
        instanceCI.enabledLayerCount = 0;
        instanceCI.ppEnabledLayerNames = nullptr;
    }

    // Create a vulkan instance
    VkResult result = vkCreateInstance( &instanceCI, &m_allocationCallbacks, &m_instance );
    if( result != VK_SUCCESS )
        throw crVkException( "Error:vkCreateInstance: ", result );
}

void crVulkanContext::ShutDownInstance(void)
{
    if ( m_instance != nullptr )
    {
        vkDestroyInstance( m_instance, &m_allocationCallbacks );
        m_instance = nullptr;
    }
    
    m_instanceLayerProperties.Clear();
    m_enabledExtensions.Clear();
    m_enabledLayerNames.Clear();
}

void crVulkanContext::InitDebugOutput(void)
{
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{};
    if ( !m_enableValidationLayers ) 
        return;

    debugUtilsMessengerCI.pfnUserCallback = debugCallback;
    debugUtilsMessengerCI.pUserData = nullptr;
    debugUtilsMessengerCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCI.messageSeverity = 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    
    debugUtilsMessengerCI.messageType = 
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    VkResult result = vkCreateDebugUtilsMessengerEXT( m_instance, &debugUtilsMessengerCI, &m_allocationCallbacks, &m_debugOutput ); 
    if ( result != VK_SUCCESS )
        throw crVkException( "failed to set up debug messenger!", result );
}

void crVulkanContext::ReleaseDebugOutput(void)
{
    if ( m_debugOutput != nullptr )
    {
        vkDestroyDebugUtilsMessengerEXT( m_instance, m_debugOutput, &m_allocationCallbacks );
        m_debugOutput = nullptr;
    }
}

void crVulkanContext::ListDevices(void)
{
    uint32_t physicalDevicesCount = 0;
    vkEnumeratePhysicalDevices( m_instance, &physicalDevicesCount, nullptr );
    m_devicesProperties.Resize( physicalDevicesCount );
    m_physicalDevices.Resize( physicalDevicesCount );
    vkEnumeratePhysicalDevices( m_instance, &physicalDevicesCount, m_physicalDevices.Ptr() );

    if ( physicalDevicesCount == 0 )
        throw idException("failed to find GPUs with Vulkan support!");

    // get phisical device properties
    for ( uint32_t i = 0; i < physicalDevicesCount; i++)
    {
        uint32_t extensionCount = 0;
        uint32_t surfaceFormatsCount = 0;
        uint32_t queueFamilyCount = 0;
        uint32_t presentModeCount = 0;
        auto physicalDevice = m_physicalDevices[i];
        auto &deviceProperties = m_devicesProperties[i];
        
        //
        vkGetPhysicalDeviceProperties( physicalDevice, &deviceProperties.properties );
        vkGetPhysicalDeviceFeatures( physicalDevice, &deviceProperties.features );
        vkGetPhysicalDeviceMemoryProperties( physicalDevice, &deviceProperties.memoryProperties );
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDevice, m_surface, &deviceProperties.surfaceCapabilities );
    
        // list device extensions
        vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, nullptr);
        deviceProperties.availableExtensions.Resize( extensionCount );
        vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, deviceProperties.availableExtensions.Ptr() );

        //
        vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, m_surface, &surfaceFormatsCount, nullptr );
        deviceProperties.surfaceFormats.Resize( surfaceFormatsCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, m_surface, &surfaceFormatsCount, deviceProperties.surfaceFormats.Ptr() );

        //
        vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, m_surface, &presentModeCount, nullptr);
        deviceProperties.presentModes.Resize( presentModeCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, m_surface, &presentModeCount, deviceProperties.presentModes.Ptr() );

        //
        vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, nullptr );
        deviceProperties.queueFamily.Resize( queueFamilyCount );
        vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, deviceProperties.queueFamily.Ptr() );

        // list and organize queues
        for ( uint32_t j = 0; j < queueFamilyCount; j++)
        {
            VkBool32 presentSupport = VK_FALSE;
            auto family = deviceProperties.queueFamily[i];
            
            vkGetPhysicalDeviceSurfaceSupportKHR( physicalDevice, i, m_surface, &presentSupport);
            // list present suported queues 
            if ( presentSupport )
                deviceProperties.queuesList.presentFamily.Append( i );

            // has atleast one present queue 
            if( presentSupport && !hasFeature( deviceProperties.suportedFeatures, SuportedFeatures::FEATURE_PRESENT_QUEUE ) )
                deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_PRESENT_QUEUE;
            
            // graphic queues 
            if ( family.queueFlags & VK_QUEUE_GRAPHICS_BIT ) 
            {
                deviceProperties.queuesList.graphicsFamily.Append( i );
                if( !hasFeature( deviceProperties.suportedFeatures, SuportedFeatures::FEATURE_GRAPHIC_QUEUE ) )
                    deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_GRAPHIC_QUEUE;
            }
            // list compute queues 
            if ( family.queueFlags & VK_QUEUE_COMPUTE_BIT )
            { 
                deviceProperties.queuesList.computeFamily.Append( i );
                if( !hasFeature( deviceProperties.suportedFeatures, SuportedFeatures::FEATURE_COMPUTE_QUEUE ) )
                    deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_COMPUTE_QUEUE;
            }

            // list transfer queues 
            if ( family.queueFlags & VK_QUEUE_TRANSFER_BIT )
            { 
                deviceProperties.queuesList.transferFamily.Append( i );
                if( !hasFeature( deviceProperties.suportedFeatures, SuportedFeatures::FEATURE_TRANSFER_QUEUE ) )
                    deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_TRANSFER_QUEUE;
            }
        }

            // Mark the avaidable properties
        if ( deviceProperties.features.geometryShader )
        {
            deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_GEOMETRY_SHADER;
            deviceProperties.score += 10;
        }
        
        // Tesselation suport
        if ( deviceProperties.features.tessellationShader)
        {
            deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_TESSELATION_SHADER;
            deviceProperties.score += 10;
        }

        // Multidraw indirect
        if ( deviceProperties.features.multiDrawIndirect )
        {
            deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_MULTIDRAW_INDIRECT;
            deviceProperties.score += 10;
        }
        
        // Multiviewport 
        if( deviceProperties.features.multiViewport )
        {
            deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_MULTI_VIEWPORT;
            deviceProperties.score += 10;
        }    
        
        // Sampler anisostropy 
        if( deviceProperties.features.samplerAnisotropy )
        {
            deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_SAMPLER_ANISOSTROPY;
            deviceProperties.score += 10;
        }

        // ETC2 texture compression 
        if( deviceProperties.features.textureCompressionETC2 )
        {    
            deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_COMPRESSION_ETC2;
            deviceProperties.score += 10;
        }

        // Oclusion query precise 
        if ( deviceProperties.features.occlusionQueryPrecise )
        {
            deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_OCLUSION_QUERY;    
            deviceProperties.score += 10;
        }

        // Depth bouds     
        if( deviceProperties.features.depthBounds )
        {
            deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_DEPTH_BOUNDS;
            deviceProperties.score += 10;
        }
        
        // Depth bias clamp 
        if( deviceProperties.features.depthBiasClamp )
        {
            deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_DEPTH_CLAMP;
            deviceProperties.score += 10;
        }

        // Maximum possible size of textures affects graphics quality
        deviceProperties.score += deviceProperties.properties.limits.maxImageDimension2D;
        deviceProperties.score += deviceProperties.properties.limits.maxImageArrayLayers;

        for ( uint32_t i = 0; i < presentModeCount; i++)
        {
            switch ( deviceProperties.presentModes[i] )
            {

            case VK_PRESENT_MODE_IMMEDIATE_KHR:
                deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_SWAP_CHAIN_MODE_IMMEDIATE;
                deviceProperties.score += 10;
                break;
            case VK_PRESENT_MODE_MAILBOX_KHR:
            deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_SWAP_CHAIN_MODE_MAILBOX;
                deviceProperties.score += 20;
                break;
            case VK_PRESENT_MODE_FIFO_KHR:
                deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_SWAP_CHAIN_MODE_FIFO;
                deviceProperties.score += 30;
                break;
            case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
                deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_SWAP_CHAIN_MODE_FIFO_RELAXED;
                deviceProperties.score += 30;
                break;
            case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
                deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_SWAP_CHAIN_MODE_SHARED_DEMAND_REFRESH;
                deviceProperties.score += 10;
                break;
            case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
                deviceProperties.suportedFeatures |= SuportedFeatures::FEATURE_SWAP_CHAIN_MODE_SHARED_CONTINUOUS_REFRESH;
                deviceProperties.score += 10;
                break;
            }
        }
        
        // Check the avaidable devices types
        switch (deviceProperties.properties.deviceType)
        {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            deviceProperties.score += 100;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            deviceProperties.score += 50;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            deviceProperties.score += 10;
            break;
        }
    }   
}

uint32_t crVulkanContext::GetBestDevice(void)
{
    uint32_t best = 0;
    // get the best score device
    for ( uint32_t i = 0; i < m_devicesProperties.Num(); i++)
    {
        if ( m_devicesProperties[i].score >  m_devicesProperties[best].score )
            best = i;
    }
    
    return best;
}

bool crVulkanContext::ExtensionPresent(const uint32_t device, const char *ext)
{
    bool extFound = false;

    // out of range 
    if( m_devicesProperties.Num() <= device )
        return false;

    // search in the device extensions 
    for ( uint32_t i = 0; i < m_devicesProperties[device].availableExtensions.Num(); i++ )
    {
        if( strcmp( m_devicesProperties[device].availableExtensions[i].extensionName, ext ) == 0 )
        {
            extFound = true;
            break;
        }
    }
    
    if ( extFound )
        common->Printf("Vulkan device extension %s found\n", ext );
    else
        common->Warning("Vulkan device extension %s not found\n", ext );
    
    return extFound;
}

bool crVulkanContext::InitDevice(const uint32_t device)
{
    float queuePriority = 1.0f;
    uint32_t enabledExtensionCount = 0;
    uint32_t queueFamilyCount = 0;
    VkDeviceCreateInfo deviceCI{};
    idList<const char*> enabledExtensionNames;
    idList<VkDeviceQueueCreateInfo> queuesCI;
    VkPhysicalDeviceFeatures deviceFeatures{};

    if ( !ExtensionPresent( device, VK_KHR_SWAPCHAIN_EXTENSION_NAME ) || !ExtensionPresent( device, VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME ) )
    {
        common->Warning( "Selected device don't suport \'VK_KHR_swapchain\' " );
        return false;
    }

    // prepare all device queues ( do this, to don't bother searching for what queues we gona enable at this time )
    queueFamilyCount = m_devicesProperties[device].queueFamily.Num(); 
    queuesCI.Resize( queueFamilyCount );
    for ( uint32_t i = 0; i < queueFamilyCount; i++)
    {
        queuesCI[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queuesCI[i].queueFamilyIndex = i;
        queuesCI[i].queueCount = m_devicesProperties[device].queueFamily[i].queueCount;
        queuesCI[i].pQueuePriorities = &queuePriority;
    }
    
    // features enabled if avaidable
    deviceFeatures.geometryShader = m_devicesProperties[device].features.geometryShader;
    deviceFeatures.independentBlend = m_devicesProperties[device].features.independentBlend;
    deviceFeatures.tessellationShader = m_devicesProperties[device].features.tessellationShader;
    deviceFeatures.sampleRateShading = m_devicesProperties[device].features.sampleRateShading;
    deviceFeatures.multiDrawIndirect = m_devicesProperties[device].features.multiDrawIndirect;
    deviceFeatures.multiViewport = m_devicesProperties[device].features.multiViewport;
    deviceFeatures.samplerAnisotropy = m_devicesProperties[device].features.samplerAnisotropy;
    deviceFeatures.occlusionQueryPrecise = m_devicesProperties[device].features.occlusionQueryPrecise;
    deviceFeatures.pipelineStatisticsQuery = m_devicesProperties[device].features.pipelineStatisticsQuery;
    deviceFeatures.shaderResourceResidency = m_devicesProperties[device].features.shaderResourceResidency;
    deviceFeatures.shaderResourceMinLod = m_devicesProperties[device].features.shaderResourceMinLod;
    deviceFeatures.sparseBinding = m_devicesProperties[device].features.sparseBinding;
    deviceFeatures.sparseResidencyBuffer = m_devicesProperties[device].features.sparseResidencyBuffer;
    deviceFeatures.depthClamp = m_devicesProperties[device].features.depthClamp; 
    deviceFeatures.depthBounds = m_devicesProperties[device].features.depthBounds;
    deviceFeatures.variableMultisampleRate = m_devicesProperties[device].features.variableMultisampleRate;

    // Device Creation configurration
    deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCI.queueCreateInfoCount = queuesCI.Num();
    deviceCI.pQueueCreateInfos = queuesCI.Ptr();
    deviceCI.pEnabledFeatures = &deviceFeatures;
    
    // enable device exts 
    enabledExtensionCount = m_devicesProperties[device].availableExtensions.Num();
    enabledExtensionNames.Resize( enabledExtensionCount );
    for ( uint32_t i = 0; i < enabledExtensionCount; i++)
    {
        enabledExtensionNames[i] = m_devicesProperties[device].availableExtensions[i].extensionName;
    }
    
    deviceCI.enabledExtensionCount = enabledExtensionNames.Num();
    deviceCI.ppEnabledExtensionNames = enabledExtensionNames.Ptr();

    if ( m_enableValidationLayers )
    {
        deviceCI.enabledLayerCount = m_enabledLayerNames.Num();
        deviceCI.ppEnabledLayerNames = m_enabledLayerNames.Ptr();
    }
    else
    {
        deviceCI.enabledLayerCount = 0;
        deviceCI.ppEnabledLayerNames = nullptr;
    }

    VkResult result = vkCreateDevice( m_physicalDevices[device], &deviceCI, &m_allocationCallbacks, &m_device );
    if( result != VK_SUCCESS )
    {
        common->Warning( "vkCreateDevice failed " );
        return false;
    }

    m_currentDevice = device;
    return true;
}

void crVulkanContext::ReleaseDevice(void)
{
    if( m_device != nullptr )
    {
        vkDestroyDevice( m_device, &m_allocationCallbacks );
        m_device = nullptr;
    }

    m_devicesProperties.Clear();
    m_physicalDevices.Clear();
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData )
{
    const char* severityMSG = nullptr;
    const char* typeMSG = nullptr;
    switch ( messageSeverity )
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        severityMSG = " VULKAN VERBOSE:";
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        severityMSG = "VULKAN INFO:";
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        severityMSG = "!VULKAN WARNING!:";
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        severityMSG = "!VULKAN ERROR!:";
        break;
    }

    switch (messageType)
    {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        typeMSG = "Some event has hap-pened that is unrelated to the specification or performance";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        typeMSG = "Something has happened that violates the specification or indicates a possible mistake";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        typeMSG = "Potential non-optimal use of Vulkan";
        break;
    }

    printf( "%s %s\n %s\n", severityMSG, typeMSG, pCallbackData->pMessage );

    return VK_FALSE;
}

void* VKAPI_CALL vkAllocation( void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    return SDL_aligned_alloc( alignment, size );
}

void* VKAPI_CALL vkReallocation( void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope )
{
    void* ptr = SDL_aligned_alloc( alignment, size );
    if ( pOriginal )
    {
        SDL_memcpy( ptr, pOriginal, sizeof(pOriginal) );
        SDL_aligned_free( pOriginal );
    }
    return ptr;    
}

void VKAPI_CALL  vkFree( void* pUserData, void* pMemory )
{
    SDL_aligned_free( pMemory );
}

void VKAPI_CALL  vkInternalAllocation( void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
    // vkCtx.allocedMemory += size;
    //printf("[Vulkan] Internal allocation of %zu bytes, total %zu\n", size, vkCtx.allocedMemory );
}

void VKAPI_CALL  vkInternalFree( void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope )
{
    // vkCtx.allocedMemory -= size; 
    //printf("[Vulkan] Internal free of %zu bytes, total %zu\n", size, vkCtx.allocedMemory );
}