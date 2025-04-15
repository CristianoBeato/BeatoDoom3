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
#include "vkFramebuffer.h"

crVKFrameBuffer::crVKFrameBuffer( void )
{
}

crGLPipeline::~crGLPipeline( void )
{
}

void crVKFrameBuffer::Create( const uint32_t width, const uint32_t height, const uint32_t samples, const uint32_t layers, const uint32_t attachmentCount, crTexture** colorAttachament )
{
    VkResult res = VK_SUCCESS;
    idList<VkImageView> attachmentsView;
    auto device = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetAllocator();
    VkAttachmentDescription colorAttachment{};
    idList<VkAttachmentReference> colorAttachmentsRefs;
    VkSubpassDescription subpass{};
    VkRenderPassCreateInfo renderPassCI{};
    VkFramebufferCreateInfo framebufferCI{};

    m_extent.width = width;
    m_extent.height = height;

    // configure frame buffer attachements 
    colorAttachment.format = VK_FORMAT_B8G8R8_SRGB;
    colorAttachment.samples = static_cast<VkSampleCountFlagBits>( samples );
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Create color attachment references
    colorAttachmentsRefs.Resize( attachmentCount );
    attachmentsView.Resize( attachmentCount );
    for ( uint32_t i = 0; i < attachmentCount; i++)
    {
        colorAttachmentsRefs[i].attachment = i;
        colorAttachmentsRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachmentsView[i] = dynamic_cast<crVKTexture*>( colorAttachament[i] )->GetViewHandler();
    }

    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = attachmentCount;
    subpass.pColorAttachments = colorAttachmentsRefs.Ptr();
    
    // todo: add depth stencil attachment
    subpass.pDepthStencilAttachment = nullptr;

    // create the render pass  
    renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCI.attachmentCount = 1;
    renderPassCI.pAttachments = &colorAttachment;
    renderPassCI.subpassCount = 1;
    renderPassCI.pSubpasses = &subpass;

    res = vkCreateRenderPass( device, &renderPassCI, nullptr, &m_renderPass );
    if ( res != VK_SUCCESS )
        throw crVkException( "vkCreateRenderPass", res );

    // create frame buffer
    framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCI.renderPass = m_renderPass;
    framebufferCI.attachmentCount = attachmentCount;
    framebufferCI.pAttachments = attachmentsView.Ptr();
    framebufferCI.width = width;
    framebufferCI.height = height;
    framebufferCI.layers = 1;

    res = vkCreateFramebuffer(device, &framebufferCI, nullptr, &m_framebuffer );
    if ( res != VK_SUCCESS )
        throw crVkException( "vkCreateFramebuffer", res );
}

void crVKFrameBuffer::Delete( void )
{
    auto allocator = tr.vulkan->GetAllocator();
    auto device = tr.vulkan->GetDevice();
    if ( m_framebuffer != nullptr )
    {
        vkDestroyFramebuffer( device, m_framebuffer, &allocator );
        m_framebuffer = nullptr;
    }

    if( m_renderPass != nullptr )
    {
        vkDestroyRenderPass( device, m_renderPass, &allocator );
        m_renderPass = nullptr;
    }
}
