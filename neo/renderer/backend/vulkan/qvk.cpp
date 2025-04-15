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
#include "vkContext.h"

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
PFN_vkGetPhysicalDeviceFormatProperties         vkGetPhysicalDeviceFormatProperties = nullptr;

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
PFN_vkAcquireNextImageKHR                        vkAcquireNextImageKHR = nullptr;

//  Image
PFN_vkCreateImage                                vkCreateImage = nullptr;
PFN_vkDestroyImage                               vkDestroyImage = nullptr;

// Image View
PFN_vkCreateImageView                            vkCreateImageView = nullptr;
PFN_vkDestroyImageView                           vkDestroyImageView = nullptr;

// sampler
PFN_vkCreateSampler                             vkCreateSampler = nullptr;
PFN_vkDestroySampler                            vkDestroySampler = nullptr;

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
PFN_vkBindImageMemory                           vkBindImageMemory = nullptr;
PFN_vkGetBufferMemoryRequirements               vkGetBufferMemoryRequirements = nullptr;
PFN_vkGetImageMemoryRequirements                vkGetImageMemoryRequirements = nullptr;

// Buffer
PFN_vkCreateBuffer                              vkCreateBuffer = nullptr;
PFN_vkCreateBufferView                          vkCreateBufferView = nullptr;
PFN_vkDestroyBuffer                             vkDestroyBuffer = nullptr;
PFN_vkDestroyBufferView                         vkDestroyBufferView = nullptr;

// decriptros 
PFN_vkCreateDescriptorSetLayout                 vkCreateDescriptorSetLayout = nullptr;
PFN_vkDestroyDescriptorSetLayout                vkDestroyDescriptorSetLayout = nullptr;
PFN_vkCreateDescriptorPool                      vkCreateDescriptorPool = nullptr;
PFN_vkDestroyDescriptorPool                     vkDestroyDescriptorPool = nullptr;
PFN_vkAllocateDescriptorSets                    vkAllocateDescriptorSets = nullptr;
PFN_vkFreeDescriptorSets                        vkFreeDescriptorSets = nullptr;
PFN_vkUpdateDescriptorSets                      vkUpdateDescriptorSets = nullptr;

// Fence
PFN_vkCreateFence                                vkCreateFence = nullptr;
PFN_vkDestroyFence                               vkDestroyFence = nullptr;
PFN_vkWaitForFences                              vkWaitForFences = nullptr;
PFN_vkResetFences                                vkResetFences = nullptr;
PFN_vkGetFenceStatus                             vkGetFenceStatus = nullptr;


PFN_vkCreateCommandPool                          vkCreateCommandPool = nullptr;
PFN_vkDestroyCommandPool                         vkDestroyCommandPool = nullptr;
PFN_vkResetCommandPool                           vkResetCommandPool = nullptr;

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
inline void vkGetInstaProc( const VkInstance instance, _t &proc, const char* name )
{
    assert( ( proc = reinterpret_cast<_t>( vkGetInstanceProcAddr( instance, name ) ) ) != nullptr );
}
#define VK_LOAD_PROC( X , I ) vkGetInstaProc( I, X, #X )

// Get vulkan instance functions
void crVulkanContext::LoadVulkanFunctions( void )
{
    // Instance properties
    VK_LOAD_PROC( vkDestroyInstance, m_instance );
    VK_LOAD_PROC( vkEnumerateInstanceExtensionProperties, m_instance );
    
    // Surface
    VK_LOAD_PROC( vkDestroySurfaceKHR, m_instance );
    
    // Physical Device
    VK_LOAD_PROC( vkEnumeratePhysicalDevices, m_instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceProperties, m_instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceFeatures, m_instance );
    VK_LOAD_PROC( vkEnumerateDeviceExtensionProperties, m_instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceSurfaceSupportKHR, m_instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceSurfaceFormatsKHR, m_instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceSurfaceCapabilitiesKHR, m_instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceSurfacePresentModesKHR, m_instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceMemoryProperties, m_instance );
    VK_LOAD_PROC( vkGetPhysicalDeviceFormatProperties, m_instance );

    // Logical device
    VK_LOAD_PROC( vkCreateDevice, m_instance );
    VK_LOAD_PROC( vkDestroyDevice, m_instance );
    VK_LOAD_PROC( vkGetDeviceQueue, m_instance );
    VK_LOAD_PROC( vkDeviceWaitIdle, m_instance );

    VK_LOAD_PROC( vkGetPhysicalDeviceQueueFamilyProperties, m_instance );

    //
    VK_LOAD_PROC( vkQueueSubmit, m_instance );
    VK_LOAD_PROC( vkQueueWaitIdle, m_instance );

    //
    VK_LOAD_PROC( vkCreateSwapchainKHR, m_instance );
    VK_LOAD_PROC( vkDestroySwapchainKHR, m_instance );
    VK_LOAD_PROC( vkGetSwapchainImagesKHR, m_instance );
    VK_LOAD_PROC( vkQueuePresentKHR, m_instance );
    VK_LOAD_PROC( vkAcquireNextImageKHR, m_instance );

    //
    VK_LOAD_PROC( vkCreateImage, m_instance );
    VK_LOAD_PROC( vkDestroyImage, m_instance );

    //
    VK_LOAD_PROC( vkCreateImageView, m_instance );
    VK_LOAD_PROC( vkDestroyImageView, m_instance );

    //
    VK_LOAD_PROC( vkCreateSampler, m_instance );
    VK_LOAD_PROC( vkDestroySampler, m_instance );

    //
    VK_LOAD_PROC( vkCreateDebugUtilsMessengerEXT, m_instance );
    VK_LOAD_PROC( vkDestroyDebugUtilsMessengerEXT, m_instance );

    //
    VK_LOAD_PROC( vkCreateShaderModule, m_instance );
    VK_LOAD_PROC( vkDestroyShaderModule, m_instance );

    //
    VK_LOAD_PROC( vkCreateGraphicsPipelines, m_instance );
    VK_LOAD_PROC( vkCreateComputePipelines, m_instance );
    VK_LOAD_PROC( vkDestroyPipeline, m_instance );
    VK_LOAD_PROC( vkCreatePipelineLayout, m_instance );
    VK_LOAD_PROC( vkDestroyPipelineLayout, m_instance );

    // 
    VK_LOAD_PROC( vkCreateRenderPass, m_instance );
    VK_LOAD_PROC( vkDestroyRenderPass, m_instance );

    // Pipeline 
    VK_LOAD_PROC( vkDestroyPipelineCache, m_instance );
    VK_LOAD_PROC( vkCreatePipelineCache, m_instance );
    VK_LOAD_PROC( vkGetPipelineCacheData, m_instance );
    VK_LOAD_PROC( vkMergePipelineCaches, m_instance );

    // Frame Buffer 
    VK_LOAD_PROC( vkCreateFramebuffer, m_instance );
    VK_LOAD_PROC( vkDestroyFramebuffer, m_instance );

    // Memory control 
    VK_LOAD_PROC( vkAllocateMemory, m_instance );
    VK_LOAD_PROC( vkFreeMemory, m_instance );
    VK_LOAD_PROC( vkMapMemory, m_instance );
    VK_LOAD_PROC( vkUnmapMemory, m_instance );
    VK_LOAD_PROC( vkFlushMappedMemoryRanges, m_instance );
    VK_LOAD_PROC( vkInvalidateMappedMemoryRanges, m_instance );
    VK_LOAD_PROC( vkBindBufferMemory, m_instance );
    VK_LOAD_PROC( vkBindImageMemory, m_instance );
    VK_LOAD_PROC( vkGetBufferMemoryRequirements, m_instance );
    VK_LOAD_PROC( vkGetImageMemoryRequirements, m_instance );

    // Buffer object 
    VK_LOAD_PROC( vkCreateBuffer, m_instance );
    VK_LOAD_PROC( vkCreateBufferView, m_instance );
    VK_LOAD_PROC( vkDestroyBuffer, m_instance );
    VK_LOAD_PROC( vkDestroyBufferView, m_instance );

    // decriptors 
    VK_LOAD_PROC( vkCreateDescriptorSetLayout, m_instance );
    VK_LOAD_PROC( vkDestroyDescriptorSetLayout, m_instance );
    VK_LOAD_PROC( vkCreateDescriptorPool, m_instance );
    VK_LOAD_PROC( vkDestroyDescriptorPool, m_instance );
    VK_LOAD_PROC( vkAllocateDescriptorSets, m_instance );
    VK_LOAD_PROC( vkFreeDescriptorSets, m_instance );
    VK_LOAD_PROC( vkUpdateDescriptorSets, m_instance );

    //
    VK_LOAD_PROC( vkCreateFence, m_instance );
    VK_LOAD_PROC( vkDestroyFence, m_instance );
    VK_LOAD_PROC( vkWaitForFences, m_instance );
    VK_LOAD_PROC( vkResetFences, m_instance );
    VK_LOAD_PROC( vkGetFenceStatus, m_instance );

    // Command buffer 
    VK_LOAD_PROC( vkAllocateCommandBuffers, m_instance );
    VK_LOAD_PROC( vkFreeCommandBuffers, m_instance );
    VK_LOAD_PROC( vkBeginCommandBuffer, m_instance );
    VK_LOAD_PROC( vkEndCommandBuffer, m_instance );
    VK_LOAD_PROC( vkResetCommandBuffer, m_instance );

    //
    VK_LOAD_PROC( vkAllocateCommandBuffers, m_instance );
    VK_LOAD_PROC( vkFreeCommandBuffers, m_instance );
    VK_LOAD_PROC( vkBeginCommandBuffer, m_instance );
    VK_LOAD_PROC( vkEndCommandBuffer, m_instance );
    VK_LOAD_PROC( vkResetCommandBuffer, m_instance );

    // Comand buffer 
    VK_LOAD_PROC( vkCmdBindPipeline, m_instance );
    VK_LOAD_PROC( vkCmdSetViewport, m_instance );
    VK_LOAD_PROC( vkCmdSetScissor, m_instance );
    VK_LOAD_PROC( vkCmdSetLineWidth, m_instance );
    VK_LOAD_PROC( vkCmdSetDepthBias, m_instance );
    VK_LOAD_PROC( vkCmdSetBlendConstants, m_instance );
    VK_LOAD_PROC( vkCmdSetDepthBounds, m_instance );
    VK_LOAD_PROC( vkCmdSetStencilCompareMask, m_instance );
    VK_LOAD_PROC( vkCmdSetStencilWriteMask, m_instance );
    VK_LOAD_PROC( vkCmdSetStencilReference, m_instance );
    VK_LOAD_PROC( vkCmdBindDescriptorSets, m_instance );
    VK_LOAD_PROC( vkCmdBindIndexBuffer, m_instance );
    VK_LOAD_PROC( vkCmdBindVertexBuffers, m_instance );
    VK_LOAD_PROC( vkCmdDraw, m_instance );
    VK_LOAD_PROC( vkCmdDrawIndexed, m_instance );
    VK_LOAD_PROC( vkCmdDrawIndirect, m_instance );
    VK_LOAD_PROC( vkCmdDrawIndexedIndirect, m_instance );
    VK_LOAD_PROC( vkCmdDispatch, m_instance );
    VK_LOAD_PROC( vkCmdDispatchIndirect, m_instance );
    VK_LOAD_PROC( vkCmdCopyBuffer, m_instance );
    VK_LOAD_PROC( vkCmdCopyImage, m_instance );
    VK_LOAD_PROC( vkCmdBlitImage, m_instance );
    VK_LOAD_PROC( vkCmdCopyBufferToImage, m_instance );
    VK_LOAD_PROC( vkCmdCopyImageToBuffer, m_instance );
    VK_LOAD_PROC( vkCmdUpdateBuffer, m_instance );
    VK_LOAD_PROC( vkCmdFillBuffer, m_instance );
    VK_LOAD_PROC( vkCmdClearColorImage, m_instance );
    VK_LOAD_PROC( vkCmdClearDepthStencilImage, m_instance );
    VK_LOAD_PROC( vkCmdClearAttachments, m_instance );
    VK_LOAD_PROC( vkCmdResolveImage, m_instance );
    VK_LOAD_PROC( vkCmdSetEvent, m_instance );
    VK_LOAD_PROC( vkCmdResetEvent, m_instance );
    VK_LOAD_PROC( vkCmdWaitEvents, m_instance );
    VK_LOAD_PROC( vkCmdPipelineBarrier, m_instance );
    VK_LOAD_PROC( vkCmdBeginQuery, m_instance );
    VK_LOAD_PROC( vkCmdEndQuery, m_instance );
    VK_LOAD_PROC( vkCmdResetQueryPool, m_instance );
}
