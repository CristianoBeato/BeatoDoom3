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
#include "vkFrameBuffer.h"

crVKFrameBuffer::crVKFrameBuffer( void )
{
}

crGLPipeline::~crGLPipeline( void )
{
}

void crVKFrameBuffer::Create( const frameBuffer_t* frameBufferCreateInf )
{
    auto device = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetAllocator();
    idList<VkImageView> attachmentsView;

    attachmentsView.Resize( frameBufferCreateInf->attachmentCount );

    for ( uint32_t i = 0; i < frameBufferCreateInf->attachmentCount; i++ )
    {
        attachmentsView[i] = dynamic_cast<crVKTexture*>( frameBufferCreateInf->colorAttachament[i] )->GetViewHandler();
    }
    
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass; // Render pass compatível com os attachments
    framebufferInfo.attachmentCount = attachmentsView.Num();       // Número de attachments; ajuste se tiver mais
    framebufferInfo.pAttachments = attachmentsView.Ptr();
    framebufferInfo.width = frameBufferCreateInf->width;
    framebufferInfo.height = frameBufferCreateInf->height;
    framebufferInfo.layers = frameBufferCreateInf->layers;

    VkResult res = vkCreateFramebuffer( device, &framebufferInfo, nullptr, &m_framebuffers ); 
    if ( res != VK_SUCCESS )
        throw idException("Falha ao criar framebuffer!");
}

void crVKFrameBuffer::Delete( void )
{
    if ( m_framebuffers != nullptr )
    {
        vkDestroyFramebuffer( tr.vulkan->GetDevice(), m_framebuffers, &tr.vulkan->GetAllocator() );
        m_framebuffers = nullptr;
    }
}
