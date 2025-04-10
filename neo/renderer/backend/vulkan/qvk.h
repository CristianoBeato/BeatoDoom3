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

#ifndef __QVULKAN_H__
#define __QVULKAN_H__

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

//
static const uint32_t   k_VSYNC_IMEDIATE = 0;   // No Vsync, imediate update can cause tearing
static const uint32_t   k_VSYNC_WAIT_FRAME = 1;   // Wait frame end before update, can cause leg 
static const uint32_t   k_VSYNC_ADAPTATIVE = 2; // Present frame soon as possible 

// Device suported features flags
static const uint32_t   k_FEATURE_SWAP_CHAIN = 1 << 0;
static const uint32_t   k_FEATURE_GEOMETRY_SHADER = 1 << 1;
static const uint32_t   k_FEATURE_TESSELATION_SHADER = 1 << 2;
static const uint32_t   k_FEATURE_MULTIDRAW_INDIRECT = 1 << 3;
static const uint32_t   k_FEATURE_MULTI_VIEWPORT = 1 << 4;
static const uint32_t   k_FEATURE_SAMPLER_ANISOSTROPY = 1 << 5;
static const uint32_t   k_FEATURE_COMPRESSION_ETC2 = 1 << 6;
static const uint32_t   k_FEATURE_OCLUSION_QUERY = 1 << 7;
static const uint32_t   k_FEATURE_DEPTH_BOUNDS = 1 << 8;
static const uint32_t   k_FEATURE_DEPTH_CLAMP = 1 << 9;
static const uint32_t   k_FEATURE_GRAPHIC_QUEUE = 1 << 10;
static const uint32_t   k_FEATURE_COMPUTE_QUEUE = 1 << 11;
static const uint32_t   k_FEATURE_TRANSFER_QUEUE = 1 << 12;
static const uint32_t   k_FEATURE_SWAP_CHAIN_MODE_IMMEDIATE = 1 << 13;
static const uint32_t   k_FEATURE_SWAP_CHAIN_MODE_FIFO = 1 << 14;
static const uint32_t   k_FEATURE_SWAP_CHAIN_MODE_FIFO_RELAXED = 1 << 15; 
static const uint32_t   k_FEATURE_SWAP_CHAIN_MODE_MAILBOX = 1 << 16;
static const uint32_t   k_FEATURE_SWAP_CHAIN_FORMAT_BGRA8_SRGB = 1 << 17;

typedef struct 
{
	int			width;
	int			height;
    int         device;
	bool		fullScreen;
	bool		stereo;
	int			displayHz;
	int			multiSamples;
} vkParms_t;

extern void                 Sys_StartUpVulkanAPI( void );
extern void                 Sys_ShutDownVulkanAPI( void );
extern const VkInstance     Sys_GetVulkanInstance( void );
extern const bool           Sys_InitVulkanDevice( const vkParms_t parms );
extern void                 Sys_ShutDownVulkanDevice( void );
extern void                 Sys_SwapBuffersVK( void );
extern const uint32_t       Sys_GetDeviceCount( void );
extern const uint32_t       Sys_GetVulkanDeviceFeatures( void );
extern const VkDevice       Sys_GetVulkanDevice( void );
extern const VkSwapchainKHR Sys_GetVulkanSwapChain( void );
extern const VkSurfaceKHR   Sys_GetVulkanSurface( void );
extern void                 LoadVulkanFunctions( const VkInstance instance );

/*
=================================================================================================
=================================================================================================
*/

// Instance properties
extern PFN_vkCreateInstance                             vkCreateInstance;
extern PFN_vkGetInstanceProcAddr                        vkGetInstanceProcAddr;
extern PFN_vkDestroyInstance                            vkDestroyInstance;
extern PFN_vkEnumerateInstanceLayerProperties           vkEnumerateInstanceLayerProperties;
extern PFN_vkEnumerateInstanceExtensionProperties       vkEnumerateInstanceExtensionProperties;

// Surface
extern PFN_vkDestroySurfaceKHR                         vkDestroySurfaceKHR;

// Physical device properties
extern PFN_vkEnumeratePhysicalDevices                   vkEnumeratePhysicalDevices;
extern PFN_vkGetPhysicalDeviceProperties                vkGetPhysicalDeviceProperties;
extern PFN_vkGetPhysicalDeviceFeatures                  vkGetPhysicalDeviceFeatures;
extern PFN_vkEnumerateDeviceExtensionProperties         vkEnumerateDeviceExtensionProperties;
extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR         vkGetPhysicalDeviceSurfaceSupportKHR;
extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR         vkGetPhysicalDeviceSurfaceFormatsKHR;
extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR    vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
extern PFN_vkGetPhysicalDeviceSurfacePresentModesKHR    vkGetPhysicalDeviceSurfacePresentModesKHR;
extern PFN_vkGetPhysicalDeviceMemoryProperties          vkGetPhysicalDeviceMemoryProperties;

// Logic device
extern PFN_vkCreateDevice                               vkCreateDevice;
extern PFN_vkDestroyDevice                              vkDestroyDevice;
extern PFN_vkGetDeviceQueue                             vkGetDeviceQueue;
extern PFN_vkDeviceWaitIdle                             vkDeviceWaitIdle;

// Queue family
extern PFN_vkGetPhysicalDeviceQueueFamilyProperties     vkGetPhysicalDeviceQueueFamilyProperties;

// Queue objects
extern PFN_vkQueueSubmit                                vkQueueSubmit;
extern PFN_vkQueueWaitIdle                              vkQueueWaitIdle;

// Swapchain
extern PFN_vkCreateSwapchainKHR                         vkCreateSwapchainKHR;
extern PFN_vkDestroySwapchainKHR                        vkDestroySwapchainKHR;
extern PFN_vkGetSwapchainImagesKHR                      vkGetSwapchainImagesKHR;
extern PFN_vkQueuePresentKHR                            vkQueuePresentKHR;

//  Image
extern PFN_vkCreateImage                                vkCreateImage;
extern PFN_vkDestroyImage                               vkDestroyImage;                    

// Image View
extern PFN_vkCreateImageView                            vkCreateImageView;
extern PFN_vkDestroyImageView                           vkDestroyImageView;                        

// VK_EXT_debug_utils
extern PFN_vkCreateDebugUtilsMessengerEXT               vkCreateDebugUtilsMessengerEXT;
extern PFN_vkDestroyDebugUtilsMessengerEXT              vkDestroyDebugUtilsMessengerEXT;

// Shaders
extern PFN_vkCreateShaderModule                         vkCreateShaderModule;
extern PFN_vkDestroyShaderModule                        vkDestroyShaderModule;

// Pipeline 
extern PFN_vkCreateGraphicsPipelines                    vkCreateGraphicsPipelines;
extern PFN_vkCreateComputePipelines                     vkCreateComputePipelines;
extern PFN_vkDestroyPipeline                            vkDestroyPipeline;

// Pipeline Layout
extern PFN_vkCreatePipelineLayout                       vkCreatePipelineLayout;
extern PFN_vkDestroyPipelineLayout                      vkDestroyPipelineLayout;

// Render Pass
extern PFN_vkCreateRenderPass                           vkCreateRenderPass;
extern PFN_vkDestroyRenderPass                          vkDestroyRenderPass;

// Render cache
extern PFN_vkDestroyPipelineCache                       vkDestroyPipelineCache;
extern PFN_vkCreatePipelineCache                        vkCreatePipelineCache;
extern PFN_vkGetPipelineCacheData                       vkGetPipelineCacheData;
extern PFN_vkMergePipelineCaches                        vkMergePipelineCaches;

// Frame buffer 
extern PFN_vkCreateFramebuffer                          vkCreateFramebuffer;
extern PFN_vkDestroyFramebuffer                         vkDestroyFramebuffer;

// Memory
extern PFN_vkAllocateMemory                             vkAllocateMemory;
extern PFN_vkFreeMemory                                 vkFreeMemory;
extern PFN_vkMapMemory                                  vkMapMemory;
extern PFN_vkUnmapMemory                                vkUnmapMemory;
extern PFN_vkFlushMappedMemoryRanges                    vkFlushMappedMemoryRanges;
extern PFN_vkInvalidateMappedMemoryRanges               vkInvalidateMappedMemoryRanges;
extern PFN_vkBindBufferMemory                           vkBindBufferMemory;

// Buffer
extern PFN_vkCreateBuffer                               vkCreateBuffer;
extern PFN_vkCreateBufferView                           vkCreateBufferView;
extern PFN_vkDestroyBuffer                              vkDestroyBuffer;
extern PFN_vkDestroyBufferView                          vkDestroyBufferView;

// Comand Buffers
extern PFN_vkAllocateCommandBuffers                     vkAllocateCommandBuffers;
extern PFN_vkFreeCommandBuffers                         vkFreeCommandBuffers;
extern PFN_vkBeginCommandBuffer                         vkBeginCommandBuffer;
extern PFN_vkEndCommandBuffer                           vkEndCommandBuffer;
extern PFN_vkResetCommandBuffer                         vkResetCommandBuffer;

extern PFN_vkCmdBindPipeline                            vkCmdBindPipeline;
extern PFN_vkCmdSetViewport                             vkCmdSetViewport;
extern PFN_vkCmdSetScissor                              vkCmdSetScissor;
extern PFN_vkCmdSetLineWidth                            vkCmdSetLineWidth;
extern PFN_vkCmdSetDepthBias                            vkCmdSetDepthBias;
extern PFN_vkCmdSetBlendConstants                       vkCmdSetBlendConstants;
extern PFN_vkCmdSetDepthBounds                          vkCmdSetDepthBounds;
extern PFN_vkCmdSetStencilCompareMask                   vkCmdSetStencilCompareMask;
extern PFN_vkCmdSetStencilWriteMask                     vkCmdSetStencilWriteMask;
extern PFN_vkCmdSetStencilReference                     vkCmdSetStencilReference;
extern PFN_vkCmdBindDescriptorSets                      vkCmdBindDescriptorSets;
extern PFN_vkCmdBindIndexBuffer                         vkCmdBindIndexBuffer;
extern PFN_vkCmdBindVertexBuffers                       vkCmdBindVertexBuffers;
extern PFN_vkCmdDraw                                    vkCmdDraw;
extern PFN_vkCmdDrawIndexed                             vkCmdDrawIndexed;
extern PFN_vkCmdDrawIndirect                            vkCmdDrawIndirect;
extern PFN_vkCmdDrawIndexedIndirect                     vkCmdDrawIndexedIndirect;
extern PFN_vkCmdDispatch                                vkCmdDispatch;
extern PFN_vkCmdDispatchIndirect                        vkCmdDispatchIndirect;
extern PFN_vkCmdCopyBuffer                              vkCmdCopyBuffer;
extern PFN_vkCmdCopyImage                               vkCmdCopyImage;
extern PFN_vkCmdBlitImage                               vkCmdBlitImage;
extern PFN_vkCmdCopyBufferToImage                       vkCmdCopyBufferToImage;
extern PFN_vkCmdCopyImageToBuffer                       vkCmdCopyImageToBuffer;
extern PFN_vkCmdUpdateBuffer                            vkCmdUpdateBuffer;
extern PFN_vkCmdFillBuffer                              vkCmdFillBuffer;
extern PFN_vkCmdClearColorImage                         vkCmdClearColorImage;
extern PFN_vkCmdClearDepthStencilImage                  vkCmdClearDepthStencilImage;
extern PFN_vkCmdClearAttachments                        vkCmdClearAttachments;
extern PFN_vkCmdResolveImage                            vkCmdResolveImage;
extern PFN_vkCmdSetEvent                                vkCmdSetEvent;
extern PFN_vkCmdResetEvent                              vkCmdResetEvent;
extern PFN_vkCmdWaitEvents                              vkCmdWaitEvents;
extern PFN_vkCmdPipelineBarrier                         vkCmdPipelineBarrier;
extern PFN_vkCmdBeginQuery                              vkCmdBeginQuery;
extern PFN_vkCmdEndQuery                                vkCmdEndQuery;
extern PFN_vkCmdResetQueryPool                          vkCmdResetQueryPool;

/*
=================================================================================================

=================================================================================================
*/
class crVkException : public idException
{
public:
    crVkException( const char* _error, const VkResult errorCode ) : idException()
    {
        const char* errStr = nullptr;
        switch( errorCode )
        {
            case VK_SUCCESS: 
                errStr = "VK_SUCCESS";
                break;
            case VK_NOT_READY: 
                errStr = "VK_NOT_READY";
                break;
            case VK_TIMEOUT: 
                errStr = "VK_TIMEOUT";
                break;
            case VK_EVENT_SET: 
                errStr = "VK_EVENT_SET";
                break;
            case VK_EVENT_RESET: 
                errStr = "VK_EVENT_RESET";
                break;
            case VK_INCOMPLETE: 
                errStr = "VK_INCOMPLETE";
                break;
            case VK_ERROR_OUT_OF_HOST_MEMORY: 
                errStr = "VK_ERROR_OUT_OF_HOST_MEMORY";
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY: 
                errStr = "VK_ERROR_OUT_OF_DEVICE_MEMORY";
                break;
            case VK_ERROR_INITIALIZATION_FAILED: 
                errStr = "VK_ERROR_INITIALIZATION_FAILED";
                break;
            case VK_ERROR_DEVICE_LOST: 
                errStr = "VK_ERROR_DEVICE_LOST";
                break;
            case VK_ERROR_MEMORY_MAP_FAILED: 
                errStr = "VK_ERROR_MEMORY_MAP_FAILED";
                break;
            case VK_ERROR_LAYER_NOT_PRESENT: 
                errStr = "VK_ERROR_LAYER_NOT_PRESENT";
                break;
            case VK_ERROR_EXTENSION_NOT_PRESENT: 
                errStr = "VK_ERROR_EXTENSION_NOT_PRESENT";
                break;
            case VK_ERROR_FEATURE_NOT_PRESENT: 
                errStr = "VK_ERROR_FEATURE_NOT_PRESENT";
                break;
            case VK_ERROR_INCOMPATIBLE_DRIVER: 
                errStr = "VK_ERROR_INCOMPATIBLE_DRIVER";
                break;
            case VK_ERROR_TOO_MANY_OBJECTS: 
                errStr = "VK_ERROR_TOO_MANY_OBJECTS";
                break;
            case VK_ERROR_FORMAT_NOT_SUPPORTED: 
                errStr = "VK_ERROR_FORMAT_NOT_SUPPORTED";
                break;
            case VK_ERROR_FRAGMENTED_POOL: 
                errStr = "VK_ERROR_FRAGMENTED_POOL";
                break;
            case VK_ERROR_UNKNOWN: 
                errStr = "VK_ERROR_UNKNOWN";
                break;
            case VK_ERROR_OUT_OF_POOL_MEMORY: 
                errStr = "VK_ERROR_OUT_OF_POOL_MEMORY";
                break;
            case VK_ERROR_INVALID_EXTERNAL_HANDLE: 
                errStr = "VK_ERROR_INVALID_EXTERNAL_HANDLE";
                break;
            case VK_ERROR_FRAGMENTATION: 
                errStr = "VK_ERROR_FRAGMENTATION";
                break;
            case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: 
                errStr = "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
                break;
            case VK_PIPELINE_COMPILE_REQUIRED: 
                errStr = "VK_PIPELINE_COMPILE_REQUIRED";
                break;
            case VK_ERROR_SURFACE_LOST_KHR: 
                errStr = "VK_ERROR_SURFACE_LOST_KHR";
                break;
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: 
                errStr = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
                break;
            case VK_SUBOPTIMAL_KHR: 
                errStr = "VK_SUBOPTIMAL_KHR";
                break;
            case VK_ERROR_OUT_OF_DATE_KHR: 
                errStr = "VK_ERROR_OUT_OF_DATE_KHR"; 
                break;
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: 
                errStr = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"; 
                break;
            case VK_ERROR_VALIDATION_FAILED_EXT: 
                errStr = "VK_ERROR_VALIDATION_FAILED_EXT"; 
                break;
            case VK_ERROR_INVALID_SHADER_NV: 
                errStr = "VK_ERROR_INVALID_SHADER_NV"; 
                break;
        #ifdef VK_ENABLE_BETA_EXTENSIONS
            case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR: 
                errStr = "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
                break;;
        #endif
        #ifdef VK_ENABLE_BETA_EXTENSIONS
            case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR: 
                errStr = "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
                break;
        #endif
        #ifdef VK_ENABLE_BETA_EXTENSIONS
                case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR: 
                errStr = "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
                break;
        #endif
        #ifdef VK_ENABLE_BETA_EXTENSIONS
            case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR: 
                errStr = "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
                break;
        #endif
        #ifdef VK_ENABLE_BETA_EXTENSIONS
            case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR: 
                errStr = "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
                break;
        #endif
        #ifdef VK_ENABLE_BETA_EXTENSIONS
            case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR: 
                errStr = "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
                break;
        #endif
            case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: 
                errStr = "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"; 
                break;
            case VK_ERROR_NOT_PERMITTED_KHR: 
                errStr = "VK_ERROR_NOT_PERMITTED_KHR"; 
                break;
            case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: 
                errStr = "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT"; 
                break;
            case VK_THREAD_IDLE_KHR: 
                errStr = "VK_THREAD_IDLE_KHR"; 
                break;
            case VK_THREAD_DONE_KHR: 
                errStr = "VK_THREAD_DONE_KHR"; 
                break;
            case VK_OPERATION_DEFERRED_KHR: 
                errStr = "VK_OPERATION_DEFERRED_KHR"; 
                break;
            case VK_OPERATION_NOT_DEFERRED_KHR: 
                errStr = "VK_OPERATION_NOT_DEFERRED_KHR"; 
                break;
            case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: 
                errStr = "VK_ERROR_COMPRESSION_EXHAUSTED_EXT"; 
                break;
            case VK_RESULT_MAX_ENUM: 
                errStr = "VK_RESULT_MAX_ENUM"; 
                break;
            
            default: 
                errStr = "??????";
                break;
        };

        idException( "VkError: %s\n Code: %i\n %s\n", _error, errorCode, errStr );
    }
};

#endif //__QVULKAN_H__