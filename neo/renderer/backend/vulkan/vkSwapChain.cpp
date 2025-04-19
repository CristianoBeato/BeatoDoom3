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
#include "vkSwapChain.h"

crVKSwapChain::crVKSwapChain( void ) : 
    crSwapChain(),
    m_swapChain( 0 ),
    m_renderPass( 0 ),
    m_presentQueue( 0 )
{
}

crVKSwapChain::~crVKSwapChain( void )
{
    Destroy();
}

static bool isSupportedFormat( VkSurfaceFormatKHR requiredSurfaceFormat, idList<VkSurfaceFormatKHR> supported )
{
    //m_surfaceFormat = surfaceFormats[0];
    for ( uint32_t i = 0; i < supported.Num(); i++)
    {
        auto format = supported[i];
        if ( format.format == requiredSurfaceFormat.format && format.colorSpace == requiredSurfaceFormat.colorSpace ) 
            return true;
    }

    return false;
}

void crVKSwapChain::Create( const uint32_t width, const uint32_t height, const uint32_t vsync, const uint32_t samples )
{
    VkResult res = VK_SUCCESS;
    uint32_t graphicQueue = tr.vulkan->GetDeviceGraphicQueue();
    uint32_t presentQueue = tr.vulkan->GetDevicePresnetQueue();
    SuportedFeatures suportedFeatures = tr.vulkan->GetDeviceSuportedFeaturesFlags();
    uint32_t queueFamilyIndices[2];
    VkSwapchainCreateInfoKHR swapchainCI{};
    VkImageViewCreateInfo imageViewCI; 
    VkAttachmentDescription colorAttachment{};
    VkAttachmentReference colorAttachmentRef{};
    VkSubpassDescription subpass{};
    VkRenderPassCreateInfo renderPassCI{};
    VkFramebufferCreateInfo framebufferCI{};
    VkFenceCreateInfo fenceCI{};
    auto device = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetDevice();
    VkExtent2D deviceSurfaceExtent = tr.vulkan->GetDeviceSurfaceExtent();
    VkExtent2D currentExtent{ width, height };

    m_width = width;
    m_height = height;

    // todo use a engine common format for OpenGL and  
    VkSurfaceFormatKHR format{ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    auto formatList = tr.vulkan->GetDeviceSurfaceFormatList();
    if( !isSupportedFormat( format, formatList ) )
    {
        // get the first suported format 
        format = formatList[0]; // 
    }

    // format and color space 
    swapchainCI.imageFormat = format.format;
    swapchainCI.imageColorSpace = format.colorSpace;

    if ( deviceSurfaceExtent.width != std::numeric_limits<uint32_t>::max() || deviceSurfaceExtent.height != std::numeric_limits<uint32_t>::max() ) 
    {
        m_swapChainExtent = currentExtent;
    } 
    else 
    {
        auto minImageExtent = tr.vulkan->GetDeviceSurfaceMinExtent();
        auto maxImageExtent = tr.vulkan->GetDeviceSurfaceMaxExtent();
        m_swapChainExtent.width = std::clamp( currentExtent.width, minImageExtent.width, maxImageExtent.width );
        m_swapChainExtent.height = std::clamp( currentExtent.height, minImageExtent.height, maxImageExtent.height );
    }
    

    
    //
    swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCI.surface = tr.vulkan->GetSurface();
    swapchainCI.imageColorSpace = m_surfaceFormat.colorSpace;
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCI.clipped = VK_TRUE;
    swapchainCI.minImageCount = tr.vulkan->GetDeviceSurfaceImagesCount();
    swapchainCI.imageExtent =  m_swapChainExtent;
    swapchainCI.preTransform = tr.vulkan->GetDeviceSurfaceTransform();
        

    // If we are updating a old swap chain
    if ( m_swapChain )
        swapchainCI.oldSwapchain = m_swapChain; // If we are updating a existing SwapChain

    // "Adaptative" sync 
    
    if (  hasFeature( suportedFeatures, SuportedFeatures::FEATURE_SWAP_CHAIN_MODE_MAILBOX ) && vsync == 1 )
        swapchainCI.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    else if ( hasFeature( suportedFeatures, SuportedFeatures::FEATURE_SWAP_CHAIN_MODE_FIFO ) && vsync == 2 )
        swapchainCI.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    else
        swapchainCI.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
     
    //
    if ( graphicQueue == presentQueue )
    {
        swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCI.queueFamilyIndexCount = 0; // Optional
        swapchainCI.pQueueFamilyIndices = nullptr; // Optional
    }
    else
    {
        queueFamilyIndices[1] = presentQueue;
        queueFamilyIndices[0] = graphicQueue;
        swapchainCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCI.queueFamilyIndexCount = 2;
        swapchainCI.pQueueFamilyIndices = queueFamilyIndices;
    }

    // Create the swap chain
    VkResult result = vkCreateSwapchainKHR( device, &swapchainCI, nullptr, &m_swapChain );
    if ( result != VK_SUCCESS )
        throw crVkException( "Can't create swapchain", result );
        
    //Get swapChain images
    res = vkGetSwapchainImagesKHR( device, m_swapChain, &m_imageCount, nullptr );
    if ( res != VK_SUCCESS )
         throw crVkException( "vkGetSwapchainImagesKHR", res );
    
    // create image, framebuffer array 
    m_images.Resize( m_imageCount );
    m_imagesView.Resize( m_imageCount );
    m_frameBuffers.Resize( m_imageCount );

    // List the swap chain imagens 
    res = vkGetSwapchainImagesKHR( device, m_swapChain, &m_imageCount, m_images.Ptr() );
    if ( res != VK_SUCCESS )
        throw crVkException( "vkGetSwapchainImagesKHR", res );

    // Swap Chain Image Creation 
    imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCI.format = m_surfaceFormat.format;
    imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCI.subresourceRange.baseMipLevel = 0;
    imageViewCI.subresourceRange.levelCount = 1;
    imageViewCI.subresourceRange.baseArrayLayer = 0;
    imageViewCI.subresourceRange.layerCount = 1;
    imageViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    // Configure render pass
    colorAttachment.format = m_surfaceFormat.format;
    colorAttachment.samples = static_cast<VkSampleCountFlagBits>( samples );
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCI.attachmentCount = 1;
    renderPassCI.pAttachments = &colorAttachment;
    renderPassCI.subpassCount = 1;
    renderPassCI.pSubpasses = &subpass;

    res = vkCreateRenderPass( device, &renderPassCI, nullptr, &m_renderPass );
    if ( res != VK_SUCCESS )
        throw crVkException( "vkCreateRenderPass", res );

    // Swap Chain Frambe buffers Creation
    framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCI.renderPass = m_renderPass;
    framebufferCI.attachmentCount = 1;
    framebufferCI.width = width;
    framebufferCI.height = height;
    framebufferCI.layers = 1;

    // Swap chain frame fences
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    
    //
    for ( uint32_t i = 0; i < m_imageCount; i++)
    {
        imageViewCI.image = m_images[i];
        res = vkCreateImageView( device, &imageViewCI, nullptr, &m_imagesView[i] );
        if ( res != VK_SUCCESS )
            throw crVkException( "vkCreateImageView", res );
    
        // Create frame buffer
        framebufferCI.pAttachments = &m_imagesView[i];
        res = vkCreateFramebuffer( device, &framebufferCI, nullptr, &m_frameBuffers[i] );
        if ( res != VK_SUCCESS )
            throw crVkException( "vkCreateFramebuffer", res );
    }
}

void crVKSwapChain::Destroy(void)
{
    auto logicDevice = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetAllocator();

    // Release frame buffers, textures, views  
    for ( uint32_t i = 0; i < m_imageCount; i++)
    {
        vkDestroyFramebuffer( logicDevice, m_frameBuffers[i], &allocator );
        vkDestroyImageView( logicDevice, m_imagesView[i], &allocator );
        vkDestroyImage( logicDevice, m_images[i], &allocator );
    }

    // destroy render pass and swap chain
    vkDestroyRenderPass( logicDevice, m_renderPass, &allocator );
    vkDestroySwapchainKHR( logicDevice, m_swapChain, &allocator );

    m_frameBuffers.Clear();
    m_imagesView.Clear();
    m_images.Clear();
}

void crVKSwapChain::Begin( const uint32_t frame )
{
    VkResult res = VK_SUCCESS;
    auto device = tr.vulkan->GetDevice();
    res = vkAcquireNextImageKHR( device, m_swapChain, UINT64_MAX, m_semaphore, nullptr,  &m_currentFrame );

    if ( res != VK_SUCCESS )
        common->Error( "vkAcquireNextImageKHR" );

}

void crVKSwapChain::End(void)

{
}

void crVKSwapChain::SwapBuffers(void)
{
    VkResult res = VK_SUCCESS;

    //
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_semaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapChain;
    presentInfo.pImageIndices = &m_currentFrame;

    // present render 
    res = vkQueuePresentKHR( m_presentQueue, &presentInfo );
    if ( res != VK_SUCCESS )
        common->Error( "vkAcquireNextImageKHR" );
}
