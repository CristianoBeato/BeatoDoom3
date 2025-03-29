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

#include "qvk.h"

// Instance properties
PFN_vkCreateInstance                            vkCreateInstance = nullptr;
PFN_vkGetInstanceProcAddr                       vkGetInstanceProcAddr = nullptr;
PFN_vkDestroyInstance                           vkDestroyInstance = nullptr;
PFN_vkEnumerateInstanceLayerProperties          vkEnumerateInstanceLayerProperties = nullptr;
PFN_vkEnumerateInstanceExtensionProperties      vkEnumerateInstanceExtensionProperties = nullptr;

// Vulkan Surface
PFN_vkDestroySurfaceKHR                         vkDestroySurfaceKHR = nullptr;

// Physical device properties
PFN_vkEnumeratePhysicalDevices                  vkEnumeratePhysicalDevices = nullptr;
PFN_vkGetPhysicalDeviceProperties               vkGetPhysicalDeviceProperties = nullptr;
PFN_vkGetPhysicalDeviceFeatures                 vkGetPhysicalDeviceFeatures = nullptr;
PFN_vkEnumerateDeviceExtensionProperties        vkEnumerateDeviceExtensionProperties = nullptr;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR        vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR        vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR   vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR   vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
PFN_vkGetPhysicalDeviceMemoryProperties         vkGetPhysicalDeviceMemoryProperties = nullptr;

// Logic device
PFN_vkCreateDevice                              vkCreateDevice = nullptr;
PFN_vkDestroyDevice                             vkDestroyDevice = nullptr;
PFN_vkDeviceWaitIdle                            vkDeviceWaitIdle = nullptr;
PFN_vkGetDeviceQueue                            vkGetDeviceQueue = nullptr;

// Queue family
PFN_vkGetPhysicalDeviceQueueFamilyProperties    vkGetPhysicalDeviceQueueFamilyProperties = nullptr;

// Queue objects
PFN_vkQueueSubmit                               vkQueueSubmit = nullptr;
PFN_vkQueueWaitIdle                             vkQueueWaitIdle = nullptr;

// Swap Chain
PFN_vkCreateSwapchainKHR                         vkCreateSwapchainKHR = nullptr;
PFN_vkDestroySwapchainKHR                        vkDestroySwapchainKHR = nullptr;
PFN_vkGetSwapchainImagesKHR                      vkGetSwapchainImagesKHR = nullptr;
PFN_vkQueuePresentKHR                            vkQueuePresentKHR = nullptr;

//  Image
PFN_vkCreateImage                                vkCreateImage = nullptr;
PFN_vkDestroyImage                               vkDestroyImage = nullptr;

// Image View
PFN_vkCreateImageView                            vkCreateImageView = nullptr;
PFN_vkDestroyImageView                           vkDestroyImageView = nullptr;

// VK_EXT_debug_utils
PFN_vkCreateDebugUtilsMessengerEXT              vkCreateDebugUtilsMessengerEXT = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT             vkDestroyDebugUtilsMessengerEXT = nullptr;

// Shaders
PFN_vkCreateShaderModule                        vkCreateShaderModule = nullptr;
PFN_vkDestroyShaderModule                       vkDestroyShaderModule = nullptr;

// Pipeline
PFN_vkCreateGraphicsPipelines                   vkCreateGraphicsPipelines = nullptr;
PFN_vkCreateComputePipelines                    vkCreateComputePipelines = nullptr;
PFN_vkDestroyPipeline                           vkDestroyPipeline = nullptr;

// Pipeline Layout
PFN_vkCreatePipelineLayout                      vkCreatePipelineLayout = nullptr;
PFN_vkDestroyPipelineLayout                     vkDestroyPipelineLayout = nullptr;

// Render Pass
PFN_vkCreateRenderPass                          vkCreateRenderPass = nullptr;
PFN_vkDestroyRenderPass                         vkDestroyRenderPass = nullptr;

// Pipeline Cache
PFN_vkDestroyPipelineCache                      vkDestroyPipelineCache = nullptr;
PFN_vkCreatePipelineCache                       vkCreatePipelineCache = nullptr;
PFN_vkGetPipelineCacheData                      vkGetPipelineCacheData = nullptr;
PFN_vkMergePipelineCaches                       vkMergePipelineCaches = nullptr;

//
PFN_vkCreateFramebuffer                         vkCreateFramebuffer = nullptr;
PFN_vkDestroyFramebuffer                        vkDestroyFramebuffer = nullptr;

//
PFN_vkAllocateMemory                            vkAllocateMemory = nullptr;
PFN_vkFreeMemory                                vkFreeMemory = nullptr;
PFN_vkMapMemory                                 vkMapMemory = nullptr;
PFN_vkUnmapMemory                               vkUnmapMemory = nullptr;
PFN_vkFlushMappedMemoryRanges                   vkFlushMappedMemoryRanges = nullptr;
PFN_vkInvalidateMappedMemoryRanges              vkInvalidateMappedMemoryRanges = nullptr;
PFN_vkBindBufferMemory                          vkBindBufferMemory = nullptr;

PFN_vkCreateBuffer                              vkCreateBuffer = nullptr;
PFN_vkCreateBufferView                          vkCreateBufferView = nullptr;
PFN_vkDestroyBuffer                             vkDestroyBuffer = nullptr;
PFN_vkDestroyBufferView                         vkDestroyBufferView = nullptr;

// Comand Buffers
PFN_vkAllocateCommandBuffers                     vkAllocateCommandBuffers = nullptr;
PFN_vkFreeCommandBuffers                         vkFreeCommandBuffers = nullptr;
PFN_vkBeginCommandBuffer                         vkBeginCommandBuffer = nullptr;
PFN_vkEndCommandBuffer                           vkEndCommandBuffer = nullptr;
PFN_vkResetCommandBuffer                         vkResetCommandBuffer = nullptr;

PFN_vkCmdBindPipeline                            vkCmdBindPipeline = nullptr;
PFN_vkCmdSetViewport                             vkCmdSetViewport = nullptr;
PFN_vkCmdSetScissor                              vkCmdSetScissor = nullptr;
PFN_vkCmdSetLineWidth                            vkCmdSetLineWidth = nullptr;
PFN_vkCmdSetDepthBias                            vkCmdSetDepthBias = nullptr;
PFN_vkCmdSetBlendConstants                       vkCmdSetBlendConstants = nullptr;
PFN_vkCmdSetDepthBounds                          vkCmdSetDepthBounds = nullptr;
PFN_vkCmdSetStencilCompareMask                   vkCmdSetStencilCompareMask = nullptr;
PFN_vkCmdSetStencilWriteMask                     vkCmdSetStencilWriteMask = nullptr;
PFN_vkCmdSetStencilReference                     vkCmdSetStencilReference = nullptr;
PFN_vkCmdBindDescriptorSets                      vkCmdBindDescriptorSets = nullptr;
PFN_vkCmdBindIndexBuffer                         vkCmdBindIndexBuffer = nullptr;
PFN_vkCmdBindVertexBuffers                       vkCmdBindVertexBuffers = nullptr;
PFN_vkCmdDraw                                    vkCmdDraw = nullptr;
PFN_vkCmdDrawIndexed                             vkCmdDrawIndexed = nullptr;
PFN_vkCmdDrawIndirect                            vkCmdDrawIndirect = nullptr;
PFN_vkCmdDrawIndexedIndirect                     vkCmdDrawIndexedIndirect = nullptr;
PFN_vkCmdDispatch                                vkCmdDispatch = nullptr;
PFN_vkCmdDispatchIndirect                        vkCmdDispatchIndirect = nullptr;
PFN_vkCmdCopyBuffer                              vkCmdCopyBuffer = nullptr;
PFN_vkCmdCopyImage                               vkCmdCopyImage = nullptr;
PFN_vkCmdBlitImage                               vkCmdBlitImage = nullptr;
PFN_vkCmdCopyBufferToImage                       vkCmdCopyBufferToImage = nullptr;
PFN_vkCmdCopyImageToBuffer                       vkCmdCopyImageToBuffer = nullptr;
PFN_vkCmdUpdateBuffer                            vkCmdUpdateBuffer = nullptr;
PFN_vkCmdFillBuffer                              vkCmdFillBuffer = nullptr;
PFN_vkCmdClearColorImage                         vkCmdClearColorImage = nullptr;
PFN_vkCmdClearDepthStencilImage                  vkCmdClearDepthStencilImage = nullptr;
PFN_vkCmdClearAttachments                        vkCmdClearAttachments = nullptr;
PFN_vkCmdResolveImage                            vkCmdResolveImage = nullptr;
PFN_vkCmdSetEvent                                vkCmdSetEvent = nullptr;
PFN_vkCmdResetEvent                              vkCmdResetEvent = nullptr;
PFN_vkCmdWaitEvents                              vkCmdWaitEvents = nullptr;
PFN_vkCmdPipelineBarrier                         vkCmdPipelineBarrier = nullptr;
PFN_vkCmdBeginQuery                              vkCmdBeginQuery = nullptr;
PFN_vkCmdEndQuery                                vkCmdEndQuery = nullptr;
PFN_vkCmdResetQueryPool                          vkCmdResetQueryPool = nullptr;

template< typename _t >
inline VkBool32 vkGetInstaProc( const VkInstance instance, _t &proc, const char* name )
{
    return ( ( proc = reinterpret_cast<_t>( vkGetInstanceProcAddr( instance, name ) ) ) != nullptr ) ? VK_TRUE : VK_FALSE;
}
#define VK_LOAD_PROC( X , I ) assert( vkGetInstaProc( I, X, #X ) == VK_TRUE )

// Get vulkan instance functions
void LoadVulkanFunctions( const VkInstance instance )
{
    // Instance properties
    VK_LOAD_PROC( vkDestroyInstance, instance );
    VK_LOAD_PROC( vkEnumerateInstanceExtensionProperties, instance );
    
    // Surface
    VK_LOAD_PROC( vkDestroySurfaceKHR, instance );
    
    // Physical Device
    VK_LOAD_PROC( vkEnumeratePhysicalDevices, instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceProperties, instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceFeatures, instance );
    VK_LOAD_PROC( vkEnumerateDeviceExtensionProperties, instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceSurfaceSupportKHR, instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceSurfaceFormatsKHR, instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceSurfaceCapabilitiesKHR, instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceSurfacePresentModesKHR, instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceMemoryProperties, instance );

    // Logical device
    VK_LOAD_PROC( vkCreateDevice, instance );
    VK_LOAD_PROC( vkDestroyDevice, instance );
    VK_LOAD_PROC( vkGetDeviceQueue, instance );
    VK_LOAD_PROC( vkDeviceWaitIdle, instance );

    VK_LOAD_PROC( vkGetPhysicalDeviceQueueFamilyProperties, instance );

    //
    VK_LOAD_PROC( vkQueueSubmit, instance );
    VK_LOAD_PROC( vkQueueWaitIdle, instance );

    //
    VK_LOAD_PROC( vkCreateSwapchainKHR, instance );
    VK_LOAD_PROC( vkDestroySwapchainKHR, instance );
    VK_LOAD_PROC( vkGetSwapchainImagesKHR, instance );
    VK_LOAD_PROC( vkQueuePresentKHR, instance );

    //
    VK_LOAD_PROC( vkCreateImage, instance );
    VK_LOAD_PROC( vkDestroyImage, instance );

    //
    VK_LOAD_PROC( vkCreateImageView, instance );
    VK_LOAD_PROC( vkDestroyImageView, instance );

    //
    VK_LOAD_PROC( vkCreateDebugUtilsMessengerEXT, instance );
    VK_LOAD_PROC( vkDestroyDebugUtilsMessengerEXT, instance );

    //
    VK_LOAD_PROC( vkCreateShaderModule, instance );
    VK_LOAD_PROC( vkDestroyShaderModule, instance );

    //
    VK_LOAD_PROC( vkCreateGraphicsPipelines, instance );
    VK_LOAD_PROC( vkCreateComputePipelines, instance );
    VK_LOAD_PROC( vkDestroyPipeline, instance );
    VK_LOAD_PROC( vkCreatePipelineLayout, instance );
    VK_LOAD_PROC( vkDestroyPipelineLayout, instance );

    // 
    VK_LOAD_PROC( vkCreateRenderPass, instance );
    VK_LOAD_PROC( vkDestroyRenderPass, instance );

    // Pipeline 
    VK_LOAD_PROC( vkDestroyPipelineCache, instance );
    VK_LOAD_PROC( vkCreatePipelineCache, instance );
    VK_LOAD_PROC( vkGetPipelineCacheData, instance );
    VK_LOAD_PROC( vkMergePipelineCaches, instance );

    // Frame Buffer 
    VK_LOAD_PROC( vkCreateFramebuffer, instance );
    VK_LOAD_PROC( vkDestroyFramebuffer, instance );

    // Memory control 
    VK_LOAD_PROC( vkAllocateMemory, instance );
    VK_LOAD_PROC( vkFreeMemory, instance );
    VK_LOAD_PROC( vkMapMemory, instance );
    VK_LOAD_PROC( vkUnmapMemory, instance );
    VK_LOAD_PROC( vkFlushMappedMemoryRanges, instance );
    VK_LOAD_PROC( vkInvalidateMappedMemoryRanges, instance );
    VK_LOAD_PROC( vkBindBufferMemory, instance );

    // Buffer object 
    VK_LOAD_PROC( vkCreateBuffer, instance );
    VK_LOAD_PROC( vkCreateBufferView, instance );
    VK_LOAD_PROC( vkDestroyBuffer, instance );
    VK_LOAD_PROC( vkDestroyBufferView, instance );

    // Command buffer 
    VK_LOAD_PROC( vkAllocateCommandBuffers, instance );
    VK_LOAD_PROC( vkFreeCommandBuffers, instance );
    VK_LOAD_PROC( vkBeginCommandBuffer, instance );
    VK_LOAD_PROC( vkEndCommandBuffer, instance );
    VK_LOAD_PROC( vkResetCommandBuffer, instance );

    // Comand buffer 
    VK_LOAD_PROC( vkCmdBindPipeline, instance );
    VK_LOAD_PROC( vkCmdSetViewport, instance );
    VK_LOAD_PROC( vkCmdSetScissor, instance );
    VK_LOAD_PROC( vkCmdSetLineWidth, instance );
    VK_LOAD_PROC( vkCmdSetDepthBias, instance );
    VK_LOAD_PROC( vkCmdSetBlendConstants, instance );
    VK_LOAD_PROC( vkCmdSetDepthBounds, instance );
    VK_LOAD_PROC( vkCmdSetStencilCompareMask, instance );
    VK_LOAD_PROC( vkCmdSetStencilWriteMask, instance );
    VK_LOAD_PROC( vkCmdSetStencilReference, instance );
    VK_LOAD_PROC( vkCmdBindDescriptorSets, instance );
    VK_LOAD_PROC( vkCmdBindIndexBuffer, instance );
    VK_LOAD_PROC( vkCmdBindVertexBuffers, instance );
    VK_LOAD_PROC( vkCmdDraw, instance );
    VK_LOAD_PROC( vkCmdDrawIndexed, instance );
    VK_LOAD_PROC( vkCmdDrawIndirect, instance );
    VK_LOAD_PROC( vkCmdDrawIndexedIndirect, instance );
    VK_LOAD_PROC( vkCmdDispatch, instance );
    VK_LOAD_PROC( vkCmdDispatchIndirect, instance );
    VK_LOAD_PROC( vkCmdCopyBuffer, instance );
    VK_LOAD_PROC( vkCmdCopyImage, instance );
    VK_LOAD_PROC( vkCmdBlitImage, instance );
    VK_LOAD_PROC( vkCmdCopyBufferToImage, instance );
    VK_LOAD_PROC( vkCmdCopyImageToBuffer, instance );
    VK_LOAD_PROC( vkCmdUpdateBuffer, instance );
    VK_LOAD_PROC( vkCmdFillBuffer, instance );
    VK_LOAD_PROC( vkCmdClearColorImage, instance );
    VK_LOAD_PROC( vkCmdClearDepthStencilImage, instance );
    VK_LOAD_PROC( vkCmdClearAttachments, instance );
    VK_LOAD_PROC( vkCmdResolveImage, instance );
    VK_LOAD_PROC( vkCmdSetEvent, instance );
    VK_LOAD_PROC( vkCmdResetEvent, instance );
    VK_LOAD_PROC( vkCmdWaitEvents, instance );
    VK_LOAD_PROC( vkCmdPipelineBarrier, instance );
    VK_LOAD_PROC( vkCmdBeginQuery, instance );
    VK_LOAD_PROC( vkCmdEndQuery, instance );
    VK_LOAD_PROC( vkCmdResetQueryPool, instance );
 }