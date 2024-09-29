/*
===========================================================================

Beato idTech 4 Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
Copyright (C) 2016-2022 Cristiano B. Santos <cristianobeato_dm@hotmail.com>.

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
#include "idlib/precompiled.h"
#pragma hdrstop

#include <SDL2/SDL_vulkan.h>
#include "renderer/qvk.h"

/*
=================================================================================================
crVkPhysicalDevice
=================================================================================================
*/
class crVkPhysicalDevice
{
public:
    crVkPhysicalDevice( void );
    ~crVkPhysicalDevice( void );
    
    void                                    Init( const VkSurfaceKHR surface, const VkPhysicalDevice device );

    inline const char*                      Name( void ) const { return m_name; }
    inline uint32_t                         Score( void ) const { return m_score; }
    inline uint32_t                         Features( void ) const { return m_features; }
    inline uint32_t                         GetNumQueues( void ) const { return m_queueFamilesCout; }
    inline uint32_t                         GraphicQueueID( void ) const { return m_queueGraphic[0]; }
    inline uint32_t                         ComputeQueueID( void ) const { return m_queueCompute[0]; }
    inline uint32_t                         TransferQueueID( void ) const { return m_queueTransfer[0]; }
    inline uint32_t                         PresentQueueID( void ) const { return m_queueTransfer[0]; }
    inline uint32_t                         GraphicQueueCount( void ) const { return m_queueGraphic[1]; }
    inline uint32_t                         ComputeQueueCount( void ) const { return m_queueCompute[1]; }
    inline uint32_t                         TransferQueueCount( void ) const { return m_queueTransfer[1]; }
    inline uint32_t                         PresentQueueCount( void ) const { return m_queueTransfer[1]; }
    inline VkSurfaceTransformFlagBitsKHR    GetTransform( void ) const { return m_transform; } 
    inline uint32_t                         GetImageCount( uint32_t actualImageCount ) const { return std::clamp( actualImageCount, m_imageCount[0], m_imageCount[1] );}
    inline VkExtent2D                       GetExtent( VkExtent2D actualExtent ) const 
    {
        actualExtent.width = std::clamp( actualExtent.width, m_imageExtent[0].width, m_imageExtent[1].width ); 
        actualExtent.height = std::clamp( actualExtent.height, m_imageExtent[0].height, m_imageExtent[1].height );
        return actualExtent;
    }
    
private:
    uint32_t                            m_score;
    uint32_t                            m_features;
    uint32_t                            m_queueFamilesCout;
    uint32_t                            m_queueGraphic[2];   // First
    uint32_t                            m_queueCompute[2];   // Compute
    uint32_t                            m_queueTransfer[2];  // 
    uint32_t                            m_queuePresent[2];   //
    uint32_t                            m_imageCount[2];
    VkExtent2D                          m_imageExtent[2];
    VkSurfaceTransformFlagBitsKHR       m_transform;
    VkPhysicalDevice                    m_physicalDevice;
    idStr                               m_name;
    crPointer<VkQueueFamilyProperties>  m_queuesFamilies;
    crPointer<VkBool32>                 m_queuesPresents;

    void    GetSurfaceCapabilities( const VkSurfaceKHR surface );
    void    GetQueuesInfo( const VkSurfaceKHR surface );
    void    GetDeviceFeatures( void );
};

//
// Vulkan context structures 
// Hold vulkan basic objects 
static struct 
{
    bool                            enableValidationLayers = false;
    uint32_t                        currentDevice;        //
    VkFormat                        swapChainImageFormat;
    VkColorSpaceKHR                 swapChainImageColorSpace;
    VkInstance                      instance = nullptr;
    VkSurfaceKHR                    surface = nullptr;
    VkDevice                        device = nullptr;
    VkSwapchainKHR                  swapChain = nullptr;
    VkDebugUtilsMessengerEXT        debugOutput = nullptr;
    idList<const char*>             instanceExtensions;
    idList<VkImage>                 swapChainImages;      // SwapChain Image chain
    idList<VkImageView>             swapChainImagesView;
    idList<VkLayerProperties>       availableLayers;      // Debug Output Layers
    idList<crVkPhysicalDevice>      physicDevicesInfo;    // Devices Properties array
    idList<VkPhysicalDevice>        physicDevices;        // Avaidable Vulkan Phisics devices
}vkContext;

static const uint32_t k_INVALID_QUEUE_FAMILY = 0xFFFFFFFF;

// Debug output callback 
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData );
static void InitInstance( const uint32_t count, const char** exts );
static void InitDebugOutput( void );
static void FindAvaidableDevices( void );
static void InitDevice( const uint32_t id, const uint32_t graphicQueueCount, const uint32_t computeQueueCount, const uint32_t transferFamilyCount );
static void InitSwapChain( const uint32_t width, const uint32_t height, const uint32_t vsync );

static const unsigned int validationLayersCount = 1;
static const char* validationLayers[validationLayersCount] = 
{
    "VK_LAYER_KHRONOS_validation"
};

const uint32_t Sys_GetDeviceCount( void )
{
    return vkContext.physicDevices.Num();
}

const VkInstance Sys_GetVulkanInstance(void)
{
    return vkContext.instance;
}

const uint32_t Sys_GetVulkanDeviceFeatures( void )
{
    return vkContext.physicDevicesInfo[vkContext.currentDevice].Features();
}

const VkDevice Sys_GetVulkanDevice( void )
{
    return vkContext.device;
}

const VkSwapchainKHR Sys_GetVulkanSwapChain( void )
{
    return vkContext.swapChain;
}

const VkSurfaceKHR Sys_GetVulkanSurface( void )
{
    return vkContext.surface;
}

//
static void LoadVulkanLibrary( const char* vkLib )
{
    // Load vulkan library 
    if ( SDL_Vulkan_LoadLibrary( vkLib ) != 0 )
        throw idException( "can't load vkLib: %s %s\n", vkLib, SDL_GetError() );

    // Get instance base functions
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)SDL_Vulkan_GetVkGetInstanceProcAddr();
    vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr( nullptr, "vkCreateInstance" );  
    vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)vkGetInstanceProcAddr( nullptr, "vkEnumerateInstanceLayerProperties" );

    if ( ( !vkGetInstanceProcAddr ) || ( !vkCreateInstance ) || ( !vkEnumerateInstanceLayerProperties ) )
        throw idException( "can't load from %s, vkGetInstanceProcAddr, vkCreateInstance or vkEnumerateInstanceLayerProperties\n", vkLib );
}


static bool CheckValidationLayerSupport( void )
{
    bool layerFound = false;
   
    for ( uint32_t i = 0; i < validationLayersCount; i++ )
    {
        const char* layerName = validationLayers[i];
        for ( uint32_t j = 0; j < vkContext.availableLayers.Num(); j++ )
        {
            const auto& layerProperties = vkContext.availableLayers[j];
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }
    }

    return ( vkContext.enableValidationLayers = layerFound );
}

//
void Sys_StartUpVulkanAPI( void )
{
    SDL_Window* window = static_cast<SDL_Window*>(Sys_videoWindowHandler());
    int width = 0;
    int height = 0;
    uint32_t extensionsCount = 0;
    uint32_t layerCount = 0;

#ifdef NDEBUG
    vkContext.enableValidationLayers = false;
#else 
    vkContext.enableValidationLayers = true;
#endif   

#if 0
    m_allocCallBack.pfnAllocation = vkAllocation;
    m_allocCallBack.pfnFree = vkFree;
    m_allocCallBack.pfnReallocation = vkReallocation;
    m_allocCallBack.pfnInternalAllocation = vkInternalAllocationNotification;
    m_allocCallBack.pfnInternalFree = vkInternalFreeNotification;
#endif

    if (!window)
        throw idException( "Invalid window object" );

    // Get vulkan shared library entry point
    LoadVulkanLibrary( nullptr );
    
    // List avaidable Vulkan Exts
    SDL_Vulkan_GetInstanceExtensions( window, &extensionsCount, nullptr );
    vkContext.instanceExtensions.Resize( extensionsCount );
    SDL_Vulkan_GetInstanceExtensions( window, &extensionsCount, vkContext.instanceExtensions );

    // List avaidable layers
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    vkContext.availableLayers.Resize( layerCount );
    vkEnumerateInstanceLayerProperties(&layerCount, vkContext.availableLayers );

    SDL_Vulkan_GetDrawableSize( window, &width, &height );

    for ( uint32_t i = 0; i < vkContext.instanceExtensions.Num(); i++)
    {
        common->Printf( vkContext.instanceExtensions[i] );
    }

    // Check for valiatation layers 
    if ( CheckValidationLayerSupport() )
        vkContext.instanceExtensions.Append( VK_EXT_DEBUG_UTILS_EXTENSION_NAME ); // Append debug ext

    // Create vulkan software instance
    InitInstance( vkContext.instanceExtensions.Num(), vkContext.instanceExtensions );

    // Create the window surface 
    if( SDL_Vulkan_CreateSurface( window, vkContext.instance, &vkContext.surface ) != SDL_TRUE )
        throw idException( SDL_GetError() );

    // Load Vulkan instance functions
    LoadVulkanFunctions( vkContext.instance );

    // 
    InitDebugOutput();

    // Find
    FindAvaidableDevices();
}

void Sys_ShutDownVulkanAPI( void )
{
    if ( vkContext.surface != nullptr )
    {
        vkDestroySurfaceKHR( vkContext.instance, vkContext.surface, nullptr );
        vkContext.surface = nullptr;
    }

    if ( vkContext.instance != nullptr )
    {
        vkDestroyInstance( vkContext.instance, nullptr );
        vkContext.instance = nullptr;
    }
    
    // Release vulkan library
    SDL_Vulkan_UnloadLibrary();   
}

const bool Sys_InitVulkanDevice( const vkParms_t parms )
{
    try
    {
        //
        InitDevice( parms.device, 1, 0, 0 );

        // TODO: Check if hardware suport 
        vkContext.swapChainImageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        vkContext.swapChainImageFormat = VK_FORMAT_B8G8R8A8_SRGB;

        //
        uint32_t vsync = k_VSYNC_WAIT_FRAME;
        InitSwapChain( parms.width, parms.height, vsync );
    }
    catch(const idException& e)
    {
        common->Error( e.What() );
        return false;
    } 
}

void Sys_ShutDownVulkanDevice(void)
{
    if ( vkContext.swapChain != nullptr )
    {
        vkDestroySwapchainKHR( vkContext.device, vkContext.swapChain, nullptr );
        vkContext.swapChain = nullptr;
    }
    
    if ( vkContext.device != nullptr )
    {
        vkDestroyDevice( vkContext.device, nullptr );
        vkContext.device = nullptr; 
    }
}

void InitInstance( const uint32_t count, const char** exts )
{
    // instance configuration
    VkApplicationInfo applicationInfo{};
    VkInstanceCreateInfo instanceCI{};

    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Hello Triangle";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "No Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_0;
    
    instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCI.pApplicationInfo = &applicationInfo;
    instanceCI.enabledExtensionCount = count;
    instanceCI.ppEnabledExtensionNames = exts;
    instanceCI.enabledLayerCount = 0;

    // is we have a layer avaidable make ative 
    if ( vkContext.enableValidationLayers )
    {
        instanceCI.enabledLayerCount = 1;
        instanceCI.ppEnabledLayerNames = validationLayers;
    }
    else
        instanceCI.enabledLayerCount = 0;
    
    // Create a vulkan instance
    VkResult result = vkCreateInstance( &instanceCI, nullptr, &vkContext.instance );
    if( result != VK_SUCCESS )
        throw crVkException( "Error:vkCreateInstance: ", result );
}

void InitDebugOutput(void)
{
    if ( !vkContext.enableValidationLayers ) 
        return;

    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{};
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

    VkResult result = vkCreateDebugUtilsMessengerEXT( vkContext.instance, &debugUtilsMessengerCI, nullptr, &vkContext.debugOutput ); 
    if ( result != VK_SUCCESS )
        throw crVkException( "failed to set up debug messenger!", result );
}

void FindAvaidableDevices(void)
{
    VkResult res = VK_SUCCESS;
    uint32_t deviceCount = 0;
    // Get avaidable Device Count
    res = vkEnumeratePhysicalDevices( vkContext.instance, &deviceCount, nullptr );
    if ( res != VK_SUCCESS ) throw crVkException( "vkEnumeratePhysicalDevices", res );

    // Alloc device handlers
    vkContext.physicDevices.Resize( deviceCount );
    vkContext.physicDevicesInfo.Resize( deviceCount );

    res = vkEnumeratePhysicalDevices( vkContext.instance, &deviceCount, vkContext.physicDevices );
    if ( res != VK_SUCCESS ) throw crVkException( "vkEnumeratePhysicalDevices", res );
    
    common->Printf( "Found %i Compatible Vulkan devicess\n", deviceCount );

    // get devices onfo
    for ( uint32_t i = 0; i < deviceCount; i++)
    {
        // Read device from list
        vkContext.physicDevicesInfo[i].Init( vkContext.surface, vkContext.physicDevices[i] );
    }
}

void InitDevice( const uint32_t id, const uint32_t graphicQueueCount, const uint32_t computeQueueCount, const uint32_t transferFamilyCount )
{
    float queuePriority = 1.0f;
    uint32_t queueCreateInfoCount = 0;
    VkDeviceQueueCreateInfo queuesCI[3]{};
    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo deviceCI{};

    if( (vkContext.physicDevicesInfo[id].Features() & k_FEATURE_SWAP_CHAIN ) != k_FEATURE_SWAP_CHAIN )
        throw idException(" Selected device don't suport swap chain present" );

    // Required Extensions
    const char* extList[2] =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME
    };

    // Configura the graphics queues
    if ( ( vkContext.physicDevicesInfo[id].Features() & k_FEATURE_GRAPHIC_QUEUE ) && ( graphicQueueCount > 0 ) )
    {
        VkDeviceQueueCreateInfo &graphicQueueCrtInf = queuesCI[queueCreateInfoCount++];
        graphicQueueCrtInf.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicQueueCrtInf.queueFamilyIndex = vkContext.physicDevicesInfo[id].GraphicQueueID();
        graphicQueueCrtInf.queueCount = min( graphicQueueCount, vkContext.physicDevicesInfo[id].GraphicQueueCount() );
        graphicQueueCrtInf.pQueuePriorities = &queuePriority;
    }

    // Configure the compute queues
    if ( ( vkContext.physicDevicesInfo[id].Features() & k_FEATURE_COMPUTE_QUEUE ) && ( computeQueueCount > 0 ) )
    {
        VkDeviceQueueCreateInfo &computeQueueCrtInf = queuesCI[queueCreateInfoCount++];
        computeQueueCrtInf.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        computeQueueCrtInf.queueFamilyIndex = vkContext.physicDevicesInfo[id].ComputeQueueID();
        computeQueueCrtInf.queueCount = min( computeQueueCount, vkContext.physicDevicesInfo[id].ComputeQueueCount() );
        computeQueueCrtInf.pQueuePriorities = &queuePriority;
    }

    // Configure transfer queues
    if ( ( vkContext.physicDevicesInfo[id].Features() & k_FEATURE_TRANSFER_QUEUE ) > 0 &&  ( transferFamilyCount > 0 ) )
    {
        VkDeviceQueueCreateInfo &transferQueueCrtInf = queuesCI[queueCreateInfoCount++];
        transferQueueCrtInf.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        transferQueueCrtInf.queueFamilyIndex = vkContext.physicDevicesInfo[id].PresentQueueID();
        transferQueueCrtInf.queueCount = min( transferFamilyCount, vkContext.physicDevicesInfo[id].PresentQueueCount() );
        transferQueueCrtInf.pQueuePriorities = &queuePriority;
    }

    // Device Creation configurration
    deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCI.pQueueCreateInfos = queuesCI;
    deviceCI.queueCreateInfoCount = queueCreateInfoCount;
    deviceCI.pEnabledFeatures = &deviceFeatures;
    
    deviceCI.enabledExtensionCount = 2;
    deviceCI.ppEnabledExtensionNames = extList;

    if ( vkContext.enableValidationLayers )
    {
        deviceCI.enabledLayerCount = validationLayersCount;
        deviceCI.ppEnabledLayerNames = validationLayers;
    }
    else
        deviceCI.enabledLayerCount = 0;

    common->Printf( "Using %s as current render device\n", vkContext.physicDevicesInfo[id].Name() );

    VkResult result = vkCreateDevice( vkContext.physicDevices[id], &deviceCI, nullptr, &vkContext.device );
    if( result != VK_SUCCESS )
        throw crVkException( "Error:vkCreateDevice: ", result );

    vkContext.currentDevice = id;
}

void InitSwapChain( const uint32_t width, const uint32_t height, const uint32_t vsync )
{
    VkExtent2D actualExtent = { width, height };
    VkResult res = VK_SUCCESS;
    uint32_t numSwapChainImages = 0;
    int32_t graphicQueueFamilyID = -1;
    int32_t presentQueueFamilyID = -1;
    uint32_t queueFamilyIndices[2];
    
    VkSwapchainCreateInfoKHR swapchainCI{};
    swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCI.surface = vkContext.surface;
    swapchainCI.minImageCount = vkContext.physicDevicesInfo[vkContext.currentDevice].GetImageCount( 3 );
    swapchainCI.imageColorSpace = vkContext.swapChainImageColorSpace;
    swapchainCI.imageExtent = vkContext.physicDevicesInfo[vkContext.currentDevice].GetExtent( actualExtent );
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCI.clipped = VK_TRUE;
    swapchainCI.preTransform = vkContext.physicDevicesInfo[vkContext.currentDevice].GetTransform();

    // If we are updating a old swap chain
    if ( vkContext.swapChain )
        swapchainCI.oldSwapchain = vkContext.swapChain; // If we are updating a existing SwapChain
    
    // "Adaptative" sync 
    uint32_t features = vkContext.physicDevicesInfo[vkContext.currentDevice].Features(); 
    if ( ( (  features & k_FEATURE_SWAP_CHAIN_MODE_MAILBOX ) == k_FEATURE_SWAP_CHAIN_MODE_MAILBOX ) && vsync == k_VSYNC_ADAPTATIVE )
        swapchainCI.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    else if ( ( ( features & k_FEATURE_SWAP_CHAIN_MODE_FIFO ) == k_FEATURE_SWAP_CHAIN_MODE_FIFO ) && vsync == k_VSYNC_WAIT_FRAME )
        swapchainCI.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    else
        swapchainCI.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    
    graphicQueueFamilyID = vkContext.physicDevicesInfo[vkContext.currentDevice].GraphicQueueID();
    presentQueueFamilyID = vkContext.physicDevicesInfo[vkContext.currentDevice].PresentQueueID();

    if ( graphicQueueFamilyID < 0 || presentQueueFamilyID < 0 )
        throw idException( "No suitabel grafical or present queue " );
    
    if ( graphicQueueFamilyID == presentQueueFamilyID )
    {
        swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCI.queueFamilyIndexCount = 0; // Optional
        swapchainCI.pQueueFamilyIndices = nullptr; // Optional
    }
    else
    {
        queueFamilyIndices[0] = graphicQueueFamilyID;
        queueFamilyIndices[1] = presentQueueFamilyID;
        swapchainCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCI.queueFamilyIndexCount = 2;
        swapchainCI.pQueueFamilyIndices = queueFamilyIndices;
    }

    // Create the swap chain
    VkResult result = vkCreateSwapchainKHR( vkContext.device, &swapchainCI, nullptr, &vkContext.swapChain );
    if ( result != VK_SUCCESS )
        throw crVkException( "Can't create swapchain", result );
        
    //Get swapChain images
    res = vkGetSwapchainImagesKHR( vkContext.device, vkContext.swapChain, &numSwapChainImages, nullptr );
    if ( res != VK_SUCCESS )
         throw crVkException( "vkGetSwapchainImagesKHR", res );
    
    vkContext.swapChainImages.Resize( numSwapChainImages );
    vkContext.swapChainImagesView.Resize( numSwapChainImages );

    // List the swap chain imagens 
    res = vkGetSwapchainImagesKHR( vkContext.device, vkContext.swapChain, &numSwapChainImages, vkContext.swapChainImages );
    if ( res != VK_SUCCESS )
        throw crVkException( "vkGetSwapchainImagesKHR", res );

    VkImageViewCreateInfo imageViewCI; 
    imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCI.format = vkContext.swapChainImageFormat;
    imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCI.subresourceRange.baseMipLevel = 0;
    imageViewCI.subresourceRange.levelCount = 1;
    imageViewCI.subresourceRange.baseArrayLayer = 0;
    imageViewCI.subresourceRange.layerCount = 1;
    imageViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    for (size_t i = 0; i < numSwapChainImages; i++)
    {
        imageViewCI.image = vkContext.swapChainImages[i];
        res = vkCreateImageView( vkContext.device, &imageViewCI, nullptr, &vkContext.swapChainImagesView[i] );
        if ( res != VK_SUCCESS )
            throw crVkException( "vkCreateImageView", res );
    }
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

    Sys_Printf( "%s %s\n %s\n", severityMSG, typeMSG, pCallbackData->pMessage );

    return VK_FALSE;
}


/*
=================================================================================================
crVkPhysicalDevice
=================================================================================================
*/
crVkPhysicalDevice::crVkPhysicalDevice( void ) : 
    m_physicalDevice( nullptr ),
    m_queueFamilesCout( 0 ),
    m_features( 0 ),
    m_score( 0 )
{
}

crVkPhysicalDevice::~crVkPhysicalDevice(void)
{
    m_name.Clear();

#if 0
    m_queues.Free();
#else
    m_queuesFamilies.Free();
    m_queuesPresents.Free();
#endif
}

void crVkPhysicalDevice::Init( const VkSurfaceKHR surface, const VkPhysicalDevice device )
{
    m_physicalDevice = device;

    GetDeviceFeatures();
    GetQueuesInfo( surface );
    GetSurfaceCapabilities( surface );

    common->Printf( "_____________________________________________________\n" );
    common->Printf( "Device: %s\nScore: %i\n Suported Features:\n", m_name, m_score ); 
    common->Printf( "Queues found: %i\n", m_queueFamilesCout );

    for (size_t i = 0; i < m_queueFamilesCout; i++)
    {
        auto queueInfo = m_queuesFamilies[i];
        auto transfer = m_queuesPresents[i];
        common->Printf( "Family: %i count %i", i, queueInfo.queueCount );
        if ( transfer )
            common->Printf( " Present" );
        
        if ( queueInfo.queueFlags & VK_QUEUE_GRAPHICS_BIT )
            common->Printf( " Graphic" );

        if ( queueInfo.queueFlags & VK_QUEUE_COMPUTE_BIT )
            common->Printf( " Compute" );
        
        if ( queueInfo.queueFlags & VK_QUEUE_TRANSFER_BIT )
            common->Printf( " Transfer" );

        common->Printf( "\n" );
    }
    
    common->Printf( "_____________________________________________________" );
}

void crVkPhysicalDevice::GetSurfaceCapabilities(const VkSurfaceKHR surface)
{
    uint32_t                    surfaceFormatCount = 0;
    uint32_t                    presentModeCount = 0;
    VkBool32                    presentSupport = false;
    VkSurfaceCapabilitiesKHR    surfaceCapabilities;
 
    // Present mode count
    vkGetPhysicalDeviceSurfacePresentModesKHR( m_physicalDevice, surface, &presentModeCount, nullptr );
    crScopedPtr<VkPresentModeKHR> presentModes = crScopedPtr<VkPresentModeKHR>( presentModeCount );
    vkGetPhysicalDeviceSurfacePresentModesKHR( m_physicalDevice, surface, &presentModeCount, &presentModes );

     // Get suported format count
    vkGetPhysicalDeviceSurfaceFormatsKHR( m_physicalDevice, surface, &surfaceFormatCount, nullptr );
    crScopedPtr<VkSurfaceFormatKHR> surfaceFormats = crScopedPtr<VkSurfaceFormatKHR>( surfaceFormatCount );
    vkGetPhysicalDeviceSurfaceFormatsKHR( m_physicalDevice, surface, &surfaceFormatCount, &surfaceFormats );

    // Get Device surface properties
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_physicalDevice, surface, &surfaceCapabilities );

    //
    m_transform = surfaceCapabilities.currentTransform;
    m_imageCount[0] = surfaceCapabilities.minImageCount;
    m_imageCount[1] = surfaceCapabilities.maxImageCount;
    m_imageExtent[0] = surfaceCapabilities.minImageExtent;
    m_imageExtent[1] = surfaceCapabilities.maxImageExtent;

    // Avaidable devices present mode
    for (size_t i = 0; i < presentModeCount; i++)
    {
        switch( presentModes[i] )
        {
            case VK_PRESENT_MODE_IMMEDIATE_KHR:
                m_features |= k_FEATURE_SWAP_CHAIN_MODE_IMMEDIATE;
                break;
            case VK_PRESENT_MODE_MAILBOX_KHR:
                m_features |= k_FEATURE_SWAP_CHAIN_MODE_MAILBOX;
                break;
            case VK_PRESENT_MODE_FIFO_KHR:
                m_features |= k_FEATURE_SWAP_CHAIN_MODE_FIFO;
                break;
            case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
                m_features |= k_FEATURE_SWAP_CHAIN_MODE_FIFO_RELAXED;
                break;
//            case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
//                m_scModes |= k_PRESENT_MODE_
//                break;
//            case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
//                break;
        }
    }

    //
    for (size_t i = 0; i < surfaceFormatCount; i++)
    {
        auto availableFormat = surfaceFormats[i];
        if ( availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
            m_features |= k_FEATURE_SWAP_CHAIN_FORMAT_BGRA8_SRGB;
    }

}

void crVkPhysicalDevice::GetQueuesInfo(const VkSurfaceKHR surface)
{
    uint32_t i = 0;

    // Get Device queues
    vkGetPhysicalDeviceQueueFamilyProperties( m_physicalDevice, &m_queueFamilesCout, nullptr );
    m_queuesFamilies.Alloc( m_queueFamilesCout );
    m_queuesPresents.Alloc( m_queueFamilesCout );
    vkGetPhysicalDeviceQueueFamilyProperties( m_physicalDevice, &m_queueFamilesCout, &m_queuesFamilies );

    // Get first Graphic queue
    for ( i = 0; i < m_queueFamilesCout; i++)
    {
        if ( m_queuesFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT )
        {
            m_queueGraphic[0] = i;
            m_queueGraphic[1] = m_queuesFamilies[i].queueCount; 
            break;
        }
    }

    //
    for ( i = 0; i < m_queueFamilesCout; i++)
    {
        if ( m_queuesFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT )
        {
            m_queueCompute[0] = i;
            m_queueCompute[1] = m_queuesFamilies[i].queueCount;
            break;
        }
    }

    //
    for ( i = 0; i < m_queueFamilesCout; i++)
    {
        if ( m_queuesFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT )
        {
            m_queueTransfer[0] = i;
            m_queueTransfer[1] = m_queuesFamilies[i].queueCount;
            break;
        }
    }

    //
    for ( i = 0; i < m_queueFamilesCout; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR( m_physicalDevice, i, surface, &m_queuesPresents[i] );
        auto queueFamily = m_queuesFamilies[i];

        m_score += queueFamily.queueCount;
        if ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
        {
            m_features |= k_FEATURE_GRAPHIC_QUEUE;
            m_score += 10;
        }

        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT )
        {
            m_features |= k_FEATURE_COMPUTE_QUEUE;
            m_score += 50;
        }

        if( queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT )
        {
            m_features |= k_FEATURE_TRANSFER_QUEUE;
            m_score += 25;
        }
    }

    //
    for ( i = 0; i < m_queueFamilesCout; i++)
    {
        auto present = m_queuesPresents[i];
        if ( present == VK_TRUE )
        {
            m_queuePresent[0] = i;
            m_queuePresent[1] = m_queuesFamilies[i].queueCount;
        }
        
    }
}

void crVkPhysicalDevice::GetDeviceFeatures(void)
{
    // List device extensions
    uint32_t extensionCount = 0;
    uint32_t extStrLen = 0;
    VkPhysicalDeviceFeatures    features;
    VkPhysicalDeviceProperties  properties;

    vkEnumerateDeviceExtensionProperties( m_physicalDevice, nullptr, &extensionCount, nullptr );
    crScopedPtr<VkExtensionProperties> availableExtensions = crScopedPtr<VkExtensionProperties>( extensionCount ); 
    vkEnumerateDeviceExtensionProperties( m_physicalDevice, nullptr, &extensionCount, &availableExtensions );

    // Get Device Properties and features
    vkGetPhysicalDeviceProperties( m_physicalDevice, &properties );
    vkGetPhysicalDeviceFeatures( m_physicalDevice, &features );

    // Get device name 
    m_name = idStr( properties.deviceName );
    
    // Mark the avaidable properties
    if ( features.geometryShader )
    {
        m_features |= k_FEATURE_GEOMETRY_SHADER;
        m_score += 1;
    }

    // Tesselation suport
    if ( features.tessellationShader)
    {
        m_features |= k_FEATURE_TESSELATION_SHADER;
        m_score += 1;
    }

    // Multidraw indirect
    if ( features.multiDrawIndirect )
    {
        m_score += 1;
        m_features |= k_FEATURE_MULTIDRAW_INDIRECT;
    }

    // Multiviewport 
    if( features.multiViewport )
    {
        m_score += 1;
        m_features |= k_FEATURE_MULTI_VIEWPORT;
    }

    // Sampler anisostropy 
    if( features.samplerAnisotropy )
    {
        m_score += 1;
        m_features |= k_FEATURE_SAMPLER_ANISOSTROPY;
    }

    // ETC2 texture compression 
    if( features.textureCompressionETC2 )
    {
        m_score += 1;
        m_features |= k_FEATURE_COMPRESSION_ETC2;
    }

    // Oclusion query precise 
    if ( features.occlusionQueryPrecise )
    {
        m_score += 1;
        m_features |= k_FEATURE_OCLUSION_QUERY;
    }

    // Depth bouds 
    if( features.depthBounds )
    {
        m_score += 1;
        m_features |= k_FEATURE_DEPTH_BOUNDS;
    }

    // Depth bias clamp 
    if( features.depthBiasClamp )
    {
        m_score += 1;
        m_features |= k_FEATURE_DEPTH_CLAMP;
    }

    // Check the avaidable devices types
    switch (properties.deviceType)
    {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        m_score += 10;
        common->Printf( "Discrete GPU\n" );
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        common->Printf( "Integrated GPU" );
        m_score += 5;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        common->Printf( "Software Renderer" );
        m_score += 1;
        break;        
    default:
        break;
    }

    //
    for ( uint32_t i = 0; i < extensionCount; i++) 
    {
        if( strcmp( availableExtensions[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME ) == 0 )
            m_features |= k_FEATURE_SWAP_CHAIN;
    }
}
